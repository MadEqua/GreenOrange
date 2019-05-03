#pragma once

#include <stdio.h>

#ifndef GO_DIST
#define GO_ENABLE_ASSERTS
#endif

#ifdef GO_ENABLE_ASSERTS
#define GO_ASSERT(x) { if(!(x)) { printf("Assertion Failed!"); __debugbreak(); } }
#else
#define GO_ASSERT(x)
#endif