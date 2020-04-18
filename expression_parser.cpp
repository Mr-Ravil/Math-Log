#pragma once

#include <string>
#include <deque>
#include <ctype.h>
#include <cctype>
#include <map>

#include "expression.cpp"
#include "variable.cpp"
#include "unary_operation.cpp"
#include "binary_operation.cpp"

using namespace std;

enum operation {
	VARIABLE,
	NOT,
	AND,
	OR,
	IMPLY,
	OPEN_BRACKET,
	CLOSE_BRACKET,
	END
};

struct expression_parser {
private:
	string cur_expr;
	size_t position;
	string value;
	operation cur_operation;

	void next_operation() {
		while (position < cur_expr.size() && isspace(cur_expr[position]))
			position++;

		if (position >= cur_expr.size()) {
			cur_operation = END;
			return;
		}
		switch (cur_expr[position]) {
		case '!':
			cur_operation = NOT;
			break;
		case '&':
			cur_operation = AND;
			break;
		case '|':
			cur_operation = OR;
			break;
		case '-':
			cur_operation = IMPLY;
			position++;
			break;
		case '(':
			cur_operation = OPEN_BRACKET;
			break;
		case ')':
			cur_operation = CLOSE_BRACKET;
			break;
		default:
			cur_operation = VARIABLE;
			value = "";
			while ((cur_expr[position] >= 'A' && cur_expr[position] <= 'Z') ||
				(cur_expr[position] >= '0' && cur_expr[position] <= '9') ||
				cur_expr[position] == '\'')
				value += cur_expr[position++];
			--position;
			break;
		}
		++position;
	}

	expression* unary() {
		next_operation();
		expression * result = nullptr;
		switch (cur_operation) {
		case VARIABLE: {
			map<string, expression*>::iterator m_var = vars.find(value);
			if (m_var != vars.end())
				result = m_var->second;
			else {
				result = new variable(value);
				vars.insert({ value, result });
			}
			next_operation();
			break;
		} case NOT:
			result = new unary_operation(unary());
			break;
		case OPEN_BRACKET:
			result = imply();
			next_operation();
			break;
		}
		return result;
	}

	expression* andB() {
		expression * result = unary();
		while (true) {
			switch (cur_operation) {
			case AND:
				result = new binary_operation(B_AND, result, unary());
				break;
			default:
				return result;
				break;
			}
		}
	}

	expression* orB() {
		expression * result = andB();
		while (true) {
			switch (cur_operation) {
			case OR:
				result = new binary_operation(B_OR, result, andB());
				break;
			default:
				return result;
				break;
			}
		}
	}

	expression* imply() {
		expression * result = orB();
		deque <expression *> q_imply;
		while (true) {
			switch (cur_operation) {
			case IMPLY:
				q_imply.push_back(result);
				result = orB();
				break;
			default:
				while (!q_imply.empty()) {
					result = new binary_operation(B_IMPLY, q_imply.back(), result);
					q_imply.pop_back();
				}
				return result;
				break;
			}
		}
	}


public:
	expression * expr;
	map<string, expression*> vars;

	expression* parse(string s) {
		cur_expr = s;
		position = 0;
		value.clear();
		expr = imply();
		return expr;
	}

	expression_parser() = default;

	expression_parser(string s) {
		parse(s);
	}

};