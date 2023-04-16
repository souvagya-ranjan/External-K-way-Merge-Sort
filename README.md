# External-K-way-Merge-Sort
In this assignment, the goal is to implement a fast K-way merge-sorting of a large number of character
strings (for simplicity we will assume all strings have only ASCII characters). You are allowed to use any
tricks for speeding up as long as you adhere to the constraints imposed by the arguments. For details see
the detailed description below:

## Workload 
We use the term workload to refer to a disk-based file consisting of variable length strings consisting
of ASCII (non-control) characters (you can assume them to be less than 1024-bytes). Each string is
terminated by a newline. We will use two different workloads – first, which contains English language
words/phrases; and the second, which contains purely random sequence of characters. The first workload
is called english.list and random.list – you must use the same terminology in your submissions as
well. Also note that you can not make any assumptions about the key distribution in these files – there
could be duplicates, there could be strings that are very small, there could be strings that contain special
characters (such as !, #, etc.), and so on.
The size of these workloads will be large (at least 10-20 times the available memory). Smaller scale
datasets (or the script to generate them) will be released two days before the submission deadline. This
is to help you to test your code against specific kind of dataset that will be used in the evaluation. For
scalability and efficiency, you may have to create and use your own datasets.

## Code Structure and Arguments
All your implementation must be in a single file (no additional header/source files). You will be required
to implement the following function which will be called from a main program that sets up the argument
values. **You must not change the function name or the argument order.**
```bash
    int external_merge_sort_withtop(const char* input, const char* output, const long key_count, const int k = 2, const int num_merges = 0);
```
The description of each argument is given in Table below. All intermediate runs will be written to files
named as temp.<stage>.<runnum>. For instance, when you create the first sorted runs from the input
file, you will output temp.0.1,temp.0.2,... and so on. The output after merging these two runs will be
temp.1.1,temp.1.2,.... etc. It is important for you to follow this convention strictly since we will use this
to monitor if your submissions are correct.
Note that you are free to use any strategy to speed your program. For instance, you can use any
data-structure / algorithm to implement the in-memory sorting step, you can use any form of compression
to reduce the intermediate disk I/O. However, if you use anything that is not part of the standard libraries
available on a Linux machine, you are expected to include the source-code in the submission file. For
example, if you want to use trie-structure for in-memory sorting of strings, you will have to implement it
yourself and include it in the submission file since it is not available as part of any standard Linux library.
Only the input and output files will be in ASCII format (i.e., they should be human readable
strings).

| Paramenter | Description |
| --- | --- |
| input | Full path of the file containing input strings | 
| output | Full path of the file where the sorted output should be written | 
| key_count | Number of keys in the input file that need to sorted. Note that there may be more keys in the file, in which case you will sort only the first key count keys from the file |
| k | the maximum fanout of the merges that are allowed. Note that this does not limit the number of runs or the size of each run that you are allowed to create. It simply restricts the number of runs that can be merged at once. Default value = 2. |
| num_merges | This is an optional argument that is given to stop the sorting process midway and inspect the intermediate results. If not specified (or set as 0), the function continues until the final sorted output is produced. Otherwise, it will stop after performing num merges number of merge steps. A merge-step is defined as the reading (at most) k runs, and writing out a merged output to a file. |
| return value | On successful completion, the function returns the number of merge-steps completed (see above). It returns a value less than 0 if it failed to complete successfully. |

## Evaluation Platform 
All programs will be built and executed on a Linux machine with single CPU, and 1GB of RAM (we
will use -O3 compiler optimization flag, with no debug symbols enabled). You can assume that there is
sufficient hard-disk space to hold the intermediate runs and the final sorted output file (apart from the
input dataset). No additional disk space can be used.

## Evaluation Instructions 
1. first clone the repository
2. cd into the directory
3. To create random input file, run the following command:
```bash
    python3 file_generator.py <seed> <num_words>
```
    The above command will create a file named random.txt with num_words number of words. The seed is specified such that each time the same input is produced. This is to ensure that the output of your program is consistent.
4. To compile the program, run the following command:
```bash
    g++ -std=c++11 main.cpp -O3 -o main 
```
5. To run the program, run the following command:
```bash
    ./main <input_file> <output_file> <num_keys> <k> <num_merges>
```
    The above command will run the program with the specified arguments. The input_file is the file that contains the input data. The output_file is the file where the sorted output will be written. The num_keys is the number of keys that need to be sorted. The k is the maximum fanout of the merges that are allowed. The num_merges is the number of merges that need to be performed. If num_merges is 0, then the program will run until the final sorted output is produced. If num_merges is greater than 0, then the program will stop after performing num_merges number of merges.