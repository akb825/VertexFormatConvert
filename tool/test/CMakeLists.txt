if (NOT GTEST_FOUND OR NOT VFC_BUILD_TESTS)
	return()
endif()

find_package(Threads)

file(GLOB_RECURSE sources *.cpp *.h)
file(GLOB extraSources  ../Base64.* ../ConfigFile.* ../Path.* ../ResultFile.*)
add_executable(vfc_tool_test ${sources} ${extraSources})

target_include_directories(vfc_tool_test PRIVATE ${GTEST_INCLUDE_DIRS} .. ../rapidjson/include)
target_link_libraries(vfc_tool_test PRIVATE vfc_lib ${GTEST_BOTH_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT})

vfc_set_folder(vfc_tool_test tests)
add_test(NAME VFCToolTest COMMAND vfc_tool_test)

set(functionalTestPath ${CMAKE_CURRENT_SOURCE_DIR}/functional)
set(vfcPath $<TARGET_FILE:vfc>)
if (WIN32)
	add_test(NAME VFCFunctionalTest COMMAND ${functionalTestPath}/run-test.bat ${vfcPath})
	add_test(NAME VFCFunctionalBase64Test COMMAND ${functionalTestPath}/run-base64-test.bat
		${vfcPath})
else()
	add_test(NAME VFCFunctionalTest COMMAND ${functionalTestPath}/run-test.sh ${vfcPath})
	add_test(NAME VFCFunctionalBase64Test COMMAND ${functionalTestPath}/run-base64-test.sh
		${vfcPath})
endif()

