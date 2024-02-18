#pragma once
#include <vector>
#include <string>
#include <set>
#include <stack>
#include <stdexcept>
#include <stack>
#include "ScriptVariant.h"
#include "ScriptContext.h"
#include "ScriptIr.h"

namespace ir {
	// This impls a simple stack.
	class SimpStack {
		Variant* ptr = 0;
		size_t stack_i;
		size_t stack_size;
		Variant n;

	public:
		SimpStack() {
			clear_and_resize(16384);
		}
		~SimpStack() {
			delete[] ptr;
		}
		void clear_and_resize(size_t sz) {
			if (ptr != nullptr)
				delete[] ptr;
			stack_size = sz;
			stack_i = 0;
			ptr = new Variant[stack_size];
		}
		void resize(size_t sz) {
			stack_i = sz;
		}
		void clear() {
			stack_i = 0;
		}
		[[msvc::noinline]] Variant top() {
			if (stack_i == 0)
				return n = {};
			Variant v = ptr[stack_i - 1];
			if (v.Type == Variant::DataType::ReturnPC)
				return n = {};
			stack_i--;
			return v;
		}
		Variant top_r() {
			if (stack_i == 0)
				return n = {};
			Variant v = ptr[--stack_i];
			return v;
		}
		Variant& top_p() {
			if (stack_i == 0)
				return n = {};
			Variant& v = ptr[stack_i - 1];
			if (v.Type == Variant::DataType::ReturnPC)
				return n = {};
			return v;
		}
		void pop() {
			stack_i--;
		}
		void push(const Variant& v) {
			ptr[stack_i++] = v;
			if (stack_i >= stack_size)
				throw std::runtime_error("Stack overflow.");
		}
		size_t size() {
			return stack_i;
		}
		Variant* begin() {
			return &ptr[0];
		}
		Variant* end() {
			return &ptr[stack_i];
		}
	};
	class Interpreter {
	public:
		Interpreter(const std::vector<char>& bytes, const std::vector<std::string>& strings)
			: Bytes(bytes), Strings(strings) {}
		void Run(ScriptContext& ctx) {
			PC = 0;
			while (PC < Bytes.size()) {
				auto opc = static_cast<Opcode>(Bytes[PC++]);
				switch (opc) {
				case OP_Add:
					Stack.push(Stack.top() + Stack.top());
					break;
				case OP_Sub:
					Stack.push(Stack.top() - Stack.top());
					break;
				case OP_Div:
					Stack.push(Stack.top() / Stack.top());
					break;
				case OP_Mul:
					Stack.push(Stack.top() * Stack.top());
					break;
				case OP_Or:
					Stack.push(Stack.top() || Stack.top());
					break;
				case OP_And:
					Stack.push(Stack.top() && Stack.top());
					break;
				case OP_Band:
					Stack.push(Stack.top() & Stack.top());
					break;
				case OP_Bor:
					Stack.push(Stack.top() | Stack.top());
					break;
				case OP_Xor:
					Stack.push(Stack.top() ^ Stack.top());
					break;
				case OP_GetProp: {
					Variant obj = Stack.top();
					auto str = Strings[Read<unsigned int>(Bytes, PC)];
					if (obj.Type == Variant::DataType::Object) {
						auto obj2 = obj.Object;
						if (obj2->GetType() == typeid(ScriptObject)) {
							auto obj3 = (ScriptObject*)obj2;
							Stack.push(obj3->Get(str));
						}
						else
							throw std::runtime_error("Left must be object.");
					}
					else
						throw std::runtime_error("Left must be object.");
				} break;
				case OP_SetProp: {
					Variant right = Stack.top();
					Variant obj = Stack.top();
					auto str = Strings[Read<unsigned int>(Bytes, PC)];
					if (obj.Type == Variant::DataType::Object) {
						auto obj2 = obj.Object;
						if (obj2->GetType() == typeid(ScriptObject)) {
							auto obj3 = (ScriptObject*)obj2;
							obj3->Set(str, right);
							Stack.push(right);
						}
						else
							throw std::runtime_error("Left must be object.");
					}
					else
						throw std::runtime_error("Left must be object.");
				} break;
				case OP_GetIndex:
					// TODO: Implement
					break;
				case OP_SetIndex:
					// TODO: Implement
					break;
				case OP_Int32:
					Stack.push(static_cast<int32_t>(Stack.top()));
					break;
				case OP_Int64:
					Stack.push(static_cast<int64_t>(Stack.top()));
					break;
				case OP_Float:
					Stack.push(static_cast<float>(Stack.top()));
					break;
				case OP_Double:
					Stack.push(static_cast<double>(Stack.top()));
					break;
				case OP_String:
					Stack.push({ ctx.gc, Strings[script_cast<int>(Stack.top())].c_str() });
					break;
				case OP_Ret: {
					auto retv = Stack.top();
					auto v = Stack.top_r();
					while (v.Type != Variant::DataType::ReturnPC)
						v = Stack.top_r();
					ctx.PopFrame(); // Pop the frame.
					PC = v.PC;		// Jump.
					Stack.push(retv);
				} break;
				case OP_RetNull: {
					auto v = Stack.top_r();
					while (v.Type != Variant::DataType::ReturnPC)
						v = Stack.top_r();
					ctx.PopFrame(); // Pop the frame.
					PC = v.PC;		// Jump.
					Stack.push({});
				} break;
				case OP_Brk:
					return;
				case OP_Err:
					throw std::runtime_error("Soft break");
				case OP_Call: {
					auto count = Read<unsigned char>(Bytes, PC);
					auto left = Stack.top();
					std::vector<Variant> variants;
					variants.resize(count);
					auto sz = Stack.size() - count;
					std::copy(Stack.begin() + (sz), Stack.end(), variants.begin());
					Stack.resize(sz);
					if (left.Type == Variant::DataType::Null)
						throw std::exception("Call on a null object.");
					if (left.Type == Variant::DataType::InternMethod) {
						Stack.push(left.InternMethod(ctx, variants));
						break;
					}
					if (left.Type == Variant::DataType::FuncPC) {
						Variant v{};
						v.Type = Variant::DataType::ReturnPC;
						v.PC = PC;
						Stack.push(v);
						PC = left.PC;
						ctx.PushFrame("ScriptMethod");
						ctx.SetFunctionVar("_this", left);
						if (variants.size() != 0)
							for (int i = (int)variants.size() - 1; i >= 0; i--) {
								Stack.push(variants[i]);
							}
						break;
					}
					throw std::exception("Left is not Callable.");
				} break;
				case OP_PushI4_0:
					Stack.push(0);
					break;
				case OP_PushI4_1:
					Stack.push(1);
					break;
				case OP_PushFuncPtr: {
					Variant v{};
					v.Type = Variant::DataType::FuncPC;
					v.PC = Read<int32_t>(Bytes, PC);
					Stack.push(v);
				} break;
				case OP_PushI4:
					Stack.push(Read<int32_t>(Bytes, PC));
					break;
				case OP_PushI8:
					Stack.push(Read<int64_t>(Bytes, PC));
					break;
				case OP_PushFP4:
					Stack.push(Read<float>(Bytes, PC));
					break;
				case OP_PushFP8:
					Stack.push(Read<double>(Bytes, PC));
					break;
				case OP_PushStr:
					Stack.push({ ctx.gc, Strings[static_cast<size_t>(Read<int32_t>(Bytes, PC))].c_str() });
					break;
				case OP_PushVar:
					Stack.push(ctx.LookupGlobal(Strings[static_cast<size_t>(Read<int32_t>(Bytes, PC))]));
					break;
				case OP_Pop:
					Stack.pop();
					break;
				case OP_Popn: {
					auto v = Read<unsigned char>(Bytes, PC);
					for (int i = 0; i < v; ++i) {
						Stack.pop();
					}
				} break;
				case OP_StoreVar:
					ctx.SetFunctionVar(Strings[static_cast<size_t>(Read<int32_t>(Bytes, PC))], Stack.top_p());
					break;
				case OP_PopVar:
					ctx.SetFunctionVar(Strings[static_cast<size_t>(Read<int32_t>(Bytes, PC))], Stack.top());
					break;
				// case OP_RstStk:
				//	Stack = {};
				//	break;
				case OP_Neg:
					Stack.push(-Stack.top());
					break;
				case OP_Not:
					Stack.push(!Stack.top());
					break;
				case OP_Bnot:
					Stack.push(~Stack.top());
					break;
				case OP_Inc:
					Stack.push(Stack.top() + Variant{ 1 });
					break;
				case OP_Dec:
					Stack.push(Stack.top() - Variant{ 1 });
					break;
				case OP_Equ:
					Stack.push(Stack.top() == Stack.top());
					break;
				case OP_Neq:
					Stack.push(Stack.top() != Stack.top());
					break;
				case OP_Gt:
					Stack.push(Stack.top() > Stack.top());
					break;
				case OP_Ge:
					Stack.push(Stack.top() >= Stack.top());
					break;
				case OP_Lt:
					Stack.push(Stack.top() < Stack.top());
					break;
				case OP_Le:
					Stack.push(Stack.top() <= Stack.top());
					break;
				case OP_Jmp:
					PC += static_cast<int32_t>(Read<int32_t>(Bytes, PC));
					break;
				case OP_Jz: {
					auto v = Read<int32_t>(Bytes, PC);
					if (Stack.top()) {
						PC += v;
					}
				} break;
				case OP_Jnz: {
					auto v = Read<int32_t>(Bytes, PC);
					if (!Stack.top()) {
						PC += v;
					}
				} break;
				case OP_Nop:
					break;
				case OP_Throw:
					throw std::runtime_error(Stack.top().ToString());
				default:
					throw std::runtime_error("Invalid opcode");
				}
			}
		}
		size_t GetPC() {
			return PC;
		}
		void PrintBuf(const void* lpBuffer, size_t dwSize) {
			for (size_t i = 0; i < dwSize; i++) {
				printf("%02X ", ((BYTE*)lpBuffer)[i]);
			}
		}
		void Disasm() {
			PC = 0;
			while (PC < Bytes.size()) {
				auto rpc = PC;
				auto opc = static_cast<Opcode>(Bytes[PC++]);
				std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << PC - 1 << ":" << std::oct;
				std::string exdesc;
				switch (opc) {
				case OP_PushStr:
				case OP_PushVar:
				case OP_StoreGlobalVar:
				case OP_StoreVar:
				case OP_PopVar:
				case OP_GetProp:
				case OP_SetProp:
					exdesc = Strings[Read<unsigned int>(Bytes, PC)];
					break;
				case OP_PushFP4:
					exdesc = std::to_string(Read<float>(Bytes, PC));
					break;
				case OP_PushFuncPtr:
				case OP_PushI4:
					exdesc = std::to_string(Read<int>(Bytes, PC));
					break;
				case OP_Jmp:
				case OP_Jz:
				case OP_Jnz:
					exdesc = std::format("0x{:x}", PC + Read<int>(Bytes, PC));
					break;
				case OP_PushI8:
					exdesc = std::to_string(Read<long long>(Bytes, PC));
					break;
				case OP_PushFP8:
					exdesc = std::to_string(Read<double>(Bytes, PC));
					break;
				case OP_Call:
				case OP_Popn:
					exdesc = std::to_string(Read<unsigned char>(Bytes, PC));
					break;
				}
				auto c = PC - rpc;
				PrintBuf(&Bytes[rpc], c);
				char buf[40]{};
				memset(buf, ' ', 3 * 9 - c * 3);
				std::cout << buf;
				std::cout << ir::GetOpCodeAbbr(opc) << " " << exdesc << "\n";
			}
		}

	public:
		template <typename T>
		static T Read(const std::vector<char>& bytes, size_t& pc) {
			T value;
			memcpy(&value, &bytes[pc], sizeof(T));
			pc += sizeof(T);
			return value;
		}

		std::vector<char> Bytes;
		std::vector<std::string> Strings;
		SimpStack Stack;
		size_t PC = 0;
	};
}