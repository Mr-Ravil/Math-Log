#pragma once

#include <string>
#include <functional>
#include "expression.cpp"

using namespace std;

class variable : public expression {
public:
	string var;

	variable() {}
	
	variable(string var) : var(var) {}

	string toString() override {

		if (!this->ex_string.empty())
			return this->ex_string;

		this->ex_string = var;
		this->ex_hash = hash<string>()(var);

		return var;
	}

};
