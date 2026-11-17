#pragma once
#include "FreeRTOSConfig.default.h"

/* Method 2 stack check (canary) — required for this lab. */
#undef configCHECK_FOR_STACK_OVERFLOW
#define configCHECK_FOR_STACK_OVERFLOW             2