# DetourLambda
Detours with C++ lambdas support.

## Usage Example
https://github.com/kerrytazi/DetourLambda/blob/037ef70d5cbbcf4231bc94cd88fcb7806a85f398/example/main.cpp#L7-L21

## How to add via cmake
Can be added via simple [add_subdirectory](https://cmake.org/cmake/help/latest/command/add_subdirectory.html)/[FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html)
```cmake
include(FetchContent)

FetchContent_Declare(
	DetourLambda
	GIT_REPOSITORY https://github.com/kerrytazi/DetourLambda.git
)
FetchContent_MakeAvailable(DetourLambda)

target_link_libraries(MyTarget PRIVATE DetourLambda)
```



## Credits

- StaticLambda [github](https://github.com/kerrytazi/StaticLambda)/[LICENSE](https://github.com/kerrytazi/StaticLambda/blob/main/LICENSE)
- Zydis [github](https://github.com/zyantific/zydis)/[LICENSE](https://github.com/zyantific/zydis/blob/master/LICENSE)

## License

DetourLambda is licensed under the MIT license.
