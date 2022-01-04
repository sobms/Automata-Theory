#include "RegexToDFA.h"

namespace RegexLib {
	class DFA {
	private:
		state* start_state;
		std::vector<state*> getting_states;
		std::vector<state*> states;
		std::set<std::string> alphabet;
	public:
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
	};
	class regex : public ST_to_DFA_transformer {
	private:
	public:
		regex() : ST_to_DFA_transformer() {}
		DFA* compile(std::string expr);
		std::set<std::string> find_all(std::string str, std::string regexpr);
		std::set<std::string> find_all(std::string str, DFA* dfa);
		std::string DFAtoRE(DFA* dfa);
	};
}