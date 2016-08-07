#ifndef __CORPUS_H__
#define __CORPUS_H__

#include <map>
#include <string>
#include <vector>

using namespace std;

struct TaggedWord {
	TaggedWord(string w, string t) {
		word = w;
		tag = t;
	};

	string word;
	string tag;
};

struct Corpus {
	// compare_corpora: return fraction of <word, tag> pairs that DO match between these.
	static double compare_corpora(Corpus * fst, Corpus * sec);

	// load_file: create a corpus object, extracted from given file
	// (format: one tagged word per line: "word\tTAG", empty line in the beginning of each sentence)
	static Corpus load_file(const char * fname);
	static vector<string> load_tagset(const char * fname);

	// tag_all_unigram: tag every word in the child using one most likely tag for each word, as implied
	// by current tagging
	Corpus tag_all_unigram(bool leave_table); // if true is passed, function will leave table that can be saved

	// tag_with_table: tag every word according to a table; for unknown tokens most frequent tag will be assumed
	Corpus tag_with_table(map<string,string> * tbl); 

	vector< vector<TaggedWord> > sentences;
	map<string, string> unigram_table; // may be filled by tag_all_unigram() if requested
};

#endif
