#ifndef __RULES_H__
#define __RULES_H__

#include <string>
#include <vector>

using namespace std;

struct Rule {

	string cue1;  // tag triggering the rule (when it is on relevant position)
	string cue2;  // the second cue (in extended rules) 

	string alter_tag; // <- change this tag to:
	string target; // result of the rule (tag which it changes to)

	long long id;

        // Rule ID consistent ordering of the tags. Various functions can thus, iterating, assign a consistent ID to each rule.
        // One can compute rule id knowing its cue tags, using the formulas given below.
        //
        // Current implementation allows corpus to have up to 499 different tags.
        // Note that long long type cap is 9,223,372,036,854,775,807.
        //
        // Rule id is:
        // (rule_number * RULE_RANGE + cue1_num * TAGSET_SIZE^2 + cue2_num * TAGSET_SIZE + target_tag_num)
        //
        // And for an extended rule:
        // (rule_number * RULE_RANGE + cue1_num * TAGSET_SIZE^3 + cue2_num * TAGSET_SIZE^2 + TAGSET_SIZE * cue3_num 
        //           + target_tag_num)
 
 
        //
        // *** Non-extended rules (taking two cues) ***
        //

	#define RULE_RANGE 124251499 // 499^3 

	// Change from tag X to Y when preceding tag is Z:
	#define RL_PRECEDING	 	0
	// Change from tag X to Y when following tag is Z:
	#define RL_FOLLOWING		1
	// Change from tag X to Y when the tag two before is Z:
	#define RL_2BEFORE 		2
	// Change from tag X to Y when the tag two after is Z:
	#define RL_2AFTER		3
	// Change from tag X to Y when one of two tags before is Z:
	#define RL_ONE_OF_2BEFORE	4
	// Change from tag X to Y when one of two tags after is Z:
	#define RL_ONE_OF_2AFTER	5
	// Change from tag X to Y when one of three tags before is Z:
	#define RL_ONE_OF_3BEFORE	6
	// Change from tag X to Y when one of three tags after is Z:
	#define RL_ONE_OF_3AFTER	7

	// We could define up to 499 non-extended rules.

        //
        // *** Extended rules (taking three cues) ***
        //
	#define EXT_RULE_RANGE 62001498001 // 499^4

	// Up to the range there are non-extended rules, so we have no zero.
	// Change from tag X to Y when preceding tag is W and following Z:
        #define EXTRL_PRECEDING_AND_FOLLOWING 	1
	// Change from tag X to Y when preceding tag is W and two before Z:
        #define EXTRL_1_AND_2_BEFORE 		2
	// Change from tag X to Y when following tag is W and two after Z:
        #define EXTRL_1_AND_2_AFTER 		3


	Corpus transform_corp(Corpus * corp);
} ;

#endif
