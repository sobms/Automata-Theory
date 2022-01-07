#include "regex.h"

using namespace RegexLib;
int main()
{
	regex re;
	DFA* automaton = re.compile("(t|m)([ph#]|i{2})\\**");// ab*([def]{2}|aa)");//"(t|m)([ph]|i{2})\\**");//"(me|m)[ph]|i*");//"r(nr|\\*)*[nr*]*n");
	std::set<std::string> set_substrings = re.find_all("tmphppii**t*gf*fmphphhh*miiqwetpppppp", automaton);
	//std::set<std::string> set_substrings2 = re.find_all("**nrnuhnr**nrnqrrn", L);
	//DFA* automaton2 = re.compile(regex);
	//std::string L2 = re.inversion(automaton);
	std::string L_comp = re.complement(automaton);
	std::string L_comp2 = re.complement(L_comp);
	std::set<std::string> set_substrings2 = re.find_all("tmphppii**t*gf*fmphphhh*miiqwetpppppp", L_comp2);
	//DFA* automaton2 = re.compile(L_comp);
	//std::string L_comp2 = re.complement(automaton2);
	//std::set<std::string> set_substrings2 = re.find_all("abb", L_comp2);
	return 0;
}
