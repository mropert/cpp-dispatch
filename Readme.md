# cpp-dispatch
A header-only string based routing and dispatching C++ library.

It could be used to simplify parsing of line-based text input with one or more rules defined.
The author's first target was routing HTTP requests in a REST service but it may find other uses.

## Overview

```cpp
#include <dispatch/dispatch.hpp>

using dispatch;
using dispatch::matchers::integer;
using dispatch::matchers::word;

// Single rule
auto r = rules::make_rule(matcher(word()) + ";" + word() + ";" + integer(),
                          [](auto first, auto last, auto age) {
                              // process entry...
                          });
r("John;Doe;42");   // Will call lambda with ("John", "Doe", 42)
r("Jane;Smith;24"); // Will call lambda with ("Jane", "Smith", 24)
r("Some bad text"); // Will not call lambda (and return false)

// Multiple dispatch
dispatcher d;
d.add(matcher("/user/") + integer(), [] (auto id) { /* ... */ });
d.add(matcher("/resource/") + word(), [] (auto name) { /* ... */ });
d.add(matcher("/foo/") + integer() + "/bar/" + word(),
              [] (auto id, auto name) { /* ... */ });

d("/user/1234");              // Will call lambda #1 with (1234)
d("/resource/john_doe");      // Will call lambda #2 with ("john_doe")
d("/foo/42/bar/jane_smith/"); // Will call lambda #3 with (42n "jane_smith")
d("/user/admin");             // Will not call any lambda (and return false)
```

## Status

cpp-dispatch is in its early development stage and a lot of features are still missing. For example:
* More flexible matchers (digit count on integers, character count on words)
* Non-binding regex matcher
* Custom (user defined) matchers
* Return values

## Compatibility

The library requires C++ 14 and a few STL features from C++ 17. Tested compilers are:
* Visual Studio 2017
* Clang 3.8 (`-std=c++1z`)

An effort should be made in the future to make the library more portable and compatible with C++ 14 compliant compilers
(by providing implementation of the necessary C++ 17 functions).

## Building cpp-dispatch

The library is header only and has currently no dependency, so simply add the `include/` directory to your include list and you should be all set.

Should you want to build and run the tests, simply run `cmake` to configure the build. This will require GTest:
* Either set `GTEST_DIR` before calling CMake
* Use Conan to fetch and setup it for you: `conan install`