cmake_minimum_required(VERSION 4.3)
project("DetourLambda")

option(DETOUR_LAMBDA_EXAMPLE "Build example project" OFF)

add_library(DetourLambda)

target_sources(DetourLambda
	PRIVATE
		src/DetourLambda.cpp

		include/DetourLambda/DetourLambda.hpp
)

target_include_directories(DetourLambda
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(DetourLambda PRIVATE cxx_std_23)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/StaticLambda StaticLambda)
target_link_libraries(DetourLambda PUBLIC StaticLambda)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/zydis zydis)
target_link_libraries(DetourLambda PRIVATE Zydis)

if(DETOUR_LAMBDA_EXAMPLE)
	add_executable(DetourLambdaExample)

	target_sources(DetourLambdaExample
		PRIVATE
			example/main.cpp
	)

	target_compile_features(DetourLambdaExample PRIVATE cxx_std_23)

	target_link_libraries(DetourLambdaExample DetourLambda)

	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	set_property(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}" PROPERTY VS_STARTUP_PROJECT DetourLambdaExample)
endif()
