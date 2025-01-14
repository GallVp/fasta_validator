/* 
 * Validate a fasta file
 *
 * Rob Edwards January 2019.
 *
 * We exit with:
 *	0: this is a valid fasta file
 *	1: the first line does not start with a >
 *	2: the ids are not unique
 *	4: lines in the sequence (that do not start >) contain characters that do not match the perl regexp /[A-Z][a-z] / with the exception that the last character can be a stop codon (. or *)
 *
 *	Over 200:
 *	internal errors, eg. unable to allocate memory, etc.
 */

#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <search.h>

#include "fasta_validate.h"


int contains_non_word_characters(char *seq, int verbose) {
	/*
	 * we consider word characters to be ASCII:
	 *      64 < char < 91   (A=65; Z=90)
	 *      96 < char < 123  (a=97; z=122)
	 */

	
	if (seq == NULL) {
		fprintf(stderr, "Empty line receieved. Empty string?\n");
		return 8;
	}

	for (int i=0; i<strlen(seq); i++) {

		if (i == (strlen(seq)-2)) {
			if ((((int) seq[i]) == 46) || (((int) seq[i]) == 42)) {
				continue;
			}
		}

		if ((int) seq[i] < 65) {
			if (((int) seq[i] != 10) && ((int) seq[i] != 13))
				return 1;
		}
		else if (((int) seq[i] > 90) && ((int) seq[i] < 97))
			return 2;
		else if ((int) seq[i] > 122)
			return 3;
	}
	return 0;
}

void help(char *nm) {
	printf("%s version %d\n\nCheck and validate a fasta file\n", nm, VERSION);
	printf("Rob Edwards.\n\n");
	printf("%s checks your fasta file and exits with:\n", nm);
	printf("\t0: this is a valid fasta file\n");
	printf("\t1: the first line does not start with a >\n");
	printf("\t2: the ids are not unique\n");
	printf("\t4: lines in the sequence (that do not start >) contain characters that do not match the perl regexp /[A-Z][a-z]/\n");
	printf("\t8: There is a sequence with zero length in it\n\n");
	printf("\tOver 200:\n");
	printf("\t\tinternal errors, eg. unable to allocate memory, etc.\n\n");
	printf("Usage: %s [options] [fasta file]\n\t-v: verbose output\n\t-V: current version\n\t-h: this help\n", nm);
}


int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("%s [-h] [-v] [-V] [fasta file]\n", argv[0]);
		exit(1);
	}

	char *filename = argv[1];
	int verbose = 0;

	if (strcmp(filename, "-h") == 0) {
		help(argv[0]);
		return 0;
	}

	if (strcmp(filename, "-V") == 0) {
		printf("%s version %d.\nFor more information please see https://github.com/linsalrob/fasta_validator\n", argv[0], VERSION);
		return 0;
	}


	if (strcmp(filename, "-v") == 0) {
		verbose = 1;
		filename = argv[2];
	}

	// create a hash with 1,000,000 possible entries
	int hc = hcreate(NUMSEQS);
	if (hc == 0) {
		fprintf(stderr, "Unable to create the hash table\n");
		return -1;
	}

	FILE * fp;

	char line[MAXLINELEN];

	if ((fp = fopen(filename, "r")) == NULL) {
		if (verbose)
			fprintf(stderr, "Can't open file %s\n", argv[1]);
		exit(1);
	}

	int firstline = 1;
	int seqcount = 0;
	while ((fgets(line, MAXLINELEN, fp)) != NULL) {
		if ((int) line[0] == 62) { // not sure why I'm using an ascii comparison, but I'm thinking ascii at the moment
			if (!firstline && seqcount == 0) {
				if (verbose) 
					fprintf(stderr, "ERROR: We have an empty sequence\n");
				return 8;
			}
			firstline = 0;
			seqcount = 0;
			// remove anything after the first space
			char *p = strchr(line, ' ');
			if (p)
				*p = '\0';

			// in case you need this!
			// fprintf(stderr, "Parsing %s\n", line);

			// check to see if we have seen this line
			// if not, add it to the hash
			ENTRY item;
			item.key = strdup(line);
			ENTRY *found_item;
			if ((found_item = hsearch(item, FIND)) != NULL) {
				if (verbose) {
					fprintf(stderr, "ERROR: Found a duplicate id: |%s|\n", line);
					fprintf(stderr, "ERROR: Found a duplicate id: |%s|\n", found_item->key);
				}
				return 2;
			}

			// fprintf(stderr, "adding |%s|\n", item.key);
			(void) hsearch(item, ENTER);
		} else {
			if (firstline > 0) {
				if (verbose)
					fprintf(stderr, "ERROR: The first line should start with a >\n");
				return 1; // the first line should start with a >
			}
			int nwc = contains_non_word_characters(line, verbose);
			if (nwc > 0) {
				if (verbose) 
					fprintf(stderr, "ERROR: We have a non word character!\n");
				return 4;
			}
			seqcount += strlen(line);
		}
	}

	if (seqcount == 0) {
		if (verbose) 
			fprintf(stderr, "ERROR: at tend We have an empty sequence\n");
		return 8;
	}
	return 0;
}


