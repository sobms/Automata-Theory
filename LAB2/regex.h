#include <iostream>
#include <tuple>
#include <algorithm>
#include <utility>
#include "RegexToDFA.h"

namespace RegexLib {

	class DFA {
	private:
		state* start_state;
		std::vector<state*> getting_states;
		std::vector<state*> states;
		std::set<std::string> alphabet;
	public:
		DFA() {}
		DFA(state* start_state, std::vector<state*> getting_states, std::vector<state*> states, std::set<std::string> alphabet) :
			start_state(start_state), getting_states(getting_states), states(states), alphabet(alphabet)
		{
			for (auto state : states) {
				state->isgetting = false;
			}
			for (auto state : getting_states) {
				state->isgetting = true;
			}
		}
		state* get_start_state() {
			return start_state;
		}
		std::vector<state*> get_states() {
			return states;
		}
		std::vector<state*> get_getting_states() {
			return getting_states;
		}
		std::set<std::string> get_alphabet() {
			return alphabet;
		}
		void set_states(std::vector<state*> new_states) {
			states = new_states;
		}
		void set_start_state(state* start_st) {
			start_state = start_st;
		}
		void set_getting_states(std::vector<state*> new_getting_states) {
			getting_states = new_getting_states;
		}
		void set_alphabet(std::set<std::string> new_alphabet) {
			alphabet = new_alphabet;
		}
	};

	class matches;

	class regex : public ST_to_DFA_transformer {
	private:
		std::map<std::tuple<long, long, long>, std::string> regexpr;
	public:
		regex() : ST_to_DFA_transformer() {}
		DFA* compile(std::string expr);
		std::set<std::string> find_all(std::string str, std::string regexpr);
		std::set<std::string> find_all(std::string str, DFA* dfa);
		matches* find_all2(std::string regex, std::string str);
		void induction_step(long start_state_id, long dest_state_id, long k, std::map<std::pair<long, long>,
			std::vector<transition*>> paths);
		std::string DFAtoRE(DFA* dfa);
		std::string inversion(DFA* dfa);
		std::string inversion(std::string regexpr);
		std::string complement(DFA* dfa);
		std::string complement(std::string regexpr);
	};
	
	class matches : public regex {
		friend class mathes_iter;
	private:
		std::string str;
	public:
		matches() : regex() {}
		void set_string(std::string s) {
			str = s;
		}
		std::set<std::string> operator[] (int idx); //remake


		typedef mathes_iter iterator;
		iterator begin();
		iterator end();
	};

	class mathes_iter {
	private:

	public:
		const mathes_iter operator ++ (int);
		matches operator * ();
		bool operator != (const mathes_iter& it);
	};
}