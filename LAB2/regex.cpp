#include "regex.h"
#include <iostream>

namespace RegexLib {
	DFA* regex::compile(std::string expr) {
		SyntaxTree tree = SyntaxTree();
		Node* root = tree.re2tree(expr);
		ST_to_DFA_transformer trans = ST_to_DFA_transformer(root);
		trans.numerate();
		trans.Calc_attributes(root);
		tree.getTreeImg();
		trans.transform();
		trans.getDFAImg();//
		trans.minimize();
		trans.getDFAImg();
		DFA* compile_obj = new DFA(trans.get_start_state(), trans.get_getting_states(), trans.get_states(), trans.get_alphabet());
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

	void regex::induction_step(long start_state_id, long dest_state_id, long k, std::map<std::pair<long, long>,
		std::vector<transition*>> paths) {
		std::string R_i_j, R_i_k, R_k_j, R_k_k;
		std::set<std::string> special_symbols = { "|", "*", "\\", "[", "]", "{","}", "#", "(", ")" };
		if (k == 0) {
			std::string str;
			if (paths[std::make_pair(start_state_id, dest_state_id)].size() > 0) {
				for (auto path : paths[std::make_pair(start_state_id, dest_state_id)]) {
					if (special_symbols.find(path->symbol) != special_symbols.end()) {
						path->symbol = "\\" + path->symbol;
					}
					str += path->symbol + "|";
				}
				str.pop_back();
				if (str.size() > 1) {
					regexpr[std::make_tuple(start_state_id, dest_state_id, k)] = "(" + str + ")";
				}
				else {
					regexpr[std::make_tuple(start_state_id, dest_state_id, k)] = str;
				}
			}
			else {
				if (start_state_id == dest_state_id) {
					regexpr[std::make_tuple(start_state_id, dest_state_id, k)] = "#";
				}
				else {
				regexpr[std::make_tuple(start_state_id, dest_state_id, k)] = "";
				}
			}
		}
		else {
			if (regexpr.count(std::make_tuple(start_state_id, dest_state_id, k - 1)) == 0) {
				induction_step(start_state_id, dest_state_id, k - 1, paths);
			}
			R_i_j = regexpr[std::make_tuple(start_state_id, dest_state_id, k - 1)];
			if (regexpr.count(std::make_tuple(start_state_id, k, k - 1)) == 0) {
				 induction_step(start_state_id, k, k - 1, paths);
			}
			R_i_k = regexpr[std::make_tuple(start_state_id, k, k - 1)];
			if (regexpr.count(std::make_tuple(k, dest_state_id, k - 1)) == 0) {
				induction_step(k, dest_state_id, k - 1, paths);
			}
			R_k_j = regexpr[std::make_tuple(k, dest_state_id, k - 1)];
			if (regexpr.count(std::make_tuple(k, k, k - 1)) == 0) {
				induction_step(k, k, k - 1, paths);
			}
			R_k_k = regexpr[std::make_tuple(k, k, k - 1)];
			if (R_i_k == "#") {
				R_i_k = "";
			}
			if (R_k_j == "#") {
				R_k_j = "";
			}
			if (R_k_k == "#") {
				R_k_k = "";
			}
			switch (4*(R_i_j.size() > 0) + 2*(R_i_k.size() > 0 && R_k_j.size() > 0) + (R_k_k.size() > 0)) {
			case (7):
				regexpr[std::make_tuple(start_state_id, dest_state_id, k)] = "(" + R_i_j + "|" + R_i_k + "(" + R_k_k + ")" + "*" + R_k_j + ")";
				break;
			case (6):
				regexpr[std::make_tuple(start_state_id, dest_state_id, k)] = "(" + R_i_j + "|" + R_i_k + R_k_j + ")";
				break;
			case (5):
				regexpr[std::make_tuple(start_state_id, dest_state_id, k)] = R_i_j;
				break;
			case (4):
				regexpr[std::make_tuple(start_state_id, dest_state_id, k)] = R_i_j;
				break;
			case (3):
				regexpr[std::make_tuple(start_state_id, dest_state_id, k)] = R_i_k + "(" + R_k_k + ")" + "*" + R_k_j;
				break;
			case (2):
				regexpr[std::make_tuple(start_state_id, dest_state_id, k)] = R_i_k + R_k_j;
				break;
			default:
				regexpr[std::make_tuple(start_state_id, dest_state_id, k)] = "";
			}
		}
	}
	
	std::string regex::DFAtoRE(DFA* dfa) {
		std::string RegExpr;
		std::map<state*, long> dfa_states;
		std::map<std::pair<long,long>, std::vector<transition*>> paths;
		std::vector<state*> states = dfa->get_states();
		std::vector<state*> getting_states = dfa->get_getting_states();
		std::set<std::string> alphabet = dfa->get_alphabet();

		int i = 1;
		for (auto st : states) { //numerate
			dfa_states.insert(std::make_pair(st, i));
			i++;
		}
		int k = dfa_states.size();
		for (auto st : states) {
			for (auto a : alphabet) {
				long from = dfa_states[st->transitions[a]->start];
				long to = dfa_states[st->transitions[a]->end];
				paths[std::make_pair(from, to)].push_back(st->transitions[a]);
			}
		}
		state* start_state = dfa->get_start_state();
		for (auto st : getting_states) {
			induction_step(dfa_states[start_state], dfa_states[st], k, paths);
			RegExpr += regexpr[std::make_tuple(dfa_states[start_state], dfa_states[st], k)] + "|";
		}
		RegExpr.pop_back();

		return RegExpr;
	}
}