#include "ps2.h"

#include "../../idt.h"

#include "../../asm_utils.h"

#include <string.h>
#include <stdio.h>

#include <kernel/control.h>

static void driver_init();
static void driver_cleanup();
static int driver_diagnoze(char** out_message);
static int driver_is_supported();

static void set_table(KeyboardTable table);
static uint get_avalible_keyboard_tables_count();
static KeyboardTable* get_avalible_keyboard_tables();

static int add_callback(KeyboardCallback callback);
static int remove_callback(KeyboardCallback callback);

typedef struct {
	unsigned char defChar;
	unsigned char onShiftChar;
	uint8_t specialData;
} KEY_ENTRY;

const KEY_ENTRY US_KEYBOARD_DATA[128] = {
	{0, 0, 0},
	{27, 27, 0},

	{'1', '!', 0},
	{'2', '@', 0},
	{'3', '#', 0},
	{'4', '$', 0},
	{'5', '%', 0},
	{'6', '^', 0},
	{'7', '&', 0},
	{'8', '*', 0},
	{'9', '(', 0},
	{'0', ')', 0},
	{'-', '_', 0},
	{'=', '+', 0},


	{'\b', '\b', 0},
	{'\t', '\t', 0},

	{'q', 'Q', 0},
	{'w', 'W', 0},
	{'e', 'E', 0},
	{'r', 'R', 0},
	{'t', 'T', 0},
	{'y', 'Y', 0},
	{'u', 'U', 0},
	{'i', 'I', 0},
	{'o', 'O', 0},
	{'p', 'P', 0},
	{'[', '{', 0},
	{']', '}', 0},

	{'\n', '\n', 0},
	{0, 0, CONTROL},

	{'a', 'A', 0},
	{'s', 'S', 0},
	{'d', 'D', 0},
	{'f', 'F', 0},
	{'g', 'G', 0},
	{'h', 'H', 0},
	{'j', 'J', 0},
	{'k', 'K', 0},
	{'l', 'L', 0},
	{';', ':', 0},
	{'\'', '"', 0},
	{'`', '~', 0},

	{0, 0, LEFT_SHIFT},

	{'\\', '|', 0},
	{'z', 'Z', 0},
	{'x', 'x', 0},
	{'c', 'C', 0},
	{'v', 'V', 0},
	{'b', 'B', 0},
	{'n', 'N', 0},
	{'m', 'M', 0},
	{',', '<', 0},
	{'.', '>', 0},
	{'/', '?', 0},
	
	{0, 0, RIGHT_SHIFT},

	{'*', '*', 0},

	{0, 0, META},

	{' ', ' ', 0},

	{0, 0, CAPS_LOCK},

	{0, 0, F1},
	{0, 0, F2},
	{0, 0, F3},
	{0, 0, F4},
	{0, 0, F5},
	{0, 0, F6},
	{0, 0, F7},
	{0, 0, F8},
	{0, 0, F9},
	{0, 0, F10},

	{0, 0, NUM_LOCK},
	{0, 0, SCROLL_LOCK},
	{0, 0, HOME},
	{0, 0, ARROW_UP},
	{0, 0, PAGE_UP},
	{'-', '-', 0},
	{0, 0, ARROW_LEFT},
	{0, 0, 0},
	{0, 0, ARROW_RIGHT},
	{'+', '+', 0},
	{0, 0, END},
	{0, 0, ARROW_DOWN},
	{0, 0, PAGE_DOWN},
	{0, 0, INSERT},
	{0, 0, DELETE},

	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},

	{0, 0, F11},
	{0, 0, F12},

	/* All other keys are undefined */
	{0, 0, 0},
};

KeyboardTable avalible_keyboard_tables[1] = {
	{(void*)US_KEYBOARD_DATA, "US keyboard(default)."},
};

KeyboardTable currentKeyboard = {NULL, "NONE"};

#define MAX_CALLBACKS_COUNT 256
#define IRQ_NUMBER 1

KeyboardCallback callbacks[MAX_CALLBACKS_COUNT];

bool specialKeysStates[64] = {};

static void keyboard_handler(Registers *r);

static void driver_init() {
	irq_install_handler(IRQ_NUMBER, keyboard_handler);

	// For safety
	memset(callbacks, 0, MAX_CALLBACKS_COUNT * sizeof(KeyboardCallback));
	memset(specialKeysStates, false, 64 * sizeof(bool));
}

static void driver_cleanup() {
	irq_install_handler(IRQ_NUMBER, NULL);

	currentKeyboard = (KeyboardTable){NULL, "NONE"};

	memset(callbacks, 0, MAX_CALLBACKS_COUNT * sizeof(KeyboardCallback));
	memset(specialKeysStates, false, 64);
}

static int driver_diagnoze(char** out_message) {
	// TODO

	if(out_message) {
		*out_message = "All is normal.";
	}

	return 0;
}

static int driver_is_supported() {
	// WHAT?
	return true;
}

static void set_table(KeyboardTable table) {
	currentKeyboard = table;
}

static uint get_avalible_keyboard_tables_count() {
	return sizeof(avalible_keyboard_tables) / sizeof(KeyboardTable);
}
static KeyboardTable* get_avalible_keyboard_tables() {
	return avalible_keyboard_tables;
}

static int add_callback(KeyboardCallback callback) {
	for(int i = 0; i < MAX_CALLBACKS_COUNT; ++i) {
		if(!callbacks[i]) {
			callbacks[i] = callback;

			return 1;
		}
	}

	return 0;
}

static int remove_callback(KeyboardCallback callback) {
	int i = 0;

	while(i < MAX_CALLBACKS_COUNT && callbacks[i] != callback) {
		++i;
	}

	if(i < MAX_CALLBACKS_COUNT) {
		// First NULL in this array means array end.
		// We ned move some elements after update.
		for(int j = i; j < MAX_CALLBACKS_COUNT - 1; ++j) {
			callbacks[j] = callbacks[j + 1];
		}

		return 1;
	} else {
		return 0;
	}
}

/* Handles the keyboard interrupt */
static void keyboard_handler(Registers *r) {
	UNUSED(r);

	unsigned char scancode;

	/* Read from the keyboard's data buffer */
	scancode = inb(0x60);

	if(currentKeyboard.data) {
		KEY_ENTRY entry = ((KEY_ENTRY*)currentKeyboard.data)[scancode & 0x7f];

		bool released = scancode & 0x80;

		if(entry.specialData) {
			specialKeysStates[entry.specialData] = !released;
		}

		unsigned char ch;

		if(specialKeysStates[RIGHT_SHIFT] || specialKeysStates[LEFT_SHIFT]) {
			ch = entry.onShiftChar;
		} else {
			ch = entry.defChar;
		}

		KeyboardEvent data = {ch, entry.specialData, !released};

		for(int i = 0; i < MAX_CALLBACKS_COUNT; ++i) {
			if(callbacks[i]) {
				callbacks[i](data);
			} else {
				// First NULL in this array means array end.
				break;
			}
		}
	} else {
		printf("Keyboard table is null!\n");
	}
}

const KeyboardDriver ps2_keyboard = {
	{
		driver_init,
		driver_cleanup,
		driver_diagnoze,
		driver_is_supported,

		"PS/2 Keyboard driver.",
		"1.0",
	},
	
	{
		set_table,
		get_avalible_keyboard_tables_count,
		get_avalible_keyboard_tables,
		add_callback,
		remove_callback,
	},
};

// static uint8_t getScancode() {
// 	uint8_t c = 0;
// 	do {
// 		if(inb(0x60) != c) {
// 			c = inb(0x60);
// 			if(c > 0)
// 				return c;
// 		}
// 	} while(1);
// }
 
// static char getchar_keyboard() {
// 	if(currentKeyboard) {
// 		KEY_ENTRY entry = currentKeyboard[getScancode()];

// 		return entry.defChar;
// 	} else {
// 		return 0;
// 	}
// }