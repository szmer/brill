#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "corpus.h"

using namespace std;

// compare_corpora: return fraction of <word, tag> pairs that DO match between these.
double Corpus::compare_corpora(Corpus * fst, Corpus * sec) {
	if(fst->sentences.size() != sec->sentences.size())
		throw "compare_corpora: unequal number of sentences!";

	int word_count = 0;
	int err_count = 0;

	for(unsigned int si = 0; si < fst->sentences.size(); si++) {
		if(fst->sentences[si].size() != sec->sentences[si].size())
			throw "compare_corpora: unequal size of one of the sentences!";

		word_count += fst->sentences[si].size(); // count each word

		for(unsigned int wi = 0; wi < fst->sentences[si].size(); wi++) // find actual disagreements
			if(fst->sentences[si][wi].tag != sec->sentences[si][wi].tag)
				err_count ++;
	}

	if(word_count == 0)
		throw "compare_corpora: corpora seem to be empty!";

	return ((double)word_count - (double)err_count) / (double)word_count;
}

// load_file: create a corpus object, extracted from given file
// (format: one tagged word per line: "word\tTAG", empty line in the beginning of each sentence)
Corpus Corpus::load_file(const char* fname) {
	Corpus corp;
	ifstream corp_file(fname);

	char buff [256];
	while(corp_file.good()) {

		corp_file.getline(buff, 256);
		if(corp_file.gcount() == 256 && buff[255] != '\0')
			throw "load_file: corpus file has a line longer than 256 characters!";

		string corp_file_line(buff);

		// Empty line = a new sentence in the corpus (initialize new sentence vector).
		if(corp_file_line.size() == 0) {
			corp.sentences.push_back( vector<TaggedWord>() );
			continue;
		}
		if(corp.sentences.size() == 0) // (occurs when there is no leading empty line in the file)
			throw "load_file: corpus file has no sentence start marker! (should be an empty line)";

		// ELSE: add new tagged word to the latest sentence vector.
		int tab_ind = corp_file_line.find("\t");
		if(tab_ind == -1 || tab_ind != (int)corp_file_line.rfind("\t"))
			throw "load_file: line in corpus \""+corp_file_line+"\" is improperly formatted!";

		string tag = corp_file_line.substr(tab_ind+1);
		corp.sentences.rbegin()->push_back( TaggedWord(
						corp_file_line.substr(0, tab_ind),
						tag
					) );
	}

	return corp;
}

vector<string> Corpus::load_tagset(const char* fname) {
	vector<string> tagset;
	ifstream ts_file(fname);

	char buff [256];
	while(ts_file.good()) {

		ts_file.getline(buff, 256);
		if(ts_file.gcount() == 256 && buff[255] != '\0')
			throw "load_tagset: tagset file has a line longer than 256 characters!";
		if(string(buff).size() > 0)
			tagset.push_back(string(buff));
	}

	return tagset;
}

// tag_all_unigram: tag every word in the child using one most likely tag for each word, as implied by current tagging
Corpus Corpus::tag_all_unigram(bool leave_table) {

	Corpus child;
	child.sentences.reserve(sentences.size());

	// Compute frequency distribution for each tag for each word token (in this Corpus instance).
	map< string, map<string,int> > tag_freqs; // word => <tag => frequency>

	for(vector< vector<TaggedWord> >::iterator stc = sentences.begin(); stc != sentences.end(); stc++) { 

		child.sentences.push_back( vector<TaggedWord>() );
		child.sentences.rbegin()->reserve( stc->size() );	// reserve space in the last sentence
									// vector of the child

		for(vector<TaggedWord>::iterator tword = stc->begin(); tword != stc->end(); tword ++) {

			if(tag_freqs[tword->word].count(tword->tag) == 0) // (entry for this tag doesn't exist yet)
				tag_freqs[tword->word][tword->tag] = 1;
			else
				tag_freqs[tword->word][tword->tag] ++;
	
			// Copy the word to the last sentence of the child Corpus (with an empty tag).
			child.sentences.rbegin()->push_back( TaggedWord(tword->word, string("")) );

		}
	}

	// Go through the words in the child Corpus and tag each with the most frequent tag.
	for(vector< vector<TaggedWord> >::iterator stc = child.sentences.begin(); stc != child.sentences.end()
						; stc++) 
		for(unsigned int wi = 0; wi < stc->size(); wi ++) {

			TaggedWord * tword = & stc->at(wi);
	
			// Now, each word will receive its only known interpretation. IF there is more than one 
			// possible tag for given word, wipe all entries in tag_freqs except for the tag with
			// the highest frequency.
			if(tag_freqs[tword->word].size() > 1) {

				string max_label = "";
				int max_val = 0;

				for(map<string,int>::iterator tag_entry = tag_freqs[tword->word].begin()
						; tag_entry != tag_freqs[tword->word].end(); tag_entry ++)
					if(tag_entry->second > max_val) {
						max_label = tag_entry->first;
						max_val = tag_entry->second;
					}

				tag_freqs[tword->word].clear();
				tag_freqs[tword->word][max_label]; // create entry, don't care about value
			}
	
			// (there is only one intepretation)
			// Extract tag label of this lone entry.
			tword->tag = tag_freqs[tword->word].begin()->first;
	
		}

	// Fill unigram_table if it is requested.
	if(leave_table)
		for(map<string, map<string,int> >::iterator i = tag_freqs.begin(); i != tag_freqs.end(); i ++)
			unigram_table[ i->first ] = i->second.begin()->first;

	return child;
}


Corpus Corpus::tag_with_table(map<string,string> * tbl) {

	// Find the most frequent tag in the table and make it default.
	map<string, int> freqs;
	for(map<string,string>::iterator i = tbl->begin(); i != tbl->end(); i++) {

		if(freqs.find(i->second) == freqs.end())
			freqs[i->second] = 1;

		else	freqs[i->second]++;

	}
	string default_tag;
	int default_freq = 0;
	for(map<string,int>::iterator i = freqs.begin(); i != freqs.end(); i++)
		if(i->second > default_freq) {
			default_freq = i->second;
			default_tag = i->first;
		}
	default_tag = "NN";

	// Create and tag the child corpus. 
	Corpus child;
	child.sentences.reserve(sentences.size());

	for( unsigned int si = 0; si < sentences.size(); si++ ) {

		vector<TaggedWord> * stc = & sentences[si];

		child.sentences.push_back( vector<TaggedWord>() );
		child.sentences.rbegin()->reserve( stc->size() );	// reserve space in the last sentence
									// vector of the child

		for( unsigned int wi = 0; wi < stc->size(); wi++ ) {
			if(tbl->find(stc->at(wi).word) != tbl->end()) // we know exact tag for this word
				child.sentences.rbegin()->push_back( TaggedWord(stc->at(wi).word,
									(*tbl)[stc->at(wi).word]) );
			else 					// use the default tag instead
				child.sentences.rbegin()->push_back( TaggedWord(stc->at(wi).word,
									default_tag) );
		}
	}

	return child;
}
