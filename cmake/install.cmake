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

function(vfc_install_library)
	set(options)
	set(oneValueArgs TARGET)
	set(multiValueArgs)
	cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	set(moduleName VFC)

	set_property(TARGET ${ARGS_TARGET} PROPERTY VERSION ${VFC_VERSION})
	set_property(TARGET ${ARGS_TARGET} PROPERTY DEBUG_POSTFIX d)

	set(interfaceIncludes
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)
	set_property(TARGET ${ARGS_TARGET} APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
		${interfaceIncludes})

	set(exportPath ${CMAKE_CURRENT_BINARY_DIR}/include/VFC/Export.h)
	set_property(TARGET ${ARGS_TARGET} APPEND PROPERTY INCLUDE_DIRECTORIES
		${CMAKE_CURRENT_BINARY_DIR}/include ${interfaceIncludes})
	if (VFC_SHARED)
		if (MSVC)
			set_property(TARGET ${ARGS_TARGET} APPEND PROPERTY COMPILE_DEFINITIONS VFC_BUILD)
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

	install(TARGETS ${ARGS_TARGET} EXPORT ${moduleName}Targets
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
		COMPONENT dev)
	install(FILES ${exportPath} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/VFC COMPONENT dev)

	include(CMakePackageConfigHelpers)
	set(versionPath ${VFC_EXPORTS_DIR}/${moduleName}ConfigVersion.cmake)
	write_basic_package_version_file(${versionPath}
		VERSION ${VFC_VERSION}
		COMPATIBILITY SameMajorVersion)

	export(EXPORT ${moduleName}Targets
		FILE ${VFC_EXPORTS_DIR}/${moduleName}Targets.cmake)

	set(configPath ${VFC_EXPORTS_DIR}/${moduleName}Config.cmake)
	file(WRITE ${configPath}
		"include(\${CMAKE_CURRENT_LIST_DIR}/${moduleName}Targets.cmake)\n"
		"set(${moduleName}_LIBRARIES ${ARGS_TARGET})\n"
		"get_target_property(${moduleName}_INCLUDE_DIRS ${ARGS_TARGET} INTERFACE_INCLUDE_DIRECTORIES)\n")

	set(configPackageDir ${CMAKE_INSTALL_LIBDIR}/cmake/VFC)
	install(EXPORT ${moduleName}Targets FILE ${moduleName}Targets.cmake
		DESTINATION ${configPackageDir})
	install(FILES ${configPath} ${versionPath} DESTINATION ${configPackageDir} COMPONENT dev)
endfunction()
