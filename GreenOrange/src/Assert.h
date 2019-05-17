#pragma once

#include <stdio.h>

#ifndef GO_DIST
#define GO_ENABLE_ASSERTS
#endif

#ifdef GO_ENABLE_ASSERTS
#define GO_ASSERT(x) { if(!(x)) { printf("Assertion Failed!"); __debugbreak(); } }
#define GO_ASSERT_ALWAYS() { printf("Assertion Failed!"); __debugbreak(); }
#else
#define GO_ASSERT(x)
#define GO_ASSERT_ALWAYS()
#endif