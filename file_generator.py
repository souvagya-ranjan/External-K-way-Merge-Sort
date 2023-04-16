import os
import string
import random
import sys

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print('Usage: python generate_file.py <seed> <num_words>')
        sys.exit(1)
    random.seed(int(sys.argv[1]))
    num_words = int(sys.argv[2])
    with open('random.txt', 'w') as f:
        for i in range(num_words):
            res = ''.join(random.choices(string.ascii_letters, k=random.randrange(1024)))
            f.write(res)
            f.write('\n')