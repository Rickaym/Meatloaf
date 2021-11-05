#include "task.h"


void Task::success(void* val)
{

};

void Task::failure(void* val, BaseException* fault)
{
};

bool Task::failed()
{
	return false;
};