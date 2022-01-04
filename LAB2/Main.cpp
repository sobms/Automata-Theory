#include "regex.h"

using namespace RegexLib;
int main()
{
	regex re;
	DFA* automaton = re.compile("ab*([def]{2}|aa)");//"(t|m)([ph]|i{2})\\**");//"(me|m)[ph]|i*");//"r(nr|\\*)*[nr*]*n");
	std::set<std::string> set_substrings = re.find_all("dsabbbaaeradtfabdeaa", "ab*([def]{2}|aa)");
	std::string regex = re.DFAtoRE(automaton);
	return 0;
}
