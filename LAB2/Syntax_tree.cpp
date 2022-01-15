#include <iostream>
#include <ctype.h>
#include <map>
#include <vector>
#include <stack>
#include <gvc.h>
#include <algorithm>
#include "Syntax_tree.h"

namespace RegexLib {

	Node* new_nodes(Node* ptr) {
		Node* new_ptr = nullptr;
		Node* new_ptr1 = nullptr;
		Node* new_ptr2 = nullptr;
		if (ptr->left) {
			new_ptr1 = new_nodes(ptr->left);
		} 
		if (ptr->right) {
			new_ptr2 = new_nodes(ptr->right);
		}
		new_ptr = new Node(*ptr);
		new_ptr->left = new_ptr1;
		if (new_ptr1) {
			new_ptr1->parent = new_ptr;
		}
		new_ptr->right = new_ptr2;
		if (new_ptr2) {
			new_ptr2->parent = new_ptr;
		}
		return new_ptr;
	}

	void SyntaxTree::reveal_repeat_nodes(Node& ptr) {
		if (ptr.left) {
			reveal_repeat_nodes(*ptr.left);
		}
		if (ptr.right) {
			reveal_repeat_nodes(*ptr.right);
		}
		if (ptr.tag == Repeat_node) {
			Node* ptr_left2 = new_nodes(ptr.left);
			Node* and_node = new Node("&", And_node, ptr.left, ptr_left2);
			ptr.left->parent = and_node;
			for (int i = 0; i < atoi(ptr.symb.c_str())-2; i++) {
				ptr.left = new_nodes(ptr.left);
				and_node = new Node("&", And_node, and_node, ptr.left);
				and_node->left->parent = and_node;
				and_node->right->parent = and_node;
			}
			if (*ptr.parent->right == ptr) {
				ptr.parent->right = and_node;
				and_node->parent = ptr.parent;
			}
			else {
				ptr.parent->left = and_node;
				and_node->parent = ptr.parent;
			}
			ptr = *and_node;			
		}
	}

	Node* SyntaxTree::reveal_cap_group_nodes(int num, std::set<int> revealed_nodes)  {
		if (revealed_nodes.count(num)) {
			throw std::exception("Recursive invocation of the capture group.");
		}
		else {
			revealed_nodes.insert(num);
		}
		if (cap_group_ptrs.size() <= num) {
			throw std::exception("No group with such number");
		}
		return copy_tree(cap_group_ptrs[num], revealed_nodes);
	}

	Node* SyntaxTree::copy_tree(Node* node, std::set<int> revealed_nodes) {
		Node* l_child = nullptr;
		Node* r_child = nullptr;
		if (node->left) {
			l_child = copy_tree(node->left, revealed_nodes);
		}
		if (node->right) {
			r_child = copy_tree(node->right, revealed_nodes);
		}
		// не страшно вызывать после рекурсивных вызовов других функций, т к у него нет предков
		if (node->tag == Capture_group_node) {
			return reveal_cap_group_nodes(atoi(node->symb.c_str())-1, revealed_nodes);
		}
		Node* new_node = new Node(*node);
		new_node->left = l_child;
		if (l_child) {
			l_child->parent = new_node;
		}
		new_node->right = r_child;
		if (r_child) {
			r_child->parent = new_node;
		}
		return new_node;
	}
	void SyntaxTree::set_cap_group(int num, Node* node) {
		int i = 0;
		while ((num) || (cap_group_ptrs[i])) {
			if (!cap_group_ptrs[i]) {
				--num;
			}
			i++;
		}
		cap_group_ptrs[i] = node;
	}

	Node* SyntaxTree::re2tree(std::string expr) {
			std::vector<Node*> cap_groups_calls;
			std::vector<Node*> expr_vec;
			int num_bracket = 0;
			int first_lbracket = expr.size();
			int count = std::count(expr.begin(), expr.end(), '(');
			if (count > 0) {
				cap_group_ptrs.clear();
				cap_group_ptrs.insert(cap_group_ptrs.begin(), count, 0);
			};
			if (expr.size()==0) { 
				throw std::exception("Empty string was gotten");
			}
			expr = "((" + expr + ")$)"; //0 //ADDED $ IN THE END OF STRING
			for (int i = 0; i < expr.length(); i++) { //string to vector(tokenkization)
				Node* node = new Node(std::string(1, expr[i]), None);
				expr_vec.push_back(node);
			}
			int ind_lbracket, ind_rbracket;
			do {

				for (int i = 0; i < expr_vec.size(); i++) {
					if ((expr_vec[i]->symb == "\\") && (special_symbols.find(expr_vec[i+1]->symb) != special_symbols.end())) {
						expr_vec.erase(expr_vec.begin() + i);
						expr_vec[i]->tag = A_node;
					}
					else if ((expr_vec[i]->symb == "\\") && (special_symbols.find(expr_vec[i + 1]->symb) == special_symbols.end()) &&
						!((expr_vec[i + 1]->symb <= "9") && (expr_vec[i + 1]->symb >= "0"))) {
						throw std::exception("Metasymbol non-shielding or not shielded slash '\\'");
					}
				} // shielding

				int flag = 0;
				for (int i = 0; i < expr_vec.size(); i++) {
					if ((expr_vec[i]->symb == "[") && (flag == 0) && (expr_vec[i]->tag == None)) {
						expr_vec.erase(expr_vec.begin() + i);
						flag = 1;
					}
					else if (flag != 0) {
						if ((expr_vec[i - 1]->symb == "]") && (expr_vec[i - 1]->tag == None)) { //case: ...[]...
							throw std::exception("There is no elements inside brackets. Do you mean: '[#]'?");
						}
						if ((expr_vec[i]->symb != "]") || ((expr_vec[i]->tag != None))) { //symbol, not close bracket
							if ((expr_vec[i]->symb == "#") && (expr_vec[i]->tag == None)) {
								expr_vec[i]->tag = Empty_str;
							}
							if (expr_vec[i - 1]->tag == None) {
								expr_vec[i - 1]->tag = A_node;
							}
							if (expr_vec[i]->tag == None) {
								expr_vec[i]->tag = A_node;
							}

							Node* CG_node = new Node(" ", Choise_group_node, expr_vec[i - 1], expr_vec[i]);
							expr_vec[i]->parent = CG_node;
							expr_vec[i - 1]->parent = CG_node;
							expr_vec[i] = CG_node;
							expr_vec.erase(expr_vec.begin() + i - 1);
							//throw exception if unshielded brackets inside Capture Group
						}
						else if ((expr_vec[i]->symb == "]") && (expr_vec[i]->tag == None)) { // close bracket
							expr_vec.erase(expr_vec.begin() + i);
							flag = 0;
						}
						i--;

					}
				} // create Choose Group Node
				if (flag == 1) {
					throw std::exception("Unmatched sqare parenthesis");
				}

				ind_lbracket = -1, ind_rbracket = -1;
				for (int i = 0; i < expr_vec.size(); i++) {
					if ((expr_vec[i]->symb == "(") && (expr_vec[i]->tag == None))
					{
						ind_lbracket = i;
						num_bracket++; // calc number of left bracket
					}
					else if ((expr_vec[i]->symb == ")") && (ind_lbracket != -1) && (expr_vec[i]->tag == None))
					{
						num_bracket -= 3; // 3 because 2 for first two brackets and 1 for the fact that indexes of vector starts from 0
						ind_rbracket = i;
						break;
					}
				} // get closest pairs
				if (ind_rbracket == -1)  {
					throw std::exception("Unmatched round parenthesis");
				}

				for (int i = ind_lbracket + 1; i < ind_rbracket; i++) {
					if ((special_symbols.find(expr_vec[i]->symb) == special_symbols.end()) && (expr_vec[i]->tag == None)) { // find symbol
						expr_vec[i]->tag = A_node;
					}
					if ((expr_vec[i]->symb == "\\") && (expr_vec[i + 1]->symb <= "9") && (expr_vec[i + 1]->symb >= "0")) {
						expr_vec.erase(expr_vec.begin() + i); // erase '\'
						ind_rbracket--; //shift right border
						std::string group_num;
						while ((expr_vec[i+1]->symb <= "9") && (expr_vec[i+1]->symb >= "0")) {
							group_num += expr_vec[i]->symb;
							expr_vec.erase(expr_vec.begin() + i);
							ind_rbracket--;
						}
						group_num += expr_vec[i]->symb;
						expr_vec[i]->tag = Capture_group_node;
						expr_vec[i]->symb = group_num.c_str();
						cap_groups_calls.push_back(expr_vec[i]);
					}
					else if ((expr_vec[i]->symb == "#") && (expr_vec[i]->tag == None)){
						expr_vec[i]->tag = Empty_str;
					}
				} // create A_nodes + Capture_group_nodes

				for (int i = ind_lbracket + 1; i < ind_rbracket; i++) {
					if ((expr_vec[i]->symb == "*") && (expr_vec[i]->tag == None)) { // find stars
						if (expr_vec[i-1]->tag == None) {
							throw std::exception("Umproper using of '*' metasymbol");
						}
						expr_vec[i]->left = expr_vec[i - 1];
						expr_vec[i]->tag = Star_node;
						expr_vec[i - 1]->parent = expr_vec[i];
						expr_vec.erase(expr_vec.begin() + i - 1);
						ind_rbracket--; //shift right border
						i--;
					}
				} // create star_nodes

				std::string count;
				flag = 0;
				Node* expr_repeat = nullptr;
				for (int i = ind_lbracket + 1; i < ind_rbracket; i++) {
					if ((expr_vec[i]->symb == "{") && (expr_vec[i]->tag == None)) {
						flag = 1;
						expr_repeat = new Node("", Repeat_node, expr_vec[i - 1], nullptr);
						expr_vec[i - 1]->parent = expr_repeat;
						expr_vec[i - 1] = expr_repeat;
						expr_vec.erase(expr_vec.begin() + i);
						ind_rbracket--;
						i--;
					}
					else if ((flag) && (expr_vec[i]->symb != "}")) {
						count += expr_vec[i]->symb;
						expr_vec.erase(expr_vec.begin() + i);
						ind_rbracket--;
						i--;
					}
					else if ((expr_vec[i]->symb == "}") && (flag)) {
						flag = 0;
						if (!count.empty() && std::all_of(count.begin(), count.end(), std::isdigit)) {
							expr_repeat->symb = count;
						}
						else {
							throw std::exception("Not a number inside curly parenthesis");
						}
						if (std::atoi(count.c_str()) < 1) {
							throw std::exception("Incorrect number inside curly parenthesis");
						}
						expr_vec.erase(expr_vec.begin() + i);
						ind_rbracket--;
						i--;
						count.clear();
					}
				} // create Repeat_node
				if (flag == 1) {
					throw std::exception("Unmatched curly parenthesis");
				}

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
					if ((expr_vec[i]->symb == "|") && (expr_vec[i]->tag == None)) { // find or
						if ((expr_vec[i + 1]->tag == None) || (expr_vec[i - 1]->tag == None)) {
							throw std::exception("Umproper using of '|' metasymbol");
						}
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
					if (num_bracket >= 0) {
						set_cap_group(num_bracket, expr_vec[ind_lbracket + 1]);
					}
					 //add new cap group in vector
					num_bracket = 0;
					expr_vec.erase(expr_vec.begin() + ind_rbracket);
					expr_vec.erase(expr_vec.begin() + ind_lbracket);
				}
			} while (expr_vec.size()>1);
			root = expr_vec[0]; // set_root(expr_vec[0]);
			reveal_repeat_nodes(*root);
			for (auto cap_group_call : cap_groups_calls) {
				std::set<int> revealed_nodes;
				Node* cap_group_root = reveal_cap_group_nodes(atoi(cap_group_call->symb.c_str())-1, revealed_nodes);
				if (cap_group_call->parent->left == cap_group_call) {
					cap_group_call->parent->left = cap_group_root;
					cap_group_root->parent = cap_group_call->parent->left;
				}
				else {
					cap_group_call->parent->right = cap_group_root;
					cap_group_root->parent = cap_group_call->parent->right;
				}
			}
			return root;
	}

	void SyntaxTree::bypass(Node* ptr, int idx, int prev) {
			std::map <int, std::string> tag = { {A_node, "A_node"}, {Star_node, "Star_node"}, {Or_node, "Or_node"},
				{And_node, "And_node"}, {Choise_group_node, "Choise_group_node"}, 
				{Repeat_node, "Repeat_node"},  {Empty_str, "Empty_str"}, {Capture_group_node, "Capture_group_node"}};
			if (ptr->parent) {
				graph += std::to_string(prev) + "->" + std::to_string(idx) + ";\n";
			}
			prev = idx;
			if (ptr->left) {
				bypass(ptr->left, 2 * idx, prev); //instead of "2" you can put amount of children
			}
			if (ptr->right) {
				bypass(ptr->right, 2 * idx + 1, prev);
			}
			std::string symb = ptr->symb;
			if (symb == "\\") {
				symb = "\\\\";
			}

			graph += std::to_string(prev) + " [label=\"" + std::to_string(prev) + " " + symb + " " +
				std::to_string(ptr->id_pos) + " (" + tag[ptr->tag] + ")" + "\"];\n";
	}

	void SyntaxTree::getTreeImg() {
		graph = "digraph graphname {";
		Node* ptr = get_root();
		bypass(ptr, 1, 1);
		graph += "}";
		Agraph_t* Graph = agmemread(graph.c_str());
		GVC_t* gvc = gvContext();
		gvLayout(gvc, Graph, "dot");
		int res = gvRenderFilename(gvc, Graph, "jpeg", "D:\\Automata-Theory-main\\LAB2\\out.jpg");
		agclose(Graph);
	}

	std::string SyntaxTree::tree2re(Node* ptr) {
		std::string prefix, suffix, str;
		if (ptr->left) {
			prefix = tree2re(ptr->left);
		}
		
		if ((special_symbols.find(ptr->symb) != special_symbols.end()) && (ptr->tag == A_node)) {
			str += "\\" + ptr->symb;
		}
		else if (ptr->tag == Choise_group_node) {
			str += "|";
		}
		else if ((ptr->tag == And_node) || (ptr->symb == "$")) {
			str += "";
		}
		else {
			str += ptr->symb;
		}
		if (ptr->right) {
			suffix = tree2re(ptr->right);
		}
		
		if ((ptr->left) && (ptr->left->tag > ptr->tag)) {
			prefix = "(" + prefix + ")";
		}
		if ((ptr->right) && (ptr->right->tag > ptr->tag)) {
			suffix = "(" + suffix + ")";
		}
		return prefix + str + suffix;
	}
}