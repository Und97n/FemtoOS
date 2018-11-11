#include <kernel/drivers/keyboard.h>
#include "keyboard/ps2.h"

static bool has_driver = false;
static KeyboardDriver current_driver;

#define INPUT_BUFFER_SIZE 256

static volatile uint32_t input_buffer_pos = 0;
static volatile KeyboardEvent input_buffer[INPUT_BUFFER_SIZE];

// For input buffer filling
static void keyboard_callback(KeyboardEvent e);

bool keyboard_install_default_driver() {
	ps2_keyboard.general_data.init();

	if(ps2_keyboard.functions.keyboard_get_avalible_keyboard_tables_count() < 1) {
		return false;
	}

	ps2_keyboard.functions.keyboard_set_table(ps2_keyboard.functions.keyboard_get_avalible_keyboard_tables()[0]);
	
	return keyboard_set_driver(ps2_keyboard);
}

bool keyboard_set_driver(KeyboardDriver driver) {
	if(!driver.general_data.diagnoze(NULL)) {
		if(has_driver) {
			current_driver.functions.keyboard_remove_callback(keyboard_callback);
		}

		current_driver = driver;
		has_driver = true;

		current_driver.functions.keyboard_add_callback(keyboard_callback);

		return true;
	}

	return false;
}

bool keyboard_get_current_driver(KeyboardDriver* driver) {
	if(has_driver) {
		if(driver)  {
			*driver = current_driver;
		}

		return true;
	} else {
		return false;
	}
}

void keyboard_set_table(KeyboardTable table) {
	if(has_driver) {
		current_driver.functions.keyboard_set_table(table);
	} else {

	}
}

uint keyboard_get_avalible_keyboard_tables_count() {
	if(has_driver) {
		return current_driver.functions.keyboard_get_avalible_keyboard_tables_count();
	} else {
		return 0;
	}
}

KeyboardTable* keyboard_get_avalible_keyboard_tables() {
	if(has_driver) {
		return current_driver.functions.keyboard_get_avalible_keyboard_tables();
	} else {
		return NULL;
	}
}


int keyboard_add_callback(KeyboardCallback callback) {
	if(has_driver) {
		return current_driver.functions.keyboard_add_callback(callback);
	} else {
		return false;
	}
}

int keyboard_remove_callback(KeyboardCallback callback) {
	if(has_driver) {
		return current_driver.functions.keyboard_remove_callback(callback);
	} else {
		return false;
	}
}

static volatile bool no_input = false;

static void keyboard_callback(KeyboardEvent e) {
	// TODO remove
	if(no_input) {
		// If now we update buffer - ignore input.
		return;
	}

	if(input_buffer_pos < INPUT_BUFFER_SIZE) {
		input_buffer[input_buffer_pos++] = e;
	} else {
		// Do something?
	}
}

KeyboardEvent keyboard_read_input_buffer() {
	// Wait part
	// TODO make something normal
	while(!input_buffer_pos);

	// For safety.
	no_input = true;

	KeyboardEvent ret = input_buffer[0];

	// Scroll queue
	for(uint i = 1; i < input_buffer_pos; ++i) {
		input_buffer[i - 1] = input_buffer[i];
	}

	--input_buffer_pos;

	no_input = false;

	return ret;
}
