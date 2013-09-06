/* mergestripes - a simple stripe merger
 *
 * Open N input files, read stripes of size S from each input file in
 * order, and sequentially write each stripe to the output file.
 *
 * Usage: ./mergestripes [-s stripe_size] outfile infile infile [infile...]
 *
 * Default stripe size is 512 bytes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv)
{
	int i, opt, numinputs, doloop;
	FILE **fin;
	FILE *fout;
	char *buf;
	ssize_t stripe_size = 512;

	opt = getopt(argc, argv, "s:");
	if(opt == 's')
		stripe_size = atoll(optarg);

	buf = malloc(stripe_size);

	/* argc - optind = non-opt argument count */
	if(argc - optind <= 2)
	{
		fprintf(stderr, "Usage: %s [-s stripe_size] outfile "
				"infile infile [infile...]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Open input files for reading */
	numinputs = argc - optind - 1;
	fin = malloc(numinputs * sizeof(FILE *));
	for(i = 0; i < numinputs; i++)
	{
		if((fin[i] = fopen(argv[optind + 1 + i], "r")) == NULL)
		{
			fprintf(stderr, "fopen(%s) failed: %s\n",
					argv[optind + 1 + i],
					strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	/* Open output file for writing */
	if((fout = fopen(argv[optind], "w")) == NULL)
	{
		fprintf(stderr, "fopen(%s) failed: %s\n",
				argv[optind],
				strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Read/write loop */
	doloop = 1;
	while(doloop)
	{
		for(i = 0; i < numinputs && doloop; i++)
		{
			size_t bytes;

			bytes = fread(buf, 1, stripe_size, fin[i]);
			if(bytes > 0)
			{
				if(fwrite(buf, 1, bytes, fout) == 0)
				{
					if(ferror(fout))
						fprintf(stderr, "fwrite() failed!\n");
					doloop = 0;
				}
			}
			else
			{
				if(ferror(fin[i]))
					fprintf(stderr, "fread() failed!\n");
				doloop = 0;
			}
		}
	}

	fclose(fout);
	for(i = 0; i < numinputs; i++)
	{
		fclose(fin[i]);
	}
	free(buf);

	free(fin);
	return 0;
}
