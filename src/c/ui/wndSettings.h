#pragma once
#include <pebble.h>

#include "../data/Model.h"

// ToggleMeetingHandler is a pointer to a function that takes no
// parameter and returns nothing.
typedef void (*ToggleMeetingHandler)(void);

// ResetMeetingHandler is a pointer to a function that takes no
// parameter and returns nothing.
typedef void (*ResetMeetingHandler)(void);

// wndSettingsHandlers is a struct that contains the values of the handlers.
typedef struct wndSettingsHandlers {
  ToggleMeetingHandler toggleMeeting;   ///< Function that this View calls to request actions from its Controller.
  ResetMeetingHandler resetMeeting;     ///< Function that this View calls to request actions from its Controller.
} wndSettingsHandlers;


void wndSettings_setHandlers(const wndSettingsHandlers);
void wndSettings_setPalette(const MPA_Palette);

void wndSettings_updateClock(struct tm*);
void wndSettings_updateData(const Model*);
void wndSettings_create();
void wndSettings_push();
void wndSettings_createPush();
void wndSettings_destroy();
