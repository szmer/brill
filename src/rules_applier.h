#ifndef __RULES_APPLIER_H__
#define __RULES_APPLIER_H__

#include <map>
#include <string>
#include <vector>

struct RulesApplier {
	RulesApplier(const char * tgfname, const char * rlfname, Corpus * standard); // one must specify rule list file and tagset file
	Corpus transform(Corpus * corp); // tag with tag_lookup, and then apply every rule - leave scores in .baseline_score
						// and .final_score

	vector<string> tag_index; // each tag by its consistent ID
	vector<long long> rules;
	map<string,string> tag_lookup; // unigram table loaded from a file (for initial tagging)
	Corpus * gold_corp; // gold standard Corpus

	double baseline_score;
	double final_score;
} ;

#endif
