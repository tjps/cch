#include "lambda.cch.h"

std::function<void()> f =
    []{ int x = 1; };

std::function<bool(int, int)> gt =
    [](int a, int b) -> bool { return a > b; };
