#include <pebble.h>

// Deactivate APP_LOG in this file.
#undef APP_LOG
#define APP_LOG(...)

#include "../misc.h"
#include "../data/Model.h"

#include "wndSettings.h"

#define NUM_MENU_SECTIONS 1

enum MenuItems {
  MNU_ITEM_TOGGLE_MTG = 0,
  MNU_ITEM_RESET_MTG,

#if 0
  MNU_ITEM_DEFAULT_PAY_RATE,
#endif

  NUM_MENU_ITEMS
};


#define STATUS_MSG_SZ 32

static Window* wndSettings;
static MPA_Palette colors;
static MenuLayer* lyrSettings;
static wndSettingsHandlers myHandlers;
static char statusMsg[STATUS_MSG_SZ];

static const Model* dataModel;
static Model_State mtgStatus = MODEL_STATE_NO_ATTENDEES;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static uint16_t wndSettings_get_num_sections_callback(MenuLayer* menu_layer, void* data) {
  return NUM_MENU_SECTIONS;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static uint16_t wndSettings_get_num_rows_callback(MenuLayer* menu_layer, uint16_t section_index, void* data) {
  switch (section_index) {
    case 0:
      return NUM_MENU_ITEMS;
    default:
      return 0;
  }
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static void wndSettings_draw_row_callback(GContext* ctx, const Layer* cell_layer, MenuIndex* cell_index, void* data) {

  switch (cell_index->section) {
    case 0: {
      switch (cell_index->row) {

        case MNU_ITEM_TOGGLE_MTG:
          menu_cell_basic_draw(ctx, cell_layer, statusMsg, NULL, NULL);
          break;

        case MNU_ITEM_RESET_MTG:
          menu_cell_basic_draw(ctx, cell_layer, "Reset Meeting", NULL, NULL);
          break;

#if 0
        case MNU_ITEM_ACKNOWLEDGEMENTS:
          menu_cell_basic_draw(ctx, cell_layer, "Acknowledgements", NULL, NULL);
          break;
#endif
      }
      break;
    }
  }
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static void wndSettings_select_callback(MenuLayer* menu_layer, MenuIndex* cell_index, void* data) {
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;

  switch (cell_index->row) {
    case MNU_ITEM_TOGGLE_MTG: {
      if (!myHandlers.toggleMeeting) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Attempted operation on NULL pointer.");
      } else if (mtgStatus == MODEL_STATE_STARTED || mtgStatus == MODEL_STATE_STOPPED) {
        (*myHandlers.toggleMeeting)();
      }

      break;
    }
    case MNU_ITEM_RESET_MTG: {
      if (!myHandlers.resetMeeting) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Attempted operation on NULL pointer.");
      } else {
        (*myHandlers.resetMeeting)();
      }

      break;
    }
#if 0
    case MNU_ITEM_ACKNOWLEDGEMENTS: {
      break;
    }
#endif
  }
}


#if defined(PBL_ROUND)
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static int16_t wndSettings_get_cell_height_callback(MenuLayer* menu_layer, MenuIndex* cell_index, void* callback_context) {
  Layer* lyrRoot = window_get_root_layer(wndSettings);
  GRect bounds = layer_get_bounds(lyrRoot);
  return (int)(bounds.size.h / 3);
}
#endif


/////////////////////////////////////////////////////////////////////////////
/// Sets our callback handlers.
/////////////////////////////////////////////////////////////////////////////
void wndSettings_setHandlers(const wndSettingsHandlers wmmh) {
  myHandlers = wmmh;
}


/////////////////////////////////////////////////////////////////////////////
/// Sets the color palette.
/////////////////////////////////////////////////////////////////////////////
void wndSettings_setPalette(const MPA_Palette pal) {
  colors = pal;
  if (wndSettings) {
    window_set_background_color(wndSettings, colors.normalBack);
  }
  if (lyrSettings) {
    menu_layer_set_normal_colors(lyrSettings, colors.normalBack, colors.normalFore);
    menu_layer_set_highlight_colors(lyrSettings, colors.highltBack, colors.highltFore);
  }
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void wndSettings_updateData(const Model* model) {
  dataModel = model;
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;

  if ( (mpaRet = Model_getStatus(dataModel, &mtgStatus)) != MPA_SUCCESS) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error encountered: %s", MagPebApp_getErrMsg(mpaRet));
  }

  switch (mtgStatus) {
    case MODEL_STATE_STOPPED: {
      if (!strxcpy(statusMsg, STATUS_MSG_SZ, "Resume Meeting", NULL)) { return; }
      break;
    }
    case MODEL_STATE_STARTED: {
      if (!strxcpy(statusMsg, STATUS_MSG_SZ, "Pause Meeting", NULL)) { return; }
      break;
    }
    default: {
      if (!strxcpy(statusMsg, STATUS_MSG_SZ, "No Attendees", NULL)) { return; }
      break;
    }
  }

  if (lyrSettings) menu_layer_reload_data(lyrSettings);

}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static void wndSettings_load(Window* window) {
  window_set_background_color(window, colors.normalBack);
  Layer* lyrRoot = window_get_root_layer(window);
  GRect bounds = layer_get_unobstructed_bounds(lyrRoot);

  if (lyrSettings) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Attempting to re-create layer before destroying.");
    return;
  }

  lyrSettings = menu_layer_create(bounds);
  menu_layer_set_callbacks(lyrSettings, NULL, (MenuLayerCallbacks) {
    .get_num_sections = wndSettings_get_num_sections_callback,
    .get_num_rows = wndSettings_get_num_rows_callback,
    .draw_row = wndSettings_draw_row_callback,
    .select_click = wndSettings_select_callback,
    .get_cell_height = PBL_IF_ROUND_ELSE(wndSettings_get_cell_height_callback, NULL)
  });

  menu_layer_set_normal_colors(lyrSettings, colors.normalBack, colors.normalFore);
  menu_layer_set_highlight_colors(lyrSettings, colors.highltBack, colors.highltFore);

  menu_layer_set_click_config_onto_window(lyrSettings, window);
  layer_add_child(lyrRoot, menu_layer_get_layer(lyrSettings));
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static void wndSettings_unload(Window* window) {
  if (!lyrSettings) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Attempting to destroy a null pointer!");
  } else {
    menu_layer_destroy(lyrSettings);   lyrSettings = NULL;
  }
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void wndSettings_push() {
  if (!wndSettings) APP_LOG(APP_LOG_LEVEL_ERROR, "Attempted operation on NULL pointer.");
  else window_stack_push(wndSettings, true);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void wndSettings_createPush() {
  wndSettings_create();
  wndSettings_push();
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void wndSettings_create() {
  if (wndSettings) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Attempting to re-create window before destroying.");
    return;
  }
  wndSettings = window_create();

  INIT_MPA_PALETTE(colors);
  (void) colors;  // silence the unused variable warning

  if (!strxcpy(statusMsg, STATUS_MSG_SZ, "Please Wait...", NULL)) { return; }

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(wndSettings, (WindowHandlers) {
    .load =   wndSettings_load,
    .unload = wndSettings_unload
  });

}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void wndSettings_destroy() {
  if (!wndSettings) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Attempting to destroy a null pointer!");
  } else {
    window_destroy(wndSettings);
    wndSettings = NULL;
  }
}
