file(GLOB_RECURSE sources src/*.cpp src/*.h include/*.h)

add_library(vfc_lib ${VFC_LIB} ${sources})
set_target_properties(vfc_lib PROPERTIES OUTPUT_NAME vfc)
target_include_directories(vfc_lib PRIVATE glm src)

vfc_set_folder(vfc_lib)
vfc_setup_filters(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
	INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/VFC
	FILES ${sources})

vfc_install_library(vfc_lib lib)
set(VFC_DOC_PROJECTS ${VFC_DOC_PROJECTS} lib PARENT_SCOPE)

add_subdirectory(test)
