#pragma once
#include "ScriptVariant.h"
#include <stack>
using ScriptInternMethod = Variant::ScriptInternMethod;
class ScriptContext {
public:
	std::unordered_map<std::string, ScriptInternMethod> InternalFunctions;
	std::unordered_map<std::string, Variant> InternalConstants;
	std::unordered_map<std::string, Variant> GlobalVars;
	struct FunctionStackInfo {
		std::string FunctionName;
		std::unordered_map<std::string, Variant> Variants;
	};
	std::stack<FunctionStackInfo> FunctionStack;
	Variant ReturnedVal{};
	enum class ScriptStatus {
		Normal,
		Return,
		Break,
		Continue,
	};
	ScriptStatus Status;
	ScriptContext() {
		InternalConstants["null"] = {};
		InternalConstants["false"] = Variant{ 0 };
		InternalConstants["true"] = Variant{ 1 };
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
		if (!FunctionStack.empty()) {
			// Search local variables
			const auto& variants = FunctionStack.top().Variants;
			if (variants.find(name) != variants.end()) {
				return variants.at(name);
			}
		}

		return {};
	}

	void PushFrame(std::string name) {
		FunctionStack.push({ name, {} });
	}
	void AddConstant(std::string name, Variant v) {
		InternalConstants[name] = v;
	}
	void SetGlobalVar(std::string name, Variant v) {
		if (InternalConstants.find(name) == InternalConstants.end())
			GlobalVars[name] = v;
	}
	void SetFunctionVar(std::string name, Variant v) {
		if (InternalConstants.find(name) == InternalConstants.end())
			FunctionStack.top().Variants[name] = v;
	}
	void DoReturn(Variant res) {
		Status = ScriptStatus::Return;
		ReturnedVal = res;
	}
	void DoReturn() {
		Status = ScriptStatus::Return;
	}
	void DoBreak() {
		Status = ScriptStatus::Break;
	}
	void DoContinue() {
		Status = ScriptStatus::Continue;
	}
	void ResetStatus() {
		Status = ScriptStatus::Normal;
	}
	bool ShouldRun() {
		return Status == ScriptStatus::Normal;
	}
	void PopFrame() {
		FunctionStack.pop();
	}
};