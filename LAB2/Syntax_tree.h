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
		Repeat_node
	};
	typedef struct Node {
		Node* left;
		Node* right;
		Node* parent;
		Tag tag;
		std::string symb;
		Node(std::string symbol, Tag t, Node* l = nullptr, Node* r = nullptr, Node* p = nullptr) :
			symb(symbol), tag(t), left(l), right(r), parent(p) {}
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