file(GLOB_RECURSE sources src/*.cpp src/*.h include/*.h)

add_library(lib ${VFC_LIB} ${sources})
add_library(VFC::lib ALIAS lib)
set_target_properties(lib PROPERTIES OUTPUT_NAME vfc)
target_include_directories(lib PRIVATE glm src)

vfc_set_folder(lib libs)
vfc_setup_filters(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
	INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/VFC
	FILES ${sources})

vfc_install_library(lib)
set(VFC_DOC_PROJECTS ${VFC_DOC_PROJECTS} lib PARENT_SCOPE)

add_subdirectory(test)
