#pragma once

#include <functional>
#include "expression.cpp"

using namespace std;

enum bin_operation {
	B_AND, B_OR, B_IMPLY
};

class binary_operation : public expression {
public:
	bin_operation type;
	expression * expr_left;
	expression * expr_right;

	binary_operation() {}

	binary_operation(bin_operation type, expression * expr_left, expression * expr_right) :
		type(type), expr_left(expr_left), expr_right(expr_right) {}
	
	string toString() override {

		if (!this->ex_string.empty())
			return this->ex_string;

		/// toString for exercise A
		//string result = "(";
		//switch (type) {
		//case B_AND:
		//	result += "&,";
		//	break;
		//case B_OR:
		//	result += "|,";
		//	break;
		//case B_IMPLY:
		//	result += "->,";
		//	break;
		//}
		//result += expr_left->toString() + "," + expr_right->toString() + ")";

		string result = "(" + expr_left->toString();
		switch (type) {
		case B_AND:
			result += " & ";
			break;
		case B_OR:
			result += " | ";
			break;
		case B_IMPLY:
			result += " -> ";
			break;
		}
		result += expr_right->toString() + ")";

		this->ex_string = result;
		this->ex_hash = hash<string>()(result);

		return result;
	}
};
