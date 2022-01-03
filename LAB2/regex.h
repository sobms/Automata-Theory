#include "RegexToDFA.h"

namespace RegexLib {
	class DFA {
	private:
		state* start_state;
		std::vector<state*> getting_states;
		std::vector<state*> states;
	public:
		DFA(state* start_state, std::vector<state*> getting_states, std::vector<state*> states) :
			start_state(start_state), getting_states(getting_states), states(states)
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
	};
	class regex : public ST_to_DFA_transformer {
	private:
	public:
		regex() : ST_to_DFA_transformer() {}
		DFA* compile(std::string expr);
		std::set<std::string> find_all(std::string str, std::string regexpr);
		std::set<std::string> find_all(std::string str, DFA* dfa);
	};
}