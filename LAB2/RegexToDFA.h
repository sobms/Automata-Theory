#include "Syntax_tree.h"
#include <map>
#include <vector>

namespace RegexLib {
	typedef struct state {
		std::map<std::string, struct transition*> transitions;
		std::set<long> positions;
		bool isgetting;
		state(bool isgetting = false) : isgetting(isgetting) {}
		state(bool isgetting, std::map<std::string, struct transition*> transitions, std::set<long> positions) : 
			isgetting(isgetting), transitions(transitions), positions(positions) {}
		state(const state& st) : transitions(st.transitions), positions(st.positions), isgetting(st.isgetting) {}
	};
	typedef struct transition {
		std::string symbol; 
		state* start;
		state* end;
		transition(std::string symbol = "\0", state* start = nullptr, state* end = nullptr) :
			start(start), end(end), symbol(symbol){}
	};

	class ST_to_DFA_transformer: public SyntaxTree {
	private:
		state* start_state;
		std::vector<state*> getting_states;
		std::map<long, std::set<long>> followPos;
		std::set<std::string> alphabet;
		std::vector<state*> states;
	public:

		ST_to_DFA_transformer(Node* root = nullptr, state* st = nullptr) : SyntaxTree(root), start_state(st){}
		
		state* get_start_state() {
			return start_state;
		}
		std::vector<state*> get_getting_states() {
			return getting_states;
		}
		std::vector<state*> get_states() {
			return states;
		}
		std::set<std::string> get_alphabet() {
			return alphabet;
		}
		void numerate(); 
		void set_nullable(Node* temp_node);
		void set_first(Node* temp_node);
		void set_last(Node* temp_node);
		void set_follow_pos(Node* temp_node);
		void Calc_attributes(Node* ptr);
		std::map<std::string, std::set<long>> get_regex_map();
		void transform();
		state* union_states(std::vector<state*> group);
		void minimize();
		void getDFAImg();
	};
}
