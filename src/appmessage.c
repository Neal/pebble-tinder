#include <pebble.h>
#include "appmessage.h"
#include "libs/pebble-assist.h"
#include "tinder.h"

static void in_received_handler(DictionaryIterator *iter, void *context);
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context);

void appmessage_init(void) {
	app_message_register_inbox_received(in_received_handler);
	app_message_register_outbox_failed(out_failed_handler);
	app_message_open_max();
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
	tinder_in_received_handler(iter);
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
	tinder_out_failed_handler(failed, reason);
}
