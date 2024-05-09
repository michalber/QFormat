set(QFORMAT_TESTS
    qformat_basic_tests
)

foreach(test_case IN LISTS QFORMAT_TESTS)
    add_executable(${test_case} ${test_case}.cpp)
    target_compile_features(${test_case} PRIVATE cxx_std_17)
    add_test(${test_case} ${test_case})
endforeach()

target_link_libraries(qformat_basic_tests PRIVATE qformat::qformat)
