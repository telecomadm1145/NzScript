﻿// NzScript.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "ScriptVariant.h"
#include "ScriptContext.h"
#include "ScriptLexer.h"
#include "ScriptAst.h"
#include "ScriptBulitins.h"

#include "GameBuffer.h"

#include <thread>
#include <windows.h>

void startup() {
	auto hstdin = GetStdHandle(STD_INPUT_HANDLE);
	unsigned long mode = 0;
	GetConsoleMode(hstdin, &mode);
	mode = (mode | ENABLE_MOUSE_INPUT) & (~ENABLE_QUICK_EDIT_MODE);
	SetConsoleMode(hstdin, mode);
	auto hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode(hstdout, &mode);
	mode = mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hstdout, mode);
	SetConsoleOutputCP(65001);
}
void doExecute(auto& p, auto exp, auto& ctx) {
	__try {
		exp = p.parse();
		exp->Execute(ctx);
	}
	__except (GetExceptionCode() == EXCEPTION_STACK_OVERFLOW) {
		std::cout << "Stack overflow.";
		_resetstkoflw();
	}
}
int main() {
	startup();
	std::string inputbuf = "";
	int cursor = 0;
	int curx = 0;
	int cury = 0;
	ScriptContext ctx{};
	LoadBasic(ctx);
	LoadCMath(ctx);
	ctx.PushFrame("main");
	GameBuffer buf{ [](const char* buf, size_t sz) {
		std::cout.write(buf, sz);
	} };
	// buf.InitConsole();
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	INPUT_RECORD ir{};
	DWORD _;
	int scrolly = 0;
	while (1) {
		GetConsoleScreenBufferInfo(hConsole, &csbi);
		buf.ResizeBuffer(csbi.dwSize.X, csbi.dwSize.Y);
		buf.Clear();
		Lexer lex{ inputbuf };
		auto tokens = lex.tokenize(true);
		enum class PredefinedColor {
			None,
			Keyword,
			Ctrlflow,
			GlobalVariable,
			LocalVariable,
			String,
			Number,
			Function,
			Comment,
			Invalid = -1,
		};
		std::vector<PredefinedColor> map;
		map.resize(inputbuf.size() + 20, PredefinedColor::None);
		for (size_t j = 0; j < tokens.size(); j++) {
			auto& tok = tokens[j];
			auto off1 = &*tok.lexeme.begin() - &*inputbuf.begin();
			auto off2 = off1 + tok.lexeme.size();
			auto clr = PredefinedColor::None;
			switch (tok.type) {
			case Lexer::TokenType::Identifier: {
				if (tok.lexeme == "function" || tok.lexeme == "var") {
					clr = PredefinedColor::Keyword;
					break;
				}
				if (tok.lexeme == "_this") {
					clr = PredefinedColor::Function;
					break;
				}
				if (tok.lexeme == "for" ||
					tok.lexeme == "while" ||
					tok.lexeme == "if" ||
					tok.lexeme == "break" ||
					tok.lexeme == "continue" ||
					tok.lexeme == "return" ||
					tok.lexeme == "else") {
					clr = PredefinedColor::Ctrlflow;
					break;
				}
				if (ctx.InternalFunctions.find(std::string(tok.lexeme)) != ctx.InternalFunctions.end()) {
					clr = PredefinedColor::Function;
					break;
				}
				if (ctx.InternalConstants.find(std::string(tok.lexeme)) != ctx.InternalConstants.end()) {
					clr = PredefinedColor::Keyword;
					break;
				}
				if (ctx.GlobalVars.find(std::string(tok.lexeme)) != ctx.GlobalVars.end()) {
					clr = PredefinedColor::GlobalVariable;
					break;
				}
				if (j < tokens.size() - 1 && tokens[j + 1].lexeme == "(" && tokens[j + 1].type == Lexer::TokenType::Delimiter) {
					clr = PredefinedColor::Function;
					break;
				}
				auto& topFrame = ctx.FunctionStack.top();
				if (topFrame.Variants.find(std::string(tok.lexeme)) != topFrame.Variants.end()) {
					auto v = topFrame.Variants[std::string(tok.lexeme)];
					switch (v.Type) {
					case Variant::DataType::Object: {
						if (v.Object->GetType() == typeid(AST::ScriptMethod)) {
							clr = PredefinedColor::Function;
							break;
						}
						break;
					}
					case Variant::DataType::Null: {
						clr = PredefinedColor::GlobalVariable;
						break;
					}
					default: {
						clr = PredefinedColor::LocalVariable;
						break;
					}
					}
					break;
				}
				clr = PredefinedColor::LocalVariable;
			} break;
			case Lexer::TokenType::FloatLiteral:
			case Lexer::TokenType::IntegerLiteral: {
				clr = PredefinedColor::Number;
				break;
			}
			case Lexer::TokenType::StringLiteral: {
				off1--;
				off2++;
				clr = PredefinedColor::String;
				break;
			}
			case Lexer::TokenType::Comment: {
				clr = PredefinedColor::Comment;
				break;
			}
			default:
				break;
			}
			for (size_t i = off1; i < off2; i++) {
				map[i] = clr;
			}
		}
		curx = cury = 0;
		bool setted = 0;
		auto cx = 4, cy = scrolly;
		int linenum = 1;
		buf.DrawString(std::to_string(linenum), 0, cy, {}, {});
		for (size_t i = 0; i < inputbuf.size(); i++) {
			auto c = inputbuf[i];
			if (!setted && i >= cursor) {
				curx = cx;
				cury = cy;
				setted = true;
			}
			if (c == '\t') {
				buf.SetPixel(cx, cy, { { 255, 20, 20, 20 }, {}, '|' });
				cx += 4;
				continue;
			}
			if (cx >= csbi.dwSize.X - 1) {
				cx = 4;
				cy++;
				i--;
				continue;
			}
			if (c == '\n') {
				linenum++;
				buf.DrawString(std::to_string(linenum), 0, cy + 1, {}, {});
				buf.SetPixel(cx, cy, { { 255, 120, 120, 120 }, {}, 0x21B5 });
				cx = 4;
				cy++;
				continue;
			}
			Color clr{};
			switch (map[i]) {
			case PredefinedColor::Ctrlflow: {
				clr = { 255, 216, 160, 223 };
				break;
			}
			case PredefinedColor::Keyword: {
				clr = { 255, 86, 156, 214 };
				break;
			}
			case PredefinedColor::String: {
				clr = { 255, 214, 157, 133 };
				break;
			}
			case PredefinedColor::Number: {
				clr = { 255, 181, 206, 168 };
				break;
			}
			case PredefinedColor::LocalVariable: {
				clr = { 255, 156, 220, 254 };
				break;
			}
			case PredefinedColor::Function: {
				clr = { 255, 220, 220, 170 };
				break;
			}
			case PredefinedColor::Invalid: {
				clr = { 255, 255, 40, 40 };
				break;
			}
			case PredefinedColor::Comment: {
				clr = { 255, 87, 166, 74 };
				break;
			}
			default:
				clr = { 255, 180, 180, 180 };
				break;
			}
			buf.SetPixel(cx, cy, { clr, {}, (unsigned int)c });
			cx++;
		}
		if (!setted) {
			curx = cx;
			cury = cy;
		}
		buf.Output();
		std::cout << "\u001b[" << cury + 1 << ";" << curx + 1 << "H";
		while (true) {
			if (cury >= csbi.dwSize.Y - 1) {
				scrolly -= 5;
				cury -= 5;
			}
			else if (cury < 0) {
				scrolly += 5;
				cury += 5;
			}
			else
				break;
		}
		ReadConsoleInput(hInput, &ir, 1, &_);
		if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
			switch (ir.Event.KeyEvent.wVirtualKeyCode) {
			case VK_LEFT: {
				if (cursor > 0)
					cursor--;
				continue;
			}
			case VK_RIGHT: {
				if (cursor < inputbuf.size())
					cursor++;
				continue;
			}
			case VK_UP: {
				auto current = cursor;
				while (cursor > 0) {
					if (inputbuf[cursor - 1] == '\n')
						break;
					cursor--;
				}
				auto off = current - cursor;
				if (cursor > 0)
					cursor--;
				while (cursor > 0) {
					if (inputbuf[cursor - 1] == '\n')
						break;
					cursor--;
				}
				while (cursor >= 0 && off > 0) {
					cursor++;
					off--;
					if (cursor > 0 && inputbuf[cursor - 1] == '\n')
						break;
				}
				if (off > 0 && cursor > 0)
					cursor--;
				continue;
			}
			case VK_DOWN: {
				auto current = cursor;
				while (cursor > 0) {
					if (inputbuf[cursor - 1] == '\n')
						break;
					cursor--;
				}
				auto off = current - cursor;
				cursor = current;
				while (cursor < inputbuf.size()) {
					if (inputbuf[cursor - 1] == '\n')
						break;
					cursor++;
				}
				if (cursor < inputbuf.size())
					cursor++;
				while (cursor < inputbuf.size() && off > 0) {
					cursor++;
					off--;
					if (inputbuf[cursor - 1] == '\n')
						break;
				}
				if (cursor > 0)
					cursor--;
				continue;
			}
			case VK_END: {
				while (cursor < inputbuf.size()) {
					if (inputbuf[cursor] == '\n')
						break;
					cursor++;
				}
				continue;
			}
			case VK_HOME: {
				while (cursor > 0) {
					if (inputbuf[cursor - 1] == '\n')
						break;
					cursor--;
				}
				continue;
			}
			case 13: {
				if ((ir.Event.KeyEvent.dwControlKeyState & 0x4) || (ir.Event.KeyEvent.dwControlKeyState & 0x8)) {
					{
						std::cout << "\u001b[38;2;255;255;255m\u001b[" << linenum + 2 << ";" << 1 << "H";
						std::cout << "Output:\n";
						Lexer lex2{ inputbuf };
						Parser p{ lex2.tokenize() };
						AST::Program* exp = 0;
						//try {
							doExecute(p, exp, ctx);
						/*}
						catch (std::exception& ex) {
							std::cout << ex.what() << "\n";
						}*/
						if (exp != 0)
							delete exp;
						ctx.gc.Collect();
						std::cin.get();
						std::cin.clear();
					}
					continue;
				}
				int indent = 0;
				int instr = 0;
				for (size_t i = 0; i < cursor; i++) {
					if (!instr && inputbuf[i] == '{') {
						indent++;
					}
					if (!instr && inputbuf[i] == '}') {
						indent--;
					}
					if (!instr && inputbuf[i] == '(') {
						indent++;
					}
					if (!instr && inputbuf[i] == ')') {
						indent--;
					}
					if (!instr && inputbuf[i] == '[') {
						indent++;
					}
					if (!instr && inputbuf[i] == ']') {
						indent--;
					}
					if (inputbuf[i] == '\"') {
						instr = !instr;
					}
					if (inputbuf[i] == '\\') {
						i++;
					}
				}
				inputbuf.insert(inputbuf.begin() + cursor, '\n');
				cursor++;
				for (size_t i = 0; i < indent; i++) {
					inputbuf.insert(inputbuf.begin() + cursor, '\t');
					cursor++;
				}
				continue;
			}
			case 8: {
				if ((ir.Event.KeyEvent.dwControlKeyState & 0x4) || (ir.Event.KeyEvent.dwControlKeyState & 0x8)) {
					cursor = 0;
					inputbuf.resize(0);
					continue;
				}
				if (cursor > 0)
					cursor--;
				inputbuf.erase(inputbuf.begin() + cursor);
				continue;
			}
			case 9: {
				inputbuf.insert(inputbuf.begin() + cursor, '\t');
				cursor++;
			}
			default:
				break;
			}
			if (ir.Event.KeyEvent.uChar.AsciiChar < 32)
				continue;
			if (ir.Event.KeyEvent.uChar.AsciiChar == '}' || ir.Event.KeyEvent.uChar.AsciiChar == ')' || ir.Event.KeyEvent.uChar.AsciiChar == ']') {
				if (cursor > 0) {
					if (inputbuf[cursor - 1] == '\t') {
						inputbuf.erase(inputbuf.begin() + --cursor);
					}
				}
			}
			inputbuf.insert(inputbuf.begin() + cursor, ir.Event.KeyEvent.uChar.AsciiChar);
			cursor++;
		}
	}
	ctx.PopFrame();
}
