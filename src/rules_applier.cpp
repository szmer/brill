#ifndef __RULES_APPLIER_CPP__
#define __RULES_APPLIER_CPP__

#include <cmath>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#include "corpus.h"
#include "rules.h"
#include "rules_applier.h"

using namespace std;

static long long atoi_longlong(char * str) {
	int len = strlen(str);
	long long num = 0;

	for(int i = 0; i < len; i++)
		num += pow(10, (len-i-1)) * (str[i] - 48); // 48 being '0' ASCII value

	return num;
}

RulesApplier::RulesApplier(const char * tgfname, const char * rlfname, Corpus * standard) {

	tag_index = Corpus::load_tagset(tgfname);

	ifstream rl_file(rlfname);
	char buff [256];
	memset(buff, 256, ' '); // avoid memory junk (see loop condition)

	// Load the unigram table.
	while(rl_file.good()) { // until we reach linebreak (where rule numbers begin)
		rl_file.getline(buff, 256, '\t');

		if(strchr(buff, '\n') != NULL) // this is where rule ids start
			break;

		if(rl_file.gcount() == 256 && buff[255] != '\0')
			throw "load_rules: tag table in ruleset file has entry longer than 256 characters!";

		string tag_entry(buff);
		// Word and tag are delimited by a space.
		tag_lookup[ tag_entry.substr(0, tag_entry.find(' ')) ]
								= tag_entry.substr( tag_entry.find(' ') + 1 );
	}

	// Return to the place where rules ids are:
	rl_file.clear();
	rl_file.seekg( rl_file.tellg() - rl_file.gcount() );

	// Load the list of the rules.
	while(rl_file.good()) {

		rl_file.getline(buff, 256);
		if(rl_file.gcount() == 256 && buff[255] != '\0')
			throw "load_rules: ruleset file has a line longer than 256 characters!";
		if(string(buff).size() > 0)
			rules.push_back( atoi_longlong(buff) );
	}

	gold_corp = standard;
}

Corpus RulesApplier::transform(Corpus * corp) {

	Corpus next_corp = * corp;

	// Tag with loaded initial (unigram) tagger. 
	next_corp = next_corp.tag_with_table(& tag_lookup);
	baseline_score = Corpus::compare_corpora( gold_corp, & next_corp);

	// Apply each rule on the list consecutively.
	for(unsigned int i = 0; i < rules.size(); i++) {

		// Figure out what rule this is.
		int rule_type = -1;
		string cue1, cue2, alter_tag, target;

		if(rules[i] >= EXT_RULE_RANGE) {
			long long rule_ind = rules[i]; // temporary variable that will be stripped

			rule_type = rule_ind / EXT_RULE_RANGE; // nice that int will strip the remainder
			rule_ind -= rule_type * EXT_RULE_RANGE;

			int tag_id = rule_ind / pow(tag_index.size(), 3);
			cue1 = tag_index[ tag_id ];
			rule_ind -= tag_id * pow(tag_index.size(), 3);

			tag_id = rule_ind / pow(tag_index.size(), 2);
			cue2 = tag_index[ tag_id ];
			rule_ind -= tag_id * pow(tag_index.size(), 2);

			tag_id = rule_ind / tag_index.size();
			alter_tag = tag_index[ tag_id ];
			rule_ind -= tag_id * tag_index.size();

			target = tag_index[ rule_ind ];
		}

		else {
			long long rule_ind = rules[i]; // temporary variable that will be stripped

			rule_type = rule_ind / RULE_RANGE;
			rule_ind -= rule_type * RULE_RANGE;

			int tag_id = rule_ind / pow(tag_index.size(), 2);
			cue1 = tag_index[ tag_id ];
			rule_ind -= tag_id * pow(tag_index.size(), 2);

			tag_id = rule_ind / tag_index.size();
			alter_tag = tag_index[ tag_id ];
			rule_ind -= tag_id * tag_index.size();

			target = tag_index[ rule_ind ];
		}

		// Create this rule and apply it.
		Rule rl;
		rl.cue1 = cue1;
		rl.cue2 = cue2;
		rl.alter_tag = alter_tag;
		rl.target = target;
		rl.id = rules[i];

		next_corp = rl.transform_corp(& next_corp);
	}

	final_score = Corpus::compare_corpora( gold_corp, & next_corp);

	return next_corp;
}
#endif
