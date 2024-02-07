#pragma once
#include "ScriptVariant.h"
#include "ScriptContext.h"
namespace AST {

	class Statement {
	public:
		Statement() = default;
		Statement(const Statement&) = delete;
		Statement(Statement&&) = delete;
		virtual void Execute(ScriptContext& ctx) = 0;
		virtual ~Statement() = default;
	};
	class Program {
	public:
		void addStatement(class Statement* statement) {
			statements_.push_back(statement);
		}

		std::vector<Statement*> getStatements() const {
			return statements_;
		}
		void Execute(ScriptContext& ctx) {
			for (auto stat : statements_) {
				stat->Execute(ctx);
				ctx.ResetStatus();
			}
		}

	private:
		std::vector<Statement*> statements_;
	};
	class Expression : public Statement {
	public:
		void Execute(ScriptContext& ctx) override {
			Eval(ctx);
		}
		virtual bool IsLeftValue() {
			return false;
		}
		virtual void Set(ScriptContext& ctx, Variant) {
			throw std::exception("Invalid operation.");
		}
		virtual Variant Eval(ScriptContext& ctx) = 0;
		virtual std::string GetVariableName() {
			throw std::exception("Invalid operation.");
		}
	};
	class LambdaExpression : public Expression {
	public:
		Variant Eval(ScriptContext& ctx) override {
			Variant v{};
			v.Type = Variant::DataType::Object;
			auto mtd = new ScriptMethod(ctx.gc, Params, Statements);
			v.Object = mtd;
			return v;
		}
		std::vector<std::string> Params;
		std::vector<Statement*> Statements;
		LambdaExpression(std::vector<std::string> Params, std::vector<Statement*> Statements)
			: Params(Params), Statements(Statements) {}
	};
	class VariantRefExpression : public Expression {
	public:
		VariantRefExpression(std::string varname) : VariantName(varname) {
		}
		std::string VariantName;
		Variant Eval(ScriptContext& ctx) override {
			return ctx.LookupGlobal(VariantName);
		}
		bool IsLeftValue() override {
			return true;
		}
		void Set(ScriptContext& ctx, Variant v) {
			ctx.SetFunctionVar(VariantName, v);
		}
		std::string GetVariableName() {
			return VariantName;
		}
	};
	class GlobalVariantRefExpression : public Expression {
	public:
		GlobalVariantRefExpression(std::string varname) : VariantName(varname) {
		}
		std::string VariantName;
		Variant Eval(ScriptContext& ctx) override {
			return ctx.LookupGlobal(VariantName);
		}
		bool IsLeftValue() override {
			return true;
		}
		void Set(ScriptContext& ctx, Variant v) {
			ctx.SetGlobalVar(VariantName, v);
		}
	};
	class StringExpression : public Expression {
	public:
		std::string str;

		// 通过 Expression 继承
		Variant Eval(ScriptContext& ctx) override {
			return { ctx.gc, str.data() };
		}

		StringExpression(const std::string& str)
			: str(str) {
		}
	};
	class NumberExpression : public Expression {
	public:
		NumberExpression(Variant v) : var(v) {}
		Variant var;
		// 通过 Expression 继承
		Variant Eval(ScriptContext&) override {
			return var;
		}
	};
	enum class UnOp {
		Nop,
		Not,
		Bnot,
		Negative,
		Positive,
		Increase,
		Decrease,
	};
	enum class BinOp {
		Nop,
		Add,
		Sub,
		Mul,
		Div,
		Mov,
		Member,
		Index,

		Greater,
		Lesser,
		IsEqual,
		GreaterOrEqual,
		LesserOrEqual,
		NotEqual,

		Or,
		And,
		Band,
		Bor,
		Xor,
	};
	class BinaryExpression : public Expression {
	public:
		BinaryExpression(Expression* leftExpression, BinOp op, Expression* rightExpression)
			: leftExpression_(leftExpression), op(op), rightExpression_(rightExpression) {}

		Expression* getLeftExpression() const {
			return leftExpression_;
		}

		Expression* getRightExpression() const {
			return rightExpression_;
		}
		~BinaryExpression() {
			if (leftExpression_ != 0)
				delete leftExpression_;
			if (rightExpression_ != 0)
				delete rightExpression_;
		}

	private:
		Expression* leftExpression_;
		Expression* rightExpression_;
		BinOp op;
		virtual bool IsLeftValue() {
			return op == BinOp::Member;
		}
		virtual void Set(ScriptContext& ctx, Variant v) {
			auto l = leftExpression_->Eval(ctx);
			auto r = rightExpression_->GetVariableName();
			if (l.Type == Variant::DataType::Object) {
				((ScriptObject*)l.Object)->Set(r, v);
			}
		}
		Variant Eval(ScriptContext& ctx) override {
			switch (op) {
			case AST::BinOp::Nop:
				return leftExpression_->Eval(ctx);
				break;
			case AST::BinOp::Add: {
				auto lft = leftExpression_->Eval(ctx);
				auto rht = rightExpression_->Eval(ctx);
				if (lft.Type == Variant::DataType::String || rht.Type == Variant::DataType::String) {
					return { ctx.gc, (lft.ToString() + rht.ToString()).c_str() };
				}
				if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
					return Variant{
						script_cast<double>(lft) + script_cast<double>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
					return Variant{
						script_cast<float>(lft) + script_cast<float>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
					return Variant{
						script_cast<long long>(lft) + script_cast<long long>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
					return Variant{
						script_cast<int>(lft) + script_cast<int>(rht)
					};
				}

				throw std::exception("Cannot promote a fit type.");
			} break;
			case AST::BinOp::Sub: {
				auto lft = leftExpression_->Eval(ctx);
				auto rht = rightExpression_->Eval(ctx);
				if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
					return Variant{
						script_cast<double>(lft) - script_cast<double>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
					return Variant{
						script_cast<float>(lft) - script_cast<float>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
					return Variant{
						script_cast<long long>(lft) - script_cast<long long>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
					return Variant{
						script_cast<int>(lft) - script_cast<int>(rht)
					};
				}

				throw std::exception("Cannot promote a fit type.");
			} break;
			case AST::BinOp::Mul: {
				auto lft = leftExpression_->Eval(ctx);
				auto rht = rightExpression_->Eval(ctx);
				if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
					return Variant{
						script_cast<double>(lft) * script_cast<double>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
					return Variant{
						script_cast<float>(lft) * script_cast<float>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
					return Variant{
						script_cast<long long>(lft) * script_cast<long long>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
					return Variant{
						script_cast<int>(lft) * script_cast<int>(rht)
					};
				}

				throw std::exception("Cannot promote a fit type.");
			} break;
			case AST::BinOp::Div: {
				auto lft = leftExpression_->Eval(ctx);
				auto rht = rightExpression_->Eval(ctx);
				if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
					return Variant{
						script_cast<double>(lft) / script_cast<double>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
					return Variant{
						script_cast<float>(lft) / script_cast<float>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
					return Variant{
						script_cast<long long>(lft) / script_cast<long long>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
					return Variant{
						script_cast<int>(lft) / script_cast<int>(rht)
					};
				}

				throw std::exception("Cannot promote a fit type.");
			} break;
			case AST::BinOp::Mov: {
				if (!leftExpression_->IsLeftValue()) {
					throw std::exception("");
				}
				Variant v = rightExpression_->Eval(ctx);
				leftExpression_->Set(ctx, v);
				return v;
			} break;
			case AST::BinOp::Member: {
				auto l = leftExpression_->Eval(ctx);
				auto r = rightExpression_->GetVariableName();
				switch (l.Type) {
				case Variant::DataType::Object: {
					if (l.Object->GetType() == typeid(ScriptObject)) {
						return ((ScriptObject*)l.Object)->Get(r);
					}
				} break;
				default:
					break;
				}
				throw std::exception("Left is not Object.");
			} break;
			case AST::BinOp::Band: {
				auto lft = leftExpression_->Eval(ctx);
				auto rht = rightExpression_->Eval(ctx);
				if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
					return Variant{
						script_cast<long long>(lft) & script_cast<long long>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
					return Variant{
						script_cast<int>(lft) & script_cast<int>(rht)
					};
				}

				throw std::exception("Cannot promote a fit type.");
			} break;
			case AST::BinOp::Bor: {
				auto lft = leftExpression_->Eval(ctx);
				auto rht = rightExpression_->Eval(ctx);
				if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
					return Variant{
						script_cast<long long>(lft) | script_cast<long long>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
					return Variant{
						script_cast<int>(lft) | script_cast<int>(rht)
					};
				}

				throw std::exception("Cannot promote a fit type.");
			} break;
			case AST::BinOp::Xor: {
				auto lft = leftExpression_->Eval(ctx);
				auto rht = rightExpression_->Eval(ctx);
				if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
					return Variant{
						script_cast<long long>(lft) ^ script_cast<long long>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
					return Variant{
						script_cast<int>(lft) ^ script_cast<int>(rht)
					};
				}

				throw std::exception("Cannot promote a fit type.");
			} break;
			case AST::BinOp::And: {
				auto lft = leftExpression_->Eval(ctx);
				if (!lft)
					return Variant{ 0 };
				auto rht = rightExpression_->Eval(ctx);
				if (rht)
					return Variant{ 1 };
				return Variant{ 0 };
			} break;
			case AST::BinOp::Or: {
				auto lft = leftExpression_->Eval(ctx);
				if (lft)
					return Variant{ 1 };
				auto rht = rightExpression_->Eval(ctx);
				if (rht)
					return Variant{ 1 };
				return Variant{ 0 };
			} break;
			case AST::BinOp::Greater: {
				auto lft = leftExpression_->Eval(ctx);
				auto rht = rightExpression_->Eval(ctx);
				if (lft.Type == Variant::DataType::String || rht.Type == Variant::DataType::String) {
					return lft.ToString() > rht.ToString();
				}
				if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
					return Variant{
						script_cast<double>(lft) > script_cast<double>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
					return Variant{
						script_cast<float>(lft) > script_cast<float>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
					return Variant{
						script_cast<long long>(lft) > script_cast<long long>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
					return Variant{
						script_cast<int>(lft) > script_cast<int>(rht)
					};
				}

				throw std::exception("Cannot promote a fit type.");
			} break;
			case AST::BinOp::Lesser: {
				auto lft = leftExpression_->Eval(ctx);
				auto rht = rightExpression_->Eval(ctx);
				if (lft.Type == Variant::DataType::String || rht.Type == Variant::DataType::String) {
					return lft.ToString() < rht.ToString();
				}
				if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
					return Variant{
						script_cast<double>(lft) < script_cast<double>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
					return Variant{
						script_cast<float>(lft) < script_cast<float>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
					return Variant{
						script_cast<long long>(lft) < script_cast<long long>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
					return Variant{
						script_cast<int>(lft) < script_cast<int>(rht)
					};
				}

				throw std::exception("Cannot promote a fit type.");
			} break;
			case AST::BinOp::GreaterOrEqual: {
				auto lft = leftExpression_->Eval(ctx);
				auto rht = rightExpression_->Eval(ctx);
				if (lft.Type == Variant::DataType::String || rht.Type == Variant::DataType::String) {
					return lft.ToString() >= rht.ToString();
				}
				if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
					return Variant{
						script_cast<double>(lft) >= script_cast<double>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
					return Variant{
						script_cast<float>(lft) >= script_cast<float>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
					return Variant{
						script_cast<long long>(lft) >= script_cast<long long>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
					return Variant{
						script_cast<int>(lft) >= script_cast<int>(rht)
					};
				}

				throw std::exception("Cannot promote a fit type.");
			} break;
			case AST::BinOp::LesserOrEqual: {
				auto lft = leftExpression_->Eval(ctx);
				auto rht = rightExpression_->Eval(ctx);
				if (lft.Type == Variant::DataType::String || rht.Type == Variant::DataType::String) {
					return lft.ToString() <= rht.ToString();
				}
				if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
					return Variant{
						script_cast<double>(lft) <= script_cast<double>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
					return Variant{
						script_cast<float>(lft) <= script_cast<float>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
					return Variant{
						script_cast<long long>(lft) <= script_cast<long long>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
					return Variant{
						script_cast<int>(lft) <= script_cast<int>(rht)
					};
				}

				throw std::exception("Cannot promote a fit type.");
			} break;
			case AST::BinOp::IsEqual: {
				auto lft = leftExpression_->Eval(ctx);
				auto rht = rightExpression_->Eval(ctx);
				if (lft.Type == Variant::DataType::String || rht.Type == Variant::DataType::String) {
					return lft.ToString() == rht.ToString();
				}
				if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
					return Variant{
						script_cast<double>(lft) == script_cast<double>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
					return Variant{
						script_cast<float>(lft) == script_cast<float>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
					return Variant{
						script_cast<long long>(lft) == script_cast<long long>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
					return Variant{
						script_cast<int>(lft) == script_cast<int>(rht)
					};
				}
				return lft.Type == rht.Type && lft.Long == rht.Long;
			} break;
			case AST::BinOp::NotEqual: {
				auto lft = leftExpression_->Eval(ctx);
				auto rht = rightExpression_->Eval(ctx);
				if (lft.Type == Variant::DataType::String || rht.Type == Variant::DataType::String) {
					return lft.ToString() != rht.ToString();
				}
				if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
					return Variant{
						script_cast<double>(lft) != script_cast<double>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
					return Variant{
						script_cast<float>(lft) != script_cast<float>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
					return Variant{
						script_cast<long long>(lft) != script_cast<long long>(rht)
					};
				}
				if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
					return Variant{
						script_cast<int>(lft) != script_cast<int>(rht)
					};
				}

				throw std::exception("Cannot promote a fit type.");
			} break;
			default:
				throw std::exception("Invalid operation.");
				break;
			}
			return {};
		}
	};
	class IndexExpression : public Expression {
	public:
		IndexExpression(Expression* leftExpression, Expression* rightExpression)
			: leftExpression_(leftExpression), rightExpression_(rightExpression) {}

		Expression* getLeftExpression() const {
			return leftExpression_;
		}

		Expression* getRightExpression() const {
			return rightExpression_;
		}
		~IndexExpression() {
			if (leftExpression_ != 0)
				delete leftExpression_;
			if (rightExpression_ != 0)
				delete rightExpression_;
		}

	private:
		Expression* leftExpression_;
		Expression* rightExpression_;
		virtual bool IsLeftValue() {
			return true;
		}
		virtual void Set(ScriptContext& ctx, Variant v) {
			auto lft = leftExpression_->Eval(ctx);
			auto rht = rightExpression_->Eval(ctx);
			switch (lft.Type) {
			case Variant::DataType::Object: {
				const auto& typ = lft.Object->GetType();
				if (typ == typeid(ScriptObject)) {
					if (rht.Type != Variant::DataType::String)
						throw std::exception("Right should be a string.");
					std::string s = ((GCString*)rht.Object)->Pointer;
					((ScriptObject*)lft.Object)->Set(s, v);
					return;
				}
				if (typ == typeid(ScriptArray)) {
					auto arr = (ScriptArray*)lft.Object;
					auto ind = script_cast<long long>(rht);
					if (ind < 0) {
						ind += arr->Size();
					}
					if (ind < 0) {
						throw std::exception("Bad backward index.");
					}
					arr->Set(ind, v);
					return;
				}
				break;
			}
			default:
				break;
			}
			throw std::exception("Invalid left val.");
		}
		Variant Eval(ScriptContext& ctx) override {
			auto lft = leftExpression_->Eval(ctx);
			auto rht = rightExpression_->Eval(ctx);
			switch (lft.Type) {
			case Variant::DataType::Object: {
				const auto& typ = lft.Object->GetType();
				if (typ == typeid(ScriptObject)) {
					if (rht.Type != Variant::DataType::String)
						throw std::exception("Right should be a string.");
					std::string s = ((GCString*)rht.Object)->Pointer;
					return ((ScriptObject*)lft.Object)->Get(s);
				}
				if (typ == typeid(ScriptArray)) {
					auto arr = (ScriptArray*)lft.Object;
					auto ind = script_cast<long long>(rht);
					if (ind < 0) {
						ind += arr->Size();
					}
					if (ind < 0) {
						throw std::exception("Bad backward index.");
					}
					return arr->Get(ind);
				}
			}
			default:
				break;
			}
			return {};
		}
	};
	class TernaryExpression : public Expression {
	public:
		TernaryExpression(Expression* condition, Expression* onTrue, Expression* onFalse) : condition(condition), onTrue(onTrue), onFalse(onFalse) {
		}
		~TernaryExpression() {
			if (condition != 0)
				delete condition;
			if (onTrue != 0)
				delete onTrue;
			if (onFalse != 0)
				delete onFalse;
		}
		Expression* condition;
		Expression* onTrue;
		Expression* onFalse;
		Variant Eval(ScriptContext& ctx) override {
			if (condition->Eval(ctx)) {
				return onTrue->Eval(ctx);
			}
			else {
				return onFalse->Eval(ctx);
			}
		}
	};
	class CallExpression : public Expression {
	public:
		CallExpression(Expression* method, std::vector<Expression*> args) : method(method), arguments(args) {
		}
		Expression* method;
		std::vector<Expression*> arguments;
		Variant Eval(ScriptContext& ctx) override {
			auto left = method->Eval(ctx);
			if (left.Type == Variant::DataType::Null)
				throw std::exception("Call on a null object.");
			auto vars = std::vector<Variant>();
			for (auto exp : arguments) {
				vars.push_back(exp->Eval(ctx));
			}
			if (left.Type == Variant::DataType::InternMethod) {
				return left.InternMethod(ctx, vars);
			}
			if (left.Type == Variant::DataType::Object && left.Object->GetType() == typeid(ScriptMethod)) {
				auto mtd = (ScriptMethod*)left.Object;
				ctx.PushFrame("ScriptMethod");
				ctx.SetFunctionVar("_this", left);
				for (size_t i = 0; i < std::min(mtd->Args.size(), vars.size()); i++) {
					ctx.SetFunctionVar(mtd->Args[i], vars[i]);
				}
				for (auto s : mtd->Statements) {
					s->Execute(ctx);
					if (ctx.Status == ScriptContext::ScriptStatus::Return) {
						ctx.ResetStatus();
						ctx.PopFrame();
						return ctx.ReturnedVal;
					}
					ctx.ResetStatus();
				}
				ctx.PopFrame();
				return {};
			}
			throw std::exception("Left is not Callable.");
			return {};
		}
		~CallExpression() {
			if (method != 0)
				delete method;
			for (auto exp : arguments) {
				if (exp != 0)
					delete exp;
			}
		}
	};
	class UnaryExpression : public Expression {
	public:
		UnaryExpression(Expression* left, UnOp op) : left(left), op(op) {
		}
		Expression* left;
		UnOp op;

		~UnaryExpression() {
			if (left != 0)
				delete left;
		}

		// 通过 Expression 继承
		Variant Eval(ScriptContext& ctx) override {
			switch (op) {
			case AST::UnOp::Nop:
				return left->Eval(ctx);
			case AST::UnOp::Not: {
				return !left->Eval(ctx);
			} break;
			case AST::UnOp::Bnot: {
				auto v = left->Eval(ctx);
				if (v.Type == Variant::DataType::Int) {
					return ~(v.Int);
				}
				if (v.Type == Variant::DataType::Long) {
					return ~(v.Long);
				}
				throw std::exception("Bad input.");
			} break;
			case AST::UnOp::Negative: {
				auto v = left->Eval(ctx);
				if (v.Type == Variant::DataType::Double) {
					return -(v.Double);
				}
				if (v.Type == Variant::DataType::Float) {
					return -(v.Float);
				}
				if (v.Type == Variant::DataType::Int) {
					return -(v.Int);
				}
				if (v.Type == Variant::DataType::Long) {
					return -(v.Long);
				}
				throw std::exception("Bad input.");
			} break;
			case AST::UnOp::Positive: {
				auto v = left->Eval(ctx);
				if (v.Type == Variant::DataType::Double) {
					return v.Double;
				}
				if (v.Type == Variant::DataType::Float) {
					return v.Float;
				}
				if (v.Type == Variant::DataType::Int) {
					return v.Int;
				}
				if (v.Type == Variant::DataType::Long) {
					return v.Long;
				}
				throw std::exception("Bad input.");
			} break;
			case AST::UnOp::Increase: {
				auto v = left->Eval(ctx);
				if (v.Type == Variant::DataType::Double) {
					left->Set(ctx, ++v.Double);
					return v.Double;
				}
				if (v.Type == Variant::DataType::Float) {
					left->Set(ctx, ++v.Float);
					return v.Float;
				}
				if (v.Type == Variant::DataType::Int) {
					left->Set(ctx, ++v.Int);
					return v.Int;
				}
				if (v.Type == Variant::DataType::Long) {
					left->Set(ctx, ++v.Long);
					return v.Long;
				}
				throw std::exception("Bad input.");
			} break;
			case AST::UnOp::Decrease: {
				auto v = left->Eval(ctx);
				if (v.Type == Variant::DataType::Double) {
					left->Set(ctx, --v.Double);
					return v.Double;
				}
				if (v.Type == Variant::DataType::Float) {
					left->Set(ctx, --v.Float);
					return v.Float;
				}
				if (v.Type == Variant::DataType::Int) {
					left->Set(ctx, --v.Int);
					return v.Int;
				}
				if (v.Type == Variant::DataType::Long) {
					left->Set(ctx, --v.Long);
					return v.Long;
				}
				throw std::exception("Bad input.");
			} break;
			default:
				break;
			}
			return {};
		}
	};
	class StatementBlock : public Statement {
	public:
		StatementBlock(std::vector<Statement*> expression)
			: expressions(expression) {}

		void Execute(ScriptContext& ctx) override {
			for (auto exp : expressions) {
				exp->Execute(ctx);
				if (!ctx.ShouldRun())
					return;
			}
		}

	private:
		std::vector<Statement*> expressions;
	};
	class ReturnStatement : public Statement {
	public:
		ReturnStatement(Expression* expression)
			: expression_(expression) {}

		Expression* getExpression() const {
			return expression_;
		}
		void Execute(ScriptContext& ctx) override {
			if (expression_ != 0)
				ctx.DoReturn(expression_->Eval(ctx));
			else
				ctx.DoReturn({});
		}

	private:
		Expression* expression_;
	};
	class IfStatement : public Statement {
	public:
		IfStatement(Expression* condition, Statement* thenStatement, Statement* elseStatement)
			: condition_(condition), thenStatement_(thenStatement), elseStatement_(elseStatement) {}

		Expression* getCondition() const {
			return condition_;
		}

		Statement* getThenStatement() const {
			return thenStatement_;
		}

		Statement* getElseStatement() const {
			return elseStatement_;
		}
		void Execute(ScriptContext& ctx) override {
			if (condition_->Eval(ctx)) {
				if (thenStatement_ != nullptr)
					thenStatement_->Execute(ctx);
			}
			else {
				if (elseStatement_ != nullptr)
					elseStatement_->Execute(ctx);
			}
		}

	private:
		Expression* condition_;
		Statement* thenStatement_;
		Statement* elseStatement_;
	};
	class WhileStatement : public Statement {
	public:
		WhileStatement(Expression* condition, Statement* bodyStatement)
			: condition_(condition), Statements(bodyStatement) {}

		Expression* getCondition() const {
			return condition_;
		}

		void Execute(ScriptContext& ctx) override {
			while (condition_->Eval(ctx)) {
				Statements->Execute(ctx);
				if (ctx.Status == ScriptContext::ScriptStatus::Break) {
					ctx.ResetStatus();
					break;
				}
				if (ctx.Status == ScriptContext::ScriptStatus::Continue) {
					ctx.ResetStatus();
					continue;
				}
				if (!ctx.ShouldRun())
					break;
			}
		}

	private:
		Expression* condition_;
		Statement* Statements;
	};
	class ForStatement : public Statement {
	public:
		ForStatement(Expression* startExpression, Expression* endExpression, Expression* stepExpression, Statement* bodyStatement)
			: startExpression_(startExpression), endExpression_(endExpression), stepExpression_(stepExpression), bodyStatement_(bodyStatement) {}
		Expression* getStartExpression() const {
			return startExpression_;
		}

		Expression* getEndExpression() const {
			return endExpression_;
		}

		Expression* getStepExpression() const {
			return stepExpression_;
		}

		Statement* getBodyStatement() const {
			return bodyStatement_;
		}
		void Execute(ScriptContext& ctx) override {
			__debugbreak();
		}

	private:
		Expression* startExpression_;
		Expression* endExpression_;
		Expression* stepExpression_;
		Statement* bodyStatement_;
	};
	class ThrowStatement : public Statement {
	public:
		ThrowStatement(Expression* expression)
			: expression_(expression) {}

		Expression* getExpression() const {
			return expression_;
		}
		void Execute(ScriptContext& ctx) override {
			auto str = expression_->Eval(ctx).ToString();
			throw std::exception(str.c_str());
		}

	private:
		Expression* expression_;
	};
	class BreakStatement : public Statement {
	public:
		BreakStatement() = default;
		void Execute(ScriptContext& ctx) override {
			ctx.DoBreak();
		}
	};
	class ContinueStatement : public Statement {
	public:
		ContinueStatement() = default;
		void Execute(ScriptContext& ctx) override {
			ctx.DoContinue();
		}
	};
}
class Parser {
public:
	Parser(std::vector<Lexer::Token> tokens) : tokens_(tokens), position_(0) {}

	AST::Program* parse() {
		AST::Program* program = new AST::Program();

		while (position_ < tokens_.size()) {
			AST::Statement* statement = parseStatement();
			if (statement != nullptr) {
				program->addStatement(statement);
			}
		}

		return program;
	}

private:
	std::vector<Lexer::Token> tokens_;
	size_t position_;

	AST::Statement* parseStatement() {
		auto stat = parseStatement_In();
		match(Lexer::TokenType::Delimiter, ";");
		return stat;
	}
	AST::Statement* parseStatement_In() {
		if (match(Lexer::TokenType::Identifier, "return")) {
			AST::Expression* expression = parseExpression();
			return new AST::ReturnStatement(expression);
		}
		else if (match(Lexer::TokenType::Identifier, "break")) {
			return new AST::BreakStatement();
		}
		else if (match(Lexer::TokenType::Identifier, "continue")) {
			return new AST::ContinueStatement();
		}
		else if (match(Lexer::TokenType::Identifier, "throw")) {
			AST::Expression* expression = parseExpression();
			if (expression != nullptr) {
				return new AST::ThrowStatement(expression);
			}
		}
		else if (match(Lexer::TokenType::Identifier, "if")) {
			AST::Expression* condition = parseExpression();
			if (condition != nullptr) {
				AST::Statement* thenStatement = parseStatement();
				if (thenStatement != nullptr) {
					AST::Statement* elseStatement = nullptr;
					if (match(Lexer::TokenType::Identifier, "else")) {
						elseStatement = parseStatement();
					}
					return new AST::IfStatement(condition, thenStatement, elseStatement);
				}
			}
		}
		else if (match(Lexer::TokenType::Identifier, "while")) {
			auto cond = parseExpression();
			if (cond != 0)
				return new AST::WhileStatement(cond, parseStatement());
		}
		else if (match(Lexer::TokenType::Identifier, "for")) {
			return new AST::ForStatement(parseExpression(), parseExpression(), parseExpression(), parseStatement());
		}
		else if (match(Lexer::TokenType::Delimiter, "{")) {
			std::vector<AST::Statement*> statements;
			while (!match(Lexer::TokenType::Delimiter, "}")) {
				auto stat = parseStatement();
				if (stat != nullptr)
					statements.push_back(stat);
			}
			return new AST::StatementBlock(statements);
		}
		return parseExpression();
	}

	AST::Expression* parseExpression() {
		return parseBinaryExpression();
	}

	AST::Expression* parseTernaryExpression() {
		AST::Expression* condition = parsePrimaryExpression();
		if (match(Lexer::TokenType::Operator, "?")) {
			AST::Expression* trueExpression = parseExpression();
			if (match(Lexer::TokenType::Operator, ":")) {
				AST::Expression* falseExpression = parseExpression();
				return new AST::TernaryExpression(condition, trueExpression, falseExpression);
			}
			else {
				// Invalid expression
				return trueExpression;
			}
		}
		return condition;
	}
	AST::UnOp unopConvert(std::string_view sv) {
		if (sv == "+") {
			return AST::UnOp::Positive;
		}
		if (sv == "-") {
			return AST::UnOp::Negative;
		}
		if (sv == "!") {
			return AST::UnOp::Not;
		}
		if (sv == "~") {
			return AST::UnOp::Bnot;
		}
		if (sv == "++") {
			return AST::UnOp::Increase;
		}
		if (sv == "--") {
			return AST::UnOp::Decrease;
		}
		return AST::UnOp::Nop;
	}
	AST::Expression* parsePrimaryExpression() {
		if (match(Lexer::TokenType::Identifier, "function")) {
			expect(Lexer::TokenType::Delimiter, "(");
			std::vector<std::string> parameters;
			std::vector<AST::Statement*> statements;

			// Parse function parameters
			while (!match(Lexer::TokenType::Delimiter, ")")) {
				// Assuming the lexer provides a method `getCurrentToken()` to get the current token
				if (match(Lexer::TokenType::Identifier)) {
					parameters.push_back(std::string(tokens_[position_ - 1].lexeme));
				}
				match(Lexer::TokenType::Delimiter, ",");
			}
			expect(Lexer::TokenType::Delimiter, "{");
			// Parse function statements
			while (!match(Lexer::TokenType::Delimiter, "}")) {
				// Assuming there is a method `parseStatement()` to parse individual statements
				AST::Statement* statement = parseStatement();
				statements.push_back(statement);
			}

			// Assuming there is a constructor for `AST::LambdaExpression` that takes the parameters and statements
			return new AST::LambdaExpression(parameters, statements);
		}
		if (match(Lexer::TokenType::Identifier)) {
			std::string_view identifier = tokens_[position_ - 1].lexeme;
			if (identifier == "var") {
				position_++;
				identifier = tokens_[position_ - 1].lexeme;
				return new AST::GlobalVariantRefExpression(std::string(identifier));
			}
			// TODO: Create an AST node for identifier expression
			return new AST::VariantRefExpression(std::string(identifier));
		}
		else if (match(Lexer::TokenType::FloatLiteral)) {
			double value = std::stod(tokens_[position_ - 1].lexeme.data());
			// TODO: Create an AST node for number expression
			Variant v{};
			v.Type = Variant::DataType::Double;
			v.Double = value;
			return new AST::NumberExpression(v);
		}
		else if (match(Lexer::TokenType::IntegerLiteral)) {
			unsigned long long value = std::stoll(tokens_[position_ - 1].lexeme.data());
			Variant v{};
			v.Type = Variant::DataType::Long;
			v.Long = value;
			return new AST::NumberExpression(v);
		}
		else if (match(Lexer::TokenType::StringLiteral)) {
			std::string sv = (std::string)tokens_[position_ - 1].lexeme;
			return new AST::StringExpression(sv);
		}
		else if (match(Lexer::TokenType::Delimiter, "(")) {
			auto pr = parseExpression();
			expect(Lexer::TokenType::Delimiter, ")");
			return pr;
		}
		else {
			auto op = unopConvert(tokens_[position_].lexeme);
			if (op == AST::UnOp::Nop)
				return nullptr;
			position_++;
			return new AST::UnaryExpression{ parsePrimaryExpression(), op };
		}
	}
	int getOperatorPrecedence(AST::BinOp op) {
		switch (op) {
		case AST::BinOp::Add:
		case AST::BinOp::Sub:
			return 98;
		case AST::BinOp::Mul:
		case AST::BinOp::Div:
			return 99;
		case AST::BinOp::Index:
		case AST::BinOp::Mov:
			return 5;
		case AST::BinOp::Greater:
		case AST::BinOp::Lesser:
		case AST::BinOp::GreaterOrEqual:
		case AST::BinOp::LesserOrEqual:
		case AST::BinOp::IsEqual:
		case AST::BinOp::NotEqual:
			return 10;
		case AST::BinOp::Band:
		case AST::BinOp::Bor:
		case AST::BinOp::Xor:
			return 97;
		case AST::BinOp::And:
		case AST::BinOp::Or:
			return 9;
		case AST::BinOp::Member:
			return 200;
		default:
			return 1;
		}
	}
	AST::BinOp opConvert(std::string_view sv) {
		if (sv == "+") {
			return AST::BinOp::Add;
		}
		if (sv == "-") {
			return AST::BinOp::Sub;
		}
		if (sv == "*") {
			return AST::BinOp::Mul;
		}
		if (sv == "/") {
			return AST::BinOp::Div;
		}
		if (sv == ".") {
			return AST::BinOp::Member;
		}
		if (sv == "!=") {
			return AST::BinOp::NotEqual;
		}
		if (sv == "==") {
			return AST::BinOp::IsEqual;
		}
		if (sv == ">") {
			return AST::BinOp::Greater;
		}
		if (sv == "<") {
			return AST::BinOp::Lesser;
		}
		if (sv == ">=") {
			return AST::BinOp::GreaterOrEqual;
		}
		if (sv == "<=") {
			return AST::BinOp::LesserOrEqual;
		}
		if (sv == "&") {
			return AST::BinOp::Band;
		}
		if (sv == "|") {
			return AST::BinOp::Bor;
		}
		if (sv == "&&") {
			return AST::BinOp::And;
		}
		if (sv == "||") {
			return AST::BinOp::Or;
		}
		if (sv == "^") {
			return AST::BinOp::Xor;
		}
		return AST::BinOp::Nop;
	}
	AST::Expression* parseBinaryExpression(int precedence = 0) {
		AST::Expression* left = parseTernaryExpression();

		while (position_ < tokens_.size()) {
			if (match(Lexer::TokenType::Delimiter, "(")) {
				std::vector<AST::Expression*> expressions{};
				while (true) {
					if (match(Lexer::TokenType::Delimiter, ")")) {
						left = new AST::CallExpression{ left, expressions };
						break;
					}
					match(Lexer::TokenType::Delimiter, ",");
					expressions.push_back(parseExpression());
				}
			}
			else if (match(Lexer::TokenType::Delimiter, "[")) {
				if (getOperatorPrecedence(AST::BinOp::Index) <= precedence) {
					position_--;
					break;
				}
				auto index = parseExpression();
				expect(Lexer::TokenType::Delimiter, "]");
				left = new AST::IndexExpression{ left, index };
			}
			else if (match(Lexer::TokenType::Operator, "=")) {
				if (getOperatorPrecedence(AST::BinOp::Mov) <= precedence) {
					position_--;
					break;
				}
				auto right = parseExpression();
				if (right == nullptr) {
					throw std::exception("Expect expression on right.");
				}
				left = new AST::BinaryExpression{ left, AST::BinOp::Mov, right };
			}
			else {
				Lexer::Token operatorToken = tokens_[position_];
				auto binop = opConvert(operatorToken.lexeme);
				if (binop == AST::BinOp::Nop)
					break;
				int operatorPrecedence = getOperatorPrecedence(binop);
				if (operatorPrecedence <= precedence) {
					break;
				}

				position_++;

				AST::Expression* right = parseBinaryExpression(operatorPrecedence);
				left = new AST::BinaryExpression(left, binop, right);
			}
		}

		return left;
	}
	bool match(Lexer::TokenType type) {
		if (position_ < tokens_.size() && tokens_[position_].type == type) {
			position_++;
			return true;
		}

		return false;
	}

	bool match(Lexer::TokenType type, std::string_view lexeme) {
		if (position_ < tokens_.size() && tokens_[position_].type == type && tokens_[position_].lexeme == lexeme) {
			position_++;
			return true;
		}

		return false;
	}
	void expect(Lexer::TokenType type, std::string_view lexeme) {
		if (position_ < tokens_.size() && tokens_[position_].type == type && tokens_[position_].lexeme == lexeme) {
			position_++;
			return;
		}
		throw std::exception("Expect a token.");
	}
};