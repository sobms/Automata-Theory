#include "Syntax_tree.h"
#include <map>
#include <vector>

namespace RegexLib {
	typedef struct state {
		std::map<std::string, struct transition*> transitions;
		std::set<long> positions;
	};
	typedef struct transition {
		std::string symbol; //возможно избыточность
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
	public:
		ST_to_DFA_transformer(Node* root, state* st = nullptr) : SyntaxTree(root), start_state(st){}
		void numerate(); 
		void set_nullable(Node* temp_node);
		void set_first(Node* temp_node);
		void set_last(Node* temp_node);
		void set_follow_pos(Node* temp_node);
		void Calc_attributes(Node* ptr);
		std::map<std::string, std::set<long>> get_regex_map();
		void transform();
		void getDFAImg();
	};

}
