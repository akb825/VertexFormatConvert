file(GLOB sources *.cpp *.h)
add_executable(tool ${sources})
add_executable(VFC::tool ALIAS tool)
set_target_properties(tool PROPERTIES OUTPUT_NAME vfc)

target_link_libraries(tool PRIVATE VFC::lib)
target_include_directories(tool PRIVATE rapidjson/include)

vfc_set_folder(tool tools)
vfc_install_executable(tool)

add_subdirectory(test)
