#ifndef _SYNTAXTREE_H_
#define _SYNTAXTREE_H_
#include <string>
#include <set>
namespace RegexLib {

	enum Tag
	{
		None,
		A_node,
		Star_node,
		Or_node,
		And_node,
		Capture_group_node,
		Repeat_node,
		Empty_str
	};
	typedef struct Node {
		Node* left;
		Node* right;
		Node* parent;
		Tag tag;
		bool nullable;
		std::set<long> first;
		std::set<long> last;
		std::string symb;
		long id_pos;
		Node(std::string symbol, Tag t, Node* l = nullptr, Node* r = nullptr, Node* p = nullptr, long id = -1) :
			symb(symbol), tag(t), id_pos(id), left(l), right(r), parent(p), nullable(false) {}
	};

	class SyntaxTree {
	std::string graph = "digraph graphname {";
	std::set<std::string> special_symbols = { "|", "*", "\\", "[", "]", "{","}", "#", "(", ")" };
	private:
		Node* root;

	public:
		SyntaxTree(Node* r = nullptr) :
			root(r) {}

		Node* get_root() { return root; }
		void set_root(Node* value) { root = value; }
		Node* re2tree(std::string expr);

		void bypass(Node* ptr, int idx, int prev);

		void getTreeImg();
	};
}
#endif