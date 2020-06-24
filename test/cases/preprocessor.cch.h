#pragma once


#include <stdio.h>
#include <string>
#include "util.cch.h"

#pragma once

#define DEBUG 1

#define MULTI_LINE_MACRO(x)  for \
        (int i = 0; i < x; i++)  \
        {                        \
    do_something(i);             \
        }

