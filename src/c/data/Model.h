#pragma once

#include <pebble.h>
#include "../libs/magpebapp.h"


#define FMTD_COST_SZ 16


// State definitions
typedef enum Model_State {
  MODEL_STATE_NO_ATTENDEES = 0,
  MODEL_STATE_STARTED,
  MODEL_STATE_STOPPED,

  LAST_MODEL_STATE
} Model_State;



// Model struct typedef
typedef struct Model Model;


Model* Model_create();
MagPebApp_ErrCode Model_destroy(Model* this);

MagPebApp_ErrCode Model_reset(Model* this);
MagPebApp_ErrCode Model_setCurrencySymbol(Model* this, const char*);
MagPebApp_ErrCode Model_setDefaultMilliHourly(Model* this, const uint32_t);
MagPebApp_ErrCode Model_adjustAttendance(Model* this, const int16_t, const uint32_t);
MagPebApp_ErrCode Model_startMeeting(Model* this);
MagPebApp_ErrCode Model_stopMeeting(Model* this);

MagPebApp_ErrCode Model_getFmtdMeetingCost(const Model* this, char**);
MagPebApp_ErrCode Model_getNumAttendees(const Model* this, uint16_t*);

MagPebApp_ErrCode Model_getCurrencySymbol(const Model* this, char**);
MagPebApp_ErrCode Model_getDefaultMilliHourly(Model* this, uint32_t*);
MagPebApp_ErrCode Model_getStatus(const Model* this, Model_State*);

MagPebApp_ErrCode Model_updateTime(Model* this, struct tm *tick_time, TimeUnits units_changed);
