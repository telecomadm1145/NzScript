#include "pch.h"
#include "CppUnitTest.h"

#include "ScriptVariant.h"
#include "ScriptContext.h"
#include "ScriptLexer.h"
#include "ScriptAst.h"
#include "ScriptBulitins.h"
#include "ScriptOptimizer.h"
#include "GameBuffer.h"
#include "ScriptJit.h"
#include <random>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NzTest
{
	TEST_CLASS(Scripting)
	{
		Variant RunScript(const std::string& content) {
			Lexer lex(content);
			Parser p{ lex.tokenize() };
			AST::Program* program = p.parse();
			ir::Emitter em;
			em.ctx = &ctx;
			program->Emit(em);
			ir::Interpreter ir(em.Bytes, em.Strings);
			return ir.Run(ctx);
		}
	public:
		Scripting() {
			LoadBasic(ctx);
			LoadCMath(ctx);
		}
		ScriptContext ctx;
		std::random_device rd;
		TEST_METHOD(BinOpAddTest)
		{
			long long a = rd();
			long long b = rd();
			auto binop = new AST::BinaryExpression(
				new AST::NumberExpression(a),
				AST::BinOp::Add,
				new AST::NumberExpression(b)
			);
			Assert::IsTrue(binop->Eval(ctx) == Variant{ a + b });
		}
		TEST_METHOD(BinOpSubTest)
		{
			long long a = rd();
			long long b = rd();
			auto binop = new AST::BinaryExpression(
				new AST::NumberExpression(a),
				AST::BinOp::Sub,
				new AST::NumberExpression(b)
			);
			Assert::IsTrue(binop->Eval(ctx) == Variant{ a - b });
		}
		TEST_METHOD(RecursionTest) {
			Assert::IsTrue(Variant{ 0 } == RunScript(
				R"a(
var func = function(n){
	if (n < 1)
		return n;
	return func(n-1);
};
return func(999);
)a"));
		}
		TEST_METHOD(GlobalVariableTest) {
			Assert::IsTrue(Variant{ 114514 } == RunScript(
				R"a(
var v = 114514;
func_0 = function(){
	return v;
};
return func_0();
)a"));
		}
		TEST_METHOD(GlobalVariableTest2) {
			Assert::IsTrue(Variant{ 114514 } == RunScript(
				R"a(
(var v) = 114514;
func_0 = function(){
	return v;
};
return func_0();
)a"));
		}
		TEST_METHOD(Fib30Test) {
			Assert::IsTrue(Variant{ 832040 } == RunScript(
				R"a(
var func_0 = function(n){
	if(n<=2)
		return 1;
	else
		return func_0(n-1) + func_0(n-2);
};
return func_0(30);
)a"));
		}
		TEST_METHOD(Fib35Test) {
			Assert::IsTrue(Variant{ 9227465 } == RunScript(
				R"a(
var func_0 = function(n){
	if(n<=2)
		return 1;
	else
		return func_0(n-1) + func_0(n-2);
};
return func_0(35);
)a"));
		}
		TEST_METHOD(RecursionTestIfStackCorruption) {
			Assert::IsTrue(Variant{ 114514 } == RunScript(
				R"a(
a = 114514;
var func = function(n){
	if (n < 1)
		return n;
	return func(n-1);
};
func(999);
return a;
)a"));
		}
		TEST_METHOD(WhileTest) {
			Assert::IsTrue(Variant{ 30 } == RunScript(
				R"a(
a = 0;
while(a<30)
	++a;
return a;
)a"));
		}
		TEST_METHOD(ForTest) {
			Assert::IsTrue(Variant{ 30 } == RunScript(
				R"a(
for(a = 0;a<30;++a)
a;
return a;
)a"));
		}
		TEST_METHOD(WhileBreakTest) {
			Assert::IsTrue(Variant{ 30 } == RunScript(
				R"a(
a = 0;
while(1)
{
	if(a>=30)
		break;
	++a;
}
return a;
)a"));
		}
		TEST_METHOD(TypeCheckTest) {
			using namespace std;
			Assert::IsTrue(RunScript("return 1;") == Variant{ 1 });
			Assert::IsTrue(RunScript("return \"hello\";") == "hello");
			Assert::IsTrue(RunScript("return null;") == NullVariant);
		}
		TEST_METHOD(IndexingTest) {
			Assert::IsTrue(RunScript(R"a(
a = array();
a[32];
a[1] = 3;
return a[1];
)a") == Variant{ 3 });
		}
		TEST_METHOD(ThrowTest) {
			Assert::ExpectException<std::runtime_error>([&]() {
				RunScript(R"a(
throw 0;
)a"); });
		}
		TEST_METHOD(ObjectStoreTest) {
			Assert::IsTrue(RunScript(R"a(
a = object();
a.b = a;
a.b.b.b.b.b = 114514;
return a.b;
)a") == Variant{ 114514 });
		}
	};
}