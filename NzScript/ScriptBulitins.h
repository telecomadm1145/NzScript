#pragma once
#include "ScriptContext.h"
#include <cmath>
#include <iostream>
void LoadBasic(ScriptContext& ctx) {
	ctx.InternalFunctions["print"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		for (auto var : vars) {
			std::cout << var.ToString();
		}
		std::cout << std::endl;
		return {};
	};
	ctx.InternalFunctions["intern"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1) {
			throw std::exception("Usage: intern(obj)");
		}
		auto v = vars[0];
		v.Type = Variant::DataType::Long;
		return v;
	};
	ctx.InternalFunctions["hex"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1) {
			throw std::exception("Usage: hex(obj)");
		}
		char chr[32];
		_ui64toa_s(vars[0].Long, chr, 32, 16);
		return { ctx.gc, chr };
	};
	ctx.InternalFunctions["typeof"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1) {
			throw std::exception("Usage: typeof(x)");
		}
		auto v = vars[0];
		return (int)v.Type;
	};
	ctx.InternalFunctions["object"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		Variant v2{};
		v2.Type = Variant::DataType::Object;
		v2.Object = new ScriptObject(ctx.gc);
		return v2;
	};
	ctx.InternalFunctions["collect"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 0) {
			throw std::exception("Usage: collect()");
		}
		ctx.gc.Collect();
		return {};
	};
	ctx.InternalFunctions["objcount"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 0) {
			throw std::exception("Usage: objcount()");
		}
		return (long long)ctx.gc.ObjectCount();
	};
	ctx.InternalFunctions["array"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		Variant v2{};
		v2.Type = Variant::DataType::Object;
		v2.Object = new ScriptArray(ctx.gc);
		return v2;
	};
}
#include "CMathBulitins.h"