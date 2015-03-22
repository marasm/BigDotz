/*
 * main.c
 * Creates a Window, InverterLayer and animates it around the screen
 * from corner to corner. It uses the `.stopped` callback to schedule the
 * next stage of the animation.
 */

#include <pebble.h>

#define BOX_SIZE 20
#define TITLE_BAR_SIZE 16

#define ANIM_DURATION 75
#define ANIM_DELAY 5

//the below 2 lines disable logging
// #undef APP_LOG
// #define APP_LOG(...)

#ifdef PBL_COLOR
static const bool is_color_supported = 1;
#else
static const bool is_color_supported = 0;
#endif


static Window *s_main_window;

static const uint32_t DIGITS_NORM[] = {
  RESOURCE_ID_IMG_BIG_0_NORM,
  RESOURCE_ID_IMG_BIG_1_NORM,
  RESOURCE_ID_IMG_BIG_2_NORM,
  RESOURCE_ID_IMG_BIG_3_NORM,
  RESOURCE_ID_IMG_BIG_4_NORM,
  RESOURCE_ID_IMG_BIG_5_NORM,
  RESOURCE_ID_IMG_BIG_6_NORM,
  RESOURCE_ID_IMG_BIG_7_NORM,
  RESOURCE_ID_IMG_BIG_8_NORM,
  RESOURCE_ID_IMG_BIG_9_NORM
};
static const uint32_t DIGITS_DARK[] = {
  RESOURCE_ID_IMG_BIG_0_DARK,
  RESOURCE_ID_IMG_BIG_1_DARK,
  RESOURCE_ID_IMG_BIG_2_DARK,
  RESOURCE_ID_IMG_BIG_3_DARK,
  RESOURCE_ID_IMG_BIG_4_DARK,
  RESOURCE_ID_IMG_BIG_5_DARK,
  RESOURCE_ID_IMG_BIG_6_DARK,
  RESOURCE_ID_IMG_BIG_7_DARK,
  RESOURCE_ID_IMG_BIG_8_DARK,
  RESOURCE_ID_IMG_BIG_9_DARK
};
static const uint32_t DIGITS_DARKEST[] = {
  RESOURCE_ID_IMG_BIG_0_DARKEST,
  RESOURCE_ID_IMG_BIG_1_DARKEST,
  RESOURCE_ID_IMG_BIG_2_DARKEST,
  RESOURCE_ID_IMG_BIG_3_DARKEST,
  RESOURCE_ID_IMG_BIG_4_DARKEST,
  RESOURCE_ID_IMG_BIG_5_DARKEST,
  RESOURCE_ID_IMG_BIG_6_DARKEST,
  RESOURCE_ID_IMG_BIG_7_DARKEST,
  RESOURCE_ID_IMG_BIG_8_DARKEST,
  RESOURCE_ID_IMG_BIG_9_DARKEST
};
typedef struct segment segment;
struct segment{
  BitmapLayer *layer;
  GBitmap *bitmap;
  int cur_value;
  int new_value;
  int cur_anim_stage;
};

static segment TIME_SEGMENTS[4];
static BitmapLayer* TIME_BITMAP_LAYERS[4];
static PropertyAnimation* TIME_ANIMATIONS[4];


// Function prototype
static void next_animation(struct segment* cur_segment, PropertyAnimation* cur_animation);

static void anim_stopped_handler(Animation *animation, bool finished, void *context) {
  // Free the animation
  if(is_color_supported)
  {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Stopped Anim Handler: Basalt: will not destroy animation");
  }
  else
  {
    property_animation_destroy((PropertyAnimation*)animation);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Stopped Anim Handler: Animation Destroyed");
  }
  // Schedule the next one, unless the app is exiting
  if (finished) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Stopped Anim Handler: Kicking off next animation");
    next_animation(context, (PropertyAnimation*)animation);
  }
}

static void next_animation(struct segment* cur_segment, PropertyAnimation* cur_animation) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Next Animation");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "cur value = %i", cur_segment->cur_value);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "new value = %i", cur_segment->new_value);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "cur anim stage = %i", cur_segment->cur_anim_stage);

  if (!cur_segment->cur_value)
  {
    cur_segment->cur_value = 0;
  }

  if (cur_segment->cur_value == cur_segment->new_value)
  {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "cur value == new value -> no update");
    return;
  }

  if (cur_segment->bitmap)
  {
    gbitmap_destroy(cur_segment->bitmap);
  }

  switch(cur_segment->cur_anim_stage)
  {
    case 0:
      cur_segment->bitmap = gbitmap_create_with_resource(DIGITS_NORM[cur_segment->cur_value]);
      break;
    case 1:
      cur_segment->bitmap = gbitmap_create_with_resource(DIGITS_DARK[cur_segment->cur_value]);
      break;
    case 2:
      cur_segment->bitmap = gbitmap_create_with_resource(DIGITS_DARKEST[cur_segment->cur_value]);
      break;
    case 3:
      cur_segment->bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_BIG_DIGIT_BLANK);
      break;
    case 4:
      cur_segment->bitmap = gbitmap_create_with_resource(DIGITS_DARKEST[cur_segment->new_value]);
      break;
    case 5:
      cur_segment->bitmap = gbitmap_create_with_resource(DIGITS_DARK[cur_segment->new_value]);
      break;
    default://done with animation for the segment
      cur_segment->bitmap = gbitmap_create_with_resource(DIGITS_NORM[cur_segment->new_value]);
      cur_segment->cur_anim_stage = -1;
      cur_segment->cur_value = cur_segment->new_value;
      break;
  }
  bitmap_layer_set_bitmap(cur_segment->layer, cur_segment->bitmap);
  layer_mark_dirty(bitmap_layer_get_layer(cur_segment->layer));

  if (cur_segment->cur_anim_stage >= 0)
  {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "about to kick off animation");
    // Schedule the next animation
    cur_animation = property_animation_create_layer_frame(
      bitmap_layer_get_layer(cur_segment->layer),
      NULL,  //Not moving the frame so start and finish are
      NULL); //the same as the layer's current position
    animation_set_duration((Animation*)cur_animation, ANIM_DURATION);
    animation_set_delay((Animation*)cur_animation, ANIM_DELAY);
    animation_set_curve((Animation*)cur_animation, AnimationCurveEaseInOut);
    animation_set_handlers((Animation*)cur_animation, (AnimationHandlers) {
      .stopped = anim_stopped_handler
    }, cur_segment);
    animation_schedule((Animation*)cur_animation);
  }
  // Increment stage and wrap
  cur_segment->cur_anim_stage = (cur_segment->cur_anim_stage + 1);
}

static unsigned short get_display_hour(unsigned short hour) {
  if (clock_is_24h_style()) {
    return hour;
  }

  unsigned short display_hour = hour % 12;
  return display_hour ? display_hour : 12;
}

static void update_datetime_display(struct tm *current_time) {
  //************************TIME***************************************
  //Hours
  unsigned short display_hour = get_display_hour(current_time->tm_hour);
  TIME_SEGMENTS[0].layer=TIME_BITMAP_LAYERS[0];
  TIME_SEGMENTS[0].new_value=display_hour/10;
  TIME_SEGMENTS[0].cur_anim_stage=0;
  next_animation(&TIME_SEGMENTS[0], TIME_ANIMATIONS[0]);

  TIME_SEGMENTS[1].layer=TIME_BITMAP_LAYERS[1];
  TIME_SEGMENTS[1].new_value=display_hour%10;
  TIME_SEGMENTS[1].cur_anim_stage=0;
  next_animation(&TIME_SEGMENTS[1], TIME_ANIMATIONS[1]);

  //Minutes


  //***********************DATE****************************************
  //DOW


  //Month


  //Day

}

static void handle_time_tick(struct tm* tick_time, TimeUnits units_changed) {

  if(units_changed & MINUTE_UNIT) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Minute tick");
    update_datetime_display(tick_time);
  }

}

static void handle_battery(BatteryChargeState charge_state) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Handle Battery");
}


static void handle_bluetooth(bool connected) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Handle Bluetooth");
  // if (bt_bitmap)
  // {
  //   gbitmap_destroy(bt_bitmap);
  // }
  // if (connected)
  // {
  //   bt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_BT_ON);
  //
  //   if (!bt_connected)
  //   {
  //     bt_connected = 1;
  //     if (bt_vibrate)
  //     {
  //       vibes_double_pulse();
  //     }
  //   }
  // }
  // else
  // {
  //   bt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_ICON_CLEAR);
  //   if (bt_connected)
  //   {
  //     bt_connected = 0;
  //     if (bt_vibrate)
  //     {
  //       vibes_long_pulse();
  //     }
  //   }
  // }
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_bluetooth connected=%i", connected);
  // bitmap_layer_set_bitmap(bt_layer, bt_bitmap);
  // layer_mark_dirty(bitmap_layer_get_layer(bt_layer));
}


static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  //setup the digit coordinates
  TIME_BITMAP_LAYERS[0] = bitmap_layer_create(GRect(1,5,71,75));
  TIME_BITMAP_LAYERS[1] = bitmap_layer_create(GRect(72,5,71,75));
  TIME_BITMAP_LAYERS[2] = bitmap_layer_create(GRect(1,80,52,75));
  TIME_BITMAP_LAYERS[3] = bitmap_layer_create(GRect(60,80,52,75));

  //add all layers to window
  layer_add_child(window_layer, bitmap_layer_get_layer(TIME_BITMAP_LAYERS[0]));
  layer_add_child(window_layer, bitmap_layer_get_layer(TIME_BITMAP_LAYERS[1]));
  layer_add_child(window_layer, bitmap_layer_get_layer(TIME_BITMAP_LAYERS[2]));
  layer_add_child(window_layer, bitmap_layer_get_layer(TIME_BITMAP_LAYERS[3]));

  //TEST Code
  // TIME_SEGMENTS[0].layer=TIME_BITMAP_LAYERS[0];
  // TIME_SEGMENTS[0].bitmap=NULL;
  // TIME_SEGMENTS[0].cur_value=0;
  // TIME_SEGMENTS[0].new_value=1;
  // TIME_SEGMENTS[0].cur_anim_stage=0;
  // next_animation(&TIME_SEGMENTS[0], TIME_ANIMATIONS[0]);
  //
  // TIME_SEGMENTS[1].layer=TIME_BITMAP_LAYERS[1];
  // TIME_SEGMENTS[1].bitmap=NULL;
  // TIME_SEGMENTS[1].cur_value=4;
  // TIME_SEGMENTS[1].new_value=5;
  // TIME_SEGMENTS[1].cur_anim_stage=0;
  // next_animation(&TIME_SEGMENTS[1], TIME_ANIMATIONS[1]);

  // Avoids a blank screen on watch start.
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);
  update_datetime_display(tick_time);

  handle_battery(battery_state_service_peek());
  handle_bluetooth(bluetooth_connection_service_peek());
  tick_timer_service_subscribe(MINUTE_UNIT, &handle_time_tick);
  battery_state_service_subscribe(&handle_battery);
  bluetooth_connection_service_subscribe(&handle_bluetooth);
}

static void main_window_unload(Window *window) {
  // Destroy InverterLayer
  for (int i = 0; i < 4; i++)
  {
    bitmap_layer_destroy(TIME_BITMAP_LAYERS[i]);
  }
}

static void init(void) {
  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
  window_set_background_color(s_main_window, GColorBlack);
}

static void deinit(void) {
  // Stop any animation in progress
  animation_unschedule_all();

  // Destroy main Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
