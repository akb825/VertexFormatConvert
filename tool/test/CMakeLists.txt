if (NOT GTEST_FOUND OR NOT VFC_BUILD_TESTS)
	return()
endif()

find_package(Threads)

file(GLOB_RECURSE sources *.cpp *.h)
list(APPEND sources ../ConfigFile.h ../ConfigFile.cpp)
add_executable(vfc_tool_test ${sources})

target_include_directories(vfc_tool_test PRIVATE ${GTEST_INCLUDE_DIRS} .. ../rapidjson/include)
target_link_libraries(vfc_tool_test PRIVATE vfc_lib ${GTEST_BOTH_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT})

vfc_set_folder(vfc_tool_test tests)
add_test(NAME VFCToolTest COMMAND vfc_tool_test)
