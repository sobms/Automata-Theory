#include "Syntax_tree.h"
#include "RegexToDFA.h"

using namespace RegexLib;
int main()
{
	SyntaxTree tree = SyntaxTree();
	Node* root = tree.re2tree("(me)*p|h[i#]\\*");//tree.re2tree("(me)*p|hi#[nrnu]{2}")//{1024}
	ST_to_DFA_transformer trans = ST_to_DFA_transformer(root);
	trans.numerate();
	trans.Calc_attributes(root);
	tree.getTreeImg();
	trans.transform();
	trans.getDFAImg();
	return 0;
}