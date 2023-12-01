#include "Debug.h"
#include <cassert>

void Debug::CheckNoEntry()
{
	assert(false);
}

void Debug::DebuggerPause()
{
	__debugbreak();
}