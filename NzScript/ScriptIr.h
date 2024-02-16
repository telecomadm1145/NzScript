#pragma once
#include <vector>
#include <string>
#include <set>
#include <stack>
#include <stdexcept>
#include <stack>
#include "ScriptVariant.h"
#include "ScriptContext.h"
/*
架构总览：

采用了类似x86的CISC设计，中间代码指令变长，并且类似RPN的计算堆栈

比如说计算1+1

就需要这样:

```
PushI4 1
PushI4 1
Add
PushVar Print
Call 1
```

对于 Jit 函数内部忽略调用规定(x64 call)，统一使用从左到右的栈调用约定(std call)
*/
namespace ir {
	enum Opcode : unsigned char {
		// Takes two values and put "BinOp result" of them into stack
		OP_Add,
		OP_Sub,
		OP_Div,
		OP_Mul,
		OP_Or,
		OP_And,
		OP_Band,
		OP_Bor,
		OP_Xor,

		// Get the property(1 == Object,2 == String(name)),put result into stack
		OP_GetProp,
		// Set the property(1 == Object,2 == String(name),3 == value)
		OP_SetProp,
		OP_GetIndex,
		OP_SetIndex,

		// Converts value on the stack top.
		OP_Int32,
		OP_Int64,
		OP_Float,
		OP_Double,
		OP_String,

		// Take last element on the stack and return it to the caller.
		OP_Ret,
		// Return null value.
		OP_RetNull,
		// Take certain arguments(imm1) into the target function(object) and put the result into stack.
		OP_Call,

		OP_PushI4_1,
		OP_PushI4_0,
		// Push literals into stack(imm4/imm8)
		OP_PushI4,
		OP_PushI8,
		OP_PushFP4,
		OP_PushFP8,
		// str
		OP_PushStr,
		// str
		OP_PushVar,
		OP_Pop,
		// Pop certain values.(imm1)
		OP_Popn,
		// Store the stack top value to certain variable
		OP_StoreVar,
		// Store the stack top value to certain variable
		OP_StoreGlobalVar,
		// Reset stack into default value.
		OP_RstStk,
		// Make value negative.
		OP_Neg,
		// Increase the value.
		OP_Inc,
		// Decrease the value.
		OP_Dec,

		OP_Not,

		OP_Bnot,

		// Compare two values and put the result into stack(in I4).
		OP_Equ,
		OP_Gt,
		OP_Lt,
		OP_Ge,
		OP_Le,
		OP_Neq,

		// Jump to offset.(Signed imm4)
		OP_Jmp,
		// Jump if Equal.(offset Signed imm4)
		OP_Jz,
		// Jump if not Equal.
		OP_Jnz,

		// Print stack top(debug)
		OP_Print = 0xfb,
		// No operation
		OP_Nop = 0xff,
	};
	const char* GetOpCodeAbbr(Opcode op) {
		switch (op) {
		case ir::OP_Add:
			return "Add";
		case ir::OP_Sub:
			return "Sub";
		case ir::OP_Div:
			return "Div";
		case ir::OP_Mul:
			return "Mul";
		case ir::OP_Or:
			return "Or";
		case ir::OP_And:
			return "And";
		case ir::OP_Band:
			return "Band";
		case ir::OP_Bor:
			return "Bor";
		case ir::OP_Xor:
			return "Xor";
		case ir::OP_GetProp:
			return "GetProp";
		case ir::OP_SetProp:
			return "SetProp";
		case ir::OP_GetIndex:
			return "GetIndex";
		case ir::OP_SetIndex:
			return "SetIndex";
		case ir::OP_Int32:
			return "Int32";
		case ir::OP_Int64:
			return "Int64";
		case ir::OP_Float:
			return "Float";
		case ir::OP_Double:
			return "Double";
		case ir::OP_String:
			return "String";
		case ir::OP_Ret:
			return "Ret";
		case ir::OP_RetNull:
			return "RetNull";
		case ir::OP_Call:
			return "Call";
		case ir::OP_PushI4:
			return "PushI4";
		case ir::OP_PushI8:
			return "PushI8";
		case ir::OP_PushFP4:
			return "PushFP4";
		case ir::OP_PushFP8:
			return "PushFP8";
		case ir::OP_PushStr:
			return "PushStr";
		case ir::OP_PushVar:
			return "PushVar";
		case ir::OP_Pop:
			return "Pop";
		case ir::OP_Popn:
			return "Pop";
		case ir::OP_StoreVar:
			return "StoreVar";
		case ir::OP_StoreGlobalVar:
			return "StoreGVar";
		case ir::OP_RstStk:
			return "RstStk";
		case ir::OP_Neg:
			return "Neg";
		case ir::OP_Inc:
			return "Inc";
		case ir::OP_Dec:
			return "Dec";
		case ir::OP_Not:
			return "Not";
		case ir::OP_Bnot:
			return "Bnot";
		case ir::OP_Equ:
			return "Equ";
		case ir::OP_Gt:
			return "Gt";
		case ir::OP_Lt:
			return "Lt";
		case ir::OP_Ge:
			return "Ge";
		case ir::OP_Le:
			return "Le";
		case ir::OP_Neq:
			return "Neq";
		case ir::OP_Jmp:
			return "Jmp";
		case ir::OP_Jz:
			return "Jz";
		case ir::OP_Jnz:
			return "Jnz";
		case ir::OP_Print:
			return "DBG_Print";
		case ir::OP_Nop:
			return "Nop";
		case ir::OP_PushI4_1:
			return "PushI4.1";
		case ir::OP_PushI4_0:
			return "PushI4.0";
		default:
			return "Unknown";
		}
	}
	class Emitter {
	public:
		std::vector<char> Bytes;
		std::vector<std::string> Strings;
		void EmitOp(Opcode opc) {
			Bytes.push_back(opc);
		}
		void EmitOpI1(Opcode opc, unsigned char imm1) {
			Bytes.push_back(opc);
			Emit(imm1);
		}
		void EmitOp(Opcode opc, int imm4) {
			Bytes.push_back(opc);
			Emit(imm4);
		}
		void EmitOp(Opcode opc, long long imm8) {
			Bytes.push_back(opc);
			Emit(imm8);
		}
		void EmitOp(Opcode opc, float imm4) {
			Bytes.push_back(opc);
			Emit(imm4);
		}
		void EmitOp(Opcode opc, double imm8) {
			Bytes.push_back(opc);
			Emit(imm8);
		}
		void EmitOp(Opcode opc, const std::string& str) {
			Bytes.push_back(opc);

			int i = 0;
			bool found = false;
			for (auto& str1 : Strings) {
				if (str1 == str) {
					found = true;
					break;
				}
				i++;
			}
			if (!found)
			{
				Strings.push_back(str);
				Emit((int)Strings.size() - 1);
				return;
			}
			Emit(i);
		}
		void Emit(auto imm) {
			Bytes.insert(Bytes.end(), (char*)&imm, (char*)(&imm + 1));
		}
		void Modify(auto where, auto imm) {
			memcpy(&*where, &imm, sizeof(imm));
		}

		enum LateBindPointType {
			Break,
			Continue,
		};
		struct LateBindPoint {
			LateBindPointType Type;
			// 指向分支指令的指针
			size_t Where;
		};

		std::vector<LateBindPoint> LateBinds;
		// 发射 后期绑定的分支指令
		void EmitOpLate(Opcode opc, LateBindPointType imm4_late) {
			LateBindPoint lbp;
			lbp.Type = imm4_late;
			lbp.Where = Bytes.size();
			LateBinds.emplace_back(lbp);

			Bytes.push_back(opc);
			Emit((int)0);
		}
		void EvalLateBinds(size_t brk, size_t cont) {
			for (auto& p : LateBinds) {
				switch (p.Type) {
				case LateBindPointType::Break:
					if (brk == (size_t)-1)
						throw std::exception("Break bind doesn't exist.");
					Modify(&Bytes[p.Where + 1], (int)((long long)brk - (long long)p.Where) - 5);
					break;
				case LateBindPointType::Continue:
					if (cont == (size_t)-1)
						throw std::exception("Continue bind doesn't exist.");
					Modify(&Bytes[p.Where + 1], (int)((long long)cont - (long long)p.Where) - 5);
					break;
				}
			}
			LateBinds.clear();
		}
	};
}