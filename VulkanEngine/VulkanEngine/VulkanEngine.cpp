#include "pch.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main()
{
    std::cout << "Hello World!\n";

    //Check for memory Leaks
    _CrtDumpMemoryLeaks();
}