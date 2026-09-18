#include "DetourLambda/DetourLambda.hpp"

#include <print>
#include <optional>
#include <vector>

SL_NOINLINE
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

	volatile auto test_add2 = &test_add;

	std::println("example1: result = {}, expected = 6", test_add2(2, 3)); // 7 + 2 - 3 = 6
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

		volatile auto test_add2 = &test_add;

		std::println("example2: result = {}, expected = 6", test_add2(2, 3)); // 7 + 2 - 3 = 6

		opt_dl.reset();
	}

	std::println("example2: result = {}, expected = 5", test_add(2, 3)); // 2 + 3 = 5
}

void example3()
{
	const static uint8_t buffer[] {
		0x48, 0x8B, 0x05, 0x00, 0x00, 0x00, 0x00, // mov rax, [rip+0]

		0x90, // nop
		0x90, // nop
		0x90, // nop
		0x90, // nop
		0x90, // nop
		0x90, // nop
		0x90, // nop
		0x90, // nop

		0xC3, // ret
	};

	auto func = (uint64_t(*)())buffer;

	{
		uint64_t c = 7;
		DetourLambda<uint64_t()> dl(func, [&](auto original) -> uint64_t {
			return c;
		});

		auto res = func();

		std::println("example3: result = {:#018x}, expected = 0x0000000000000007", func());
	}

	std::println("example3: result = {:#018x}, expected = 0x9090909090909090", func());
}

int main()
{
	example1();
	example2();
	example3();

	return 0;
}

