#include <string>
#include <set>
#include <vector>

namespace RegexLib {

	enum Tag
	{
		None,
		A_node,
		Empty_str,
		Star_node,
		And_node,
		Or_node,
		Choise_group_node,
		Repeat_node,
		Capture_group_node
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
		Node(const Node& node) : symb(node.symb), tag(node.tag), id_pos(node.id_pos), left(nullptr),
			right(nullptr), parent(nullptr), nullable(node.nullable) {}
		bool operator ==(Node& node2) {
			if ((left == node2.left) && (right == node2.right) && (parent == node2.parent) &&
				(tag == node2.tag) && (nullable == node2.nullable) && (first == node2.first) &&
				(last == node2.last) && (symb == node2.symb) && (id_pos = node2.id_pos)) {
				return true;
			}
			else {
				return false;
			}
		}
	};

	class SyntaxTree {
	std::string graph = "digraph graphname {";
	std::set<std::string> special_symbols = { "|", "*", "\\", "[", "]", "{","}", "#", "(", ")" };
	std::vector<Node*> cap_group_ptrs;
	private:
		Node* root;

	public:
		SyntaxTree(Node* r = nullptr) : root(r) {}
		SyntaxTree(std::vector<Node*> cap_group_ptrs, Node* r = nullptr) :
			root(r), cap_group_ptrs(cap_group_ptrs) {}

		Node* get_root() { return root; }
		std::vector<Node*> get_cap_group_ptrs() {
			return cap_group_ptrs;
		}
		void set_cap_group_ptrs(std::vector<Node*> group_ptrs) {
			cap_group_ptrs = group_ptrs;
		}
		void set_root(Node* value) { root = value; }
		Node* re2tree(std::string expr);
		std::string tree2re(Node* root);
		void bypass(Node* ptr, int idx, int prev);
		void reveal_repeat_nodes(Node& ptr);
		Node* copy_tree(Node* node, std::set<int> revealed_nodes);
		Node* reveal_cap_group_nodes(int num, std::set<int> revealed_nodes);
		void set_cap_group(int num, Node* node);
		void getTreeImg();
	};
}