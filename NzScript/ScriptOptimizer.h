#pragma once
#include "ScriptAst.h"
#include <type_traits>

namespace AST {
#define MATCH(x, type) \
	if (typeid(*x) == typeid(type))
#define AS(x, type, y) type& y = *(type*)x;
	template <class T>
	void ConstReduce(ScriptContext& ctx, T*& s) {
		if constexpr (std::is_same_v<T, Program>) {
			AS(s, Program, y);
			std::vector<Statement*> Statements;
			for (auto& stat : y.statements_) {
				if (stat->IsConst(ctx))
					delete stat;
				else {
					ConstReduce(ctx, stat);
					if (stat != nullptr)
						Statements.push_back(stat);
				}
			}
			y.statements_ = Statements;
		}
		else if constexpr (std::is_same_v<T, Expression>) {
			{
				AS(s, Expression, y2);
				if (y2.IsConst(ctx)) {
					s = new NumberExpression(y2.Eval(ctx));
					return;
				}
			}
			MATCH(s, BinaryExpression) {
				AS(s, BinaryExpression, y);
				ConstReduce(ctx, y.leftExpression_);
				ConstReduce(ctx, y.rightExpression_);
			}
			else MATCH(s, CallExpression) {
				AS(s, CallExpression, y);
				if (y.IsConst(ctx)) {
					s = new NumberExpression(y.Eval(ctx));
					delete &y;
				}
				else {
					ConstReduce(ctx, y.method);
					for (auto& a : y.arguments) {
						ConstReduce(ctx, a);
					}
				}
			}
			else MATCH(s, TernaryExpression) {
				AS(s, TernaryExpression, y);
				if (y.condition->IsConst(ctx)) {
					if (y.condition->Eval(ctx)) {
						s = y.onTrue;
					}
					else {
						s = y.onFalse;
					}
				}
				else {
					ConstReduce(ctx, y.condition);
					ConstReduce(ctx, y.onTrue);
					ConstReduce(ctx, y.onFalse);
				}
			}
			else MATCH(s, LambdaExpression) {
				AS(s, LambdaExpression, y);
				std::vector<Statement*> Statements;
				for (auto& stat : y.Statements) {
					if (stat->IsConst(ctx))
						delete stat;
					else {
						ConstReduce(ctx, stat);
						if (stat != nullptr)
							Statements.push_back(stat);
					}
				}
				y.Statements = Statements;
			}
		}
		else if constexpr (std::is_same_v<T, Statement>) {
			{
				AS(s, Statement, y2);
				if (y2.IsConst(ctx)) {
					s = nullptr;
					return;
				}
			}
			MATCH(s, OutNullStatement) {
				AS(s, OutNullStatement, y);
				ConstReduce(ctx, y.expr);
			}
			else MATCH(s, ReturnStatement) {
				AS(s, ReturnStatement, y);
				ConstReduce(ctx, y.expression_);
			}
			else MATCH(s, ThrowStatement) {
				AS(s, ThrowStatement, y);
				ConstReduce(ctx, y.expression_);
			}
			else MATCH(s, StatementBlock) {
				AS(s, StatementBlock, y);
				std::vector<Statement*> Statements;
				for (auto& stat : y.expressions) {
					if (stat->IsConst(ctx))
						delete stat;
					else {
						ConstReduce(ctx, stat);
						if (stat != nullptr)
							Statements.push_back(stat);
					}
				}
				y.expressions = Statements;
			}
			else MATCH(s, IfStatement) {
				AS(s, IfStatement, y);
				// 考虑 if 的条件是否是常量表达式
				if (y.condition_->IsConst(ctx)) {
					if (y.condition_->Eval(ctx)) {
						ConstReduce(ctx, y.thenStatement_);
						s = y.thenStatement_;
					}
					else {
						if (y.elseStatement_) {
							ConstReduce(ctx, y.elseStatement_);
							s = y.elseStatement_;
						}
						else {
							s = nullptr;
						}
					}
				}
				else {
					ConstReduce(ctx, y.condition_);
					ConstReduce(ctx, y.thenStatement_);
					if (y.elseStatement_)
						ConstReduce(ctx, y.elseStatement_);
				}
			}
			else MATCH(s, WhileStatement) {
				AS(s, WhileStatement, y);
				if (y.condition_->IsConst(ctx) && !y.condition_->Eval(ctx)) {
					s = nullptr;
					return;
				}
				ConstReduce(ctx, y.condition_);
				ConstReduce(ctx, y.Statements);
			}
			else MATCH(s, ForStatement) {
				AS(s, ForStatement, y);
				if (y.endExpression_->IsConst(ctx) && !y.endExpression_->Eval(ctx)) {
					s = nullptr;
					return;
				}
				ConstReduce(ctx, y.bodyStatement_);
				ConstReduce(ctx, y.startExpression_);
				ConstReduce(ctx, y.stepExpression_);
				ConstReduce(ctx, y.endExpression_);
			}
		}
	}
}