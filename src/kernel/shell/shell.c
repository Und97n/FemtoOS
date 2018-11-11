#include <string.h>

#include <kernel/shell.h>

#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/tty.h>

#include <stdio.h>

#include "simpleCommands.h"

static const char* INPUT_MARK = ">";

typedef struct {
	const char* name;
	int (*main)(int argc, char** argv);
	bool printNewLine;
} Command;

Command commands[] = {
	{"cls", cls, 0},
	{"echo", echo, 0},
	{"3d", game3D, 0},
	//{"pages", printPages, 1},
	{"memdw", memdw, 1},
};

#define USER_LINE_BUFFER_SIZE 1024

// Buffer for user input(contains only one line). + 1 Length for safety.
static char inputBuffer[USER_LINE_BUFFER_SIZE + 1];
static int inputBufferPos;

static char* argumentsBuffer[MAX_ARGUMENTS_COUNT];

static void proceed_command_raw() {
	char ch;

	int i = 0;

	int argc = 0;

	char* lastArgumentStart = inputBuffer;

	while((ch = inputBuffer[i++]) != 0 && argc < MAX_ARGUMENTS_COUNT) {
		if(ch == '"') {
			while((ch = inputBuffer[i++]) != 0 && ch != '"');
		} else if(ch == ' ') {
			inputBuffer[i - 1] = '\0';

			if(lastArgumentStart != &(inputBuffer[i - 1])) {
				argumentsBuffer[argc++] = lastArgumentStart;
			}

			lastArgumentStart = &(inputBuffer[i]);
		}
	}

	if(*lastArgumentStart != '\0' && argc < MAX_ARGUMENTS_COUNT) {
		argumentsBuffer[argc++] = lastArgumentStart;	
	}

	int retCode = proceed_command(argc, argumentsBuffer);

	if(retCode != 0) {
		printf("Program returned with exit code %d.\n", retCode);
	}

	for(int i = 0; i < MAX_ARGUMENTS_COUNT; ++i) {
		argumentsBuffer[i] = NULL;
	}
}

int proceed_command(int argc, char** argv) {
	if(argc < 1) {
		// No command = no work.
		return 0;
	}

	int commandsCount = sizeof(commands) / sizeof(Command);

	int cp = 0;

	const char* name = argv[0];

	// Find needed command in commands list.
	while(cp < commandsCount) {
		const char* sname = commands[cp].name;

		int i = 0;

		char c1, c2;

		do {
			c1 = name[i];
			c2 = sname[i];

			++i;
		} while(c1 == c2 && c1 != 0 && c2 != 0);

		if(c1 == c2 && c1 == 0) {
			// We find!
			break;
		}

		++cp;
	}

	// If we fount it.
	if(cp < commandsCount) {
		int retCode = commands[cp].main(argc, argv);

		if(commands[cp].printNewLine) {
			putchar('\n');
		}

		return retCode;
	} else {
		printf("Command \"%s\" not found.\n", name);

		return 0;
	}
}

static void onUserInput(KeyboardEvent inputData) {
	char input = inputData.symbol;
	uint additional = inputData.special_keys_data;

	if(!inputData.press) {
		// On key release we do nothing.
		return;
	}

	if(additional == ARROW_LEFT) {
		// User can't move to a characters, that not his write.
		if(inputBufferPos > 0) {
			putchar('\b');

			--inputBufferPos;
		}
	}

	// If it is visible normal char
	if(input >= 32) {
		putchar(input);

		if(inputBufferPos < USER_LINE_BUFFER_SIZE) {
			inputBuffer[inputBufferPos] = input;

			inputBufferPos = inputBufferPos + 1;
		}

	} else if(input == '\n') {
		putchar('\n');

		if(inputBufferPos > 0) {
			proceed_command_raw();
			inputBufferPos = 0;
		
			for(int i = 0; i < USER_LINE_BUFFER_SIZE + 1; ++i) {
				inputBuffer[i] = 0;
			}
		}

		puts(INPUT_MARK);
	} else if(input == '\b') {
		// User can't earse some characters, that not his write.
		if(inputBufferPos > 0) {
			// Normal Backspace working
			putchar('\b');
			putchar(' ');
			putchar('\b');

			--inputBufferPos;
			inputBuffer[inputBufferPos] = '\0';
		}
	}
}

void enter_shell() {
	tty_enable_cursor();

	for(int i = 0; i < USER_LINE_BUFFER_SIZE + 1; ++i) {
		inputBuffer[i] = 0;
	}

	inputBufferPos = 0;

	putchar('\n');
	puts(INPUT_MARK);

	while(1) {
		onUserInput(keyboard_read_input_buffer());
	}
}

void disable_shell() {
	for(int i = 0; i < USER_LINE_BUFFER_SIZE + 1; ++i) {
		inputBuffer[i] = 0;
	}

	inputBufferPos = 0;

	keyboard_remove_callback(onUserInput);
}