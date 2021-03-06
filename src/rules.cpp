#include <string>
#include <vector>

#include "corpus.h"
#include "rules.h"

string Rule::description() {
  string res;
	res = "Change " + alter_tag + " to " + target + " when ";

	if(id < (RL_PRECEDING+1) * RULE_RANGE)
		res += "preceding tag is ";
	if(id >= RL_FOLLOWING * RULE_RANGE && id < (RL_FOLLOWING+1) * RULE_RANGE)
		res += "following tag is ";
	if(id >= RL_2BEFORE * RULE_RANGE && id < (RL_2BEFORE+1) * RULE_RANGE)
		res += "tag 2 before is ";
	if(id >= RL_2AFTER * RULE_RANGE && id < (RL_2AFTER+1) * RULE_RANGE)
		res += "tag 2 after is ";
	if(id >= RL_ONE_OF_2BEFORE * RULE_RANGE && id < (RL_ONE_OF_2BEFORE+1) * RULE_RANGE)
		res += "one of 2 tags before is ";
	if(id >= RL_ONE_OF_2AFTER * RULE_RANGE && id < (RL_ONE_OF_2AFTER+1) * RULE_RANGE)
		res += "one of 2 tags after is ";
	if(id >= RL_ONE_OF_3BEFORE * RULE_RANGE && id < (RL_ONE_OF_3BEFORE+1) * RULE_RANGE)
		res += "one of 3 tags before is ";
	if(id >= RL_ONE_OF_3AFTER * RULE_RANGE && id < (RL_ONE_OF_3AFTER+1) * RULE_RANGE)
		res += "one of 3 tags after is ";
	if(id >= EXTRL_PRECEDING_AND_FOLLOWING * EXT_RULE_RANGE && id < (EXTRL_PRECEDING_AND_FOLLOWING+1) * EXT_RULE_RANGE)
		res += "preceding and following tags are ";
	if(id >= EXTRL_1_AND_2_BEFORE * EXT_RULE_RANGE && id < (EXTRL_1_AND_2_BEFORE+1) * EXT_RULE_RANGE)
		res += "preceding and 2 before tags are ";
	if(id >= EXTRL_1_AND_2_AFTER * EXT_RULE_RANGE && id < (EXTRL_1_AND_2_AFTER+1) * EXT_RULE_RANGE)
		res += "following and 2 after tags are ";

	res += cue1 + " ";
	if(id >= EXT_RULE_RANGE)
    res += "and " + cue2 + " ";

  return res;
}

Corpus Rule::transform_corp(Corpus * corp) {

	Corpus new_corp;
	new_corp.sentences.reserve(corp->sentences.size());

	// Change from tag X to Y when preceding tag is Z:
	if(id < (RL_PRECEDING+1) * RULE_RANGE) 
		for(unsigned int si = 0; si < corp->sentences.size(); si++) {
			vector<TaggedWord> new_stc(corp->sentences[si]); // make a copy
			for(unsigned int i = 1; i < corp->sentences[si].size(); i++)
				if(corp->sentences[si][i-1].tag == cue1
						&& corp->sentences[si][i].tag == alter_tag)
					new_stc[i].tag = target;
			new_corp.sentences.push_back(new_stc);
		}

	// Change from tag X to Y when following tag is Z:
	if(id >= RL_FOLLOWING * RULE_RANGE && id < (RL_FOLLOWING+1) * RULE_RANGE)
		for(unsigned int si = 0; si < corp->sentences.size(); si++) {
			vector<TaggedWord> new_stc(corp->sentences[si]); // make a copy
			for(unsigned int i = 0; i+1 < corp->sentences[si].size(); i++)
				if(corp->sentences[si][i+1].tag == cue1
						&& corp->sentences[si][i].tag == alter_tag)
					new_stc[i].tag = target;
			new_corp.sentences.push_back(new_stc);
		}

	// Change from tag X to Y when the tag two before is Z:
	if(id >= RL_2BEFORE * RULE_RANGE && id < (RL_2BEFORE+1) * RULE_RANGE)
		for(unsigned int si = 0; si < corp->sentences.size(); si++) {
			vector<TaggedWord> new_stc(corp->sentences[si]); // make a copy
			for(unsigned int i = 2; i < corp->sentences[si].size(); i++)
				if(corp->sentences[si][i-2].tag == cue1
						&& corp->sentences[si][i].tag == alter_tag)
					new_stc[i].tag = target;
			new_corp.sentences.push_back(new_stc);
		}

	// Change from tag X to Y when the tag two after is Z:
	if(id >= RL_2AFTER * RULE_RANGE && id < (RL_2AFTER+1) * RULE_RANGE)
		for(unsigned int si = 0; si < corp->sentences.size(); si++) {
			vector<TaggedWord> new_stc(corp->sentences[si]); // make a copy
			for(unsigned int i = 0; i+2 < corp->sentences[si].size(); i++)
				if(corp->sentences[si][i+2].tag == cue1
						&& corp->sentences[si][i].tag == alter_tag)
					new_stc[i].tag = target;
			new_corp.sentences.push_back(new_stc);
		}


	// Change from tag X to Y when one of two tags before is Z:
	if(id >= RL_ONE_OF_2BEFORE * RULE_RANGE && id < (RL_ONE_OF_2BEFORE+1) * RULE_RANGE)
		for(unsigned int si = 0; si < corp->sentences.size(); si++) {
			vector<TaggedWord> new_stc(corp->sentences[si]); // make a copy
			for(unsigned int i = 2; i < corp->sentences[si].size(); i++)
				if((corp->sentences[si][i-1].tag == cue1
					|| corp->sentences[si][i-2].tag == cue1)
						&& corp->sentences[si][i].tag == alter_tag)
					new_stc[i].tag = target;
			new_corp.sentences.push_back(new_stc);
		}

	// Change from tag X to Y when one of two tags after is Z:
	if(id >= RL_ONE_OF_2AFTER * RULE_RANGE && id < (RL_ONE_OF_2AFTER+1) * RULE_RANGE)
		for(unsigned int si = 0; si < corp->sentences.size(); si++) {
			vector<TaggedWord> new_stc(corp->sentences[si]); // make a copy
			for(unsigned int i = 0; i+2 < corp->sentences[si].size(); i++)
				if((corp->sentences[si][i+1].tag == cue1
					|| corp->sentences[si][i+2].tag == cue1)
						&& corp->sentences[si][i].tag == alter_tag)
					new_stc[i].tag = target;
			new_corp.sentences.push_back(new_stc);
		}

	// Change from tag X to Y when one of three tags before is Z:
	if(id >= RL_ONE_OF_3BEFORE * RULE_RANGE && id < (RL_ONE_OF_3BEFORE+1) * RULE_RANGE)
		for(unsigned int si = 0; si < corp->sentences.size(); si++) {
			vector<TaggedWord> new_stc(corp->sentences[si]); // make a copy
			for(unsigned int i = 3; i < corp->sentences[si].size(); i++)
				if((corp->sentences[si][i-1].tag == cue1
					|| corp->sentences[si][i-2].tag == cue1
					|| corp->sentences[si][i-3].tag == cue1)
						&& corp->sentences[si][i].tag == alter_tag)
					new_stc[i].tag = target;
			new_corp.sentences.push_back(new_stc);
		}

	// Change from tag X to Y when one of three tags after is Z:
	if(id >= RL_ONE_OF_3AFTER * RULE_RANGE && id < (RL_ONE_OF_3AFTER+1) * RULE_RANGE)
		for(unsigned int si = 0; si < corp->sentences.size(); si++) {
			vector<TaggedWord> new_stc(corp->sentences[si]); // make a copy
			for(unsigned int i = 0; i+3 < corp->sentences[si].size(); i++)
				if((corp->sentences[si][i+1].tag == cue1
					|| corp->sentences[si][i+2].tag == cue1
					|| corp->sentences[si][i+3].tag == cue1)
						&& corp->sentences[si][i].tag == alter_tag)
					new_stc[i].tag = target;
			new_corp.sentences.push_back(new_stc);
		}

	// Change from tag X to Y when preceding tag is W and following Z:
	if(id >= EXTRL_PRECEDING_AND_FOLLOWING * EXT_RULE_RANGE &&
			id < (EXTRL_PRECEDING_AND_FOLLOWING+1) * EXT_RULE_RANGE)
		for(unsigned int si = 0; si < corp->sentences.size(); si++) {
			vector<TaggedWord> new_stc(corp->sentences[si]); // make a copy
			for(unsigned int i = 1; i+1 < corp->sentences[si].size(); i++)
				if((corp->sentences[si][i-1].tag == cue1
					&& corp->sentences[si][i+1].tag == cue2)
						&& corp->sentences[si][i].tag == alter_tag)
					new_stc[i].tag = target;
			new_corp.sentences.push_back(new_stc);
		}

	// Change from tag X to Y when preceding tag is W and two before Z:
	if(id >= EXTRL_1_AND_2_BEFORE * EXT_RULE_RANGE &&
			id < (EXTRL_1_AND_2_BEFORE+1) * EXT_RULE_RANGE)
		for(unsigned int si = 0; si < corp->sentences.size(); si++) {
			vector<TaggedWord> new_stc(corp->sentences[si]); // make a copy
			for(unsigned int i = 2; i < corp->sentences[si].size(); i++)
				if((corp->sentences[si][i-1].tag == cue1
					&& corp->sentences[si][i-2].tag == cue2)
						&& corp->sentences[si][i].tag == alter_tag)
					new_stc[i].tag = target;
			new_corp.sentences.push_back(new_stc);
		}

	// Change from tag X to Y when following tag is W and two after Z:
	if(id >= EXTRL_1_AND_2_AFTER * EXT_RULE_RANGE &&
			id < (EXTRL_1_AND_2_AFTER+1) * EXT_RULE_RANGE)
		for(unsigned int si = 0; si < corp->sentences.size(); si++) {
			vector<TaggedWord> new_stc(corp->sentences[si]); // make a copy
			for(unsigned int i = 0; i+2 < corp->sentences[si].size(); i++)
				if((corp->sentences[si][i+1].tag == cue1
					&& corp->sentences[si][i+2].tag == cue2)
						&& corp->sentences[si][i].tag == alter_tag)
					new_stc[i].tag = target;
			new_corp.sentences.push_back(new_stc);
		}

	return new_corp;
}
