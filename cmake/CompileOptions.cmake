set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(
    -Werror -Wall -Wextra -pedantic
    -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
    -Wold-style-cast # warn for c-style casts
    -Wcast-align # warn for potential performance problem casts
    -Wunused # warn on anything being unused
    -Woverloaded-virtual # warn if you overload(not override) a virtual function
    -Wpedantic # warn if non-standard C++ is used
    -Wconversion # warn on type conversions that may lose data
    -Wsign-conversion # warn on sign conversions
    -Wmisleading-indentation # warn if indentation implies blocks where blocks do not exist
    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond> # warn if if / else chain has duplicated conditions
    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-branches> # warn if if / else branches have duplicated code
    $<$<CXX_COMPILER_ID:GNU>:-Wlogical-op> # warn about logical operations being used where bitwise were probably wanted
    -Wnull-dereference # warn if a null dereference is detected
    $<$<CXX_COMPILER_ID:GNU>:-Wuseless-cast> # warn if you perform a cast to the same type
    -Wdouble-promotion # warn if float is implicitly promoted to double
    -Wformat=2 # warn on security issues around functions that format output(i.e., printf)
    -Wimplicit-fallthrough # warn when case statements fall-through.(Included with -Wextra in GCC, not in clang)
)