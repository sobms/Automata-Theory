#include "regex.h"

using namespace RegexLib;
int main()
{
	regex re;
	DFA* automaton = re.compile("(me|m)[ph]|i*");// ab*([def]{2}|aa)");//"(t|m)([ph]|i{2})\\**");//"(me|m)[ph]|i*");//"r(nr|\\*)*[nr*]*n");
	std::set<std::string> set_substrings = re.find_all("mempiiemiiierfdfmepemphhmhhiii", "(me|m)[ph]|i*");
	std::string regex = re.DFAtoRE(automaton);
	return 0;
}
