cmake_minimum_required(VERSION 4.3)
project("DetourLambda")

include(FetchContent)

option(DETOUR_LAMBDA_EXAMPLE "Build example project" OFF)

add_library(DetourLambda)

target_sources(DetourLambda
	PRIVATE
		src/DetourLambda.cpp
	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
		FILES
			include/DetourLambda/DetourLambda.hpp
)

target_compile_features(DetourLambda PRIVATE cxx_std_23)

# Testing:
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../StaticLambda StaticLambda)

if(NOT TARGET StaticLambda)
	FetchContent_Declare(
		StaticLambda
		GIT_REPOSITORY https://github.com/kerrytazi/StaticLambda.git
		GIT_TAG        7e9fb7a95673ab26b6f4ade72d7fc24c9c1fa731
		GIT_SHALLOW    TRUE
	)
	FetchContent_MakeAvailable(StaticLambda)
endif()
target_link_libraries(DetourLambda PUBLIC StaticLambda)

if(NOT TARGET Zydis)
	FetchContent_Declare(
		Zydis
		GIT_REPOSITORY https://github.com/zyantific/zydis.git
		GIT_TAG        9bfadd6a55fc92dbd37fa3ba089bf8b36622df4f
		GIT_SHALLOW    TRUE
	)
	FetchContent_MakeAvailable(Zydis)
endif()
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
