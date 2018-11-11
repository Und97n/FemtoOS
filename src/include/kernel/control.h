#pragma once

/**
	Print "Kernel panic: " and a message. After this - infinite cycle. For fatal problems.
**/
void kernel_panic(const char* message);