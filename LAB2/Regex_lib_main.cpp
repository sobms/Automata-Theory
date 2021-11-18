#pragma once
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <gvc.h>

namespace RegexLib {
	std::string graph = "digraph graphname {";
	std::set<std::string> special_symbols = { "|", "*", "\\", "[", "]", "{","}" };
	enum Tag
	{
		None,
		A_node,
		Star_node,
		Or_node,
		And_node,
		Capture_group_node,
		Expr_repeat
	};
	typedef struct Node {
		Node* left;
		Node* right;
		Node* parent;
		Tag tag;
		std::string symb; 
		Node(std::string symbol, Tag t, Node* l = nullptr, Node* r = nullptr, Node* p = nullptr):
			symb(symbol), tag(t), left(l), right(r), parent(p) {}
	};

	class SyntaxTree {
	private:
		Node* root;

	public:
		SyntaxTree(Node* r = nullptr) :
			root(r) {}

		Node* re2tree(std::string expr) {
			std::vector<Node*> expr_vec;

			expr = "(" + expr + ")"; //0
			for (int i = 0; i < expr.length(); i++) { //string to vector(tokenkization)
				Node* node = new Node(std::string(1, expr[i]), None);
				expr_vec.push_back(node);
			}
			int ind_lbracket, ind_rbracket;
			do {
				ind_lbracket = -1, ind_rbracket = -1;
				for (int i = 0; i < expr_vec.size(); i++) {
					if (expr_vec[i]->symb == "(")
					{
						ind_lbracket = i;
					}
					else if ((expr_vec[i]->symb == ")") && (ind_lbracket != -1))
					{
						ind_rbracket = i;
						break;
					}
				} // get closest pairs
				for (int i = ind_lbracket + 1; i < ind_rbracket; i++) {
					if ((special_symbols.find(expr_vec[i]->symb) == special_symbols.end()) && (expr_vec[i]->tag == None)) { // find symbol
						expr_vec[i]->tag = A_node;
					}
				} // create A_nodes

				int flag = 0;
				for (int i = ind_lbracket + 1; i < ind_rbracket; i++) {
					if ((expr_vec[i]->symb == "[") && ((flag == 0))) {
						expr_vec.erase(expr_vec.begin() + i);
						flag = 1;
						ind_rbracket--;
					}
					else if (flag != 0) {
						if (expr_vec[i]->symb != "]") {
							Node* CG_node = new Node(" ", Capture_group_node, expr_vec[i - 1], expr_vec[i]);
							expr_vec[i]->parent = CG_node;
							expr_vec[i - 1]->parent = CG_node;
							expr_vec[i] = CG_node;
							expr_vec.erase(expr_vec.begin() + i - 1);
							//throw exception if unshielded brackets inside Capture Group
						}
						else {
							expr_vec.erase(expr_vec.begin() + i);
							flag = 0;
						}
						i--;
						ind_rbracket--;
						
					}
				} // create Capture Group Node

				for (int i = ind_lbracket + 1; i < ind_rbracket; i++) {
					if (expr_vec[i]->symb == "*") { // find stars
						expr_vec[i]->left = expr_vec[i - 1];
						expr_vec[i]->tag = Star_node;
						expr_vec[i - 1]->parent = expr_vec[i];
						expr_vec.erase(expr_vec.begin() + i - 1);
						ind_rbracket--; //shift right border
						i--;
					}
				} // create star_nodes

				/*std::string count;
				flag = 0;
				Node* expr_repeat = nullptr;
				for (int i = ind_lbracket + 1; i < ind_rbracket; i++) {
					if (expr_vec[i]->symb == "{") {
						flag = 1;
						expr_repeat = new Node("", Expr_repeat, expr_vec[i - 1], nullptr);
						expr_vec.erase(expr_vec.begin() + i);
						expr_vec[i - 1] = expr_repeat;
						ind_rbracket--;
						i--;
						continue;
					}
					if ((flag) && (expr_vec[i]->symb != "}")) {
						count += expr_vec[i]->symb;
						expr_vec.erase(expr_vec.begin() + i);
						ind_rbracket--;
						i--;
					}
					else if ((expr_vec[i]->symb == "}") && (flag)) {
						flag = 0;
						expr_repeat->symb = count;
						expr_vec.erase(expr_vec.begin() + i);
						count.clear();
					}
				} // create expr_repeat*/

				for (int i = ind_lbracket + 1; i < ind_rbracket; i++) {
					if ((expr_vec[i - 1]->tag != None) &&
						!((special_symbols.find(expr_vec[i]->symb) != special_symbols.end()) && (expr_vec[i]->tag == None))) { // find and
						Node* and_node = new Node("&", And_node, expr_vec[i - 1], expr_vec[i]);
						expr_vec[i]->parent = and_node;
						expr_vec[i - 1]->parent = and_node;
						expr_vec[i] = and_node;
						expr_vec.erase(expr_vec.begin() + i - 1);
						ind_rbracket--;
						i--;
					}
				} // create and nodes

				for (int i = ind_lbracket + 1; i < ind_rbracket; i++) {
					if (expr_vec[i]->symb == "|") { // find or
						expr_vec[i + 1]->parent = expr_vec[i];
						expr_vec[i - 1]->parent = expr_vec[i];
						expr_vec[i]->left = expr_vec[i - 1];
						expr_vec[i]->right = expr_vec[i + 1];
						expr_vec[i]->tag = Or_node;
						expr_vec.erase(expr_vec.begin() + i + 1);
						expr_vec.erase(expr_vec.begin() + i - 1);
						ind_rbracket -= 2;//check
						i--;
					}
				} // create or nodes
				if ((ind_rbracket != -1) && (ind_lbracket != -1)) {
					expr_vec.erase(expr_vec.begin() + ind_rbracket);
					expr_vec.erase(expr_vec.begin() + ind_lbracket);
				}
			} while ((ind_lbracket != -1) && (ind_rbracket != -1));
			root = expr_vec[0];
			return root;
		}
		void bypass(Node* ptr, int idx, int prev) {
			std::map <int, std::string> tag = { {A_node, "A_node"}, {Star_node, "Star_node"}, {Or_node, "Or_node"}, 
				{And_node, "And_node"}, {Capture_group_node, "Capture_group_node"}, {Expr_repeat, "Expr_repeat"} };
			if (ptr->parent) {
				graph += std::to_string(prev) + "->" + std::to_string(idx) + ";\n";
			}
			prev = idx;
			if (ptr->left) {
				bypass(ptr->left, 2*idx, prev); //instead of "2" you can put amount of children
			}
			if (ptr->right) {
				bypass(ptr->right, 2*idx+1, prev);
			}
			graph += std::to_string(prev) + " [label=\"" + std::to_string(prev) + ptr->symb + " (" + tag[ptr->tag] + ")" + "\"];\n";
		}
		void getTreeImg() {
			Node* ptr = root;
			bypass(ptr, 1, 1);
			graph += "}";
			Agraph_t* Graph = agmemread(graph.c_str());
			GVC_t* gvc = gvContext();
			gvLayout(gvc, Graph, "dot");
			int res = gvRenderFilename(gvc, Graph, "jpeg", "\\Users\\79192\\Desktop\\Automata_Theory\\LAB2\\out.jpg");
			agclose(Graph);
		}
	};
	
}

using namespace RegexLib;
int main()
{
	SyntaxTree tree = SyntaxTree();
	Node* root = tree.re2tree("me|phi((a[bcfge]d*)*si)");//{1024}
	tree.getTreeImg();
	return 0;
}