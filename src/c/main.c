#include <pebble.h>

// Deactivate APP_LOG in this file.
#undef APP_LOG
#define APP_LOG(...)

#include "comm.h"
#include "misc.h"
#include "ui/wndMain.h"
#include "ui/wndSettings.h"


/////////////////////////////////////////////////////////////////////////////
/// Used for the creation of all Pebble SDK elements.
/////////////////////////////////////////////////////////////////////////////
static void init() {
  MPA_Palette colors = (MPA_Palette) {
    .normalBack = COLOR_FALLBACK(GColorMalachite, GColorBlack),
    .normalFore = COLOR_FALLBACK(GColorBlack, GColorWhite),
    .highltBack = COLOR_FALLBACK(GColorMintGreen, GColorWhite),
    .highltFore = GColorBlack
  };

  wndMain_createPush();
  wndMain_setPalette(colors);
  wndSettings_setPalette(colors);
  wndMain_setHandlers( (wndMainHandlers) {
    .adjustAttendance = comm_adjustAttendance
  });

  wndSettings_setHandlers( (wndSettingsHandlers) {
    .toggleMeeting = comm_toggleMeeting,
    .resetMeeting = comm_resetMeeting
  });

  comm_setHandlers( (CommHandlers) {
    .updateViewTime = wndMain_updateTime,
    .updateViewData = wndMain_updateData
  });
  comm_open();

  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, comm_tickHandler);
}


/////////////////////////////////////////////////////////////////////////////
/// Used for the destruction of all Pebble SDK elements.
/////////////////////////////////////////////////////////////////////////////
static void deinit() {
  wndMain_destroy();

  comm_close();
}


/////////////////////////////////////////////////////////////////////////////
/// Standard Pebble main function
/////////////////////////////////////////////////////////////////////////////
int main(void) {
  init();
  app_event_loop();
  deinit();
}
