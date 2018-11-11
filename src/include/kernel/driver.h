#pragma once

typedef struct {
	/**
		Function for driver initialization.
		memory_start - start pointer of memory, allocated for this driver.
	**/
	void (*init)();

	/**
		Function for driver cleanup. After calling this function we can free driver memory. 
	**/
	void (*cleanup)();

	/**
		Search for errors in this driver.
		Returns 0 if no problems.
		out_message - nice text message for debugging. 
	**/
	int (*diagnoze)(char** out_message);

	/**
		True if driver is supported in current environment.
	**/
	int (*is_supported)();

	/**
		Just a driver name
	**/
	const char* name;

	/**
		Numbers, that represends version in format [number.number. ....]
		Newest version have biggest numbers(fist have biggest priority).
	**/
	const char* version;
} GeneralDriverData;