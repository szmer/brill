#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "src/corpus.h" 
#include "src/rules.h" 
#include "src/rules_scoretab.h" 

#include "src/corpus.cpp" 
#include "src/rules.cpp" 
#include "src/rules_scoretab.cpp" 

using namespace std;

int main(int argc, char * argv[]) {

	if(argc < 3 || argc > 4) {
		cout << "Usage: Brill-train CORPUS_FILE TAGSET_FILE (OUTPUT_FILE)" << endl;
		return 1;
	}

	try {
		Corpus base_corp = Corpus::load_file(argv[1]);
		vector<string> tagset = Corpus::load_tagset(argv[2]);

		// Tag the corpus with the baseline unigram tagger:
		Corpus old_corp = base_corp.tag_all_unigram(argc == 4); // request leaving unigram table
									// (if we intend to make a tagger file)
		float old_score = Corpus::compare_corpora(& base_corp, & old_corp);
		cout << "Unigram accuracy: " << old_score << endl;
		
		// Prepare a file where we will output discovered rules.
		ofstream out;
		if(argc == 4) {
			out.open(argv[3], ofstream::trunc);

			for(map<string,string>::iterator i = base_corp.unigram_table.begin();
				i != base_corp.unigram_table.end(); i++)
				out << i->first << ' ' << i->second << '\t';

			base_corp.unigram_table.clear();

			out << "\n";
			out.flush();
		}

		// Try to obtain improving transformation, until improving margin is less than .01%. 
		RulesScoreTab tab(tagset, & base_corp);
		Corpus new_corp;
		float new_score = -1.0;
		int i = 0;
		for(; i < 100; i++) {
			if (new_score > 0.0) {
				old_score = new_score;
				old_corp = new_corp;
			}

			tab.clear();

			try { tab.examine(& old_corp); }
			catch(const char * e) { // if the program couldn't find any new good rules, just terminate
						// the loop, and rethrow any other exception
				if(e == "No good rules found")	break;
				else	throw e;
			}

			Rule * winner = tab.get_winner();
			new_corp = winner -> transform_corp(& old_corp);
			if(out.is_open())
				out << winner->id << "\n";

			new_score = Corpus::compare_corpora(& base_corp, & new_corp);
			cout << "-> " << new_score << endl; // .get_winner() would print the rule itself
		}

		cout << "Final score, with " << i<< " rules: " << new_score << endl;
	}

	catch(const char * e) {
		cout << e << endl;
		return 1;
	}

	return 0;
}
