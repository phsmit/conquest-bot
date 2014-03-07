add_executable( test_math test_math.cc )

add_dependencies(test_math googletest)

target_link_libraries(
    test_math
    debug ${binary_dir}/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${Suffix}
    optimized ${binary_dir}/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${Suffix}
    ${Pthread})


add_executable( test_debugger test_debugger.cc )
