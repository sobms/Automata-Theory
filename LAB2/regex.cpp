#include "regex.h"

namespace RegexLib {
	DFA* regex::compile(std::string expr) {
		SyntaxTree tree = SyntaxTree();
		Node* root = tree.re2tree(expr);
		ST_to_DFA_transformer trans = ST_to_DFA_transformer(root);
		trans.numerate();
		trans.Calc_attributes(root);
		tree.getTreeImg();
		trans.transform();
		trans.minimize();
		trans.getDFAImg();
		DFA* compile_obj = new DFA(trans.get_start_state(), trans.get_getting_states(), trans.get_states());
		return compile_obj;
	}
	std::set<std::string> regex::find_all(std::string str, DFA * dfa) {
		state* temp_state = dfa->get_start_state();
		std::set<std::string> correct_string_set;
		if (temp_state->isgetting) {
			correct_string_set.insert("");
		}
		std::string correct_str, temp_str;
		for (int i = 0; i < str.size(); i++) {
			std::string symb = str.substr(i, 1);
			if (temp_state->transitions.count(symb) > 0) {
				temp_state = temp_state->transitions[symb]->end;
				if (temp_state->positions.size() > 0) {
					temp_str.push_back(str.at(i));
				}
				else {
					if (temp_str.size() > 0) {
						i--;
						temp_str.erase(temp_str.begin(), temp_str.end());
					}
					if (correct_str.size() > 0) {
						correct_string_set.insert(correct_str);
					}
					correct_str.clear();
					temp_state = dfa->get_start_state();
				}
			}
			else {
				if (temp_str.size()>0) {
					i--;
					temp_str.erase(temp_str.begin(), temp_str.end());
				}
				if (correct_str.size() > 0) {
					correct_string_set.insert(correct_str);
				}
				correct_str.clear();
				temp_state = dfa->get_start_state();
			}
			if (temp_state->isgetting) {
				correct_str = temp_str;
			}
		}
		if (correct_str.size() > 0) {
			correct_string_set.insert(correct_str);
			correct_str.clear();
		}
		
		return correct_string_set;
	}

	std::set<std::string> regex::find_all(std::string str, std::string regexpr) {
		return regex::find_all(str, regex::compile(regexpr));
	}
}