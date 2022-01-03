#include "regex.h"

using namespace RegexLib;
int main()
{
	regex re;
	DFA* automaton = re.compile("(t|m)([ph]|i*)\\**");//"(me|m)[ph]|i*");//"r(nr|\\*)*[nr*]*n");
	std::set<std::string> set_substrings = re.find_all("qtphymii*mp", "(t|m)([ph]|i*)\\**");
	return 0;
}
