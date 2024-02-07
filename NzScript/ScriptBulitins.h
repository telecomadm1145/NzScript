#pragma once
#include "ScriptContext.h"
#include <cmath>
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
		auto v = vars[0];
		v.Type = Variant::DataType::String;
		auto chr = new char[32];
		_ui64toa_s(vars[0].Long, chr, 32, 16);
		v.String = chr;
		return v;
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
		v2.Object = new ScriptObject();
		return v2;
	};
	ctx.InternalFunctions["delete"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1) {
			throw std::exception("Usage: delete(obj)");
		}
		delete vars[0].Object;
		return {};
	};
	ctx.InternalFunctions["array"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		Variant v2{};
		v2.Type = Variant::DataType::Array;
		v2.Array = new ScriptArray();
		return v2;
	};
}
#include "CMathBulitins.h"