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
	ctx.InternalFunctions["tostring"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1) {
			throw std::exception("Usage: tostring(obj)");
		}
		auto v = vars[0];
		return { ctx.gc, v.ToString().c_str() };
	};
	ctx.InternalFunctions["int"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1) {
			throw std::exception("Usage: int(x)");
		}
		auto v = vars[0];
		return script_cast<int>(v);
	};
	ctx.InternalFunctions["long"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1) {
			throw std::exception("Usage: long(x)");
		}
		auto v = vars[0];
		return script_cast<long long>(v);
	};
	ctx.InternalFunctions["nameof"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1) {
			throw std::exception("Usage: nameof(x)");
		}
		auto v = vars[0];
		switch (v.Type) {
		case Variant::DataType::Double:
		case Variant::DataType::Float:
		case Variant::DataType::Int:
		case Variant::DataType::Long:
		case Variant::DataType::String:
			return { ctx.gc, "{Literal}" };
		case Variant::DataType::Object:
			return { ctx.gc, ((ScriptObject*)v.Object)->GetType().name() };
		case Variant::DataType::InternMethod: {
			std::string name = "{CppMethod:Unknown}";
			for (auto func : ctx.InternalFunctions) {
				if (func.second == v.InternMethod) {
					name = "{CppMethod:";
					name += func.first;
					name += "}";
					break;
				}
			}
			return { ctx.gc, name.c_str() };
		}
		default:
			break;
		}
		return { ctx.gc, "{Unknown}" };
	};
	ctx.InternalFunctions["dir"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() == 0) {
			auto sa = new ScriptArray(ctx.gc);
			auto topFrame = ctx.FunctionStack.top();
			for (auto var : topFrame.Variants) {
				sa->Add(Variant{ ctx.gc, var.first.c_str() });
			}
			for (auto var : ctx.GlobalVars) {
				sa->Add(Variant{ ctx.gc, var.first.c_str() });
			}
			for (auto var : ctx.InternalConstants) {
				sa->Add(Variant{ ctx.gc, var.first.c_str() });
			}
			for (auto var : ctx.InternalFunctions) {
				sa->Add(Variant{ ctx.gc, var.first.c_str() });
			}
			Variant v{};
			v.Type = Variant::DataType::Object;
			v.Object = sa;
			return v;
		}
		else if (vars.size() == 1) {
			auto v2 = vars[0];
			auto sa = new ScriptArray(ctx.gc);
			switch (v2.Type) {
			case Variant::DataType::Double:
			case Variant::DataType::Float:
			case Variant::DataType::Int:
			case Variant::DataType::Long:
			case Variant::DataType::String:
			case Variant::DataType::InternMethod:
				break;
			case Variant::DataType::Object: {
				if (v2.Object->GetType() == typeid(ScriptObject))
					for (auto var : ((ScriptObject*)v2.Object)->Fields) {
						sa->Add(Variant{ ctx.gc, var.first.c_str() });
					}
			} break;
			default:
				break;
			}
			Variant v{};
			v.Type = Variant::DataType::Object;
			v.Object = sa;
			return v;
		}
		else {
			throw std::exception("Usage: dir() or dir(x)");
		}
	};
}
#include "CMathBulitins.h"