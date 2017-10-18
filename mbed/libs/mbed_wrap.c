/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "mbed_toolchain.h"
#include "mbed_wrap.h"




// ****************************************************************************
// mbed_main is a function that is called before main()
// mbed_sdk_init() is also a function that is called before main(), but unlike
// mbed_main(), it is not meant for user code, but for the SDK itself to perform
// initializations before main() is called.

void mbed_main(void);

void mbed_main(void)
{
}



int __real_main(void);

int __wrap_main(void)
{
    mbed_main();
    return __real_main();
}












void _exit(int return_code)
{
    while (1);
}


extern void __wrap_exit(int return_code)
{
    _exit(return_code);
}

int __wrap_atexit(void (*func)())
{
    return 1;
}
