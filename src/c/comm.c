#include <pebble.h>

// Deactivate APP_LOG in this file.
// #undef APP_LOG
// #define APP_LOG(...)

#include "misc.h"

#include "comm.h"
#include "data/Model.h"
#include "libs/RingBuffer.h"


static Model* dataModel;
static CommHandlers commHandlers;
static RingBuffer* sendBuffer;
static ClaySettings settings;
static char** strSettings;
static AppTimer* sendRetryTimer;
static uint8_t sendRetryCount;
static bool pebkitReady;

const uint8_t MAX_SEND_RETRIES = 5;
const uint8_t SEND_BUF_SIZE = 10;
const uint32_t SETTINGS_STRUCT_KEY = 0x1000;


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static Message* comm_msg_create(uint32_t msgKey, void* payload) {
  Message* newMsg = malloc(sizeof(*newMsg));
  if (newMsg == NULL) return NULL;

  newMsg->key = msgKey;
  newMsg->payload = payload;
  return newMsg;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static void comm_msg_destroy(Message* msg) {
  if (msg != NULL) { free(msg); msg = NULL; }
}


/////////////////////////////////////////////////////////////////////////////
/// Converts a tuple to a simple data type.
/////////////////////////////////////////////////////////////////////////////
static bool unloadTupleStr(char** buffer, size_t bufsize, Tuple* tuple, const char* readable) {
  if (*buffer == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "unloadTupleStr: Cannot write to null buffer!");
    return false;
  }
  if (tuple != NULL) {
    long ret = 0;
    if ((ret = snprintf(*buffer, bufsize, "%s", tuple->value->cstring)) < 0) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "%s string was not written correctly. Ret=%ld", readable, ret);
      return false;
    } else if ((size_t)ret >= bufsize) {
      APP_LOG(APP_LOG_LEVEL_WARNING, "%s string was truncated. %ld characters required.", readable, ret);
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "unloadTupleStr %s: %s", readable, *buffer);
    return true;
  }
  return false;
}


/////////////////////////////////////////////////////////////////////////////
/// Converts a tuple to a simple data type.
/////////////////////////////////////////////////////////////////////////////
static bool unloadTupleLong(long int* buffer, Tuple* tuple, const char* readable) {
  if (tuple) {
    *buffer = (long int)tuple->value->int32;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "unloadTupleLong %s: %ld", readable, *buffer);
    return true;
  }
  return false;
}


/////////////////////////////////////////////////////////////////////////////
/// Handles callbacks from the JS component
/////////////////////////////////////////////////////////////////////////////
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Inbox receive successful.");

  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;
  Tuple *tuple = NULL;

  if ( (tuple = dict_find(iterator, MESSAGE_KEY_PEBKIT_READY)) != NULL ) {
    // PebbleKit JS is ready! Safe to send messages
    pebkitReady = true;
    APP_LOG(APP_LOG_LEVEL_INFO, "PebbleKit JS sent ready message!");

    bool empty = true;
    if ( (sendBuffer != NULL) && ( (mpaRet = RingBuffer_empty(sendBuffer, &empty)) != MPA_SUCCESS) ) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error checking sendBuffer: %s", MagPebApp_getErrMsg(mpaRet));
        return;
    }
    if (!empty) { comm_sendBufMsg(); }

    // Ready to load saved data from persistent memory now.
    comm_loadPersistent();
  }

  if ( (tuple = dict_find(iterator, MESSAGE_KEY_CURRENCY_SYMBOL)) != NULL ) {
    const char* readable = "Currency symbol";
    size_t bufsize = strlen(tuple->value->cstring)+1;
    char* currencySymbolBuf = NULL;
    currencySymbolBuf = malloc(bufsize);
    if (!unloadTupleStr(&currencySymbolBuf, bufsize, tuple, readable)) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error in unloadTupleStr.");
    } else {
      APP_LOG(APP_LOG_LEVEL_INFO, "currencySymbolBuf = %s", currencySymbolBuf);
      if ( (mpaRet = Model_setCurrencySymbol(dataModel, currencySymbolBuf)) != MPA_SUCCESS) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error setting %s in data model: %s", readable, MagPebApp_getErrMsg(mpaRet));
      }
    }
    if (currencySymbolBuf != NULL) { free(currencySymbolBuf); currencySymbolBuf = NULL; }
    comm_savePersistent();
  }


  if (!commHandlers.updateViewData) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Attempted operation on NULL pointer.");
  } else {
    (*commHandlers.updateViewData)(dataModel);
  }

}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Inbox receive failed! Reason: %d", (int)reason);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed! Reason: %d", (int)reason);
  comm_startResendTimer();
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send successful.");
}


/////////////////////////////////////////////////////////////////////////////
/// Returns whether the communication channel is ready for sending.
/////////////////////////////////////////////////////////////////////////////
bool comm_pebkitReady() {
  return pebkitReady;
}


/////////////////////////////////////////////////////////////////////////////
/// Queue data and send to PebbleKit.
/////////////////////////////////////////////////////////////////////////////
void comm_enqMsg(Message* msg) {
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;
  if (msg == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Tried to buffer a null message.");
    return;
  }

  if (sendBuffer == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Send buffer is null.");
    return;
  }

  if ( (mpaRet = RingBuffer_write(sendBuffer, (void*)msg)) != MPA_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error buffering message: %s", MagPebApp_getErrMsg(mpaRet));
    comm_msg_destroy(msg);
    return;
  }

  comm_sendBufMsg();
}


/////////////////////////////////////////////////////////////////////////////
/// Send buffered data to PebbleKit.
/////////////////////////////////////////////////////////////////////////////
void comm_sendBufMsg() {
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;

  void* data = NULL;
  if ( (mpaRet = RingBuffer_peek(sendBuffer, &data)) != MPA_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error reading buffered message: %s", MagPebApp_getErrMsg(mpaRet));
    return;
  }

  Message* msg = (Message*) data;
  if (msg == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Send buffer contained a null message.");
    return;
  }

  // At this point, we know there is a non-null message in the send buffer.

  // Check if PebbleKit JS is ready to receive...
  if (!comm_pebkitReady()) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Buffering message to phone until PebbleKit JS is ready...");
    comm_startResendTimer();
    return;
  }

  // Prepare the outbox buffer for this message
  DictionaryIterator *outIter;
  AppMessageResult result = app_message_outbox_begin(&outIter);
  if (result != APP_MSG_OK) {
    // The outbox cannot be used right now
    APP_LOG(APP_LOG_LEVEL_WARNING, "Error preparing the outbox for message %d.  Result: %d", (int)msg->key, (int)result);
    comm_startResendTimer();
    return;
  }

  // Ready to write to app message outbox...
  dict_write_cstring(outIter, msg->key, msg->payload);

  // Send this message
  result = app_message_outbox_send();

  if(result != APP_MSG_OK) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Error sending the outbox for message %d.  Result: %d", (int)msg->key, (int)result);
    comm_startResendTimer();
    return;
  }

  // Successful send attempt!
  APP_LOG(APP_LOG_LEVEL_INFO, "Sent outbox message %d!", (int)msg->key);
  sendRetryCount = 0;
  RingBuffer_drop(sendBuffer);
  comm_msg_destroy(msg);
  if (sendRetryTimer != NULL) { app_timer_cancel(sendRetryTimer);  sendRetryTimer = NULL; }
}


/////////////////////////////////////////////////////////////////////////////
/// Starts a backoff timer to retry the first message in the send buffer.
/////////////////////////////////////////////////////////////////////////////
void comm_startResendTimer() {
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;

  void* data = NULL;
  if ( (mpaRet = RingBuffer_peek(sendBuffer, &data)) != MPA_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error reading buffered message: %s", MagPebApp_getErrMsg(mpaRet));
    return;
  }

  Message* msg = (Message*) data;
  if (msg == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Send buffer contained a null message.");
    return;
  }

  if (sendRetryCount < MAX_SEND_RETRIES) {
    sendRetryCount++;
    uint16_t retryIntervalMs = (sendRetryCount * 1000);
    APP_LOG(APP_LOG_LEVEL_INFO, "Retrying message (%d) send in %d ms...", (int)msg->key, retryIntervalMs);
    sendRetryTimer = app_timer_register(retryIntervalMs, comm_sendBufMsg, NULL);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Max retries failed. Abandoning message (%d).", (int)msg->key);
    sendRetryCount = 0;
    RingBuffer_drop(sendBuffer);
    if (sendRetryTimer != NULL) { app_timer_cancel(sendRetryTimer);  sendRetryTimer = NULL; }
  }
}


/////////////////////////////////////////////////////////////////////////////
/// Send data to PebbleKit.
/////////////////////////////////////////////////////////////////////////////
void comm_sendMsg(const Message* msg) {
  if (msg == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Tried to send a null message.");
    return;
  }

  if (!comm_pebkitReady()) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Tried to send a message from the watch before PebbleKit JS is ready.");
    return;
  }

  // Declare the dictionary's iterator
  DictionaryIterator *outIter;

  // Prepare the outbox buffer for this message
  AppMessageResult result = app_message_outbox_begin(&outIter);
  if (result == APP_MSG_OK) {
    dict_write_cstring(outIter, msg->key, msg->payload);

    // Send this message
    result = app_message_outbox_send();

    if(result == APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_INFO, "Sent outbox message %d!", (int)msg->key);
    } else {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox for message %d.  Result: %d", (int)msg->key, (int)result);
    }

  } else {
    // The outbox cannot be used right now
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox for message %d.  Result: %d", (int)msg->key, (int)result);
  }
}


/////////////////////////////////////////////////////////////////////////////
/// Saves app settings to persistent storage.
/////////////////////////////////////////////////////////////////////////////
void comm_savePersistent() {
  if (dataModel == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Data model is not yet initialized.");
    return;
  }

  persist_write_data(SETTINGS_STRUCT_KEY, &settings, sizeof(settings));

  // Write all string settings
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;
  for (int keyIdx=0; keyIdx<LAST_STR_SETTING; keyIdx++) {
    // Fetch data stored elsewhere
    switch(keyIdx) {
      case CURRENCY_SYMBOL_SETTING: {
        if (strSettings[keyIdx] != NULL) { free(strSettings[keyIdx]); strSettings[keyIdx] = NULL; }
        if ( (mpaRet = Model_getCurrencySymbol(dataModel, &strSettings[keyIdx])) != MPA_SUCCESS) {
          APP_LOG(APP_LOG_LEVEL_ERROR, "Could not retrieve custom string: %s", MagPebApp_getErrMsg(mpaRet));
          return;
        }
        break;
      }
    } // end switch(keyIdx)

    
    // Write data to persistent memory on watch
    status_t result = 0;
    if ( (result = persist_write_string(keyIdx, strSettings[keyIdx])) < 0) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Could not write string #%d to persistent storage. Error: %ld", keyIdx, result);
    } else {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Wrote string #%d (%s) to persistent storage.", keyIdx, strSettings[keyIdx]);
    }

  } // end for
  return;

}


/////////////////////////////////////////////////////////////////////////////
/// Loads app settings from persistent storage.
/////////////////////////////////////////////////////////////////////////////
void comm_loadPersistent() {
  if (dataModel == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Data model is not yet initialized.");
    return;
  }

  if (persist_exists(SETTINGS_STRUCT_KEY)) {
    persist_read_data(SETTINGS_STRUCT_KEY, &settings, sizeof(settings));
  }

  // Read all string settings
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;
  int keyIdx;
  for (keyIdx=0; keyIdx<LAST_STR_SETTING; keyIdx++) {
    if (persist_exists(keyIdx)) {
      if (strSettings[keyIdx] != NULL) { free(strSettings[keyIdx]); strSettings[keyIdx] = NULL; }
      int size = persist_get_size(keyIdx);
      if ( (strSettings[keyIdx] = calloc(size, sizeof(*strSettings[keyIdx])) ) == NULL) { goto freemem; }

      // Read the string from persistent watch storage.
      status_t result;
      if ( (result = persist_read_string(keyIdx, strSettings[keyIdx], size)) < 0) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not read string #%d from persistent storage. Error: %ld", keyIdx, result);
      } else {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Read string #%d (%s) from persistent storage.", keyIdx, strSettings[keyIdx]);
      }

      switch(keyIdx) {
        case CURRENCY_SYMBOL_SETTING: {
          if ( (mpaRet = Model_setCurrencySymbol(dataModel, strSettings[keyIdx])) != MPA_SUCCESS) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "Error setting %s in data model: %s", "Currency symbol", MagPebApp_getErrMsg(mpaRet));
          } else {
            // update any UI dependent on custom string
          }
          break;
        }
      } // end switch(keyIdx)
    } // end if persist_exists(keyIdx)
  } // end for

  return;

freemem:
  APP_LOG(APP_LOG_LEVEL_ERROR, "Error... freeing memory");
  if (strSettings[keyIdx] != NULL) { free(strSettings[keyIdx]); strSettings[keyIdx] = NULL; }
}


/////////////////////////////////////////////////////////////////////////////
/// Callback for TickTimerService
/////////////////////////////////////////////////////////////////////////////
void comm_tickHandler(struct tm *tick_time, TimeUnits units_changed) {
  { // limiting timebuf in a local scope
    const size_t bufsize = 40;
    char timebuf[bufsize];
    size_t ret = 0;

    if ( (ret = strftime(timebuf, (int)bufsize, "%a, %d %b %Y %T %z", tick_time)) == 0) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error returned: %d", ret);
      return;
    }
    HEAP_LOG(timebuf);
  }

  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;
  if ( (mpaRet = Model_updateTime(dataModel, tick_time, units_changed)) != MPA_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error encountered during model time update: %s", MagPebApp_getErrMsg(mpaRet));
  }
  
  if (!commHandlers.updateViewTime) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Attempted operation on NULL pointer.");
  }
  (*commHandlers.updateViewTime)(tick_time);
  
  if (!commHandlers.updateViewData) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Attempted operation on NULL pointer.");
  } else {
    (*commHandlers.updateViewData)(dataModel);
  }

}


/////////////////////////////////////////////////////////////////////////////
/// Set our callback handlers.
/////////////////////////////////////////////////////////////////////////////
void comm_setHandlers(const CommHandlers cmh) {
  commHandlers = cmh;
}


/////////////////////////////////////////////////////////////////////////////
/// Opens communication to PebbleKit and allocates memory.
/////////////////////////////////////////////////////////////////////////////
void comm_open() {
  dataModel = NULL;
  if ( (dataModel = Model_create("")) == NULL) { APP_LOG(APP_LOG_LEVEL_ERROR, "Could not initialize data model."); }

  strSettings = NULL;
  if ( (strSettings = calloc(LAST_STR_SETTING, sizeof(*strSettings)) ) == NULL) { goto freemem; }
  // Initialize each string setting to NULL.
  for (int idx=0; idx<LAST_STR_SETTING; idx++) {
    strSettings[idx] = NULL;
  }

  sendBuffer = NULL;
  if ( (sendBuffer = RingBuffer_create(SEND_BUF_SIZE)) == NULL) { APP_LOG(APP_LOG_LEVEL_ERROR, "Could not initialize send buffer."); }

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
  // JRB TODO: Adjust message queue size appropriately.
  app_message_open(300, 300);

  return;

freemem:
  APP_LOG(APP_LOG_LEVEL_ERROR, "Error... freeing memory");
  if (strSettings != NULL) { free(strSettings);  strSettings = NULL; }
}


/////////////////////////////////////////////////////////////////////////////
/// Closes communication and frees memory.
/////////////////////////////////////////////////////////////////////////////
void comm_close() {
  if (dataModel != NULL) {
    Model_destroy(dataModel);  dataModel = NULL;
  }

  if (sendBuffer != NULL) {
    RingBuffer_destroy(sendBuffer);  sendBuffer = NULL;
  }

  if (strSettings != NULL) {
    for (int idx=0; idx<LAST_STR_SETTING; idx++) {
      if (strSettings[idx] != NULL) { free(strSettings[idx]); strSettings[idx] = NULL; }
    }
    if (strSettings != NULL) { free(strSettings); strSettings = NULL; }
  }

  app_message_deregister_callbacks();
}


/////////////////////////////////////////////////////////////////////////////
/// Conveys an attendance adjustment to the model.
/////////////////////////////////////////////////////////////////////////////
void comm_adjustAttendance(const int16_t attenIncr, const uint16_t attenSalary) {
  if (dataModel == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Data model is not yet initialized.");
    return;
  }

  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;
  if ( (mpaRet = Model_adjustAttendance(dataModel, attenIncr, attenSalary)) != MPA_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error adjusting attendance: %s", MagPebApp_getErrMsg(mpaRet));
    return;
  }
  
  if (!commHandlers.updateViewData) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Attempted operation on NULL pointer.");
  } else {
    (*commHandlers.updateViewData)(dataModel);
  }

}


/////////////////////////////////////////////////////////////////////////////
/// Conveys a meeting status toggle to the model.
/////////////////////////////////////////////////////////////////////////////
void comm_toggleMeeting() {
  if (dataModel == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Data model is not yet initialized.");
    return;
  }
  
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;
  Model_State status = MODEL_STATE_NO_ATTENDEES;
  if ( (mpaRet = Model_getStatus(dataModel, &status)) != MPA_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error encountered: %s", MagPebApp_getErrMsg(mpaRet));
    return;
  }

  switch (status) {
    case (MODEL_STATE_STARTED): {
      if ( (mpaRet = Model_stopMeeting(dataModel)) != MPA_SUCCESS) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error encountered: %s", MagPebApp_getErrMsg(mpaRet));
        return;
      }
      break;
    }
    case (MODEL_STATE_STOPPED): {
      if ( (mpaRet = Model_startMeeting(dataModel)) != MPA_SUCCESS) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error encountered: %s", MagPebApp_getErrMsg(mpaRet));
        return;
      }
      break;
    }
    default: {
      APP_LOG(APP_LOG_LEVEL_WARNING, "Unexpected model state: %d", status);
    }
  }
      
  if (!commHandlers.updateViewData) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Attempted operation on NULL pointer.");
  } else {
    (*commHandlers.updateViewData)(dataModel);
  }

}


/////////////////////////////////////////////////////////////////////////////
/// Conveys a meeting reset to the model.
/////////////////////////////////////////////////////////////////////////////
void comm_resetMeeting() {
  if (dataModel == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Data model is not yet initialized.");
    return;
  }
  
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;
  if ( (mpaRet = Model_reset(dataModel)) != MPA_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error encountered: %s", MagPebApp_getErrMsg(mpaRet));
    return;
  }

  if (!commHandlers.updateViewData) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Attempted operation on NULL pointer.");
  } else {
    (*commHandlers.updateViewData)(dataModel);
  }

}

