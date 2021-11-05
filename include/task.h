#pragma once

#include "errors.h"
/**
 * A simple wrapper around tasks to keep track of their state.
*/

struct Task
{
	BaseException* error;
	
	void success(void* val);

	void failure(void* val, BaseException* fault);

	bool failed();
};