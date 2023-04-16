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
    
    ```c++
    int external_merge_sort_withtop(const char* input, const char* output, const long key_count, const int k = 2, const int num_merges = 0);
    ```
