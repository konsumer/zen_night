#include "pebble.h"

 void out_sent_handler(DictionaryIterator *sent, void *context) {
   // outgoing message was delivered
 }


 void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
   // outgoing message failed
 }


 void in_received_handler(DictionaryIterator *received, void *context) {
   // incoming message received
 }


 void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
 }
	
	
void accel_data_handler(AccelData *data, uint32_t num_samples) {
	// APP_LOG	(APP_LOG_LEVEL_DEBUG, "accel: %d, %d, %d", (int)data->x, (int)data->y, (int)data->z);
	
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	Tuplet x = TupletInteger(1, data->x);
	Tuplet y = TupletInteger(2, data->y);
	Tuplet z = TupletInteger(3, data->z);
	dict_write_tuplet(iter, &x);
	dict_write_tuplet(iter, &y);
	dict_write_tuplet(iter, &z);
	app_message_outbox_send();
}
	
static void init_zen_night(){
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);
	const uint32_t inbound_size = 64;
	const uint32_t outbound_size = 64;
	app_message_open(inbound_size, outbound_size);
	
	accel_data_service_subscribe(10, &accel_data_handler);
	accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
}

static void deinit_zen_night(){
	accel_data_service_unsubscribe();
}

// all below here is watchface look
// for now, just classio-battery-connection

Window *window;
TextLayer *time_layer; 
TextLayer *battery_layer;
TextLayer *connection_layer;

static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "100% charged";
  if (charge_state.is_charging) {
    snprintf(battery_text, sizeof(battery_text), "charging");
  } else {
    snprintf(battery_text, sizeof(battery_text), "%d%% charged", charge_state.charge_percent);
  }
  text_layer_set_text(battery_layer, battery_text);
}

static void handle_second_tick(struct tm* tick_time, TimeUnits units_changed) {
  static char time_text[] = "00:00:00"; // Needs to be static because it's used by the system later.
  strftime(time_text, sizeof(time_text), "%T", tick_time);
  text_layer_set_text(time_layer, time_text);
  handle_battery(battery_state_service_peek());
}

static void handle_bluetooth(bool connected) {
  text_layer_set_text(connection_layer, connected ? "connected" : "disconnected");
}

static void do_init(void) {
  window = window_create();
  window_stack_push(window, true);
  window_set_background_color(window, GColorBlack);

  Layer *root_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(root_layer);

  time_layer = text_layer_create(GRect(0, 40, frame.size.w /* width */, 34/* height */));
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);

  connection_layer = text_layer_create(GRect(0, 90, /* width */ frame.size.w, 34 /* height */));
  text_layer_set_text_color(connection_layer, GColorWhite);
  text_layer_set_background_color(connection_layer, GColorClear);
  text_layer_set_font(connection_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(connection_layer, GTextAlignmentCenter);
  text_layer_set_text(connection_layer, "connected");

  battery_layer = text_layer_create(GRect(0, 120, /* width */ frame.size.w, 34 /* height */));
  text_layer_set_text_color(battery_layer, GColorWhite);
  text_layer_set_background_color(battery_layer, GColorClear);
  text_layer_set_font(battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(battery_layer, GTextAlignmentCenter);
  text_layer_set_text(battery_layer, "100% charged");

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_second_tick(current_time, SECOND_UNIT);

  tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
  battery_state_service_subscribe(&handle_battery);
  bluetooth_connection_service_subscribe(&handle_bluetooth);

  layer_add_child(root_layer, text_layer_get_layer(time_layer));
  layer_add_child(root_layer, text_layer_get_layer(connection_layer));
  layer_add_child(root_layer, text_layer_get_layer(battery_layer));
}

static void do_deinit(void) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  text_layer_destroy(time_layer);
  text_layer_destroy(connection_layer);
  text_layer_destroy(battery_layer);
  window_destroy(window);
}

// end watch-face code

int main(void) {
  do_init();
  init_zen_night();
  app_event_loop();
  do_deinit();
  deinit_zen_night();
}
