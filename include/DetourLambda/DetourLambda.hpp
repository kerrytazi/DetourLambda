#pragma once

#include "StaticLambda/StaticLambda.hpp"


struct _DetourLambda_MemBase : _StaticLambda_MemBase
{
	char proxy[64];
	char original[8];
	void* original_addr;
};

template <typename>
struct _DetourLambda_FuncUtils;

template <typename TRet, typename... TArgs>
struct _DetourLambda_FuncUtils<TRet(TArgs...)>
{
	template <typename TLambda>
	static auto CreateProxyLambda(TLambda&& lambda)
	{
		return [lambda{ std::move(lambda) }](TArgs... args, _DetourLambda_MemBase* mem) -> TRet {
			return lambda(args..., (typename _StaticLambda_FuncUtils<TRet(TArgs...)>::func_t)(char*)mem->proxy);
		};
	}
};

void _DetourLambda_CreateProxy(_DetourLambda_MemBase* mem, void* target);
void _DetourLambda_DestroyProxy(_DetourLambda_MemBase* mem);

template <typename TSignature>
struct DetourLambda
{
	StaticLambda<TSignature> _lambda;

	template <typename TLambda, typename TNearTarget>
	explicit DetourLambda(TNearTarget target, TLambda&& lambda)
		: _lambda{ _DetourLambda_FuncUtils<TSignature>::CreateProxyLambda(std::move(lambda)), target, _StaticLambda_tag_type<_DetourLambda_MemBase>{} }
	{
		_DetourLambda_CreateProxy((_DetourLambda_MemBase*)_lambda._mem, target);
	}

	~DetourLambda()
	{
		_DetourLambda_DestroyProxy((_DetourLambda_MemBase*)_lambda._mem);
	}
};
