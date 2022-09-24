# Copyright 2020-2022 Aaron Barany
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

function(vfc_install_library target name)
	set_target_properties(${target} PROPERTIES VERSION ${VFC_VERSION} DEBUG_POSTFIX d
		EXPORT_NAME ${name})
	add_library(VFC::${name} ALIAS ${target})

	set(interfaceIncludes
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)
	set_property(TARGET ${target} APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
		${interfaceIncludes})

	set(exportPath ${CMAKE_CURRENT_BINARY_DIR}/include/VFC/Export.h)
	set_property(TARGET ${target} APPEND PROPERTY INCLUDE_DIRECTORIES
		${CMAKE_CURRENT_BINARY_DIR}/include ${interfaceIncludes})
	if (VFC_SHARED)
		if (MSVC)
			set_property(TARGET ${target} APPEND PROPERTY COMPILE_DEFINITIONS VFC_BUILD)
			configure_file(${VFC_SOURCE_DIR}/cmake/templates/WindowsExport.h ${exportPath} COPYONLY)
		else()
			configure_file(${VFC_SOURCE_DIR}/cmake/templates/UnixExport.h ${exportPath} COPYONLY)
		endif()
	else()
		configure_file(${VFC_SOURCE_DIR}/cmake/templates/NoExport.h ${exportPath} COPYONLY)
	endif()

	if (NOT VFC_INSTALL)
		return()
	endif()

	install(TARGETS ${target} EXPORT ${target}Targets
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
		COMPONENT dev)
	install(FILES ${exportPath} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/VFC COMPONENT dev)

	export(EXPORT ${target}Targets FILE ${VFC_EXPORTS_DIR}/${target}-targets.cmake)
	install(EXPORT ${target}Targets NAMESPACE VFC:: FILE ${target}-targets.cmake
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VFC)
endfunction()

function(vfc_install_executable target name)
	set_target_properties(${target} PROPERTIES EXPORT_NAME ${name})
	add_executable(VFC::${name} ALIAS ${target})

	if (NOT VFC_INSTALL)
		return()
	endif()

	install(TARGETS ${target} EXPORT ${target}Targets
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
	export(EXPORT ${target}Targets FILE ${VFC_EXPORTS_DIR}/${target}-targets.cmake)
	install(EXPORT ${target}Targets NAMESPACE VFC:: FILE ${target}-targets.cmake
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VFC)
endfunction()
