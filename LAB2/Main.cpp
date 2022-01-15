#include "regex.h"

using namespace RegexLib;
int main()
{
	regex re;
	//DFA* automaton = re.compile("(t|m)([ph#]|i{2})\\**");// ab*([def]{2}|aa)");//"(t|m)([ph]|i{2})\\**");//"(me|m)[ph]|i*");//"r(nr|\\*)*[nr*]*n");
	//std::set<std::string> set_substrings = re.find_all("tmphppii**t*gf*fmphphhh*miiqwetpppppp", automaton);
	//std::set<std::string> set_substrings2 = re.find_all("**nrnuhnr**nrnqrrn", L);
	//DFA* automaton2 = re.compile("r(nr|\\*)*(n|r)*n"); //"\\2(aqw\\3)(cd*)([f2]\\2)"\\"(\\3\\2(aqw\\5)(cd*))(([f2\\3]))"//"(\\3\\2(aqw\\5)(cd*))(([f2]\\3))"
	//std::vector<Node*> cap_group_ptrs = re.get_cap_group_ptrs();
	//std::set<std::string> set_substrings = re.find_all("aqwfcd2aaa", automaton2);
	DFA* automaton = re.compile("(t|m)([ph#]|i{2})\\**");
	std::set<std::string> set_substrings = re.find_all("tmhiimii**", automaton);
	matches* match = re.find_all2("(t|m)([ph#]|i{2})\\**", "tmhiimii**");
	std::set<std::string> gr2 = (*match)[0];
	std::set<std::string> gr1 = (*match)[1];
	std::set<std::string> gr0 = (*match)[2];
	//std::string L2 = re.inversion(automaton);
	//std::string L_comp = re.complement(automaton);
	//std::string L = re.DFAtoRE(automaton);
	//std::string L_comp = re.complement("#");
	
	//std::set<std::string> set_substrings2 = re.find_all("tmphppii**t*gf*fmphphhh*miiqwetpppppp", L_comp2);
	//DFA* automaton2 = re.compile(L_comp);
	//std::string L_comp2 = re.complement(automaton2);
	//std::set<std::string> set_substrings2 = re.find_all("abb", L_comp2);
	return 0;
}
