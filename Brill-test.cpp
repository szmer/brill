#include <iostream>
#include <vector>

#include "src/corpus.h" 
#include "src/rules.h" 
#include "src/rules_applier.h" 

#include "src/corpus.cpp" 
#include "src/rules.cpp" 
#include "src/rules_applier.cpp" 

using namespace std;

int main(int argc, char ** argv) {

	if(argc != 4) {
		cout << "Usage: Brill-test CORPUS_FILE TAGSET_FILE TRANSFORMATIONS_FILE" << endl;
		return 1;
	}

	try {
		Corpus base_corp = Corpus::load_file(argv[1]);
		RulesApplier appl(argv[2], argv[3], & base_corp);

		appl.transform(& base_corp); // discard returned corpus, we need only the scores

		cout << "Unigrams: " << appl.baseline_score << endl;
		cout << "Transformations: " << appl.final_score << endl;
	}

	catch(const char * e) {
		cout << e << endl;
		return 1;
	}

	return 0;
}
