#include <cstdlib>
#include <cstring>
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

void print_usage() {
  cout << "Usage: Brill-train CORPUS_FILE TAGSET_FILE [-o OUTPUT_FILE] [-m DESIRED_MARGIN]" << endl << "      [-r MAX_RULES]" << endl << endl;
  cout << "   CORPUS_FILE  should be in format: word\\tcorrect-tag\\n for each word-tag pair," << endl << "                with each sentence starting with one empty line." << endl;
  cout << "   TAGSET_FILE  should contain all the possible tags: tag\\n for each tag." << endl;
  cout << "   -o (OUTPUT_FILE)  will be overwritten." << endl << endl;
  cout << "   Ways to tell when to stop learning." << endl;
  cout << "   -m  is desired margin, as a fractional number, of improvement of the last" << endl << "       transformation comparing to the one step earlier." << endl;
  cout << "   -r  specifies how many rules should the trainer produce (default: 1000)." << endl;
}

void prepare_output_file(ofstream * out, string name, Corpus * corp) {
  if(name == "")
    return;

  out->open(name.c_str(), ofstream::trunc);

  for(map<string,string>::iterator i = corp->unigram_table.begin();
                                   i != corp->unigram_table.end(); i++)
    (*out) << i->first << ' ' << i->second << '\t';

  corp->unigram_table.clear();

  (*out) << "\n";
  out->flush();
}

int main(int argc, char * argv[]) {
	if(argc < 3 || (argc > 3 && (argc-3) % 2 != 0)) {
    print_usage();
		return 1;
	}

  string output_file_name = string("");
  double improv_margin = 0.0;
  int max_rules = 1000;
  ofstream out;

  // Parse the arguments.
  for (int i = 3; i+1 < argc; i += 2) {
    if(strcmp(argv[i], "-o") == 0)
      output_file_name = string(argv[i+1]);
    if(strcmp(argv[i], "-m") == 0)
      improv_margin = atof(argv[i+1]);
    if(strcmp(argv[i], "-r") == 0)
      max_rules = atoi(argv[i+1]);
  }

	try {
    // Make the base corpus (containing the truth) and the tagset vector.
		Corpus base_corp = Corpus::load_file(argv[1]);
		vector<string> tagset = Corpus::load_tagset(argv[2]);

		// Tag the corpus with the baseline unigram tagger:
		Corpus old_corp = base_corp.tag_all_unigram(output_file_name != "");
    // /\ request leaving unigram table(if we intend to make a tagger file)
		double old_score = Corpus::compare_corpora(& base_corp, & old_corp);
		cout << "Unigram accuracy: " << old_score << endl;

		// Prepare a file where we will output discovered rules (btw put unigrams there).
    prepare_output_file(&out, output_file_name, &base_corp);

		RulesScoreTab tab(tagset, & base_corp);
		Corpus new_corp;
    double new_score = 10.0; // to be compared to the old score (first defined on unigrams above)
    int counter = 0;
		while(counter != max_rules && improv_margin < new_score-old_score) {

      if (counter > 0) { // some cleanup necessary, but not in the first iteration
        tab.clear(); // remove the old rules scores
        old_score = new_score;
        old_corp = new_corp;
      }

      // Try to find some useful rules.
			try { tab.examine(& old_corp); }
			catch(const char * e) { // if the program couldn't find any new good rules, just terminate
						// the loop, and rethrow any other exception
				if(e == "No good rules found")	break;
				else	throw e;
			}

			Rule * winner = tab.get_winner();
			new_corp = winner -> transform_corp(& old_corp);

      // print the rule to the output file, if present
			if(out.is_open())
				out << winner->id << "\n";

			new_score = Corpus::compare_corpora(& base_corp, & new_corp);
			cout << (counter+1) << ". " << winner->description() << "-> " << new_score << endl;

      counter ++;
		} // generate the rules

		cout << "Final score, with " << counter << " rules: " << new_score << endl;
  } // try

  // Catch and print any errors, and exit.
	catch(const char * e) {
		cout << e << endl;
		return 1;
	}

	return 0;
}
