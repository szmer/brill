#include <iostream>

#include "src/corpus.h" 

using namespace std;

int main(int argc, char * argv[]) {
	if(argc != 2) {
		cout << "Usage: unigram CORPUS_FILE\n";
		return 1;
	}

	try {
		Corpus base_corp = Corpus::load_file(argv[1]);
		Corpus uni_corp = base_corp.tag_all_unigram();
		cout << "Accuracy: " << Corpus::compare_corpora(& base_corp, & uni_corp) << "\n";
	}
	catch(const char * e) {
		cout << e << "\n";
		return 1;
	}
	return 0;
}
