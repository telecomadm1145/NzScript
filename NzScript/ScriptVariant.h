#pragma once
#include "ScriptGC.h"
struct Variant {
	using ScriptInternMethod = struct Variant (*)(class ScriptContext&, std::vector<struct Variant>&);
	Variant() = default;
	Variant(int value) : Type(DataType::Int), Int(value) {}
	Variant(long value) : Type(DataType::Int), Int(value) {}
	Variant(long long value) : Type(DataType::Long), Long(value) {}
	Variant(float value) : Type(DataType::Float), Float(value) {}
	Variant(double value) : Type(DataType::Double), Double(value) {}
	Variant(GC& gc, const char* value) : Type(DataType::String) {
		Object = new GCString(gc, value);
	}
	// Variant(Variant* value) : Type(DataType::VariantPtr), VariantPtr(value) {}
	union {
		int Int;
		long long Long = 0;
		float Float;
		double Double;
		GCObject* Object;
		ScriptInternMethod InternMethod;
		size_t PC;
		// Variant* VariantPtr;
	};
	enum class DataType {
		Null,
		Int,
		Long,
		Float,
		Double,
		IUnk_NotUsed,
		IDisp_NotUsed,
		Object,
		Array_NotUsed,
		Method_NotUsed,
		InternMethod,
		String,
		VariantPtr_NotUsed,
		ReturnPC,
		FuncPC,
	} Type =  DataType::Null;
	std::string ToString() const;
	operator bool() {
		if (Type == DataType::Null)
			return false;
		if (Type == DataType::Int || Type == DataType::Float)
			return Int ? 1 : 0;
		return Long ? 1 : 0;
	}
};
class ScriptObject : public GCObject {
public:
	ScriptObject(GC& gc) : GCObject(gc) {
	}

public:
	std::unordered_map<std::string, Variant> Fields;
	const std::type_info& GetType() const noexcept override {
		return typeid(ScriptObject);
	}
	void Set(std::string s, Variant v) {
		auto& v2 = Fields[s];
		if (v2.Type == Variant::DataType::Object || v.Type == Variant::DataType::String) {
			RemoveRef(v2.Object);
		}
		if (v.Type == Variant::DataType::Object || v.Type == Variant::DataType::String) {
			AddRef(v.Object);
		}
		Fields[s] = v;
		return;
	}
	Variant Get(std::string s) {
		return Fields[s];
	}
};
class ScriptArray : public ScriptObject {
public:
	std::vector<Variant> Variants;
	ScriptArray(GC& gc) : ScriptObject(gc) {
	}
	const std::type_info& GetType() const noexcept override {
		return typeid(ScriptArray);
	}
	void Set(size_t index, Variant v) {
		if (index >= Variants.size()) {
			Variants.resize(index + 1);
		}
		auto& v2 = Variants[index];
		if (v2.Type == Variant::DataType::Object || v.Type == Variant::DataType::String) {
			RemoveRef(v2.Object);
		}
		if (v.Type == Variant::DataType::Object || v.Type == Variant::DataType::String) {
			AddRef(v.Object);
		}
		Variants[index] = v;
		return;
	}
	void Add(Variant v) {
		if (v.Type == Variant::DataType::Object || v.Type == Variant::DataType::String) {
			AddRef(v.Object);
		}
		Variants.push_back(v);
		return;
	}
	size_t Size() {
		return Variants.size();
	}
	Variant Get(size_t index) {
		if (index >= Variants.size()) {
			Variants.resize(index + 1);
			return {};
		}
		return Variants[index];
	}
};
template <class T>
T script_cast(Variant);
template <class T>
Variant script_cast(T);
#pragma warning(push)
#pragma warning(disable:4244)
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
		if (v.Object->GetType() == typeid(GCString)) {
			return std::atoi(((GCString*)v.Object)->Pointer);
		}
		break;
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
		if (v.Object->GetType() == typeid(GCString)) {
			return std::atoll(((GCString*)v.Object)->Pointer);
		}
		break;
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
		if (v.Object->GetType() == typeid(GCString)) {
			return std::atof(((GCString*)v.Object)->Pointer);
		}
		break;
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
		if (v.Object->GetType() == typeid(GCString)) {
			return std::atof(((GCString*)v.Object)->Pointer);
		}
		break;
	}
	throw std::exception("Cannot convert.");
}
template <>
std::string script_cast(Variant v) {
	return v.ToString();
}
std::string Variant::ToString() const {
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
	case DataType::Object: {
		const auto& typ = Object->GetType();
		if (typ == typeid(ScriptObject)) {
			std::string s = "{";
			for (auto p : ((ScriptObject*)Object)->Fields) {
				s += p.first;
				s += "=";
				s += p.second.ToString();
				s += ",";
			}
			if (s.size() != 1)
				s.erase(s.end() - 1);
			s += "}";
			return s;
		}
		if (typ == typeid(ScriptArray)) {
			std::string s = "[";
			for (auto p : ((ScriptArray*)Object)->Variants) {
				s += p.ToString();
				s += ",";
			}
			if (s.size() != 1)
				s.erase(s.end() - 1);
			s += "]";
			return s;
		}
		return "Unknown";
	}
	case DataType::InternMethod:
		return "{Internal Method}";
	case DataType::String:
		return ((GCString*)Object)->Pointer;
	default:
		return "Unknown";
	}
}
#pragma warning(pop)

#define AUTO_OPDEF(x)                                                                         \
	Variant operator##x##(const Variant& lft, const Variant& rht) {                           \
		if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) { \
			return Variant{                                                                   \
				script_cast<double>(lft)##x##script_cast<double>(rht)                         \
			};                                                                                \
		}                                                                                     \
		if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {   \
			return Variant{                                                                   \
				script_cast<float>(lft)##x##script_cast<float>(rht)                           \
			};                                                                                \
		}                                                                                     \
		if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {     \
			return Variant{                                                                   \
				script_cast<long long>(lft)##x##script_cast<long long>(rht)                   \
			};                                                                                \
		}                                                                                     \
		if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {       \
			return Variant{                                                                   \
				script_cast<int>(lft)##x##script_cast<int>(rht)                               \
			};                                                                                \
		}                                                                                     \
		throw std::runtime_error("Cannot convert.");                                          \
	}
#define AUTO_OPDEF2(x)                                                                    \
	Variant operator##x##(const Variant& lft, const Variant& rht) {                       \
		if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) { \
			return Variant{                                                               \
				script_cast<long long>(lft)##x##script_cast<long long>(rht)               \
			};                                                                            \
		}                                                                                 \
		if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {   \
			return Variant{                                                               \
				script_cast<int>(lft)##x##script_cast<int>(rht)                           \
			};                                                                            \
		}                                                                                 \
		throw std::runtime_error("Cannot convert.");                                      \
	}
#define AUTO_OPDEF3(x)                               \
	Variant operator##x##(Variant& lft) {            \
		if (lft.Type == Variant::DataType::Double) { \
			return Variant{                          \
				x##lft.Double                        \
			};                                       \
		}                                            \
		if (lft.Type == Variant::DataType::Float) {  \
			return Variant{                          \
				x##lft.Float                         \
			};                                       \
		}                                            \
		if (lft.Type == Variant::DataType::Long) {   \
			return Variant{                          \
				x##lft.Long                          \
			};                                       \
		}                                            \
		if (lft.Type == Variant::DataType::Int) {    \
			return Variant{                          \
				x##lft.Int                           \
			};                                       \
		}                                            \
		throw std::runtime_error("Cannot convert."); \
	}
#define AUTO_OPDEF4(x)                               \
	Variant operator##x##(const Variant& lft) {      \
		if (lft.Type == Variant::DataType::Double) { \
			return Variant{                          \
				x##lft.Double                        \
			};                                       \
		}                                            \
		if (lft.Type == Variant::DataType::Float) {  \
			return Variant{                          \
				x##lft.Float                         \
			};                                       \
		}                                            \
		if (lft.Type == Variant::DataType::Long) {   \
			return Variant{                          \
				x##lft.Long                          \
			};                                       \
		}                                            \
		if (lft.Type == Variant::DataType::Int) {    \
			return Variant{                          \
				x##lft.Int                           \
			};                                       \
		}                                            \
		throw std::runtime_error("Cannot convert."); \
	}
#define AUTO_OPDEF5(x)                               \
	Variant operator##x##(const Variant& lft) {      \
		if (lft.Type == Variant::DataType::Long) {   \
			return Variant{                          \
				x##lft.Long                          \
			};                                       \
		}                                            \
		if (lft.Type == Variant::DataType::Int) {    \
			return Variant{                          \
				x##lft.Int                           \
			};                                       \
		}                                            \
		throw std::runtime_error("Cannot convert."); \
	}

AUTO_OPDEF(+)
AUTO_OPDEF(-)
AUTO_OPDEF(*)
AUTO_OPDEF(/)
AUTO_OPDEF2(^)
AUTO_OPDEF2(&)
AUTO_OPDEF2(&&)
AUTO_OPDEF2(|)
AUTO_OPDEF2(||)
AUTO_OPDEF(==)
AUTO_OPDEF(!=)
AUTO_OPDEF(<)
AUTO_OPDEF(>)
AUTO_OPDEF(<=)
AUTO_OPDEF(>=)
AUTO_OPDEF3(++)
AUTO_OPDEF3(--)
AUTO_OPDEF4(-)
AUTO_OPDEF5(~)


#undef AUTO_OPDEF
#undef AUTO_OPDEF2
