Please enter following command in the appropriate directory:
gcc RajFilters.c BmpProcessor.c PixelProcessor.c -o RajFilters -lm -pthread

To run the program, run the following command next:
./RajFilters -i test3.bmp -o output.bmp -f b

Note: The -i input file name, -o output file name, and -f filter type are mandatory arguments. For filter type, "b" denotes box blur filter while "c" denotes swiss cheese filter. If using box blur filter, the program prompts the user for how many threads are desired to apply the filter. 