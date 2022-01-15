#include "regex.h"
#include <iostream>
#include <algorithm>

namespace RegexLib {

	DFA* regex::compile(std::string expr) {
		SyntaxTree tree = SyntaxTree();
		Node* root = tree.re2tree(expr);
		ST_to_DFA_transformer trans = ST_to_DFA_transformer(tree.get_cap_group_ptrs(), root);
		set_cap_group_ptrs(tree.get_cap_group_ptrs());
		trans.numerate();
		trans.Calc_attributes(root);
		tree.getTreeImg();
		trans.transform();
		trans.getDFAImg();
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
		int i = 0;
		int reserve_idx = -1;
		int sz = str.size();
		while (i < sz) {
			std::string symb = str.substr(i, 1);
			if (temp_state->transitions.count(symb) > 0) {
				//making the transition
				temp_state = temp_state->transitions[symb]->end;
				//save reserve idx
				if ((temp_state != dfa->get_start_state()) && (reserve_idx == -1)) {
					reserve_idx = i+1;
				}
				//if state is not error state then save temporary string
				if (temp_state->positions.size() > 0) {
					temp_str.push_back(str.at(i));
				}
				//if state is error state
				else {
					
					//if correct string is not null then save it
					if (correct_str.size() > 0) {
						correct_string_set.insert(correct_str);
						reserve_idx = -1;
						correct_str.clear();
						temp_str.clear();
						//return automaton to start state
						temp_state = dfa->get_start_state();
						i--;
					}
					//if correct string is null then return to last reserve idx
					else {
						i = reserve_idx;
						reserve_idx = -1;
						temp_state = dfa->get_start_state();
						temp_str.clear();
						continue;
					}
				}
			}
			// if there is no transition by the symbol from this state
			else {
				//if correct string is not null then save temporary string in correct string
				if (correct_str.size() > 0) {
					correct_string_set.insert(correct_str);
					correct_str.clear();
					reserve_idx = -1;
					temp_str.clear();
					//return automaton to start state
					temp_state = dfa->get_start_state();
					i--;
				}
				//if correct string is null then return to last reserve idx
				else {
					
					temp_state = dfa->get_start_state();
					temp_str.clear();
					
				}
			}
			//save temporary string to correct string if automaton in getting state
			if (temp_state->isgetting) {
				correct_str = temp_str;
			}
			i++;
			if ((i == str.size()) && (correct_str.size() == 0) && (reserve_idx < str.size())) {
				i = reserve_idx;
				temp_state = dfa->get_start_state();
				temp_str.clear();
				reserve_idx = -1;
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
	/////////////////////////
	matches* regex::find_all2(std::string regex, std::string str) {
		matches* m = new matches();
		m->re2tree(regex);
		m->set_string(str);
		return m;
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
		regexpr.clear();
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
		if (!RegExpr.empty()) {
			RegExpr.pop_back();
		}
		else {
			RegExpr = ""; //empty set of language (not empty string)
		}

		return RegExpr;
	}

	void inversion_bypass(Node* ptr) {
		if (ptr->tag == Or_node) {
			inversion_bypass(ptr->left);
			inversion_bypass(ptr->right);
		} 
		else if (ptr->tag == And_node) {
			Node* temp_ptr = ptr->left;
			ptr->left = ptr->right;
			ptr->right = temp_ptr;
			inversion_bypass(ptr->left);
			inversion_bypass(ptr->right);
		}
		else if (ptr->tag == Star_node) {
			inversion_bypass(ptr->left);
		}
		else if (ptr->tag == Choise_group_node) {
			inversion_bypass(ptr->left);
			inversion_bypass(ptr->right);
		}
	}

	DFA* dfa_production(DFA* L, DFA* M) {
		DFA* prod = new DFA();
		std::map<std::pair<state*, state*>, state*> state_map;
		int id = 0;
		for (auto st1 : L->get_states()) {
			for (auto st2 : M->get_states()) {
				//create state
				state* st = new state();
				st->isgetting = false;
				//fill positions
				st->positions.insert(id);
				id++;
				st->transitions.clear();
				//set start state
				if ((st1 == L->get_start_state()) && (st2 == M->get_start_state())) {
					prod->set_start_state(st);
				}
				//set getting states
				if (st1->isgetting && (!st2->isgetting)) {
					st->isgetting = true;
					std::vector<state*> states_vec = prod->get_getting_states();
					states_vec.push_back(st);
					prod->set_getting_states(states_vec);
				}
				//add to state map
				state_map[std::make_pair(st1, st2)] = st;
				//set states
				std::vector<state*> states_vec = prod->get_states();
				states_vec.push_back(st);
				prod->set_states(states_vec);
			}
		}
		//intersect alphabets
		std::set<std::string> alphabet, alphabet1, alphabet2;
		alphabet1 = L->get_alphabet();
		alphabet2 = M->get_alphabet();
		std::set_intersection(alphabet1.begin(), alphabet1.end(),
			alphabet2.begin(), alphabet2.end(),
			std::inserter(alphabet, alphabet.begin()));
		prod->set_alphabet(alphabet);
		for (auto st1 : L->get_states()) {
			for (auto st2 : M->get_states()) {
				for (auto a : alphabet) {
					transition* rib = new transition(a, state_map[std::make_pair(st1, st2)], state_map[std::make_pair(st1->transitions[a]->end,
						st2->transitions[a]->end)]);
					state_map[std::make_pair(st1, st2)]->transitions[a] = rib;
				}
			}
		}
		return prod;
	}

	std::string regex::inversion(std::string regexpr) {
		Node* root = re2tree(regexpr);
		inversion_bypass(root);
		return tree2re(root);
	}

	std::string regex::inversion(DFA* dfa) {
		return inversion(DFAtoRE(dfa));
	}

	std::string regex::complement(DFA* dfa) {
		std::set<std::string> alphabet = dfa->get_alphabet();
		std::set<std::string> special_symbols = { "|", "*", "\\", "[", "]", "{","}", "#", "(", ")" };
		std::string universum_L = "(";

		if (alphabet.size() == 0) {
			universum_L = "(#$)"; //because alphabet is empty
		}
		for (auto a : alphabet) {
			if (special_symbols.find(a) != special_symbols.end()) {
				a = "\\" + a;
			}
			universum_L += a + "|";
		}
		if (alphabet.size()) {
			universum_L.pop_back();
			universum_L += ")*";
		}
		DFA* dfa_universum = compile(universum_L);
		DFA* difference = dfa_production(dfa_universum, dfa);

		return DFAtoRE(difference);
	}

	std::string regex::complement(std::string regexpr) {
		DFA* dfa = compile(regexpr);
		return complement(dfa);
	}
	//////////////////////////
	std::set<std::string> matches::operator[] (int idx) {
		Node* main_root = get_root();  // get root of tree
		std::vector<Node*> group_ptrs = get_cap_group_ptrs();
		if (idx > group_ptrs.size()) {
			throw std::exception("Index out of range.");
		}
		else if (idx == 0) {
			ST_to_DFA_transformer(get_cap_group_ptrs(), get_root());
			numerate();
			Calc_attributes(get_root());
			getTreeImg();
			transform();
			getDFAImg();
			minimize();
			getDFAImg();
			DFA* dfa = new DFA(get_start_state(), get_getting_states(), get_states(), get_alphabet());
			clear();
			return find_all(str, dfa);
		}
		Node* root = group_ptrs[idx-1]; // get root of subtree

		Node* parent = root->parent;	//remember real parent
		Node* end_node = new Node("$", A_node, nullptr, nullptr);
		Node* fake_root = new Node("&", And_node, root, end_node);
		root->parent = fake_root;
		end_node->parent = fake_root;

		set_root(fake_root); //!!!

		ST_to_DFA_transformer(get_cap_group_ptrs(), get_root());
		numerate();
		Calc_attributes(get_root());
		getTreeImg();
		transform();
		getDFAImg();
		minimize();
		getDFAImg();
		DFA* cap_group_dfa = new DFA(get_start_state(), get_getting_states(), get_states(), get_alphabet());
		clear();

		//return all back
		delete end_node;
		delete fake_root;
		root->parent = parent;
		set_root(main_root);

		return find_all(str, cap_group_dfa);
	}
}