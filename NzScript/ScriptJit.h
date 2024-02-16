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
	template <class T>
	class SimpStack {
	public:
		std::deque<T> container;
		[[msvc::noinline]] void clear() {
			container.clear();
		}
		[[msvc::noinline]] Variant& top() {
			decltype(auto) v = container.back();
			container.pop_back();
			return v;
		}
		[[msvc::noinline]] Variant& top_p() {
			decltype(auto) v = container.back();
			return v;
		}
		[[msvc::noinline]] void pop() {
			container.pop_back();
		}
		[[msvc::noinline]] void push(const Variant& v) {
			container.push_back(v);
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
				case OP_GetProp:
					// TODO: Implement
					break;
				case OP_SetProp:
					// TODO: Implement
					break;
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
				case OP_Ret:
					ctx.DoReturn(Stack.top());
					return;
				case OP_RetNull:
					ctx.DoReturn({});
					return;
				case OP_Call: {
					auto count = Read<unsigned char>(Bytes, PC);
					auto left = Stack.top();
					std::vector<Variant> variants;
					variants.resize(count);
					auto sz = Stack.container.size() - count;
					std::copy(Stack.container.begin() + (sz), Stack.container.end(), variants.begin());
					Stack.container.resize(sz);
					if (left.Type == Variant::DataType::Null)
						throw std::exception("Call on a null object.");
					if (left.Type == Variant::DataType::InternMethod) {
						Stack.push(left.InternMethod(ctx, variants));
						break;
					}
					if (left.Type == Variant::DataType::Object && left.Object->GetType() == typeid(ScriptMethod)) {
						auto mtd = (ScriptMethod*)left.Object;
						ctx.PushFrame("ScriptMethod");
						ctx.SetFunctionVar("_this", left);
						for (size_t i = 0; i < min(mtd->Args.size(), variants.size()); i++) {
							ctx.SetFunctionVar(mtd->Args[i], variants[i]);
						}
						for (auto s : mtd->Statements) {
							s->Execute(ctx);
							if (ctx.Status == ScriptContext::ScriptStatus::Return) {
								ctx.ResetStatus();
								ctx.PopFrame();
								Stack.push(ctx.ReturnedVal);
								break;
							}
							ctx.ResetStatus();
						}
						ctx.PopFrame();
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
				case OP_RstStk:
					Stack = {};
					break;
				case OP_Neg:
					Stack.push(-Stack.top());
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
					if (!Stack.top()) {
						PC += v;
					}
				} break;
				case OP_Jnz: {
					auto v = Read<int32_t>(Bytes, PC);
					if (Stack.top()) {
						PC += v;
					}
				} break;
				case OP_Nop:
					break;
				default:
					throw std::runtime_error("Invalid opcode");
				}
			}
		}
		size_t GetPC() {
			return PC;
		}
		void PrintBuf(const void* lpBuffer, DWORD dwSize) {
			for (DWORD i = 0; i < dwSize; i++) {
				printf("%02X ", ((BYTE*)lpBuffer)[i]);
			}
		}
		void Disasm() {
			PC = 0;
			while (PC < Bytes.size()) {
				auto rpc = PC;
				auto opc = static_cast<Opcode>(Bytes[PC++]);
				std::string exdesc;
				switch (opc) {
				case OP_PushStr:
				case OP_PushVar:
				case OP_StoreGlobalVar:
				case OP_StoreVar:
					exdesc = Strings[Read<unsigned int>(Bytes, PC)];
					break;
				case OP_PushFP4:
					exdesc = std::to_string(Read<float>(Bytes, PC));
					break;
				case OP_PushI4:
				case OP_Jmp:
				case OP_Jz:
				case OP_Jnz:
					exdesc = std::to_string(Read<int>(Bytes, PC));
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
				std::cout << PC << ":";
				auto c = PC - rpc;
				PrintBuf(&Bytes[rpc], c);
				char buf[40]{};
				memset(buf, ' ', 3*9 - c * 3);
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
		SimpStack<Variant> Stack;
		size_t PC = 0;
	};
}