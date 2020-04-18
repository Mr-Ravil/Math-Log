#pragma once

#include <string>

using namespace std;

class expression {
public:
	size_t ex_hash;
	string ex_string;

	expression() = default;

	string virtual toString() = 0;

	size_t get_hash() {
		if (ex_string.empty())
			this->toString();
		return ex_hash;
	}

	bool operator==(expression& a) {
		return this->get_hash() == a.get_hash() && this->toString() == a.toString();
	}

	bool operator!=(expression& a) {
		return !(*this == a);
	}

	bool operator<(expression& a) {
		return this->get_hash() < a.get_hash();
	}
};
