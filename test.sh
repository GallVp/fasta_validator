#!/bin/bash

# code to test fasta validate

echo "Testing a good fasta file. Should return 0"
./fasta_validate test/good.fasta;
echo "Returned: $?"

echo "Testing a good fasta file with stop codons. Should return 0"
./fasta_validate test/good_with_stop_codons.fasta;
echo "Returned: $?"

echo "Testing another good fasta file. Should return 0"
./fasta_validate test/good_mixed.fasta;
echo "Returned: $?"

echo "Testing the last good fasta file. Should also return 0"
./fasta_validate test/good_multiline.fasta
echo "Returned: $?"

echo -e "\nThese commands should give error messages. We have turned on verbose so you can see:"
echo "A fasta file with no > on the first line";
./fasta_validate -v test/no_first_line.fasta
echo "Returned: $?"

echo "A fasta file with spaces in the sequence"
./fasta_validate -v test/space.fasta
echo "Returned: $?"

echo "A fasta file with duplicate ids"
./fasta_validate -v test/duplicates_no_spaces.fasta
echo "Returned: $?"

echo "A fasta file with duplicate ids even though there are spaces"
./fasta_validate -v test/duplicates_with_spaces.fasta
echo "Returned: $?"

echo "A fasta file with stop codon in the middle"
./fasta_validate -v test/bad_with_stop_codons.fasta;
echo "Returned: $?"

echo "A fasta file with php code in the middle. Yes, we've seen this!"
./fasta_validate -v test/code.fasta
echo "Returned: $?"

echo "Files with blank sequences:"
./fasta_validate -v test/blank_sequence1.fasta
echo "First sequence returned $?";
./fasta_validate -v test/blank_sequence2.fasta
echo "Middle sequence returned $?";
./fasta_validate -v test/blank_sequence3.fasta
echo "Last sequence returned $?";
