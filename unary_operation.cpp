#pragma once

#include <functional>
#include "expression.cpp"
#include "variable.cpp"

using namespace std;

class unary_operation : public expression {
public:
	expression * expr;

	unary_operation() {}
	
	unary_operation(expression * expr) : expr(expr) {}

	string toString() override {

		if (!this->ex_string.empty())
			return this->ex_string;

		/// toString for exercise A
		//string result = "(!" + expr->toString() + ")";

		string result = "!" + expr->toString();

		this->ex_string = result;
		this->ex_hash = hash<string>()(result);

		return result;
	}

};
