#include "DetourLambda/DetourLambda.hpp"

#include <print>
#include <optional>

__declspec(noinline)
int test_add(int a, int b)
{
	return a + b;
}

void example1()
{
	// Lambda with capture
	int c = 7;
	DetourLambda<int(int, int)> dl(&test_add, [&](int a, int b, auto original) -> int {
		return original(c, a) - b;
	});

	std::println("example1: result = {}, expected = 6", test_add(2, 3)); // 7 + 2 - 3 = 6
}

void example2()
{
	// using std::optional for delayed initialization
	std::optional<DetourLambda<int(int, int)>> opt_dl;

	std::println("example2: result = {}, expected = 5", test_add(2, 3)); // 2 + 3 = 5

	{
		int c = 7;
		opt_dl.emplace(&test_add, [&](int a, int b, auto original) -> int {
			return original(c, a) - b;
		});

		std::println("example2: result = {}, expected = 6", test_add(2, 3)); // 7 + 2 - 3 = 6

		opt_dl.reset();
	}

	std::println("example2: result = {}, expected = 5", test_add(2, 3)); // 2 + 3 = 5
}

int main()
{
	example1();
	example2();
}
