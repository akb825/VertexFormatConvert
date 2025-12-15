file(GLOB sources *.cpp *.h)
add_executable(vfc ${sources})

if (WIN32)
	configure_file(vfc.manifest.in ${CMAKE_CURRENT_BINARY_DIR}/vfc.manifest @ONLY)
	target_sources(vfc PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/vfc.manifest)
endif()

target_link_libraries(vfc PRIVATE VFC::lib)
target_include_directories(vfc PRIVATE rapidjson/include)

vfc_set_folder(vfc)
vfc_install_executable(vfc tool)

add_subdirectory(test)
