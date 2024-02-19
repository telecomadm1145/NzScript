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
		Variant blank_val{};

	public:
		Variant* ptr = 0;
		/// <summary>
		/// 传入的参数
		/// </summary>
		size_t bp = 0;
		/// <summary>
		/// 本地变量
		/// </summary>
		size_t bp2 = 0;
		size_t sp = 0;
		size_t max;
		/// <summary>
		/// 新建一个栈
		/// </summary>
		SimpStack() {
			clear_and_resize(4096);
		}
		/// <summary>
		/// 清理栈
		/// </summary>
		~SimpStack() {
			delete[] ptr;
		}
		/// <summary>
		/// 重置并分配指定大小的栈
		/// </summary>
		/// <param name="sz">栈大小</param>
		void clear_and_resize(size_t sz) {
			if (ptr != nullptr)
				delete[] ptr;
			max = sz;
			sp = 0;
			ptr = new Variant[max];
		}
		/// <summary>
		/// 重置SP到指定值
		/// </summary>
		/// <param name="sz">SP的新值</param>
		void reset(size_t sz) {
			sp = sz;
		}
		void clear() {
			sp = 0;
		}
		Variant top() {
			if (sp == 0)
				return blank_val;
			return ptr[--sp];
		}
		Variant top_r() {
			if (sp == 0)
				return blank_val;
			Variant v = ptr[--sp];
			return v;
		}
		Variant& top_p() {
			if (sp == 0)
				return blank_val = {};
			return ptr[sp - 1];
		}
		Variant& operator[](size_t i) {
			return ptr[i];
		}
		void pop() {
			sp--;
		}
		void push(const Variant& v) {
			ptr[sp++] = v;
			if (sp >= max)
				throw std::runtime_error("Stack overflow.");
		}
		Variant& get_arg(size_t i) {
			if (i> 20)
				throw std::runtime_error("Invalid Operation.");
			return ptr[bp + i];
		}
		Variant& get_local(size_t i) {
			if (i > 20)
				throw std::runtime_error("Invalid Operation.");
			return ptr[bp2 + i];
		}
		Variant& get_lr() {
			return ptr[bp2 - 1]; // Linked address
		}
		Variant& get_last_bp2() {
			return ptr[bp2 - 2]; // 上一个 Base Pointer(2)
		}
		Variant& get_last_bp() {
			return ptr[bp2 - 3]; // 上一个 Base Pointer
		}
		/// <summary>
		/// 弹出栈帧，并返回跳转的PC
		/// </summary>
		/// <returns>PC</returns>
		size_t pop_frame() {
			auto lr = get_lr().PC;
			// 重置栈指针
			sp = bp;
			bp = get_last_bp().PC;
			bp2 = get_last_bp2().PC;
			return lr;
		}
		size_t size() {
			return sp;
		}
		Variant* begin() {
			return &ptr[0];
		}
		Variant* end() {
			return &ptr[sp];
		}
	};
	class Interpreter {
	public:
		Interpreter(const std::vector<char>& bytes, const std::vector<std::string>& strings)
			: Bytes(bytes), Strings(strings) {}
		void Run(ScriptContext& ctx) {
			PC = 0;
			while (PC < Bytes.size()) {
				//auto p = PC;
				//DecodeAsm(p);
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
					auto str = Strings[Read<UImm4>(Bytes, PC)];
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
					auto str = Strings[Read<UImm4>(Bytes, PC)];
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
					Stack.push(script_cast<Imm4>(Stack.top()));
					break;
				case OP_Int64:
					Stack.push(script_cast<Imm8>(Stack.top()));
					break;
				case OP_Float:
					Stack.push(script_cast<float>(Stack.top()));
					break;
				case OP_Double:
					Stack.push(script_cast<double>(Stack.top()));
					break;
				case OP_String:
					Stack.push({ ctx.gc, Strings[script_cast<int>(Stack.top())].c_str() });
					break;
				case OP_Ret: {
					auto v = Stack.top();
					PC = Stack.pop_frame();
					Stack.push(v);
				} break;
				case OP_RetNull: {
					PC = Stack.pop_frame();
					Stack.push({});
				} break;
				case OP_Brk:
					return;
				case OP_Err:
					throw std::runtime_error("Soft break");
				case OP_Call: {
					auto count = Read<Imm1>(Bytes, PC);
					auto left = Stack.top();
					if (left.Type == Variant::DataType::Null)
						throw std::exception("Call on a null object.");
					if (left.Type == Variant::DataType::InternMethod) {
						std::vector<Variant> variants;
						variants.resize(count);
						auto sz = Stack.size() - count;
						std::copy(Stack.begin() + (sz), Stack.end(), variants.begin());
						Stack.reset(sz);
						Stack.push(left.InternMethod(ctx, variants));
						break;
					}
					if (left.Type == Variant::DataType::FuncPC) {
						auto rbp = Stack.sp - count;
						Variant v{};
						v.Type = Variant::DataType::ReturnPC;
						v.PC = Stack.bp;
						Stack.push(v);
						v.PC = Stack.bp2;
						Stack.push(v);
						v.PC = PC;
						Stack.push(v);
						Stack.bp = rbp;
						Stack.bp2 = Stack.sp;

						PC = left.PC;
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
					v.PC = Read<UImm4>(Bytes, PC);
					Stack.push(v);
				} break;
				case OP_PushI4:
					Stack.push(Read<Imm4>(Bytes, PC));
					break;
				case OP_PushI8:
					Stack.push(Read<Imm8>(Bytes, PC));
					break;
				case OP_PushFP4:
					Stack.push(Read<float>(Bytes, PC));
					break;
				case OP_PushFP8:
					Stack.push(Read<double>(Bytes, PC));
					break;
				case OP_PushStr:
					Stack.push({ ctx.gc, Strings[Read<UImm4>(Bytes, PC)].c_str() });
					break;
				case OP_PushNull:
					Stack.push({});
					break;
				case OP_PushGlobalVar:
					Stack.push(ctx.LookupGlobal(Strings[Read<UImm4>(Bytes, PC)]));
					break;
				case OP_StoreGlobalVar:
					ctx.GlobalVars[Strings[Read<UImm4>(Bytes, PC)]] = Stack.top_p();
					break;
				case OP_PushArg:
					Stack.push(Stack.get_arg(Read<Imm1>(Bytes, PC)));
					break;
				case OP_PushLocalI1:
					Stack.push(Stack.get_local(Read<Imm1>(Bytes, PC)));
					break;
				case OP_PushLocalI4:
					Stack.push(Stack.get_local(Read<UImm4>(Bytes, PC)));
					break;
				case OP_StoreLocalI1:
					Stack.get_local(Read<Imm1>(Bytes, PC)) = Stack.top_p();
					break;
				case OP_StoreLocalI4:
					Stack.get_local(Read<UImm4>(Bytes, PC)) = Stack.top_p();
					break;
				case OP_Pop:
					Stack.pop();
					break;
				case OP_Popn: {
					auto v = Read<Imm1>(Bytes, PC);
					for (int i = 0; i < v; ++i) {
						Stack.pop();
					}
				} break;
				case OP_PushN: {
					auto v = Read<Imm1>(Bytes, PC);
					for (int i = 0; i < v; ++i) {
						Stack.push({});
					}
				} break;
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
					PC += Read<Imm4>(Bytes, PC);
					break;
				case OP_Jz: {
					auto v = Read<Imm4>(Bytes, PC);
					if (Stack.top()) {
						PC += v;
					}
				} break;
				case OP_Jnz: {
					auto v = Read<Imm4>(Bytes, PC);
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
				DecodeAsm(PC);
			}
		}

		void DecodeAsm(size_t& PC) {
			auto rpc = PC;
			auto opc = static_cast<Opcode>(Bytes[PC++]);
			std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << PC - 1 << ":" << std::oct;
			std::string exdesc;
			switch (opc) {
			case OP_PushStr:
			case OP_PushGlobalVar:
			case OP_StoreGlobalVar:
			case OP_GetProp:
			case OP_SetProp:
				exdesc = Strings[Read<unsigned int>(Bytes, PC)];
				break;
			case OP_PushFP4:
				exdesc = std::to_string(Read<float>(Bytes, PC));
				break;
			case OP_PushLocalI4:
			case OP_StoreLocalI4:
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
			case OP_PushArg:
			case OP_Call:
			case OP_Popn:
			case OP_PushN:
			case OP_PushLocalI1:
			case OP_StoreLocalI1:
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