#ifndef __tests_functionptrs_cch__
#define __tests_functionptrs_cch__

int (*add)(int x, int y);
int (*floor)(double d);

static void (*v)();
static void (*v1)(float f);

#endif
