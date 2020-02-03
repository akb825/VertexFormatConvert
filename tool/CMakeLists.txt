file(GLOB sources *.cpp *.h)
add_executable(vfc ${sources})

target_link_libraries(vfc PRIVATE vfc_lib)
target_include_directories(vfc PRIVATE rapidjson/include)

vfc_set_folder(vfc tools)

if (VFC_INSTALL)
	install(TARGETS vfc RUNTIME DESTINATION bin COMPONENT tool)
endif()

add_subdirectory(test)
