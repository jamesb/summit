#pragma once

#include "data/Model.h"

typedef struct ClaySettings {
  uint8_t settingsVer;
  uint32_t defaultMilliHourly;
} ClaySettings;


// Setting string definitions
typedef enum StrSettings {
  CURRENCY_SYMBOL_SETTING  = 0,

  LAST_STR_SETTING
} StrSettings;


typedef struct Message {
  uint32_t  key;
  char*     payload;
} Message;


// UpdateViewTimeHandler is a pointer to a function that takes a single
// parameter (struct tm pointer) and returns nothing.
typedef void (*UpdateViewTimeHandler)(struct tm*);

// UpdateViewDataHandler is a pointer to a function that takes a single
// parameter (const Model pointer) and returns nothing.
typedef void (*UpdateViewDataHandler)(const Model*);

// CommHandlers is a struct that contains the values of the handlers.
typedef struct CommHandlers {
  UpdateViewTimeHandler  updateViewTime;      ///< Function that Comm calls to notify the View of a time update.
  UpdateViewDataHandler  updateViewData;      ///< Function that Comm calls to notify the View of a data model update.
} CommHandlers;


bool comm_pebkitReady();
void comm_open();
void comm_close();

// For buffered sending
void comm_enqMsg(Message*);
void comm_sendBufMsg();
void comm_startResendTimer();

// For unbuffered sending
void comm_sendMsg(const Message*);

void comm_tickHandler(struct tm *tick_time, TimeUnits units_changed);
void comm_setHandlers(const CommHandlers);

void comm_savePersistent();
void comm_loadPersistent();

// For view-to-model communication
void comm_adjustAttendance(const int16_t attenIncr, const uint16_t attenSalary);
void comm_toggleMeeting();
void comm_resetMeeting();
