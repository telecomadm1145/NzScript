#pragma once
#include "ScriptVariant.h"
#include <stack>
#include <map>
using ScriptInternMethod = Variant::ScriptInternMethod;
class ScriptContext {
public:
	std::unordered_map<std::string, ScriptInternMethod> InternalFunctions;
	std::unordered_map<std::string, Variant> InternalConstants;
	std::unordered_map<std::string, Variant> GlobalVars;
	GC gc;
	ScriptContext() {
		InternalConstants["null"] = {};
		InternalConstants["false"] = Variant{ 0 };
		InternalConstants["true"] = Variant{ 1 };
	}
	bool GlobalExists(const std::string& name) {
		if (InternalConstants.find(name) != InternalConstants.end()) {
			return true;
		}
		if (InternalFunctions.find(name) != InternalFunctions.end()) {
			return true;
		}
		if (GlobalVars.find(name) != GlobalVars.end()) {
			return true;
		}
		return false;
	}
	Variant LookupGlobal(std::string name) {
		if (InternalConstants.find(name) != InternalConstants.end()) {
			return InternalConstants[name];
		}
		if (InternalFunctions.find(name) != InternalFunctions.end()) {
			Variant v{};
			v.Type = Variant::DataType::InternMethod;
			v.InternMethod = InternalFunctions[name];
			return v;
		}
		if (GlobalVars.find(name) != GlobalVars.end()) {
			return GlobalVars[name];
		}

		return {};
	}

	void AddConstant(std::string name, Variant v) {
		InternalConstants[name] = v;
	}
	void SetGlobalVar(std::string name, Variant v) {
		if (InternalConstants.find(name) == InternalConstants.end())
		{
			auto& v2 = GlobalVars[name];
			if (v2.Type == Variant::DataType::Object || v2.Type == Variant::DataType::String)
				gc.RemoveRoot(v2.Object);
			if (v.Type == Variant::DataType::Object || v.Type == Variant::DataType::String)
				gc.AddRoot(v.Object);
			v2 = v;
		}
	}
};