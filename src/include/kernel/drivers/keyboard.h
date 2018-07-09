#pragma once

#include <kernel/driver.h>

enum KeyboardSpecialKeys {
	NONE = 0,
	CONTROL,
	META,
	LEFT_SHIFT,
	RIGHT_SHIFT,
	CAPS_LOCK,

	NUM_LOCK,
	SCROLL_LOCK,
	HOME,

	ARROW_UP,
	ARROW_LEFT,
	ARROW_RIGHT,
	ARROW_DOWN,

	PAGE_UP,
	PAGE_DOWN,

	INSERT,
	DELETE,

	END,

	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
};

typedef struct {
	unsigned char symbol;
	uint8_t special_keys_data;
	bool press;
} KeyboardEvent;

typedef void (*KeyboardCallback)(KeyboardEvent event);

typedef struct {
	void* data;
	const char* name;
} KeyboardTable;

typedef struct {
	void (*keyboard_set_table)(KeyboardTable table);

	uint (*keyboard_get_avalible_keyboard_tables_count)();
	KeyboardTable* (*keyboard_get_avalible_keyboard_tables)();
	
	int (*keyboard_add_callback)(KeyboardCallback callback);
	int (*keyboard_remove_callback)(KeyboardCallback callback);
} KeyboardDriverFunctions;

typedef struct {
	GeneralDriverData general_data;
	KeyboardDriverFunctions functions;
} KeyboardDriver;

/**
	Try to install simplest driver for current system.
	If succes - return true.
**/
bool keyboard_install_default_driver();

/**
	Return true if driver is successfully setted.
	If some problems - return false.
**/
bool keyboard_set_driver(KeyboardDriver driver);

/**
	Try to recieve current terminal driver.
	Return true if it is success. If no terminal - return false.
	If driver == NULL, no assignment.
**/
bool keyboard_get_current_driver(KeyboardDriver* driver);

void keyboard_set_table(KeyboardTable table);

uint keyboard_get_avalible_keyboard_tables_count();
KeyboardTable* keyboard_get_avalible_keyboard_tables();

/**
	Theese callbacks work in interrupts, and they must be very small, simple and fast.
	Return value is 1, if adding/removing is success. Else - false
**/
int keyboard_add_callback(KeyboardCallback callback);
int keyboard_remove_callback(KeyboardCallback callback);

/**
	Get first unread event from event buffer.
	If no events - wait.
**/
KeyboardEvent keyboard_read_input_buffer();