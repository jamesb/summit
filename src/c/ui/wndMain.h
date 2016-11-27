#pragma once

#include "../data/Model.h"

// AdjustAttendeesHandler is a pointer to a function that takes
// two integer parameters and returns nothing.
typedef void (*AdjustAttendanceHandler)(const int16_t, const uint16_t);

// wndMainHandlers is a struct that contains the values of the handlers.
typedef struct wndMainMenuHandlers {
  AdjustAttendanceHandler adjustAttendance;   ///< Function that this View calls to request data changes of its Controller.
} wndMainHandlers;


void wndMain_setHandlers(const wndMainHandlers);
void wndMain_setPalette(const MPA_Palette);


void wndMain_updateTime(struct tm*);
void wndMain_updateData(const Model*);
void wndMain_create();
void wndMain_push();
void wndMain_createPush();
void wndMain_destroy();
