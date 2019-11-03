# A really simple make file because this code is not complex!

FLAGS := $(FLAGS) -Wall -std=c99

all:
	gcc $(FLAGS) -o fasta_validate ./fasta_validate.c

install: all
	cp fasta_validate /usr/local/bin

test: all
	./test.sh

clean:
	rm -f fasta_validate
