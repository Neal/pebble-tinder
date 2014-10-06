#include <pebble.h>
#include "tinder.h"

int main(void) {
	tinder_init();
	app_event_loop();
	tinder_deinit();
}
