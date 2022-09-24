if (NOT GTEST_FOUND OR NOT VFC_BUILD_TESTS)
	return()
endif()

find_package(Threads)

file(GLOB_RECURSE sources *.cpp *.h)
add_executable(vfc_lib_test ${sources})

target_include_directories(vfc_lib_test PRIVATE ${GTEST_INCLUDE_DIRS} ../glm)
target_link_libraries(vfc_lib_test PRIVATE VFC::lib ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

vfc_set_folder(vfc_lib_test)
add_test(NAME VFCLibTest COMMAND vfc_lib_test)
