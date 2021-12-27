#include "Syntax_tree.h"

using namespace RegexLib;
int main()
{
	SyntaxTree tree = SyntaxTree();
	Node* root = tree.re2tree("ab\\**{48}(in)|#|[#]\\#");//{1024}
	tree.getTreeImg();
	return 0;
}