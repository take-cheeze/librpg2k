function(create_precompiled_header type output_file)
  string(REGEX REPLACE "^.*/\([^/]*\)$" "\\1" target_name ${output_file})
  string(REPLACE " " "" CMAKE_${type}_COMPILER_ARG1 ${CMAKE_${type}_COMPILER_ARG1})
  set(compiler "${CMAKE_${type}_COMPILER}")
  set(compiler_args ${CMAKE_${type}_COMPILER_ARG1} ${CMAKE_${type}_COMPILER_ARG2})

  if(CMAKE_GENERATOR MATCHES "Makefile")
	get_directory_property(prop INCLUDE_DIRECTORIES)
	foreach(i ${prop})
	  list(APPEND include_path "-I${i}")
	endforeach()
	get_directory_property(prop COMPILE_DEFINITIONS)
	list(APPEND compile_definitions ${prop})
	get_directory_property(prop COMPILE_DEFINITIONS_${CMAKE_BUILD_TYPE})
	list(APPEND compile_definitions ${prop})
	foreach(i ${compile_definitions})
	  list(APPEND definitions "-D${i}")
	endforeach()

	if(CMAKE_CXX_OMPILER_ID MATCHES "GCC")
	  add_custom_target(
		${target_name}
		COMMAND ${compiler} ${compiler_args}
		${ARGN} -o ${output_file} ${include_path} ${definitions}
		DEPENDS ${input_file} VERBATIM)
	elseif(CMAKE_${type}_COMPILER_ID MATCHES "Clang")
	  add_custom_target(
		${target_name}
		COMMAND ${compiler} ${compiler_args}
		-cc1 -emit-pth -x c++-header
		${ARGN} -o ${output_file} ${include_path} ${definitions}
		DEPENDS ${input_file} VERBATIM)
	else()
	  add_custom_target(${target_name})
	endif()
  else()
	add_custom_target(${target_name})
  endif()
endfunction()

function(precompiled_header_definition type output_variable output_file)
  if(CMAKE_GENERATOR MATCHES "Makefile")
	if(CMAKE_${type}_OMPILER_ID MATCHES "GCC")
	  set(${output_variable}
		${${output_variable}} "-include ${output_file}" PARENT_SCOPE)
	elseif(CMAKE_${type}_COMPILER_ID MATCHES "Clang")
	  set(${output_variable}
		${${output_variable}} "-cc1 -token-cache ${output_file}" PARENT_SCOPE)
	endif()
  endif()
endfunction()
