#include <pebble.h>

// Deactivate APP_LOG in this file.
//#undef APP_LOG
//#define APP_LOG(...)

#include "../data/Model.h"
#include "../misc.h"
#include "wndMain.h"
#include "wndSettings.h"
#include "lyrDigitime.h"

static Window* wndMain;
static MPA_Palette colors;
static ActionBarLayer* actionBar;
static GBitmap *bmpPersonPlus, *bmpPersonMinus, *bmpSettings, *bmpAttendIcon;
static TextLayer* lyrMeetingCost;
static TextLayer* lyrAttendees;
static BitmapLayer* lyrAttendIcon;

static wndMainHandlers myHandlers;


static const uint8_t relHtTime = 5;
static const uint8_t relHtCost = 30;
static const uint8_t relHtAttend = 63;

/////////////////////////////////////////////////////////////////////////////
/// Set our callback handlers.
/////////////////////////////////////////////////////////////////////////////
void wndMain_setHandlers(const wndMainHandlers wmh) {
  myHandlers = wmh;
}


/////////////////////////////////////////////////////////////////////////////
/// Sets the color palette.
/////////////////////////////////////////////////////////////////////////////
void wndMain_setPalette(const MPA_Palette pal) {
  colors = pal;
  if (wndMain) {
    window_set_background_color(wndMain, colors.normalBack);
  }
}


/////////////////////////////////////////////////////////////////////////////
/// Updates the displayed digital time
/////////////////////////////////////////////////////////////////////////////
void wndMain_updateTime(struct tm* in_time) {
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;
  struct tm* curr_time = NULL;

  if (in_time != NULL) {
    curr_time = in_time;
  } else {
    // Get a tm structure
    time_t temp = time(NULL);
    curr_time = localtime(&temp);
  }

  if (lyrDigitime_getLayer() != NULL && (mpaRet = lyrDigitime_updateTime(curr_time)) != MPA_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "MPA error: %s", MagPebApp_getErrMsg(mpaRet));
  }

}


/////////////////////////////////////////////////////////////////////////////
/// Updates the displayed information.
/////////////////////////////////////////////////////////////////////////////
void wndMain_updateData(const Model* model) {
  if (!wndMain) return;
  wndSettings_updateData(model);
  if (!window_is_loaded(wndMain)) return;
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;
  long lret = 0;

  // Update meeting cost
  char* meetingCost = NULL;
  static char meetingCostBuf[16];
  if ( (mpaRet = Model_getFmtdMeetingCost(model, &meetingCost)) != MPA_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Could not get string: %s", MagPebApp_getErrMsg(mpaRet));
  } else {
    if (!(lret = strxcpy(meetingCostBuf, sizeof(meetingCostBuf), meetingCost, "Meeting cost"))) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "String was not written correctly. Ret=%ld", lret);
    } else {
      text_layer_set_text(lyrMeetingCost, meetingCostBuf);
    }
  }
  if (meetingCost != NULL) { free(meetingCost);  meetingCost = NULL; }

  // Update attendees
  uint16_t numAttendees = 0;
  static char attendeesBuf[16];
  if ( (mpaRet = Model_getNumAttendees(model, &numAttendees)) != MPA_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Could not get value: %s", MagPebApp_getErrMsg(mpaRet));
  } else {
    if ( (lret = snprintf(attendeesBuf, sizeof(attendeesBuf), "%d", numAttendees)) < 0) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "String was not written correctly. Ret=%ld", lret);
    } else if ((size_t)lret >= sizeof(attendeesBuf)) {
      APP_LOG(APP_LOG_LEVEL_WARNING, "String was truncated. %ld characters required.", lret);
    } else {
      text_layer_set_text(lyrAttendees, attendeesBuf);
    }
  }

}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void wndMain_clickHandlerPersonPlus(ClickRecognizerRef recognizer, void *context) {
  if (!myHandlers.adjustAttendance) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Attempted operation on NULL pointer.");
  } else {
    (*myHandlers.adjustAttendance)(1, 0);
  }
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void wndMain_clickHandlerPersonMinus(ClickRecognizerRef recognizer, void *context) {
  if (!myHandlers.adjustAttendance) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Attempted operation on NULL pointer.");
  } else {
    (*myHandlers.adjustAttendance)(-1, 0);
  }
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void wndMain_clickHandlerSettings(ClickRecognizerRef recognizer, void *context) {
  wndSettings_push();
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void wndMain_clickConfigProvider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP,     (ClickHandler) wndMain_clickHandlerPersonPlus);
  window_single_click_subscribe(BUTTON_ID_DOWN,   (ClickHandler) wndMain_clickHandlerPersonMinus);
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) wndMain_clickHandlerSettings);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void wndMain_push() {
  if (!wndMain) APP_LOG(APP_LOG_LEVEL_ERROR, "Attempted operation on NULL pointer.");
  else window_stack_push(wndMain, true);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void wndMain_createPush() {
  wndMain_create();
  wndMain_push();
}


/* JRB NOTE: Watch apps don't get quick view notifications!
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static void unobstructedChange(AnimationProgress progress, void *context) {
  // Get the total available screen real-estate
  GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(wndMain));
  GRect frame;

  // Shift layers
  frame = layer_get_frame(text_layer_get_layer(lyrMeetingCost));
  frame.origin.y = rel2Pxl(bounds.size.h, relHtCost);
  layer_set_frame(text_layer_get_layer(lyrMeetingCost), frame);

  frame = layer_get_frame(bitmap_layer_get_layer(lyrAttendIcon));
  frame.origin.y = rel2Pxl(bounds.size.h, relHtAttend);
  layer_set_frame(bitmap_layer_get_layer(lyrAttendIcon), frame);

  frame = layer_get_frame(text_layer_get_layer(lyrAttendees));
  frame.origin.y = rel2Pxl(bounds.size.h, relHtAttend);
  layer_set_frame(text_layer_get_layer(lyrAttendees), frame);
}
*/


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static void wndMain_load(Window* window) {
  window_set_background_color(window, colors.normalBack);
  Layer* lyrRoot = window_get_root_layer(window);
  GRect bounds = layer_get_unobstructed_bounds(lyrRoot);

  // Digital Time
  lyrDigitime_create(GRect(0, RELH(bounds, relHtTime), bounds.size.w, 20), lyrRoot);
  lyrDigitime_stylize(GColorClear, colors.normalFore, GTextAlignmentCenter, fonts_get_system_font(FONT_KEY_GOTHIC_14));

  // Load icon bitmaps
  bmpPersonPlus =  gbitmap_create_with_resource(RESOURCE_ID_IMG_PERSON_PLUS);
  bmpPersonMinus = gbitmap_create_with_resource(RESOURCE_ID_IMG_PERSON_MINUS);
  bmpSettings =    gbitmap_create_with_resource(RESOURCE_ID_IMG_SETTINGS);
  bmpAttendIcon =  gbitmap_create_with_resource(RESOURCE_ID_IMG_ATTENDEES);

  // Initialize the action bar
  actionBar = action_bar_layer_create();
  action_bar_layer_set_click_config_provider(actionBar, wndMain_clickConfigProvider);
  action_bar_layer_add_to_window(actionBar, window);

  // Set the action bar icons
  action_bar_layer_set_icon_animated(actionBar, BUTTON_ID_UP,     bmpPersonPlus,  true);
  action_bar_layer_set_icon_animated(actionBar, BUTTON_ID_DOWN,   bmpPersonMinus, true);
  action_bar_layer_set_icon_animated(actionBar, BUTTON_ID_SELECT, bmpSettings,    true);

  // Meeting cost
  lyrMeetingCost = text_layer_create( GRect(0, RELH(bounds, relHtCost), bounds.size.w, 30) );
  textLayer_stylize(lyrMeetingCost, GColorClear, colors.normalFore, GTextAlignmentCenter, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text(lyrMeetingCost, "");
  layer_add_child(lyrRoot, text_layer_get_layer(lyrMeetingCost));

  // Number of attendees
  GRect rctNumAttend = GRect(RELW(bounds, 50), RELH(bounds, relHtAttend), RELW(bounds, 30), 30);
  lyrAttendees = text_layer_create(rctNumAttend);
  textLayer_stylize(lyrAttendees, GColorClear, colors.normalFore, GTextAlignmentLeft, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text(lyrAttendees, "0");
  layer_add_child(lyrRoot, text_layer_get_layer(lyrAttendees));

  // Attendees icon layer
  GRect rctAttendIcon = gbitmap_get_bounds(bmpAttendIcon);
//   rctAttendIcon.origin.y = RELH(bounds, relHtAttend);
//   grect_align(&rctAttendIcon, &rctNumAttend, GAlignTop, false);
  rctAttendIcon.origin.x = RELW(bounds, 45) - rctAttendIcon.size.w;
  rctAttendIcon.origin.y = rctNumAttend.origin.y + rctNumAttend.size.h - rctAttendIcon.size.h;
  lyrAttendIcon = bitmap_layer_create(rctAttendIcon);
  bitmap_layer_set_background_color(lyrAttendIcon, GColorClear);

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(lyrAttendIcon, bmpAttendIcon);
  bitmap_layer_set_compositing_mode(lyrAttendIcon, GCompOpSet);
  layer_add_child(lyrRoot, bitmap_layer_get_layer(lyrAttendIcon));

/* JRB NOTE: Watch apps don't get quick view notifications!
  UnobstructedAreaHandlers handlers = {
    .change = unobstructedChange
  };
  unobstructed_area_service_subscribe(handlers, NULL);
*/

}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static void wndMain_unload(Window* window) {
  // Destroy attendee icon layer
  bitmap_layer_destroy(lyrAttendIcon);

  // Destroy text layers
  text_layer_destroy(lyrAttendees);    lyrAttendees = NULL;
  text_layer_destroy(lyrMeetingCost);  lyrMeetingCost = NULL;

  // Destroy the ActionBarLayer
  action_bar_layer_destroy(actionBar);

  // Destroy the icon GBitmaps
  gbitmap_destroy(bmpPersonPlus);
  gbitmap_destroy(bmpPersonMinus);
  gbitmap_destroy(bmpSettings);
  gbitmap_destroy(bmpAttendIcon);

  lyrDigitime_destroy();
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void wndMain_create() {
  if (wndMain) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Attempting to re-create window before destroying.");
    return;
  }
  wndMain = window_create();

  INIT_MPA_PALETTE(colors);
  (void) colors;  // silence the unused variable warning

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(wndMain, (WindowHandlers) {
    .load =   wndMain_load,
    .unload = wndMain_unload
  });

  window_set_click_config_provider(wndMain, (ClickConfigProvider) wndMain_clickConfigProvider);

  wndMain_updateTime(NULL);
  wndSettings_create();
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void wndMain_destroy() {
  if (!wndMain) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Attempting to destroy a null pointer!");
  } else {
    wndSettings_destroy();
    window_destroy(wndMain);
    wndMain = NULL;
  }
}

