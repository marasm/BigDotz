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

static Window *s_main_window;

static const uint32_t DIGITS_NORM[] = {
  RESOURCE_ID_IMG_BIG_3_NORM,
  RESOURCE_ID_IMG_BIG_3_NORM,
  RESOURCE_ID_IMG_BIG_2_NORM,
  RESOURCE_ID_IMG_BIG_3_NORM,
  RESOURCE_ID_IMG_BIG_3_NORM,
  RESOURCE_ID_IMG_BIG_3_NORM,
  RESOURCE_ID_IMG_BIG_3_NORM,
  RESOURCE_ID_IMG_BIG_3_NORM,
  RESOURCE_ID_IMG_BIG_3_NORM,
  RESOURCE_ID_IMG_BIG_3_NORM
};
static const uint32_t DIGITS_DARK[] = {
  RESOURCE_ID_IMG_BIG_3_DARK,
  RESOURCE_ID_IMG_BIG_3_DARK,
  RESOURCE_ID_IMG_BIG_2_DARK,
  RESOURCE_ID_IMG_BIG_3_DARK,
  RESOURCE_ID_IMG_BIG_3_DARK,
  RESOURCE_ID_IMG_BIG_3_DARK,
  RESOURCE_ID_IMG_BIG_3_DARK,
  RESOURCE_ID_IMG_BIG_3_DARK,
  RESOURCE_ID_IMG_BIG_3_DARK,
  RESOURCE_ID_IMG_BIG_3_DARK
};
static const uint32_t DIGITS_DARKEST[] = {
  RESOURCE_ID_IMG_BIG_3_DARKEST,
  RESOURCE_ID_IMG_BIG_3_DARKEST,
  RESOURCE_ID_IMG_BIG_2_DARKEST,
  RESOURCE_ID_IMG_BIG_3_DARKEST,
  RESOURCE_ID_IMG_BIG_3_DARKEST,
  RESOURCE_ID_IMG_BIG_3_DARKEST,
  RESOURCE_ID_IMG_BIG_3_DARKEST,
  RESOURCE_ID_IMG_BIG_3_DARKEST,
  RESOURCE_ID_IMG_BIG_3_DARKEST,
  RESOURCE_ID_IMG_BIG_3_DARKEST
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
static void next_animation(struct segment* cur_digit, PropertyAnimation* cur_animation);

static void anim_stopped_handler(Animation *animation, bool finished, void *context) {
  // Free the animation
  property_animation_destroy((PropertyAnimation*)animation);

  // Schedule the next one, unless the app is exiting
  if (finished) {
    next_animation(context, (PropertyAnimation*)animation);
  }
}

static void next_animation(struct segment* cur_digit, PropertyAnimation* cur_animation) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Next Animation");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "cur value = %i", cur_digit->cur_value);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "new value = %i", cur_digit->new_value);

  if (cur_digit->cur_value == cur_digit->new_value)
  {
    return;
  }

  if (cur_digit->bitmap)
  {
    gbitmap_destroy(cur_digit->bitmap);
  }

  switch(cur_digit->cur_anim_stage)
  {
    case 0:
      cur_digit->bitmap = gbitmap_create_with_resource(DIGITS_NORM[cur_digit->cur_value]);
      break;
    case 1:
      cur_digit->bitmap = gbitmap_create_with_resource(DIGITS_DARK[cur_digit->cur_value]);
      break;
    case 2:
      cur_digit->bitmap = gbitmap_create_with_resource(DIGITS_DARKEST[cur_digit->cur_value]);
      break;
    case 3:
      cur_digit->bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_BIG_DIGIT_BLANK);
      break;
    case 4:
      cur_digit->bitmap = gbitmap_create_with_resource(DIGITS_DARKEST[cur_digit->new_value]);
      break;
    case 5:
      cur_digit->bitmap = gbitmap_create_with_resource(DIGITS_DARK[cur_digit->new_value]);
      break;
    default:
      cur_digit->bitmap = gbitmap_create_with_resource(DIGITS_NORM[cur_digit->new_value]);
      cur_digit->cur_anim_stage = -1;
      break;
  }
  bitmap_layer_set_bitmap(cur_digit->layer, cur_digit->bitmap);
  layer_mark_dirty(bitmap_layer_get_layer(cur_digit->layer));

  if (cur_digit->cur_anim_stage >= 0)
  {
    GRect coordinates = layer_get_frame((Layer*)cur_digit->layer);
    // Schedule the next animation
    cur_animation = property_animation_create_layer_frame(
      bitmap_layer_get_layer(cur_digit->layer),
      &coordinates,
      &coordinates);
    animation_set_duration((Animation*)cur_animation, ANIM_DURATION);
    animation_set_delay((Animation*)cur_animation, ANIM_DELAY);
    animation_set_curve((Animation*)cur_animation, AnimationCurveEaseInOut);
    animation_set_handlers((Animation*)cur_animation, (AnimationHandlers) {
      .stopped = anim_stopped_handler
    }, cur_digit);
    animation_schedule((Animation*)cur_animation);
  }

  // Increment stage and wrap
  cur_digit->cur_anim_stage = (cur_digit->cur_anim_stage + 1);
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


  TIME_SEGMENTS[0].layer=TIME_BITMAP_LAYERS[0];
  TIME_SEGMENTS[0].bitmap=NULL;
  TIME_SEGMENTS[0].cur_value=2;
  TIME_SEGMENTS[0].new_value=3;
  TIME_SEGMENTS[0].cur_anim_stage=0;
  next_animation(&TIME_SEGMENTS[0], TIME_ANIMATIONS[0]);

  TIME_SEGMENTS[1].layer=TIME_BITMAP_LAYERS[1];
  TIME_SEGMENTS[1].bitmap=NULL;
  TIME_SEGMENTS[1].cur_value=2;
  TIME_SEGMENTS[1].new_value=3;
  TIME_SEGMENTS[1].cur_anim_stage=0;
  next_animation(&TIME_SEGMENTS[1], TIME_ANIMATIONS[1]);
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
