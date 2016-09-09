#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;

RuleScore::RuleScore(string cue1, string cue2, string alter_tag, string target, long long id) {
	good = bad = 0;

	rule = Rule();
	rule.cue1 = cue1;
	rule.cue2 = cue2;
	rule.alter_tag = alter_tag;
	rule.target = target;
	rule.id = id;
}

RulesScoreTab::RulesScoreTab(vector<string> & tagset, Corpus * standard) {

	gold_corp = standard;

	for(int i = 0; i < tagset.size(); i++) 
		tag_index[ tagset[i] ] = i; // give the tag a consistent number
}

// Increase good for a rule taking two cues.
void RulesScoreTab::inc_good(int rule_type, string cue1, string alter_tag, string target) {

	long long rule_id = rule_type * RULE_RANGE
			+ tag_index[ cue1 ] * tag_index.size()*tag_index.size()
			+ tag_index[ alter_tag ] * tag_index.size()
			+ tag_index[ target ];

	if(tab.find(rule_id) == tab.end())  // if there's no entry for this rule, create one
		tab[ rule_id ] = RuleScore(cue1, "", alter_tag, target, rule_id);

	tab[rule_id].good ++;
	eligible_rules.insert(rule_id);
}

// Increase good for "extended" rules (one that needs 3 cues).
void RulesScoreTab::inc_good_ext(int rule_type, string cue1, string cue2, string alter_tag, string target) {

	long long rule_id = rule_type * EXT_RULE_RANGE
			+ tag_index[ cue1 ] * tag_index.size()*tag_index.size()*tag_index.size()
			+ tag_index[ cue2 ] * tag_index.size()*tag_index.size()
			+ tag_index[ alter_tag ] * tag_index.size()
			+ tag_index[ target ];

	if(tab.find(rule_id) == tab.end())  // if there's no entry for this rule, create one
		tab[ rule_id ] = RuleScore(cue1, cue2, alter_tag, target, rule_id);

	tab[rule_id].good ++;
	eligible_rules.insert(rule_id);
}

// Increase "bad" score for each rule that would change good_target to something different.
void RulesScoreTab::inc_bad(int rule_type, string cue1, string good_target) {
	// Compute the high and low bound for rules concerning this situation.
	long long rule_ind_lowbnd = rule_type * RULE_RANGE
			+ tag_index[ cue1 ] * tag_index.size()*tag_index.size()
			+ tag_index[ good_target ] * tag_index.size(); // here as the alter_tag
	long long rule_ind_hibnd = rule_type * RULE_RANGE
			+ tag_index[ cue1 ] * tag_index.size()*tag_index.size()
			+ tag_index[ good_target ] * tag_index.size()
			+ tag_index.size();

	inc_bad_general(rule_ind_lowbnd, rule_ind_hibnd);
}

// Increase "bad" score for each ext. rule that would change good_target to something different.
void RulesScoreTab::inc_bad_ext(int rule_type, string cue1, string cue2, string good_target) {
	// Compute the high and low bound for rules concerning this situation.
	long long rule_ind_lowbnd = rule_type * EXT_RULE_RANGE
			+ tag_index[ cue1 ] * tag_index.size()*tag_index.size()*tag_index.size()
			+ tag_index[ cue2 ] * tag_index.size()*tag_index.size()
			+ tag_index[ good_target ] * tag_index.size(); // here as the alter_tag
	long long rule_ind_hibnd = rule_type * EXT_RULE_RANGE
			+ tag_index[ cue1 ] * tag_index.size()*tag_index.size()*tag_index.size()
			+ tag_index[ cue2 ] * tag_index.size()*tag_index.size()
			+ tag_index[ good_target ] * tag_index.size()
			+ tag_index.size();

	inc_bad_general(rule_ind_lowbnd, rule_ind_hibnd);
}

void RulesScoreTab::inc_bad_general(long long rule_ind_lowbnd, long long rule_ind_hibnd) {
	long long exam_ind = rule_ind_lowbnd - 1;

	// Basically ascend to the highest index in the set, until we reach indices which are out of the
	// scope (.upper_bound() returns first larger value that is present in a set)
	while((exam_ind =
			* eligible_rules.upper_bound(exam_ind))
			< rule_ind_hibnd && exam_ind >= rule_ind_lowbnd
	) {

		tab[exam_ind].bad ++;

		if(tab[exam_ind].good - tab[exam_ind].bad <= 0)
			eligible_rules.erase(exam_ind);

		if(eligible_rules.size() == 0)
			throw "No good rules found";

		// We must avoid dereferencing .end() iterator in the loop condition:
		if(exam_ind == * eligible_rules.rbegin())
			return;
	}
}


void RulesScoreTab::examine(Corpus * test_corp) {

	// Find errors in the test corpus, and rules that potentially correct them.
	for( int si = 0; si < test_corp->sentences.size(); si++ ) {

		vector<TaggedWord> * stc = & (*test_corp).sentences[si];
		vector<TaggedWord> * gold_stc = & (*gold_corp).sentences[si];

		for( int wi = 0; wi < stc->size(); wi++ ) {

			if(stc->at(wi).tag == gold_stc->at(wi).tag) // if classification is already true, we don't care
				continue;

			// Note that any "good" score will put a rule in the eligible rules list.
			if(wi != 0) {
				inc_good(RL_PRECEDING, stc->at(wi-1).tag, stc->at(wi).tag, gold_stc->at(wi).tag);
			}
			if(wi+1 != stc->size()) {
				inc_good(RL_FOLLOWING, stc->at(wi+1).tag, stc->at(wi).tag, gold_stc->at(wi).tag);
			}
			if(wi > 1) {
				inc_good(RL_2BEFORE, stc->at(wi-2).tag, stc->at(wi).tag, gold_stc->at(wi).tag);

				inc_good(RL_ONE_OF_2BEFORE, stc->at(wi-1).tag, stc->at(wi).tag, gold_stc->at(wi).tag);
				inc_good(RL_ONE_OF_2BEFORE, stc->at(wi-2).tag, stc->at(wi).tag, gold_stc->at(wi).tag);

				inc_good_ext(EXTRL_1_AND_2_BEFORE, stc->at(wi-1).tag, stc->at(wi-2).tag,
					stc->at(wi).tag, gold_stc->at(wi).tag);
			}
			if(wi + 2 < stc->size()) {
				inc_good(RL_2AFTER, stc->at(wi+2).tag, stc->at(wi).tag, gold_stc->at(wi).tag);

				inc_good(RL_ONE_OF_2AFTER, stc->at(wi+1).tag, stc->at(wi).tag, gold_stc->at(wi).tag);
				inc_good(RL_ONE_OF_2AFTER, stc->at(wi+2).tag, stc->at(wi).tag, gold_stc->at(wi).tag);

				inc_good_ext(EXTRL_1_AND_2_AFTER, stc->at(wi+1).tag, stc->at(wi+2).tag,
					stc->at(wi).tag, gold_stc->at(wi).tag);
			}
			if(wi > 2) {
				inc_good(RL_ONE_OF_3BEFORE, stc->at(wi-1).tag, stc->at(wi).tag, gold_stc->at(wi).tag);
				inc_good(RL_ONE_OF_3BEFORE, stc->at(wi-2).tag, stc->at(wi).tag, gold_stc->at(wi).tag);
				inc_good(RL_ONE_OF_3BEFORE, stc->at(wi-3).tag, stc->at(wi).tag, gold_stc->at(wi).tag);
			}
			if(wi + 3 < stc->size()) {
				inc_good(RL_ONE_OF_3AFTER, stc->at(wi+1).tag, stc->at(wi).tag, gold_stc->at(wi).tag);
				inc_good(RL_ONE_OF_3AFTER, stc->at(wi+2).tag, stc->at(wi).tag, gold_stc->at(wi).tag);
				inc_good(RL_ONE_OF_3AFTER, stc->at(wi+3).tag, stc->at(wi).tag, gold_stc->at(wi).tag);
			}
			if(wi+1 != stc->size() && wi > 1)
				inc_good_ext(EXTRL_PRECEDING_AND_FOLLOWING, stc->at(wi-1).tag, stc->at(wi+1).tag,
					stc->at(wi).tag, gold_stc->at(wi).tag);

		}
	}

	// Assign "bad" scores to the eligible rules.
	for( int si = 0; si < test_corp->sentences.size(); si++ ) {

		vector<TaggedWord> * stc = & (*test_corp).sentences[si];
		vector<TaggedWord> * gold_stc = & (*gold_corp).sentences[si];

		for( int wi = 0; wi < stc->size(); wi++ ) {

			#ifdef DUMP_WORDS
			cout << stc->at(wi).word << " " << stc->at(wi).tag << endl << "======" << endl;
			#endif

			if(stc->at(wi).tag != gold_stc->at(wi).tag) // already taken care of those
				continue;

			// Check the preceding tag rules that could introduce error.
			// Note that any rule whose "bad" score exceeds "good" will be wiped out from eligible rules
			// list.
			if(wi != 0)
				inc_bad(RL_PRECEDING, stc->at(wi-1).tag, stc->at(wi).tag);
			if(wi+1 != stc->size())
				inc_bad(RL_FOLLOWING, stc->at(wi+1).tag, stc->at(wi).tag);
			if(wi > 1) {
				inc_bad(RL_2BEFORE, stc->at(wi-2).tag, stc->at(wi).tag);

				inc_bad(RL_ONE_OF_2BEFORE, stc->at(wi-1).tag, stc->at(wi).tag);
				inc_bad(RL_ONE_OF_2BEFORE, stc->at(wi-2).tag, stc->at(wi).tag);

				inc_bad_ext(EXTRL_1_AND_2_BEFORE, stc->at(wi-1).tag, stc->at(wi-2).tag,
					stc->at(wi).tag);
			}
			if(wi + 2 < stc->size()) {
				inc_bad(RL_2AFTER, stc->at(wi+2).tag, stc->at(wi).tag);

				inc_bad(RL_ONE_OF_2AFTER, stc->at(wi+1).tag, stc->at(wi).tag);
				inc_bad(RL_ONE_OF_2AFTER, stc->at(wi+2).tag, stc->at(wi).tag);

				inc_bad_ext(EXTRL_1_AND_2_AFTER, stc->at(wi+1).tag, stc->at(wi+2).tag,
					stc->at(wi).tag);
			}
			if(wi > 2) {
				inc_bad(RL_ONE_OF_3BEFORE, stc->at(wi-1).tag, stc->at(wi).tag);
				inc_bad(RL_ONE_OF_3BEFORE, stc->at(wi-2).tag, stc->at(wi).tag);
				inc_bad(RL_ONE_OF_3BEFORE, stc->at(wi-3).tag, stc->at(wi).tag);
			}
			if(wi + 3 < stc->size()) {
				inc_bad(RL_ONE_OF_3AFTER, stc->at(wi+1).tag, stc->at(wi).tag);
				inc_bad(RL_ONE_OF_3AFTER, stc->at(wi+2).tag, stc->at(wi).tag);
				inc_bad(RL_ONE_OF_3AFTER, stc->at(wi+3).tag, stc->at(wi).tag);
			}
			if(wi+1 != stc->size() && wi > 1)
				inc_bad_ext(EXTRL_PRECEDING_AND_FOLLOWING, stc->at(wi-1).tag, stc->at(wi+1).tag,
					stc->at(wi).tag);
		}
	}
}

void RulesScoreTab::clear() {
	tab.clear();
	eligible_rules.clear();	
}

Rule * RulesScoreTab::get_winner() {
	RuleScore * winner_score;
	winner_score = & tab[ *(eligible_rules.begin()) ];
	// Look for a winner (one with better score than all already known):
	for(set<long long>::iterator rls_pnt = eligible_rules.begin(); rls_pnt != eligible_rules.end(); rls_pnt++)
		if((tab[ *rls_pnt ].good - tab[ *rls_pnt ].bad) > (winner_score->good - winner_score->bad))
			winner_score = & tab[ *rls_pnt ];

	Rule * winner;
	winner = & winner_score->rule;

	return winner;
}
