## Transformation-based POS tagger
This is a transformational part-of-speech tagger, implementing 8 basic rule templates from the classic Brill paper [1]:

> **Change tag a to tag b when**:
> 1. The preceding (following) word is w.
> 2. The word two before (after) is w.
> 3. One of the two preceding (following) words is w.
> 4. The current word is w and the preceding (following) word is x.
> 5. The current word is w and the preceding (following) word is tagged z.
> 6. The current word is w.
> 7. The preceding (following) word is w and the preceding (following) tag is t.
> 8. The current word is w, the preceding (following) word is w2 and the preceding (following) tag is t.

## Howto
Installation:
1. `git clone https://github.com/szmer/brill.git`
2. `cd brill`
3. `make && make clean`

Use `./Brill-train` to learn the transformations (on your training corpus). Use `./Brill-test` to perform the test (on your test corpus). Both these programs display some help if invoked without arguments.

Your tagset can have up to 499 distinct entries.

## About & Implementation
I wrote this as an assignment for the Linguistic Engineering class (part of the Cognitive Science program in the University of Warsaw). The main assignment was to implement a trigram tagger in Python from scratch, but of course I chose to go as hardcore as I possibly could.

600 transformations learned on training corpus (extracted from the Brown corpus, 900k words, 472 tags) give 93.42% accuracy on the test corpus (100k words). (To train this it takes ~4 hours.) This compares to the 90.14% achieved by an unigram tagger and 92.28% accuracy achieved by a trigram tagger (shipped with NLTK Python library).

The main problem with implementation of a Brill classifier is that the size of space of possible rules is rather large. The Brown corpus, which I used as an example, actually has 472 part-of-speech tags, yielding ca. 670k possible rules just from the templates mentioned above. This figure should be multiplied by the number of the words in the corpus, and the number of algorithm iterations (we need one for each rule to be learned).

Some way of mitigating this problem was proposed e.g. in [2], and in the papers cited there. I had little experience with more advanced data structures at the time when I wrote this program, and eventually came up with my own crude, but working solution. Essentially:

1. Each possible rule is assigned an unique integer (which we can compute in place) for speed of access.
2. We compute the introduced error only for the rules which correct some mistake from the previous iterations (these are the eligible rules),
3. In the second loop, these rules are removed from eligible rules set as soon as they prove to introduce more errors than they correct.

[1] E. Brill, 1995. Transformation-Based Error-Driven Learning and Natural Language Processing: A Case Study in Part-of-Speech Tagging. *Computation Linguistics*, 21(4):543-565. [(pdf)](http://www.aclweb.org/anthology/J/J95/J95-4004.pdf)

[2] G. Ngai and R. Florian, 2001. Transformation-Based Learning in the Fast Lane. Second Meeting of the North American Chapter of the Association for Computational Linguistics. [(pdf)](http://arxiv.org/abs/cs/0107020)

## License
MIT.