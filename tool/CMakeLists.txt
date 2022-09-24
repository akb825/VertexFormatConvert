file(GLOB sources *.cpp *.h)
add_executable(vfc ${sources})

target_link_libraries(vfc PRIVATE VFC::lib)
target_include_directories(vfc PRIVATE rapidjson/include)

vfc_set_folder(vfc)
vfc_install_executable(vfc tool)

add_subdirectory(test)
