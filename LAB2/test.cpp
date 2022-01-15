#include "pch.h"
#include <iostream>
#include <stack>
#include <vector>
#include "../Regex_lib/regex.h"

using namespace RegexLib;

TEST(root_setter, SyntaxTreeTest) {
	SyntaxTree tree = SyntaxTree();
	Node* root = new Node("a", A_node);
	tree.set_root(root);
	EXPECT_EQ(root, tree.get_root());
}
TEST(re2tree_exceptions, SyntaxTreeTest) {
	SyntaxTree tree = SyntaxTree();
	ASSERT_THROW(tree.re2tree("((abv*\\*[fd])"), std::exception); //Unmatched round parenthesis
	ASSERT_THROW(tree.re2tree("abv*\\*[]de{2}"), std::exception); //There is no elements inside brackets. Do you mean: '[#]'
	ASSERT_THROW(tree.re2tree("abv*\\*a[dsde"), std::exception); //Unmatched sqare parenthesis
	ASSERT_THROW(tree.re2tree("abv*\\*a(ds){}de"), std::exception); //Not a number inside curly parenthesis
	ASSERT_THROW(tree.re2tree("abv*\\*a(ds){a}de"), std::exception); //Not a number inside curly parenthesis
	ASSERT_THROW(tree.re2tree("abv*\\*a(ds){0}de"), std::exception); //Incorrect number inside curly parenthesis
	ASSERT_THROW(tree.re2tree("abv*\\*a(ds){-1}de"), std::exception); //Incorrect number inside curly parenthesis
	ASSERT_THROW(tree.re2tree("abv*\\*a(ds){2de"), std::exception); //Unmatched curly parenthesis
	ASSERT_THROW(tree.re2tree("abv**a\\dse"), std::exception); //Metasymbol non-shielding or not shielded slash '\\'
}
TEST(re2tree, SyntaxTreeTest) {
	SyntaxTree tree = SyntaxTree();
	Node* root = tree.re2tree("(t|m)([ph#]|i{2})\\**");
	std::string regex = tree.tree2re(root);
	EXPECT_EQ(regex, "(t|m)((p|h|#)|ii)\\**");
}
TEST(numerate, RegexToDFATest) {
	SyntaxTree tree = SyntaxTree();
	Node* root = tree.re2tree("(t|m)([ph#]|i{2})\\**");
	ST_to_DFA_transformer trans = ST_to_DFA_transformer(root);
	trans.numerate();
	Node* ptr = root;
	std::stack <Node*> S;
	int i = 0;
	//bypass to check numerate
	while (!S.empty() || ptr) {
		while (ptr) {
			S.push(ptr);
			ptr = ptr->left;
		}
		if (!S.empty()) {
			ptr = S.top();

			if ((ptr->tag == A_node) || (ptr->tag == Empty_str)) {
				ASSERT_EQ(ptr->id_pos, i);
				i++;
			}
			S.pop();
			ptr = ptr->right;
		}
	}
}
void Set_all_nullable(Node* ptr, ST_to_DFA_transformer trans) {
	if (ptr->left) {
		Set_all_nullable(ptr->left, trans);
	}
	if (ptr->right) {
		Set_all_nullable(ptr->right, trans);
	}
	trans.set_nullable(ptr);
}
std::vector<long> Get_nullable_vec(Node* root) {
	std::vector<long> vec_nullable;
	Node* ptr = root;
	std::stack <Node*> S;
	int i = 0;
	while (!S.empty() || ptr) {
		while (ptr) {
			S.push(ptr);
			ptr = ptr->left;
		}
		if (!S.empty()) {
			ptr = S.top();

			vec_nullable.push_back(ptr->nullable);
			S.pop();
			ptr = ptr->right;
		}
	}
	return vec_nullable;
}

TEST(set_nullable, RegexToDFATest) {
	SyntaxTree tree = SyntaxTree();
	Node* root = tree.re2tree("(t|m)|([ph#])\\**");
	ST_to_DFA_transformer trans = ST_to_DFA_transformer(root);
	Set_all_nullable(root, trans);
	tree.getTreeImg();
	std::vector<long> vec_nullable = Get_nullable_vec(root);
	std::vector<long> vec_nullable_true = { 0,0,0,1,0,0,0,1,1,1,0,1,0,0 };
	ASSERT_EQ(vec_nullable, vec_nullable_true);
}

void Set_all_first(Node* ptr, ST_to_DFA_transformer trans) {
	if (ptr->left) {
		Set_all_first(ptr->left, trans);
	}
	if (ptr->right) {
		Set_all_first(ptr->right, trans);
	}
	trans.set_nullable(ptr);
	trans.set_first(ptr);
}
std::vector<std::set<long>> Get_first_vec(Node* root) {
	std::vector<std::set<long>> vec_first;
	Node* ptr = root;
	std::stack <Node*> S;
	int i = 0;
	while (!S.empty() || ptr) {
		while (ptr) {
			S.push(ptr);
			ptr = ptr->left;
		}
		if (!S.empty()) {
			ptr = S.top();

			vec_first.push_back(ptr->first);
			S.pop();
			ptr = ptr->right;
		}
	}
	return vec_first;
}
TEST(set_first, RegexToDFATest) {
	SyntaxTree tree = SyntaxTree();
	Node* root = tree.re2tree("(t|m)|([ph#])\\**");
	ST_to_DFA_transformer trans = ST_to_DFA_transformer(root);
	trans.numerate();
	Set_all_first(root, trans);
	std::vector<std::set<long>> vec_first = Get_first_vec(root);
	std::vector<std::set<long>> true_sets_vec;
	true_sets_vec.push_back({ 0 });
	true_sets_vec.push_back({ 0, 1 });
	true_sets_vec.push_back({ 1 });
	true_sets_vec.push_back({ 0,1,2,3,5 });
	true_sets_vec.push_back({ 2 });
	true_sets_vec.push_back({ 2,3 });
	true_sets_vec.push_back({ 3 });
	true_sets_vec.push_back({ 2,3 });
	true_sets_vec.push_back({  });
	true_sets_vec.push_back({ 2,3,5 });
	true_sets_vec.push_back({ 5 });
	true_sets_vec.push_back({ 5 });
	true_sets_vec.push_back({ 0,1,2,3,5,6 });
	true_sets_vec.push_back({ 6 });
	ASSERT_EQ(vec_first, true_sets_vec);
}

void Set_all_last(Node* ptr, ST_to_DFA_transformer trans) {
	if (ptr->left) {
		Set_all_last(ptr->left, trans);
	}
	if (ptr->right) {
		Set_all_last(ptr->right, trans);
	}
	trans.set_nullable(ptr);
	trans.set_last(ptr);
}
std::vector<std::set<long>> Get_last_vec(Node* root) {
	std::vector<std::set<long>> vec_last;
	Node* ptr = root;
	std::stack <Node*> S;
	int i = 0;
	while (!S.empty() || ptr) {
		while (ptr) {
			S.push(ptr);
			ptr = ptr->left;
		}
		if (!S.empty()) {
			ptr = S.top();

			vec_last.push_back(ptr->last);
			S.pop();
			ptr = ptr->right;
		}
	}
	return vec_last;
}
TEST(set_last, RegexToDFATest) {
	SyntaxTree tree = SyntaxTree();
	Node* root = tree.re2tree("(t|m)|([ph#])\\**");
	ST_to_DFA_transformer trans = ST_to_DFA_transformer(root);
	trans.numerate();
	Set_all_last(root, trans);
	std::vector<std::set<long>> vec_last = Get_last_vec(root);
	std::vector<std::set<long>> true_sets_vec;
	true_sets_vec.push_back({ 0 });
	true_sets_vec.push_back({ 0, 1 });
	true_sets_vec.push_back({ 1 });
	true_sets_vec.push_back({ 0,1,2,3,5 });
	true_sets_vec.push_back({ 2 });
	true_sets_vec.push_back({ 2,3 });
	true_sets_vec.push_back({ 3 });
	true_sets_vec.push_back({ 2,3 });
	true_sets_vec.push_back({  });
	true_sets_vec.push_back({ 2,3,5 });
	true_sets_vec.push_back({ 5 });
	true_sets_vec.push_back({ 5 });
	true_sets_vec.push_back({ 6 });
	true_sets_vec.push_back({ 6 });
	ASSERT_EQ(vec_last, true_sets_vec);
}

void Set_all_follow(Node* ptr, ST_to_DFA_transformer* trans) {
	if (ptr->left) {
		Set_all_follow(ptr->left, trans);
	}
	if (ptr->right) {
		Set_all_follow(ptr->right, trans);
	}
	trans->set_nullable(ptr);
	trans->set_first(ptr);
	trans->set_last(ptr);
	trans->set_follow_pos(ptr);
}

TEST(set_follow, RegexToDFATest) {
	SyntaxTree tree = SyntaxTree();
	Node* root = tree.re2tree("(t|m)|([ph#])\\**");
	ST_to_DFA_transformer* trans = new ST_to_DFA_transformer(root);
	trans->numerate();
	Set_all_follow(root, trans);
	std::map<long, std::set<long>> followPos = trans->get_followPos();
	std::map<long, std::set<long>> true_fp;
	true_fp[0] = { 6 };
	true_fp[1] = { 6 };
	true_fp[2] = { 5,6 };
	true_fp[3] = { 5,6 };
	true_fp[5] = { 5,6 };
	ASSERT_EQ(true_fp, followPos);
}

TEST(find_all, regexTest) {
	regex re;
	DFA* automaton = re.compile("(t|m)([ph#]|i{2})\\**");
	std::set<std::string> set_substrings = re.find_all("tmphppii**t*mphhh*miiqwetppp", automaton);
	std::set<std::string> true_set = { "mii", "mp", "t", "t*", "tp" };
	ASSERT_EQ(set_substrings, true_set);

	std::set<std::string> set_substrings2 = re.find_all("mempiiemiiierfdfmepemphhmhhiii", "(me|m)[ph]|i*");
	std::set<std::string> true_set2 = { "mep", "mp", "mh", "ii", "iii", "" };
	ASSERT_EQ(set_substrings2, true_set2);

	DFA* automaton2 = re.compile("ab*([def]{2}|aa)");
	std::set<std::string> set_substrings3 = re.find_all("abdefaababaaaabdeadebaabdeaa", automaton2);
	std::set<std::string> true_set3 = { "abde", "ade", "abaa" };
	ASSERT_EQ(set_substrings3, true_set3);
}

TEST(DFAtoRE, regexTest) {
	regex re;
	DFA* automaton = re.compile("(t|m)([ph#]|i{2})\\**");
	std::string regexpr = re.DFAtoRE(automaton);
	DFA* automaton4 = re.compile(regexpr);
	std::set<std::string> set_substrings = re.find_all("tmphppii**t*mphhh*miiqwetppp", regexpr);
	std::set<std::string> true_set = { "mii", "mp", "t", "t*", "tp" };
	ASSERT_EQ(set_substrings, true_set);

	DFA* automaton2 = re.compile("a|b*");
	std::string regexpr2 = re.DFAtoRE(automaton2);
	ASSERT_EQ(regexpr2, "#|a|(b|b(b)*b)");

	DFA* automaton3 = re.compile("r(nr|\\*)*[nr*]*n");
	std::string regexpr3 = re.DFAtoRE(automaton3);
	std::set<std::string> set_substrings3 = re.find_all("**nrnuhnr**nrnqrrn", regexpr3);
	std::set<std::string> true_set3 = { "rn", "r**nrn", "rrn" };
	ASSERT_EQ(set_substrings3, true_set3);
}

TEST(inversion, regexTest) {
	regex re;
	DFA* automaton = re.compile("(t|m)([ph#]|i{2})\\**");
	std::set<std::string> set_substrings = re.find_all("tmphppii**t*mphhh*miiqwetppp", automaton);
	std::string L1 = re.inversion(automaton);
	std::string L2 = re.inversion(L1);
	std::set<std::string> set_substrings2 = re.find_all("tmphppii**t*mphhh*miiqwetppp", L2);
	ASSERT_EQ(set_substrings, set_substrings2);

	std::set<std::string> set_substrings3 = re.find_all("mempiiemiiierfdfmepemphhmhhiii", "e(me|m)[ph]*|i*");
	std::string L3 = re.inversion("e(me|m)[ph]*|i*");
	std::string L4 = re.inversion(L3);
	std::set<std::string> set_substrings4 = re.find_all("mempiiemiiierfdfmepemphhmhhiii", L4);
	ASSERT_EQ(set_substrings4, set_substrings3);
	ASSERT_EQ(L4, "(e(me|m)(p|h)*|i*)");

	std::string L5 = re.inversion("ab*([def]{2})|ac");
	ASSERT_EQ(L5, "((d|e|f)(d|e|f)b*a|ca)");
}

TEST(complement, regexTest) {
	regex re;
	DFA* automaton = re.compile("(t|m)([ph#]|i{2})\\**");
	std::set<std::string> set_substrings = re.find_all("tmphppii**t*gf*fmphphhh*miiqwetpppppp", automaton);
	std::string L_comp = re.complement(automaton);
	std::string L_comp2 = re.complement(L_comp);
	std::set<std::string> set_substrings2 = re.find_all("tmphppii**t*gf*fmphphhh*miiqwetpppppp", L_comp2);
	ASSERT_EQ(set_substrings, set_substrings2);

	std::string L_comp3 = re.complement("a|b*");
	std::string L_comp4 = re.complement(L_comp3);
	std::string L_comp5 = re.complement("#");
	ASSERT_EQ("#|(b|b(b)*b)|a", L_comp4);
}
int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}