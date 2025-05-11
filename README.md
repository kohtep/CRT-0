# CRT-0

[![License: MIT-0](https://img.shields.io/badge/License-MIT--0-yellow)](https://opensource.org/licenses/MIT-0) ![CRT-0](https://img.shields.io/badge/MSVC-CRT--0-indianred?logo=visualstudio&logoColor=white)

*C Run-Time Library* reimplementer for the MSVC compiler.

> [!NOTE]
> This project is not intended as a full replacement for the *CRT*, but it can significantly simplify the integration of projects that rely on the *CRT*. Many functions may still be missing, but the author has successfully linked `Lua 5.4.7` in their own Non-CRT project.

## What is it?
*CRT-0* is a small extension for VC++ projects where the *CRT* needs to be disabled. This may be necessary for projects that require greater control over the internal functions of the *C* library.

## How does it work?
When using the `/NODEFAULTLIB` flag in MSVC, the linker does not include the library that provides implementations of various functions - from basic ones like `tolower` or `strcpy` to more complex ones like `sprintf` or `acos`. The project also loses the *CRT*-specific entry point `_DllMainCRTStartup`/`mainCRTStartup`. This provides a high degree of control over the runtime environment, but at the same time requires giving up all the functions that are normally linked by default.

This usually doesn't cause issues if the developer already has their own set of functions capable of replacing the *CRT* core. However, there are cases when it's necessary to integrate third-party projects that actively use *C* functions - for example, [Lua](https://www.lua.org/). Adapting such projects can be too burdensome, so it's often much easier to simply reimplement the functions they rely on, allowing the linker to resolve them from your own files.

This is exactly what *CRT-0* does - it provides the linker with the same functions that would normally be linked if the `/NODEFAULTLIB` flag were not used.

## Why not macros?
Some developers simply define macros that redirect to their own implementations. For example:

```cpp
#define strcpy(d, s) my_strcpy(d, s)
```

This can be convenient and even more correct in some cases. However, when working with MSVC, it can lead to conflicts with *CRT* header files and other macro-related issues. Ultimately, you'll still need to modify the source code of the projects that use *CRT* functions. The reimplementation approach helps minimize such modifications - although in rare cases, some editing may still be necessary, it is required to a much lesser extent.

## How to use
Simply add all `.cpp` files to your project so the compiler can build the functions that will serve as *CRT* alternatives. Use the standard development environment headers (`cstring`, etc.).

## Recommendations
*CRT-0* assumes that you will not use *Thread Local Storage* in your project (e.g., static variables inside functions, `thread_local`, etc.), as this is considered overhead. Use the `/Zc:threadSafeInit-` flag to prevent the generation of TLS-related code for static variables within functions.

In general, any options related to the *CRT* should be disabled. This includes `Basic Runtime Checks`, `C++ Exceptions`, `Runtime Library`, `Security Check`, `SDL checks`, and `Run-Time Type Information`. If any of these options are enabled, *CRT-0* is likely to fail to compile due to linkage conflicts.

The project has only been built and tested for Windows, x64. Other configurations have not been thoroughly tested, and it is unlikely that *CRT-0* will work for them. *CRT-0* definitely cannot be built for x32 due to the compiler implicitly inserting functions like `__alldiv`, `__allrem`, etc., when handling 64-bit arithmetic.

## License
This product is distributed under the MIT-0 License.