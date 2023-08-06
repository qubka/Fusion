find_package(glfw QUIET)
if(NOT glfw_FOUND)
	foreach(_glfw_option "GLFW_BUILD_TESTS" "GLFW_BUILD_EXAMPLES" "GLFW_BUILD_DOCS" "GLFW_INSTALL")
		set(${_glfw_option} OFF CACHE INTERNAL "")
	endforeach()

	add_subdirectory(glfw)
endif()
list(APPEND ALL_LIBS glfw)