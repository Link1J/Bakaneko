cmake_minimum_required(VERSION 3.11.0)
option(ljh_BUILD_TESTS "Build the Self Tests" OFF)
project(ljh)

add_library(ljh STATIC
	source/system_directories.cpp
	source/memory_mapped_file.cpp
	source/system_info.cpp
	source/function_pointer.cpp
	$<$<PLATFORM_ID:Darwin>:
		source/system_info.mm
	>
	$<$<PLATFORM_ID:Windows>:
		source/windows/registry.cpp
	>
)
set_target_properties(ljh PROPERTIES 
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
)
target_include_directories(ljh
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)
target_link_libraries(ljh
	PUBLIC
		$<$<PLATFORM_ID:Darwin>:
			"-framework CoreFoundation"
			"-framework Foundation"
			"-framework CoreServices"
		>
		$<$<PLATFORM_ID:Linux>:
			dl
		>
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if (ljh_BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()

install(TARGETS ljh EXPORT ljh
	LIBRARY       DESTINATION lib
	ARCHIVE       DESTINATION lib
	RUNTIME       DESTINATION bin
	PUBLIC_HEADER DESTINATION include
	BUNDLE        DESTINATION bin
)
install(EXPORT ljh NAMESPACE ljh:: DESTINATION lib/cmake/ljh FILE ljh-config.cmake)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)