#ifndef __RULES_SCORETAB_H__
#define __RULES_SCORETAB_H__

#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

struct RuleScore {
	RuleScore() {} // empty constructor for map instantiation
	RuleScore(string cue1, string cue2, string alter_tag, string target, long long id);

	Rule rule;
	int good, bad;
} ;

struct RulesScoreTab {
	RulesScoreTab(vector<string> & tagset, Corpus * standard);

	//void init_scores(Corpus * test_corp);
	//void update_scores(Corpus * new_corp, Corpus * old_corp);

	void examine(Corpus * test_corp);
	void clear();

	Rule * get_winner();

	Corpus * gold_corp; // gold standard Corpus
	set<long long> eligible_rules; // rules that have positive (good-bad) score
	map<long long, RuleScore> tab; // sort rules by their ID (see rules.h)
	map<string, int> tag_index; // consistent ID's for each tag

	private:
	void inc_good(int rule_type, string cue1, string alter_tag, string target);
	void inc_good_ext(int rule_type, string cue1, string cue2, string alter_tag, string target);

	void inc_bad(int rule_type, string cue1, string good_target);
	void inc_bad_ext(int rule_type, string cue1, string cue2, string good_target);
	void inc_bad_general(long long rule_ind_lowbnd, long long rule_ind_hibnd);
} ;

#endif
