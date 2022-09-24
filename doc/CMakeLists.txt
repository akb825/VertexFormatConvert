if (NOT DOXYGEN_FOUND OR NOT VFC_BUILD_DOCS)
	return()
endif()

set(rootDir ${CMAKE_CURRENT_SOURCE_DIR}/..)

set(docSource "\"${rootDir}/README.md\" \"${rootDir}/tool/README.md\"")
set(includeDirs)
set(dependencies ${rootDir}/README.md ${rootDir}/tool/README.md)
foreach (docProject ${VFC_DOC_PROJECTS})
	set(thisDir ${rootDir}/${docProject})
	set(docSource "${docSource} \"${thisDir}/include\" \"${thisDir}/README.md\"")
	set(includeDirs "${includeDirs} \"${thisDir}/include\"")
	file(GLOB_RECURSE theseDependencies ${thisDir}/include/*.h)
	set(dependencies ${dependencies} ${theseDependencies} ${thisDir}/README.md)
endforeach()

configure_file(Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
set(outputFile ${CMAKE_CURRENT_BINARY_DIR}/html/index.html)
add_custom_command(OUTPUT ${outputFile}
	COMMAND ${DOXYGEN_EXECUTABLE} ARGS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
	WORKING_DIRECTORY ${rootDir}
	DEPENDS ${dependencies} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile ${rootDir}/README.md
	COMMENT "Generating API documentation with Doxygen" VERBATIM)
add_custom_target(vfc_doc ALL DEPENDS ${outputFile})

vfc_set_folder(vfc_doc)

if (VFC_INSTALL)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ DESTINATION ${CMAKE_INSTALL_DOCDIR})
endif()
