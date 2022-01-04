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
	std::map<std::pair<long, long>, std::set<std::vector<transition*>>> add_k_paths(int k, std::map<std::pair<long, long>,
		std::set<std::vector<transition*>>> k_paths, long start_state_id, long dest_state_id);

	std::map<std::pair<long, long>, std::set<std::vector<transition*>>> check_k_path(int k, std::map<std::pair<long, long>,
		std::set<std::vector<transition*>>> k_paths, long start_state_id, long dest_state_id) {
		if (k == -1) {
			return k_paths;
		}
		if (k_paths[std::make_pair(start_state_id, k)].size() == 0) {
			k_paths = check_k_path(k - 1, k_paths, start_state_id, k);
		}
		if (k_paths[std::make_pair(k, dest_state_id)].size() == 0) {
			k_paths = check_k_path(k - 1, k_paths, k, dest_state_id);
		}
		if ((k_paths[std::make_pair(start_state_id, k)].size() > 0) && (k_paths[std::make_pair(k, dest_state_id)].size() > 0)) {
			for (auto p : k_paths[std::make_pair(start_state_id, k)]) {
				for (auto p2 : k_paths[std::make_pair(k, dest_state_id)]) {
					if (k_paths[std::make_pair(k, k)].size() > 0) {
						for (auto p1 : k_paths[std::make_pair(k, k)]) {
							std::vector<transition*> path;
							path.insert(path.end(), p.begin(), p.end());
							path.insert(path.end(), p1.begin(), p1.end());
							path.insert(path.end(), p2.begin(), p2.end());
							k_paths[std::make_pair(start_state_id, dest_state_id)].insert(path);
						}
					}
					else {
						std::vector<transition*> path;
						path.insert(path.end(), p.begin(), p.end());
						path.insert(path.end(), p2.begin(), p2.end());
						k_paths[std::make_pair(start_state_id, dest_state_id)].insert(path);
					}
				}	
			}
		}
		return k_paths;
	}
	std::map<std::pair<long, long>, std::set<std::vector<transition*>>> add_k_paths(int k, std::map<std::pair<long, long>, 
		std::set<std::vector<transition*>>> k_paths, long start_state_id, long dest_state_id, std::map<state*, long> dfa_states) {
		for (int i = k; i > -1; i--) {
			k_paths = check_k_path(i, k_paths, start_state_id, dest_state_id);

			std::map<std::pair<long, long>, std::set<std::vector<transition*>>>::iterator it;
			for (it = k_paths.begin(); it != k_paths.end();) {
				if (it->second.size() == 0) {
					it = k_paths.erase(it);
				}
				else {
					it++;
				}
			}
		}		
		return k_paths;
	}

	std::string regex::DFAtoRE(DFA* dfa) {
		std::string RegExpr;
		std::map<state*, long> dfa_states;
		std::map<std::pair<long,long>, std::set<std::vector<transition*>>> k_paths;
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
				std::vector<transition*> path;
				path.push_back(st->transitions[a]);
				k_paths[std::make_pair(from, to)].insert(path);
			}
		}
		state* start_state = dfa->get_start_state();
		for (auto st : getting_states) {
			k_paths = add_k_paths(k, k_paths, dfa_states[start_state], dfa_states[st], dfa_states);
			int i = 0;
			int sz = k_paths[std::make_pair(dfa_states[start_state], dfa_states[st])].size();
			for (auto path : k_paths[std::make_pair(dfa_states[start_state], dfa_states[st])]) {
				for (auto t : path) {
					if (t->start == t->end) {
						RegExpr += t->symbol + "*";
					}
					else {
						RegExpr += t->symbol;
					}
				}
				if (i < sz - 1) {
					RegExpr += "|";
					i++;
				}
			}
		}

		return RegExpr;
	}
}