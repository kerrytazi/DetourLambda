#include "DetourLambda/DetourLambda.hpp"

#include "Zydis/Zydis.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <winternl.h>
#pragma comment(lib, "ntdll.lib")

extern "C"
NTSYSCALLAPI
NTSTATUS
NTAPI
NtProtectVirtualMemory(
	_In_ HANDLE ProcessHandle,
	_Inout_ PVOID *BaseAddress,
	_Inout_ PSIZE_T RegionSize,
	_In_ ULONG NewProtection,
	_Out_ PULONG OldProtection
	);

template <typename TFunc>
static void _DetourLambda_Reprotected(void* prot_addr, size_t prot_size, TFunc&& func)
{
	ULONG prot_old_protection = 0;
	NtProtectVirtualMemory(HANDLE(-1), &prot_addr, &prot_size, PAGE_EXECUTE_READWRITE, &prot_old_protection);

	func();

	NtProtectVirtualMemory(HANDLE(-1), &prot_addr, &prot_size, prot_old_protection, &prot_old_protection);
}
#endif

void* _DetourLambda_Unjump(void* _target)
{
	auto target = (uint8_t*)_target;

	while (true)
	{
		if (target[0] == 0xE9)
		{
			int32_t offset;
			memcpy(&offset, target + 1, 4);
			target = target + offset + 5;
		}
		else
		if (target[0] == 0xFF && target[1] == 0x25)
		{
			int32_t offset;
			memcpy(&offset, target + 2, 4);
			uint8_t* addr = target + offset + 6;
			memcpy(&target, addr, 8);
		}
		else
		{
			break;
		}
	}

	return target;
}

void _DetourLambda_CreateProxy(_DetourLambda_MemBase* mem, void* target)
{
	size_t offset_from = 0;
	size_t offset_to = 0;

	{
		ZydisDecoder decoder;
		ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);

		ZydisDecodedInstruction instruction;
		ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];

		const uint8_t* const addr = (uint8_t*)target;
		const size_t proxy_len = sizeof(mem->proxy);

		while (true)
		{
			if (ZYAN_FAILED(ZydisDecoderDecodeFull(&decoder, addr + offset_from, proxy_len - offset_from, &instruction, operands)))
				throw 1;

			ZydisEncoderRequest req;
			if (ZYAN_FAILED(ZydisEncoderDecodedInstructionToEncoderRequest(&instruction, operands, instruction.operand_count_visible, &req)))
				throw 1;

			for (int i = 0; i < req.operand_count; ++i)
			{
				if (req.operands[i].type == ZYDIS_OPERAND_TYPE_MEMORY && req.operands[i].mem.base == ZYDIS_REGISTER_RIP)
				{
					req.operands[i].mem.displacement -= ZyanI64((char*)mem->proxy);
					req.operands[i].mem.displacement += ZyanI64(addr);
				}
			}

			ZyanUSize encoded_length = sizeof(mem->proxy) - offset_to;
			if (ZYAN_FAILED(ZydisEncoderEncodeInstruction(&req, mem->proxy + offset_to, &encoded_length)))
				throw 1;

			offset_from += instruction.length;
			offset_to += encoded_length;

			if (offset_from >= 5)
			{
				break;
			}
		}
	}

	{
		mem->proxy[offset_to] = 0xE9;
		int32_t jmp_back = int32_t(intptr_t((uint8_t*)target + offset_from) - intptr_t(mem->proxy + offset_to) - 5);
		memcpy(mem->proxy + offset_to + 1, &jmp_back, 4);
	}

	memcpy(mem->original, target, sizeof(mem->original));

	char new_original[sizeof(mem->original)];

	{
		memcpy(new_original, target, sizeof(mem->original));
		new_original[0] = 0xE9;
		int32_t jmp_forward = int32_t(intptr_t(mem->code) - intptr_t(target) - 5);
		memcpy(new_original + 1, &jmp_forward, 4);
	}

	_DetourLambda_Reprotected(target, sizeof(mem->original), [&]() {
		memcpy(target, new_original, sizeof(new_original));
	});
}

void _DetourLambda_DestroyProxy(_DetourLambda_MemBase* mem)
{
	_DetourLambda_Reprotected(mem->near_target, sizeof(mem->original), [&]() {
		memcpy(mem->near_target, mem->original, sizeof(mem->original));
	});
}
