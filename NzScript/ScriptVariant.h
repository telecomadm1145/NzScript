#pragma once
#include "ScriptGC.h"


struct Variant;
class ScriptObject : public GCObject {
public:
	ScriptObject(GC& gc) : GCObject(gc) {
	}
	std::unordered_map<std::string, Variant> Fields;
	const std::type_info& GetType() const noexcept override {
		return typeid(ScriptObject);
	}
	void Set(std::string s, Variant v) {
		auto& v2 = Fields[s];
		if (v2.Type == Variant::DataType::Object) {
			RemoveRef(v2.Object);
		}
		if (v.Type == Variant::DataType::Object) {
			AddRef(v.Object);
		}
		Fields[s] = v;
		return;
	}
	Variant Get(std::string s) {
		return Fields[s];
	}
};
namespace AST {
	class ScriptMethod : public ScriptObject {
	public:
		std::vector<std::string> Args;
		std::vector<class Statement*> Statements;
		ScriptMethod(GC& gc, std::vector<std::string> Args,
			std::vector<class Statement*> Statements) : Args(Args), Statements(Statements), ScriptObject(gc) {
		}
		const std::type_info& GetType() const noexcept override {
			return typeid(ScriptMethod);
		}
	};
}
class ScriptArray : public ScriptObject {
public:
	std::vector<class Variant*> Variants;
	ScriptArray(GC& gc) : ScriptObject(gc) {
	}
	const std::type_info& GetType() const noexcept override {
		return typeid(ScriptArray);
	}
};
using ScriptMethod = AST::ScriptMethod;
struct Variant {
	using ScriptInternMethod = struct Variant (*)(class ScriptContext&, std::vector<struct Variant>&);
	Variant() = default;
	Variant(int value) : Type(DataType::Int), Int(value) {}
	Variant(long value) : Type(DataType::Int), Int(value) {}
	Variant(long long value) : Type(DataType::Long), Long(value) {}
	Variant(float value) : Type(DataType::Float), Float(value) {}
	Variant(double value) : Type(DataType::Double), Double(value) {}
	Variant(const char* value) : Type(DataType::String), String(_strdup(value)) {}
	Variant(Variant* value) : Type(DataType::VariantPtr), VariantPtr(value) {}
	union {
		int Int;
		long long Long = 0;
		float Float;
		double Double;
		ScriptObject* Object;
		char* String;
		ScriptInternMethod InternMethod;
		Variant* VariantPtr;
	};
	enum class DataType {
		Null,
		Int,
		Long,
		Float,
		Double,
		IUnk,
		IDisp,
		Object,
		Array,
		Method,
		InternMethod,
		String,
		VariantPtr,
	} Type;
	std::string ToString() {
		switch (Type) {
		case DataType::Null:
			return "Null";
		case DataType::Int:
			return std::to_string(Int);
		case DataType::Long:
			return std::to_string(Long);
		case DataType::Float:
			return std::to_string(Float);
		case DataType::Double:
			return std::to_string(Double);
		case DataType::Object:
			using std::operator""s;
			return "{"s + Object->GetType().name() + "}"s;
		case DataType::InternMethod:
			return "{Internal Method}";
		case DataType::String:
			return String;
		default:
			return "Unknown";
		}
	}
	~Variant() {
		if (Type == DataType::String) {
			if (String == 0) {
				free(String);
				String = 0;
			}
		}
	}
	operator bool() {
		if (Type == DataType::Null)
			return false;
		if (Type == DataType::Int || Type == DataType::Float)
			return Int ? 1 : 0;
		return Long ? 1 : 0;
	}
	Variant& Deref() {
		if (Type == DataType::VariantPtr)
			return *VariantPtr;
		return *this;
	}
};
template <class T>
T script_cast(Variant);
template <class T>
Variant script_cast(T);
template <>
int script_cast(Variant v) {
	switch (v.Type) {
	case Variant::DataType::Double:
		return v.Double;
	case Variant::DataType::Float:
		return v.Float;
	case Variant::DataType::Int:
		return v.Int;
	case Variant::DataType::Long:
		return v.Int;
	case Variant::DataType::String:
		return std::atoi(v.String);
	}
	throw std::exception("Cannot convert.");
}
template <>
long long script_cast(Variant v) {
	switch (v.Type) {
	case Variant::DataType::Double:
		return v.Double;
	case Variant::DataType::Float:
		return v.Float;
	case Variant::DataType::Int:
		return v.Int;
	case Variant::DataType::Long:
		return v.Long;
	case Variant::DataType::String:
		return std::atoll(v.String);
	}
	throw std::exception("Cannot convert.");
}
template <>
float script_cast(Variant v) {
	switch (v.Type) {
	case Variant::DataType::Double:
		return v.Double;
	case Variant::DataType::Float:
		return v.Float;
	case Variant::DataType::Int:
		return v.Int;
	case Variant::DataType::Long:
		return v.Long;
	case Variant::DataType::String:
		return std::atof(v.String);
	}
	throw std::exception("Cannot convert.");
}
template <>
double script_cast(Variant v) {
	switch (v.Type) {
	case Variant::DataType::Double:
		return v.Double;
	case Variant::DataType::Float:
		return v.Float;
	case Variant::DataType::Int:
		return v.Int;
	case Variant::DataType::Long:
		return v.Long;
	case Variant::DataType::String:
		return std::atof(v.String);
	}
	throw std::exception("Cannot convert.");
}
template <>
std::string script_cast(Variant v) {
	return v.ToString();
}