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
  RESOURCE_ID_IMG_BIG_0_NORM,//Big Digits
  RESOURCE_ID_IMG_BIG_1_NORM,
  RESOURCE_ID_IMG_BIG_2_NORM,
  RESOURCE_ID_IMG_BIG_3_NORM,
  RESOURCE_ID_IMG_BIG_4_NORM,
  RESOURCE_ID_IMG_BIG_5_NORM,
  RESOURCE_ID_IMG_BIG_6_NORM,
  RESOURCE_ID_IMG_BIG_7_NORM,
  RESOURCE_ID_IMG_BIG_8_NORM,
  RESOURCE_ID_IMG_BIG_9_NORM,
  RESOURCE_ID_IMG_MED_0_NORM,//Medium Digits
  RESOURCE_ID_IMG_MED_1_NORM,
  RESOURCE_ID_IMG_MED_2_NORM,
  RESOURCE_ID_IMG_MED_3_NORM,
  RESOURCE_ID_IMG_MED_4_NORM,
  RESOURCE_ID_IMG_MED_5_NORM,
  RESOURCE_ID_IMG_MED_6_NORM,
  RESOURCE_ID_IMG_MED_7_NORM,
  RESOURCE_ID_IMG_MED_8_NORM,
  RESOURCE_ID_IMG_MED_9_NORM,
  RESOURCE_ID_IMG_SML_0_NORM,//SMALL Digits
  RESOURCE_ID_IMG_SML_1_NORM,
  RESOURCE_ID_IMG_SML_2_NORM,
  RESOURCE_ID_IMG_SML_3_NORM,
  RESOURCE_ID_IMG_SML_4_NORM,
  RESOURCE_ID_IMG_SML_5_NORM,
  RESOURCE_ID_IMG_SML_6_NORM,
  RESOURCE_ID_IMG_SML_7_NORM,
  RESOURCE_ID_IMG_SML_8_NORM,
  RESOURCE_ID_IMG_SML_9_NORM,
  RESOURCE_ID_IMG_DOW_SUN_NORM,//DOW
  RESOURCE_ID_IMG_DOW_MON_NORM,
  RESOURCE_ID_IMG_DOW_TUE_NORM,
  RESOURCE_ID_IMG_DOW_WED_NORM,
  RESOURCE_ID_IMG_DOW_THU_NORM,
  RESOURCE_ID_IMG_DOW_FRI_NORM,
  RESOURCE_ID_IMG_DOW_SAT_NORM
};
static const uint32_t DIGITS_DARK[] = {
  RESOURCE_ID_IMG_BIG_0_DARK,//Big Digits
  RESOURCE_ID_IMG_BIG_1_DARK,
  RESOURCE_ID_IMG_BIG_2_DARK,
  RESOURCE_ID_IMG_BIG_3_DARK,
  RESOURCE_ID_IMG_BIG_4_DARK,
  RESOURCE_ID_IMG_BIG_5_DARK,
  RESOURCE_ID_IMG_BIG_6_DARK,
  RESOURCE_ID_IMG_BIG_7_DARK,
  RESOURCE_ID_IMG_BIG_8_DARK,
  RESOURCE_ID_IMG_BIG_9_DARK,
  RESOURCE_ID_IMG_MED_0_DARK,//Medium Digits
  RESOURCE_ID_IMG_MED_1_DARK,
  RESOURCE_ID_IMG_MED_2_DARK,
  RESOURCE_ID_IMG_MED_3_DARK,
  RESOURCE_ID_IMG_MED_4_DARK,
  RESOURCE_ID_IMG_MED_5_DARK,
  RESOURCE_ID_IMG_MED_6_DARK,
  RESOURCE_ID_IMG_MED_7_DARK,
  RESOURCE_ID_IMG_MED_8_DARK,
  RESOURCE_ID_IMG_MED_9_DARK,
  RESOURCE_ID_IMG_SML_0_DARK,//Small Digits
  RESOURCE_ID_IMG_SML_1_DARK,
  RESOURCE_ID_IMG_SML_2_DARK,
  RESOURCE_ID_IMG_SML_3_DARK,
  RESOURCE_ID_IMG_SML_4_DARK,
  RESOURCE_ID_IMG_SML_5_DARK,
  RESOURCE_ID_IMG_SML_6_DARK,
  RESOURCE_ID_IMG_SML_7_DARK,
  RESOURCE_ID_IMG_SML_8_DARK,
  RESOURCE_ID_IMG_SML_9_DARK,
  RESOURCE_ID_IMG_DOW_SUN_DARK,//DOW
  RESOURCE_ID_IMG_DOW_MON_DARK,
  RESOURCE_ID_IMG_DOW_TUE_DARK,
  RESOURCE_ID_IMG_DOW_WED_DARK,
  RESOURCE_ID_IMG_DOW_THU_DARK,
  RESOURCE_ID_IMG_DOW_FRI_DARK,
  RESOURCE_ID_IMG_DOW_SAT_DARK
};
static const uint32_t DIGITS_DARKEST[] = {
  RESOURCE_ID_IMG_BIG_0_DARKEST,//Big Digits
  RESOURCE_ID_IMG_BIG_1_DARKEST,
  RESOURCE_ID_IMG_BIG_2_DARKEST,
  RESOURCE_ID_IMG_BIG_3_DARKEST,
  RESOURCE_ID_IMG_BIG_4_DARKEST,
  RESOURCE_ID_IMG_BIG_5_DARKEST,
  RESOURCE_ID_IMG_BIG_6_DARKEST,
  RESOURCE_ID_IMG_BIG_7_DARKEST,
  RESOURCE_ID_IMG_BIG_8_DARKEST,
  RESOURCE_ID_IMG_BIG_9_DARKEST,
  RESOURCE_ID_IMG_MED_0_DARKEST,//Medium Digits
  RESOURCE_ID_IMG_MED_1_DARKEST,
  RESOURCE_ID_IMG_MED_2_DARKEST,
  RESOURCE_ID_IMG_MED_3_DARKEST,
  RESOURCE_ID_IMG_MED_4_DARKEST,
  RESOURCE_ID_IMG_MED_5_DARKEST,
  RESOURCE_ID_IMG_MED_6_DARKEST,
  RESOURCE_ID_IMG_MED_7_DARKEST,
  RESOURCE_ID_IMG_MED_8_DARKEST,
  RESOURCE_ID_IMG_MED_9_DARKEST,
  RESOURCE_ID_IMG_SML_0_DARKEST,//Small Digits
  RESOURCE_ID_IMG_SML_1_DARKEST,
  RESOURCE_ID_IMG_SML_2_DARKEST,
  RESOURCE_ID_IMG_SML_3_DARKEST,
  RESOURCE_ID_IMG_SML_4_DARKEST,
  RESOURCE_ID_IMG_SML_5_DARKEST,
  RESOURCE_ID_IMG_SML_6_DARKEST,
  RESOURCE_ID_IMG_SML_7_DARKEST,
  RESOURCE_ID_IMG_SML_8_DARKEST,
  RESOURCE_ID_IMG_SML_9_DARKEST,
  RESOURCE_ID_IMG_DOW_SUN_DARKEST,//DOW
  RESOURCE_ID_IMG_DOW_MON_DARKEST,
  RESOURCE_ID_IMG_DOW_TUE_DARKEST,
  RESOURCE_ID_IMG_DOW_WED_DARKEST,
  RESOURCE_ID_IMG_DOW_THU_DARKEST,
  RESOURCE_ID_IMG_DOW_FRI_DARKEST,
  RESOURCE_ID_IMG_DOW_SAT_DARKEST
};

static const uint32_t BATTERY[] = {
  RESOURCE_ID_IMG_BAT_0,
  RESOURCE_ID_IMG_BAT_20,
  RESOURCE_ID_IMG_BAT_40,
  RESOURCE_ID_IMG_BAT_40,
  RESOURCE_ID_IMG_BAT_60,
  RESOURCE_ID_IMG_BAT_60,
  RESOURCE_ID_IMG_BAT_80,
  RESOURCE_ID_IMG_BAT_80,
  RESOURCE_ID_IMG_BAT_100,
  RESOURCE_ID_IMG_BAT_100,
};

typedef struct segment segment;
struct segment{
  BitmapLayer *layer;
  GBitmap *bitmap;
  int index_shift;
  int cur_value;
  int new_value;
  int cur_anim_stage;
};

static segment TIME_SEGMENTS[4];
static BitmapLayer* TIME_BITMAP_LAYERS[4];
static PropertyAnimation* TIME_ANIMATIONS[4];

static segment DATE_SEGMENTS[4];
static BitmapLayer* DATE_BITMAP_LAYERS[4];
static PropertyAnimation* DATE_ANIMATIONS[4];

static segment dow_segment;
static BitmapLayer* dow_bitmap_layer;
static PropertyAnimation* dow_animation;

static BitmapLayer* battery_bitmap_layer;
static GBitmap* battery_bitmap;

static bool bt_connected = 1;


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

  //if new == current and bitmap not null => skip
  if (cur_segment->cur_value == cur_segment->new_value && cur_segment->bitmap)
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
      cur_segment->bitmap = gbitmap_create_with_resource(DIGITS_NORM[cur_segment->cur_value + cur_segment->index_shift]);
      break;
    case 1:
      cur_segment->bitmap = gbitmap_create_with_resource(DIGITS_DARK[cur_segment->cur_value + cur_segment->index_shift]);
      break;
    case 2:
      cur_segment->bitmap = gbitmap_create_with_resource(DIGITS_DARKEST[cur_segment->cur_value + cur_segment->index_shift]);
      break;
    case 3:
      switch(cur_segment->index_shift)
      {
        case 0: //BIG
          cur_segment->bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_BIG_DIGIT_BLANK);
          break;
        case 10://MEDIUM
          cur_segment->bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_MED_DIGIT_BLANK);
          break;
        case 20://SMALL
          cur_segment->bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_SML_DIGIT_BLANK);
          break;
        case 30://Day of week
          //cur_segment->bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_DOW_BLANK);
          break;
        default:
          break;
      }
      break;
    case 4:
      cur_segment->bitmap = gbitmap_create_with_resource(DIGITS_DARKEST[cur_segment->new_value + cur_segment->index_shift]);
      break;
    case 5:
      cur_segment->bitmap = gbitmap_create_with_resource(DIGITS_DARK[cur_segment->new_value + cur_segment->index_shift]);
      break;
    default://done with animation for the segment
      cur_segment->bitmap = gbitmap_create_with_resource(DIGITS_NORM[cur_segment->new_value + cur_segment->index_shift]);
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
  TIME_SEGMENTS[0].index_shift=0;
  TIME_SEGMENTS[0].cur_anim_stage=0;
  next_animation(&TIME_SEGMENTS[0], TIME_ANIMATIONS[0]);

  TIME_SEGMENTS[1].layer=TIME_BITMAP_LAYERS[1];
  TIME_SEGMENTS[1].new_value=display_hour%10;
  TIME_SEGMENTS[1].index_shift=0;
  TIME_SEGMENTS[1].cur_anim_stage=0;
  next_animation(&TIME_SEGMENTS[1], TIME_ANIMATIONS[1]);

  //Minutes
  TIME_SEGMENTS[2].layer=TIME_BITMAP_LAYERS[2];
  TIME_SEGMENTS[2].new_value=current_time->tm_min/10;
  TIME_SEGMENTS[2].index_shift=10;
  TIME_SEGMENTS[2].cur_anim_stage=0;
  next_animation(&TIME_SEGMENTS[2], TIME_ANIMATIONS[2]);

  TIME_SEGMENTS[3].layer=TIME_BITMAP_LAYERS[3];
  TIME_SEGMENTS[3].new_value=current_time->tm_min%10;
  TIME_SEGMENTS[3].index_shift=10;
  TIME_SEGMENTS[3].cur_anim_stage=0;
  next_animation(&TIME_SEGMENTS[3], TIME_ANIMATIONS[3]);


  //***********************DATE****************************************
  //DOW
  dow_segment.layer=dow_bitmap_layer;
  dow_segment.new_value=current_time->tm_wday;
  dow_segment.index_shift=30;
  dow_segment.cur_anim_stage=0;
  next_animation(&dow_segment, dow_animation);


  //Month
  DATE_SEGMENTS[0].layer=DATE_BITMAP_LAYERS[0];
  DATE_SEGMENTS[0].new_value=(current_time->tm_mon + 1) / 10;
  DATE_SEGMENTS[0].index_shift=20;
  DATE_SEGMENTS[0].cur_anim_stage=0;
  next_animation(&DATE_SEGMENTS[0], DATE_ANIMATIONS[0]);

  DATE_SEGMENTS[1].layer=DATE_BITMAP_LAYERS[1];
  DATE_SEGMENTS[1].new_value=(current_time->tm_mon + 1) % 10;
  DATE_SEGMENTS[1].index_shift=20;
  DATE_SEGMENTS[1].cur_anim_stage=0;
  next_animation(&DATE_SEGMENTS[1], DATE_ANIMATIONS[1]);

  //Day
  DATE_SEGMENTS[2].layer=DATE_BITMAP_LAYERS[2];
  DATE_SEGMENTS[2].new_value=current_time->tm_mday / 10;
  DATE_SEGMENTS[2].index_shift=20;
  DATE_SEGMENTS[2].cur_anim_stage=0;
  next_animation(&DATE_SEGMENTS[2], DATE_ANIMATIONS[2]);

  DATE_SEGMENTS[3].layer=DATE_BITMAP_LAYERS[3];
  DATE_SEGMENTS[3].new_value=current_time->tm_mday % 10;
  DATE_SEGMENTS[3].index_shift=20;
  DATE_SEGMENTS[3].cur_anim_stage=0;
  next_animation(&DATE_SEGMENTS[3], DATE_ANIMATIONS[3]);

}

static void handle_time_tick(struct tm* tick_time, TimeUnits units_changed) {

  if(units_changed & MINUTE_UNIT) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Minute tick");
    update_datetime_display(tick_time);
  }

}

static void handle_battery(BatteryChargeState charge_state) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Handle Battery");
  if (battery_bitmap) {
    gbitmap_destroy(battery_bitmap);
  }
  battery_bitmap = gbitmap_create_with_resource(BATTERY[charge_state.charge_percent/10]);
  bitmap_layer_set_bitmap(battery_bitmap_layer, battery_bitmap);
  layer_mark_dirty(bitmap_layer_get_layer(battery_bitmap_layer));
}


static void handle_bluetooth(bool connected) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Handle Bluetooth");
  if (connected)
  {

    if (!bt_connected)
    {
      bt_connected = 1;
      vibes_double_pulse();
    }
  }
  else
  {
    if (bt_connected)
    {
      bt_connected = 0;
      vibes_long_pulse();
    }
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_bluetooth connected=%i", connected);
}


static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  //setup the digit coordinates
  //time
  TIME_BITMAP_LAYERS[0] = bitmap_layer_create(GRect(1,6,71,75));
  TIME_BITMAP_LAYERS[1] = bitmap_layer_create(GRect(72,6,71,75));
  TIME_BITMAP_LAYERS[2] = bitmap_layer_create(GRect(41,88,52,75));
  TIME_BITMAP_LAYERS[3] = bitmap_layer_create(GRect(90,88,52,75));
  //dow
  dow_bitmap_layer = bitmap_layer_create(GRect(8,96,33,15));
  //date
  DATE_BITMAP_LAYERS[0] = bitmap_layer_create(GRect(8,113,9,11));
  DATE_BITMAP_LAYERS[1] = bitmap_layer_create(GRect(17,113,9,11));
  DATE_BITMAP_LAYERS[2] = bitmap_layer_create(GRect(21,126,9,11));
  DATE_BITMAP_LAYERS[3] = bitmap_layer_create(GRect(30,126,9,11));

  //battery
  battery_bitmap_layer = bitmap_layer_create(GRect(8,143,35,11));

  //add all layers to window
  for (int i = 0; i < 4; i++){
    layer_add_child(window_layer, bitmap_layer_get_layer(TIME_BITMAP_LAYERS[i]));
  }
  for (int i = 0; i < 4; i++){
    layer_add_child(window_layer, bitmap_layer_get_layer(DATE_BITMAP_LAYERS[i]));
  }
  layer_add_child(window_layer, bitmap_layer_get_layer(dow_bitmap_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(battery_bitmap_layer));

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
  // Destroy
  for (int i = 0; i < 4; i++)
  {
    bitmap_layer_destroy(TIME_BITMAP_LAYERS[i]);
  }
  for (int i = 0; i < 4; i++)
  {
    bitmap_layer_destroy(DATE_BITMAP_LAYERS[i]);
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
