#include <string> 
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <ctype.h>
#include <cctype>

#include "expression.cpp"
#include "variable.cpp"
#include "unary_operation.cpp"
#include "binary_operation.cpp"
#include "expression_parser.cpp"

using namespace std;

class evidence_parser {
private:
	class hash_map;

	vector <expression*> axioms;
	map <hash_map, size_t> hypotheses;
	expression* conclusion;

	vector <expression*> all_evidence;

	enum type_statement {
		HYPOTHESIS,
		AXIOM,
		MP
	};

	class hash_map {
	public:
		size_t ex_hash;
		string ex_string;

		hash_map(size_t ex_hash, string ex_string) : ex_hash(ex_hash), ex_string(ex_string) {}

		bool operator==(const hash_map& a) const {
			return this->ex_hash == a.ex_hash && this->ex_string == a.ex_string;
		}

		bool operator!=(const hash_map& a) const {
			return !(*this == a);
		}

		bool operator<(const hash_map& a) const {
			return this->ex_hash < a.ex_hash || (this->ex_hash == a.ex_hash && this->ex_string < a.ex_string);
		}


	};

public:

	class statement;

	class description {
	public:
		type_statement type;
		size_t number;
		statement * stat1 = nullptr;
		statement * stat2 = nullptr;

		description(type_statement type, statement *  stat1, statement *  stat2) : type(type), stat1(stat1), stat2(stat2) {}

		description(type_statement type, size_t number) : type(type), number(number) {}

		size_t get_count() {
			return type == MP ? stat1->count + stat2->count + 1 : 1;
		}

		string toString(size_t place) {
			string result = "[" + to_string(place) + ". ";
			switch (type) {
			case HYPOTHESIS:
				result += "Hypothesis " + to_string(number) + "] ";
				break;
			case AXIOM:
				result += "Ax. sch. " + to_string(number) + "] ";
				break;
			case MP:
				result += "M.P. " + to_string(stat1->place) + ", " + to_string(stat2->place) + "] ";
				break;
			default:
				break;
			}
			return result;
		}
	};

	class statement {
	public:
		expression * expr;
		description * desc;
		size_t count;
		size_t place;

		statement(expression * expr, description * desc, size_t place) : expr(expr), desc(desc), place(place) {
			count = desc->get_count();
		}

		string toString() {
			return desc->toString(place) + expr->toString();
		}
	};

private:

	void create_axioms() {
		axioms.resize(10);
		axioms[0] = parser.parse("A->B->A");
		axioms[1] = parser.parse("(A->B)->(A->B->C)->(A->C)");
		axioms[2] = parser.parse("A->B->A&B");
		axioms[3] = parser.parse("A&B->A");
		axioms[4] = parser.parse("A&B->B");
		axioms[5] = parser.parse("A->A|B");
		axioms[6] = parser.parse("B->A|B");
		axioms[7] = parser.parse("(A->C)->(B->C)->(A|B->C)");
		axioms[8] = parser.parse("(A->B)->(A->!B)->!A");
		axioms[9] = parser.parse("!!A->A");
	}

	expression * A;
	expression * B;
	expression * C;

	bool check_part(expression * axiom, expression * expr) {
		if (typeid(*axiom) == typeid(variable)) {
			if (axiom->toString() == "A") {
				if (A) 
					return *A == *expr;
				else {
					A = expr;
					return true;
				}
			} else if (axiom->toString() == "B") {
				if (B)
					return *B == *expr;
				else {
					B = expr;
					return true;
				}
			} else if (axiom->toString() == "C") {
				if (C)
					return *C == *expr;
				else {
					C = expr;
					return true;
				}
			}
		}
		if (typeid(*axiom) == typeid(unary_operation)) 
			return typeid(*expr) == typeid(unary_operation) && check_part(((unary_operation*)axiom)->expr, ((unary_operation*)expr)->expr);
		
		return typeid(*expr) == typeid(binary_operation)
			&& ((binary_operation*)axiom)->type == ((binary_operation*)expr)->type
			&& check_part(((binary_operation*)axiom)->expr_left, ((binary_operation*)expr)->expr_left)
			&& check_part(((binary_operation*)axiom)->expr_right, ((binary_operation*)expr)->expr_right);

	}

	int isAxiom(expression * expr) {
		for (int i = 0; i < 10; i++) {
			A = nullptr;
			B = nullptr;
			C = nullptr;
			if (check_part(axioms[i], expr)) {
				return i + 1;
			}
		}
		return -1;
	}

	bool prove() {
		if (*all_evidence.back() != *conclusion)
			return false;

		map <hash_map, statement*> cur_evidence;
		map <hash_map, pair<statement*, statement*>> MPs;
		map <hash_map, vector <statement*> > unchecked_MPs;
		bool conclusion_not_proved = true;
		size_t coutner = 0;

		for (expression* expr : all_evidence) {
			bool not_proven = true;
			map <hash_map, statement*>::iterator it_evid = cur_evidence.find(hash_map(expr->get_hash(), expr->toString()));

			//statement is a hypothese
			if (hypotheses.find(hash_map(expr->get_hash(), expr->toString())) != hypotheses.end()) {
				if (it_evid == cur_evidence.end())
					cur_evidence.insert({ hash_map(expr->get_hash(), expr->toString()), new statement(expr, new description(HYPOTHESIS, hypotheses.find(hash_map(expr->get_hash(), expr->toString()))->second), ++coutner) }), it_evid = cur_evidence.find(hash_map(expr->get_hash(), expr->toString()));
				else if (it_evid->second->count > 1) 
					it_evid->second = new statement(expr, new description(HYPOTHESIS, hypotheses.find(hash_map(expr->get_hash(), expr->toString()))->second), ++coutner);
				not_proven = false;
			}

			//statement is a axiom
			int numAxiom = isAxiom(expr);
			if (numAxiom != -1) {
				if (it_evid == cur_evidence.end())
					cur_evidence.insert({ hash_map(expr->get_hash(), expr->toString()) , new statement(expr, new description(AXIOM, numAxiom), ++coutner) }), it_evid = cur_evidence.find(hash_map(expr->get_hash(), expr->toString()));
				else if (it_evid->second->count > 1)
					it_evid->second = new statement(expr, new description(AXIOM, numAxiom), ++coutner);
				not_proven = false;
			}

			//statement is deduce for Modus Ponens
			map <hash_map, pair<statement*, statement*>>::iterator it_MP = MPs.find(hash_map(expr->get_hash(), expr->toString()));
			if (it_MP != MPs.end()) {
				if (it_evid == cur_evidence.end())
					cur_evidence.insert({ hash_map(expr->get_hash(), expr->toString()),
						new statement(expr, new description(MP, it_MP->second.first, it_MP->second.second),
							++coutner) }), not_proven = false;
				else if (it_evid->second->count > it_MP->second.first->count + it_MP->second.second->count + 1) 
					it_evid->second = new statement(expr, new description(MP, it_MP->second.first, it_MP->second.second),
						++coutner), not_proven = false;
			}

			if (not_proven && it_evid == cur_evidence.end())
				return false;

			if (*expr == *conclusion) 
				conclusion_not_proved = false;

			//statement can be used in Modus Ponens
			if (typeid(*expr) == typeid(binary_operation) &&
				((binary_operation*)expr)->type == B_IMPLY) {
				//MP (a->b, a) = b
				//MP.insert({b, {a->b, a}})
				map <hash_map, statement*>::iterator MP_right = cur_evidence.find(hash_map(((binary_operation*)expr)->expr_left->get_hash(), ((binary_operation*)expr)->expr_left->toString()));

				if (MP_right != cur_evidence.end()) {
					it_MP = MPs.find(hash_map(((binary_operation*)expr)->expr_right->get_hash(), ((binary_operation*)expr)->expr_right->toString()));
					if (it_MP == MPs.end())
						MPs.insert({ hash_map(((binary_operation*)expr)->expr_right->get_hash(), ((binary_operation*)expr)->expr_right->toString()),
							{ cur_evidence.find(hash_map(expr->get_hash(), expr->toString()))->second, cur_evidence.find(hash_map(((binary_operation*)expr)->expr_left->get_hash(), ((binary_operation*)expr)->expr_left->toString()))->second } });
					else {
						// MP1 = a->b, MP2 = a
						statement * MP1 = cur_evidence.find(hash_map(expr->get_hash(), expr->toString()))->second;
						statement * MP2 = cur_evidence.find(hash_map(((binary_operation*)expr)->expr_left->get_hash(), ((binary_operation*)expr)->expr_left->toString()))->second;
						if ((it_MP->second.first->count + it_MP->second.second->count) > (MP1->count + MP2->count))
							it_MP->second = { MP1, MP2 };
					}
				} else {
					//collect {hash(a), a->b}
					map <hash_map, vector <statement*> >::iterator it_unMP = unchecked_MPs.find(hash_map(((binary_operation*)expr)->expr_left->get_hash(), ((binary_operation*)expr)->expr_left->toString()));
					if (it_unMP == unchecked_MPs.end())
						unchecked_MPs.insert({ hash_map(((binary_operation*)expr)->expr_left->get_hash(), ((binary_operation*)expr)->expr_left->toString()), {cur_evidence.find(hash_map(expr->get_hash(), expr->toString()))->second } });
					else
						it_unMP->second.push_back(cur_evidence.find(hash_map(expr->get_hash(), expr->toString()))->second);
				}
			}

			//ckeck statement in unchecked_MPs
			map <hash_map, vector <statement*> >::iterator it_unMP = unchecked_MPs.find(hash_map(expr->get_hash(), expr->toString()));
			if (it_unMP != unchecked_MPs.end()) {
				//MP (a->b, a) = b
				//MP.insert({b, {a->b, a}})
				for (statement * MP1 : it_unMP->second) { //MP1 = a->b, expr = a

					it_MP = MPs.find(hash_map(((binary_operation*)MP1->expr)->expr_right->get_hash(), ((binary_operation*)MP1->expr)->expr_right->toString()));

					if (it_MP == MPs.end())
						MPs.insert({ hash_map(((binary_operation*)MP1->expr)->expr_right->get_hash(), ((binary_operation*)MP1->expr)->expr_right->toString()), { MP1,  cur_evidence.find(hash_map(expr->get_hash(), expr->toString()))->second } });
					else {
						// MP1 = a->b, MP2 = a
						statement * MP2 = cur_evidence.find(hash_map(expr->get_hash(), expr->toString()))->second;
						if ((it_MP->second.first->count + it_MP->second.second->count) > (MP1->count + MP2->count))
							it_MP->second = { MP1, MP2 };
					}
				}
				unchecked_MPs.erase(it_unMP);
			}
		}
		
		if (conclusion_not_proved)
			return false;

		build_evidence(cur_evidence.find(hash_map(conclusion->get_hash(), conclusion->toString()))->second);
		for (pair <size_t, statement*> it : mess_evidence) {
			statement * state = it.second;
			evidence.push_back(state);
			state->place = evidence.size();
		}

		return true;
	}

	multimap <size_t, statement*> mess_evidence;

	void build_evidence(statement * state) {
		if (mess_evidence.find(state->place) != mess_evidence.end())
			return;

		mess_evidence.insert({ state->place, state });
		if (state->desc->type == MP) {
			build_evidence(state->desc->stat1);
			build_evidence(state->desc->stat2);
		}
	}

public:
	bool provably;
	vector <statement*> evidence;
	expression_parser parser;
	string affirmation;

	evidence_parser(string & cur_affirmation, vector<string> & curs_evidence) {
		create_axioms();
		affirmation = "";
		size_t i = 0, num = 0;
		string s;
		while (i < cur_affirmation.size()) {
			if (cur_affirmation[i] == ',') {
				expression * expr = parser.parse(s);
				affirmation += expr->toString() + ", ";
				hypotheses.insert({ hash_map(expr->get_hash(), expr->toString()), ++num });
				s.clear();
			}
			else if (i + 1 < cur_affirmation.size() && cur_affirmation[i] == '|' && cur_affirmation[i + 1] == '-') {
				if (!s.empty()) {
					expression * expr = parser.parse(s);
					affirmation += expr->toString() + " ";
					hypotheses.insert({ hash_map(expr->get_hash(), expr->toString()), ++num });
					s.clear();
				}
				i++;
			}
			else if (!isspace(cur_affirmation[i]))
				s += cur_affirmation[i];
			i++;
		}
		conclusion = parser.parse(s);
		affirmation += "|- " + conclusion->toString();

		for (i = 0; i < curs_evidence.size(); i++) 
			all_evidence.push_back(parser.parse(curs_evidence[i]));

		provably = prove();
	}
};