#include <pebble.h>

// Deactivate APP_LOG in this file.
#undef APP_LOG
#define APP_LOG(...)


#include "Model_Internal.h"
#include "../misc.h"




/////////////////////////////////////////////////////////////////////////////
/// Constructor
/////////////////////////////////////////////////////////////////////////////
Model* Model_create() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Creating Model");
  int mpaRet;

  Model* newModel = malloc(sizeof(*newModel));
  if ( (mpaRet = Model_init(newModel)) != MPA_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Could not initialize: %s", MagPebApp_getErrMsg(mpaRet));
    Model_destroy(newModel);  newModel = NULL;
  }

  return newModel;
}


/////////////////////////////////////////////////////////////////////////////
/// Destroys Model and frees allocated memory.
/// @param[in,out]  this  Pointer to Model; must be already allocated
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode Model_destroy(Model* this) {
  MPA_RETURN_IF_NULL(this);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroying Model");

  // Free memory for data members
  if (this->currencySymbol != NULL)   { free(this->currencySymbol);   this->currencySymbol = NULL; }

  free(this); this = NULL;
  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// Internal initialization
/// @param[in,out]  this  Pointer to Model; must be already allocated
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode Model_init(Model* this) {
  MPA_RETURN_IF_NULL(this);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Initializing Model");
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;

  // Initialize/allocate data members
  this->currencySymbol = NULL;
  if ( (mpaRet = Model_reset(this)) != MPA_SUCCESS) { goto freemem; }

  // Determine locale
  const char *locale = i18n_get_system_locale();

  // JRB NOTE: When setting these default rates, we consider the
  // "paid work year" to be 52 weeks per year, with 40 hours per
  // work-week. (52 * 40) == 2080 work-minutes per year

  // Set default currency symbol based on locale (this can be overridden in settings)
  if (strcmp(locale, "en_US") == 0) {
    if ( (mpaRet = Model_setCurrencySymbol(this, "$")) != MPA_SUCCESS) { goto freemem; }
    // average U.S. salary = $40K
    if ( (mpaRet = Model_setDefaultMilliHourly(this, 19230)) != MPA_SUCCESS) { goto freemem; }
  } else if ( (strcmp(locale, "fr_FR") == 0) || (strcmp(locale, "de_DE") == 0) || (strcmp(locale, "es_ES") == 0) || (strcmp(locale, "it_IT") == 0) || (strcmp(locale, "pt_PT") == 0) ) {
    if ( (mpaRet = Model_setCurrencySymbol(this, "€")) != MPA_SUCCESS) { goto freemem; }
    // average EU salary = €20K
    if ( (mpaRet = Model_setDefaultMilliHourly(this, 9615)) != MPA_SUCCESS) { goto freemem; }
  } else if ( (strcmp(locale, "en_CN") == 0) || (strcmp(locale, "en_TW") == 0) ) {
    if ( (mpaRet = Model_setCurrencySymbol(this, "¥")) != MPA_SUCCESS) { goto freemem; }
    // average China salary = ¥29K
    if ( (mpaRet = Model_setDefaultMilliHourly(this, 14423)) != MPA_SUCCESS) { goto freemem; }
  } else {
    if ( (mpaRet = Model_setCurrencySymbol(this, "$")) != MPA_SUCCESS) { goto freemem; }
    if ( (mpaRet = Model_setDefaultMilliHourly(this, 19230)) != MPA_SUCCESS) { goto freemem; }
  }

  return mpaRet;

freemem:
  APP_LOG(APP_LOG_LEVEL_ERROR, "Error... freeing memory");
  if (this != NULL) { Model_destroy(this);  this = NULL; }
  return MPA_OUT_OF_MEMORY_ERR;
}


/////////////////////////////////////////////////////////////////////////////
/// Resets the meeting-dependent data to its original state.
/// (eg. No meeting attendees, no meeting cost, etc.)
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode Model_reset(Model* this) {
  MPA_RETURN_IF_NULL(this);
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;

  this->numAttendees = 0;
  this->totalMilliHourly = 0;
  this->lastRateChangeTS = 0;
  this->lastRateMilliCost = 0;
  this->currentRateMilliCost = 0;
  this->totalMeetingMilliCost = 0;
  this->status = MODEL_STATE_NO_ATTENDEES;

  return mpaRet;
}


/////////////////////////////////////////////////////////////////////////////
/// Gets the currency symbol.
/// @param[in,out]  this  Pointer to Model; must be already allocated
/// @param[out]     currSym   Pointer to the currency symbol C-string in use
///       in the Model; must be NULL on entry.
///       <em>Ownership is not transferred to the caller, so the caller
///       should not free this variable.</em>
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the Model pointer (this) is NULL.
///          MPA_INVALID_INPUT_ERR if currSym is not NULL.
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode Model_getCurrencySymbol(const Model* this, char** currSym) {
  MPA_RETURN_IF_NULL(this);
  if (*currSym != NULL) { return MPA_INVALID_INPUT_ERR; }

  *currSym = this->currencySymbol;
  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// Sets the currency symbol.
/// @param[in,out]  this  Pointer to Model; must be already allocated
/// @param[in]      currSym   Pointer to the custom C-string.
///       <em>Ownership is not transferred to this function, so the caller
///       is still responsible for freeing this variable.</em>
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode Model_setCurrencySymbol(Model* this, const char* currSym) {
  MPA_RETURN_IF_NULL(this);

  if (currSym == NULL) { return MPA_INVALID_INPUT_ERR; }
  if ( (this->currencySymbol != NULL) && (strcmp(currSym, this->currencySymbol) == 0) ) {
    return MPA_SUCCESS;
  }

  size_t bufsize = strlen(currSym)+1;
  char* tmp = realloc(this->currencySymbol, bufsize);
  if (tmp == NULL) { return MPA_OUT_OF_MEMORY_ERR; }
  this->currencySymbol = tmp;

  if (!strxcpy(this->currencySymbol, bufsize, currSym, "Currency symbol")) { return MPA_STRING_ERR; }

  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode Model_getDefaultMilliHourly(Model* this, uint32_t* defaultMilliHourly) {
  MPA_RETURN_IF_NULL(this);
  *defaultMilliHourly = this->defaultMilliHourly;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Getting default hourly rate: %lu", *defaultMilliHourly);
  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode Model_setDefaultMilliHourly(Model* this, const uint32_t defaultMilliHourly) {
  MPA_RETURN_IF_NULL(this);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting default hourly rate to: %lu", defaultMilliHourly);
  this->defaultMilliHourly = defaultMilliHourly;
  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// Adjust the number of persons attending the meeting and starts or stops
/// the meeting if there are attendees.
/// @param[in,out]  this  Pointer to Model; must be already allocated
/// @param[in]      attenIncr  Number of attendees to increment or decrement.
///       (Decrement happens if this value is negative. If decrement would
///       decrease the number of total attendees below zero, then an overflow
///       error will be the result.)
/// @param[in]      attenMiliHourly  Hourly wage (in thousandths of currency
///       units, eg. 50000 = $50/hr) of every attendee in this adjustment.
///       If this parameter is set to zero, then the model's default rate
///       will be used.
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the Model pointer (this) is NULL.
///          MPA_OVERFLOW_ERR if the adjustment would cause an integer
///          overflow.
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode Model_adjustAttendance(Model* this, const int16_t attenIncr, const uint32_t c_attenMilliHourly) {
  MPA_RETURN_IF_NULL(this);
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;

  uint32_t attenMilliHourly = (c_attenMilliHourly == 0) ? this->defaultMilliHourly : c_attenMilliHourly;

  uint16_t numAttendees = this->numAttendees;
  if ( (mpaRet = u16add_u16_s16(&numAttendees, numAttendees, attenIncr)) != MPA_SUCCESS) return mpaRet;

  int32_t salaryIncr = 0;
  if ( (mpaRet = s32mult_s32_s32(&salaryIncr, (int32_t)attenMilliHourly, attenIncr)) != MPA_SUCCESS) return mpaRet;

  uint32_t totalMilliHourly = this->totalMilliHourly;
  if ( (mpaRet = u32add_u32_s32(&totalMilliHourly, totalMilliHourly, salaryIncr)) != MPA_SUCCESS) return mpaRet;

  if ( (mpaRet = Model_calculateCost(this)) != MPA_SUCCESS) return mpaRet;

  uint32_t lastRateMilliCost = 0;
  if ( (mpaRet = u32add_u32_s32(&lastRateMilliCost, this->lastRateMilliCost, this->currentRateMilliCost)) != MPA_SUCCESS) return mpaRet;

  this->numAttendees = numAttendees;
  this->totalMilliHourly = totalMilliHourly;
  this->lastRateChangeTS = time(NULL);
  this->lastRateMilliCost = lastRateMilliCost;
  this->currentRateMilliCost = 0;
  this->totalMeetingMilliCost = this->lastRateMilliCost;

  if (this->numAttendees <= 0) {
     if ( (mpaRet = Model_stopMeeting(this)) != MPA_SUCCESS) return mpaRet;
     this->status = MODEL_STATE_NO_ATTENDEES;
  } else {
     if ( (mpaRet = Model_startMeeting(this)) != MPA_SUCCESS) return mpaRet;
  }

  return mpaRet;
}


/////////////////////////////////////////////////////////////////////////////
/// Starts the meeting, which triggers the accumulation of cost.
/// @param[in,out]  this  Pointer to Model; must be already allocated
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the Model pointer (this) is NULL.
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode Model_startMeeting(Model* this) {
  MPA_RETURN_IF_NULL(this);
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;

  this->status = MODEL_STATE_STARTED;
  if (this->lastRateChangeTS == 0) this->lastRateChangeTS = time(NULL);
  if ( (mpaRet = Model_calculateCost(this)) != MPA_SUCCESS) return mpaRet;

  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// Stops the meeting, which ceases the accumulation of cost.
/// @param[in,out]  this  Pointer to Model; must be already allocated
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the Model pointer (this) is NULL.
///          MPA_OVERFLOW_ERR if the necessary calculations would cause an
///          integer overflow.
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode Model_stopMeeting(Model* this) {
  MPA_RETURN_IF_NULL(this);
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;

  if ( (mpaRet = Model_calculateCost(this)) != MPA_SUCCESS) return mpaRet;

  uint32_t lastRateMilliCost = 0;
  if ( (mpaRet = u32add_u32_u32(&lastRateMilliCost, this->lastRateMilliCost, this->currentRateMilliCost)) != MPA_SUCCESS) return mpaRet;

  this->status = MODEL_STATE_STOPPED;
  this->lastRateChangeTS = 0;
  this->lastRateMilliCost = lastRateMilliCost;
  this->currentRateMilliCost = 0;
  this->totalMeetingMilliCost = this->lastRateMilliCost;

  return mpaRet;
}


/////////////////////////////////////////////////////////////////////////////
/// Internal calculation routine to update the cost of a meeting that is in
/// progress. To optimize memory for Pebble, this function restricts its
/// math to integers.
/// @param[in,out]  this  Pointer to Model; must be already allocated
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the Model pointer (this) is NULL.
///          MPA_OVERFLOW_ERR if the necessary calculations would cause an
///          integer overflow.
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode Model_calculateCost(Model* this) {
  MPA_RETURN_IF_NULL(this);
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;

  if (this->lastRateChangeTS == 0) return mpaRet;
  time_t currentTime = time(NULL);
  uint32_t elapsedTimeAtRate = (currentTime - this->lastRateChangeTS);


  uint32_t currentRateMilliCost = 0;
  if ( (mpaRet = u32mult_u32_u32(&currentRateMilliCost, this->totalMilliHourly, elapsedTimeAtRate)) != MPA_SUCCESS) return mpaRet;
  currentRateMilliCost /= 3600;

  uint32_t totalMeetingMilliCost = 0;
  if ( (mpaRet = u32add_u32_u32(&totalMeetingMilliCost, this->lastRateMilliCost, currentRateMilliCost)) != MPA_SUCCESS) return mpaRet;

  this->currentRateMilliCost = currentRateMilliCost;
  this->totalMeetingMilliCost = totalMeetingMilliCost;

  int32_t totalKiloSalary = this->totalMilliHourly * 208 / 100000;
  s32RoundNear(&totalKiloSalary, totalKiloSalary, 10);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Attendees: %u   Total Salary: %luK   Elapsed Time at Rate: %lu secs", this->numAttendees, totalKiloSalary, elapsedTimeAtRate);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Cents/hr: %lu   Cents/min: %lu   Cents/sec: %lu", this->totalMilliHourly/10, this->totalMilliHourly/600, this->totalMilliHourly/36000);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "LastRateCost: %lu   CurrentRateCost: %lu", this->lastRateMilliCost, this->currentRateMilliCost);

  return mpaRet;
}


/////////////////////////////////////////////////////////////////////////////
/// Updates the model as time passes.
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode Model_updateTime(Model* this, struct tm *tick_time, TimeUnits units_changed) {
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;
  if (this->status == MODEL_STATE_STARTED) {
    if ( (mpaRet = Model_calculateCost(this)) != MPA_SUCCESS) return mpaRet;
  }
  return mpaRet;
}


/////////////////////////////////////////////////////////////////////////////
/// Calculates the latest meeting cost and provides a string describing the
/// cost of the meeting formatted in the chosen currency.
/// @param[in,out]  this  Pointer to Model; must be already allocated
/// @param[out]     fmtdCost   Pointer to a C-string; must be NULL on entry.
///       <em>Ownership is transferred to the caller, and the caller
///       is responsible for freeing this variable.</em>
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the Model pointer (this) is NULL.
///          MPA_INVALID_INPUT_ERR if fmtdCost is not NULL.
///          MPA_OUT_OF_MEMORY_ERR if memory could not be allocated.
///          MPA_STRING_ERR if fmtdCost string could not be written correctly
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode Model_getFmtdMeetingCost(const Model* this, char** fmtdCost) {
  MPA_RETURN_IF_NULL(this);
  if (*fmtdCost != NULL) { return MPA_INVALID_INPUT_ERR; }
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;

  if ( (*fmtdCost = malloc(sizeof(**fmtdCost) * FMTD_COST_SZ)) == NULL) goto freemem;

  long lret = 0;
  if ( (lret = snprintf(*fmtdCost, FMTD_COST_SZ, "%s%lu.%02lu", this->currencySymbol, this->totalMeetingMilliCost/1000, (this->totalMeetingMilliCost%1000)/10)) < 0) mpaRet = MPA_STRING_ERR;
  else if ((size_t)lret >= FMTD_COST_SZ) mpaRet = MPA_STRING_ERR;

  return mpaRet;

freemem:
  if ( (*fmtdCost) != NULL) { free(*fmtdCost);  (*fmtdCost) = NULL; }
  return MPA_OUT_OF_MEMORY_ERR;
}


/////////////////////////////////////////////////////////////////////////////
/// Gets the number of attendees in the model.
/// @param[in,out]  this  Pointer to Model; must be already allocated
/// @param[out]     numAttendees   Pointer to the attendee quantity variable
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the Model pointer (this) is NULL.
///          MPA_NULL_POINTER_ERR if the numAttendees pointer is NULL.
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode Model_getNumAttendees(const Model* this, uint16_t* numAttendees) {
  MPA_RETURN_IF_NULL(this);
  MPA_RETURN_IF_NULL(numAttendees);
  *numAttendees = this->numAttendees;
  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// Gets the status of the model (meeting started, stopped, or reset)
/// @param[in,out]  this  Pointer to Model; must be already allocated
/// @param[out]     status   Pointer to the status variable
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the Model pointer (this) is NULL.
///          MPA_NULL_POINTER_ERR if the status pointer is NULL.
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode Model_getStatus(const Model* this, Model_State* status) {
  MPA_RETURN_IF_NULL(this);
  MPA_RETURN_IF_NULL(status);
  *status = this->status;
  return MPA_SUCCESS;
}

