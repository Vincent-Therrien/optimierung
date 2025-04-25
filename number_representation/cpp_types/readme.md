# C++ Number Types Performance

This program compares the execution speed of operations related to C++ number types.

There are other types, such as `float16_t` since C++23 (https://en.cppreference.com/w/cpp/types/floating-point),
but I'm only using the types listed above because CLANG STILL DOESN'T SUPPORT F16 AS I'M WRITING
THIS (APRIL 2025).


## Build

```
> mkdir build
> cd build
> cmake ..
> cmake --build . --config Release
```

You can then execute the program (`Release/main` on Linux and `Release\main.exe` on W*ndows).
