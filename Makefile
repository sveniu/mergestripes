all:
	gcc -W -Wall -Wextra -O2 -o mergestripes mergestripes.c

clean:
	rm -f mergestripes
