if (NOT GTEST_FOUND OR NOT VFC_BUILD_TESTS)
	return()
endif()

find_package(Threads)

file(GLOB_RECURSE sources *.cpp *.h)
add_executable(vfc_test ${sources})

target_include_directories(vfc_test PRIVATE ${GTEST_INCLUDE_DIRS} ../glm)
target_link_libraries(vfc_test PRIVATE vfc_lib ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

vfc_set_folder(vfc_test tests)
add_test(NAME VFCLibTest COMMAND vfc_test)
