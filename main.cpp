#include <iostream>
#include <string>
#include "expression.cpp"
#include "variable.cpp"
#include "unary_operation.cpp"
#include "expression_parser.cpp"
#include "evidence_parser.cpp"

using namespace std;

int main() {
	string affirmation;
	vector <string> evidence;
	getline(cin, affirmation);
	
	string s;
	while (getline(cin, s))
		evidence.push_back(s);

	evidence_parser * parser = new evidence_parser(affirmation, evidence);

	if (!parser->provably) {
		cout << "Proof is incorrect";
		return 0;
	}

	cout << parser->affirmation << "\n";
	for (evidence_parser::statement * state : parser->evidence) 
		cout << state->toString() << "\n";
	return 0;
}