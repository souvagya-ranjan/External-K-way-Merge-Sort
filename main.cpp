#include <bits/stdc++.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

#define MB 1024 * 1024
#define MAX_KEY_LENGTH 1024

#ifndef FILE_BUFFER_SIZE
#define FILE_BUFFER_SIZE MB
#endif

#ifndef USE_FSTREAM
#define USE_FSTREAM true
#endif

#ifndef USE_SYMLINK
#define USE_SYMLINK true
#endif

#ifndef USE_TRIE
#define USE_TRIE false
#endif

size_t getAvailableMemory() {
  size_t mem_available = 0;
  try {
    ifstream file("/proc/meminfo");
    string line;
    while (file >> line) {
      if (line == "MemAvailable:") {
        file >> mem_available;
        mem_available /= 1024;
      }
    }
  } catch (exception &e) {
    cout << "Error: " << e.what() << endl;
  }
  mem_available = mem_available > 512 ? mem_available : 512;
  return mem_available;
}

#ifndef USE_MEM_LIMIT
size_t MEM_LIMIT_MB = getAvailableMemory() * 6 / 10;
#else
size_t MEM_LIMIT_MB = USE_MEM_LIMIT;
#endif

inline string get_file_name(int stage, int run) {
  return "temp." + to_string(stage) + "." + to_string(run);
}

size_t getFileSize(const std::string &filename) {
  struct stat stat_buf;
  int rc = stat(filename.c_str(), &stat_buf);
  return rc == 0 ? stat_buf.st_size : -1;
}

#if USE_FSTREAM

class Reader {
  char *buffer;
  const size_t buffer_size;

  ifstream stream;

  size_t size_of_file = 0;
  size_t size_read = 0;

public:
  Reader(const string &filename, const size_t buffer_size)
      : buffer_size(buffer_size) {
    buffer = new char[buffer_size];
    size_of_file = getFileSize(filename);
    // manual buffering
    stream.rdbuf()->pubsetbuf(buffer, buffer_size);
    stream.open(filename, ios::binary);
  }

  string nextWord() {
    string word;
    getline(stream, word);
    size_read += word.length() + 1;
    return word;
  }

  bool isEof() { return size_read >= size_of_file; }

  ~Reader() {
    delete[] buffer;
    stream.close();
  }
};

#else

class Reader {
  /**
   * Reads words from a file.
   * Can read many characters from a file at once, storing them in a buffer.
   */
  char *buffer;
  const size_t buffer_size;
  size_t buffer_pos = 0;
  size_t buffer_end = 0;

  size_t size_of_file = 0;
  size_t size_read = 0;

  bool eof = false;
  FILE *stream;

public:
  Reader(const string &filename, const size_t buffer_size)
      : buffer_size(buffer_size) {
    buffer = new char[buffer_size];
    // manual buffering
    stream = fopen(filename.c_str(), "rb");
    buffer_end = fread(buffer, 1, buffer_size, stream);
    size_of_file = getFileSize(filename);
  }

  bool isEof() { return size_read >= size_of_file; }

  string nextWord() {
    if (eof) {
      return "";
    }
    // read the next word from the buffer
    size_t string_start = buffer_pos;
    while (buffer_pos < buffer_end && buffer[buffer_pos] != '\n') {
      buffer_pos++;
    }
    string word(buffer + string_start, buffer_pos - string_start);
    if (buffer_pos == buffer_end) {
      if (buffer_end < buffer_size) {
        // end of file
        eof = true;
        return word;
      } else {
        // read more from the file
        buffer_end = fread(buffer, 1, buffer_size, stream);
        buffer_pos = 0;
        string_start = 0;
        while (buffer_pos < buffer_end && buffer[buffer_pos] != '\n') {
          buffer_pos++;
        }
        // assuming that the word is not longer than the buffer
        word += string(buffer + string_start, buffer_pos - string_start);
      }
    }
    buffer_pos++;

    size_read += word.length() + 1;

    return word;
  }
  ~Reader() {
    delete[] buffer;
    fclose(stream);
  }
};

#endif

class Writer {
  /**
   * Writes words to a file.
   * Can buffer writes
   */
  char *buffer;
  const size_t buffer_size;
  size_t buffer_pos = 0;
  FILE *stream;

public:
  Writer(const string &filename, const size_t buffer_size)
      : buffer_size(buffer_size) {
    buffer = new char[buffer_size];
    // manual buffering
    stream = fopen(filename.c_str(), "wb");
  }
  void writeWord(const string &word) {
    if (buffer_pos + word.length() + 1 > buffer_size) {
      // flush the buffer
      fwrite(buffer, 1, buffer_pos, stream);
      buffer_pos = 0;
    }
    copy(word.begin(), word.end(), buffer + buffer_pos);
    buffer_pos += word.length();
    buffer[buffer_pos] = '\n';
    buffer_pos++;
  }
  ~Writer() {
    // flush the buffer
    if (buffer_pos > 0) {
      fwrite(buffer, 1, buffer_pos, stream);
    }
    delete[] buffer;
    fclose(stream);
  }
};

class TrieNode {
public:
  TrieNode *next, *child, *parent;
  char c = 0;
  uint32_t count = 0;

  TrieNode() { next = child = parent = nullptr; }
  // return word if this is a leaf
  string getWord(char *buffer) {
    if (parent == nullptr) {
      return "";
    }
    int i = 0;
    TrieNode *node = this;
    while (node->parent != nullptr) {
      buffer[i++] = node->c;
      node = node->parent;
    }
    string word(buffer, i);
    reverse(word.begin(), word.end());
    return word;
  }

  ~TrieNode() {
    // prev/parent called delete on this
    if (next != nullptr) {
      delete next;
    }
    if (child != nullptr) {
      delete child;
    }
  }
};

class Trie {
  char buffer[MAX_KEY_LENGTH];

public:
  size_t size = 0;

  Trie() { root = new TrieNode(); }

  TrieNode *firstNode() { return root->child; }
  TrieNode *nextNode(TrieNode *leaf) {
    // pre order traversal
    if (leaf->child != nullptr) {
      return leaf->child;
    }
    if (leaf->next != nullptr) {
      return leaf->next;
    }
    TrieNode *node = leaf->parent;
    while (node != nullptr) {
      if (node->next != nullptr) {
        return node->next;
      }
      node = node->parent;
    }
    return nullptr;
  }
  TrieNode *root;

  void insert(const string &word) {
    TrieNode *node = root;
    if (word.size() == 0) {
      node->count++;
      return;
    }
    for (char c : word) {
      if (node->child == nullptr) {
        // no child
        size += sizeof(TrieNode);
        node->child = new TrieNode();
        node->child->parent = node;
        node->child->c = c;
        node = node->child;
      } else {
        TrieNode *child = node->child;
        while (child->c < c && child->next != nullptr) {
          if (child->next->c > c) {
            break;
          }
          child = child->next;
        }
        if (child->c == c) {
          node = child;
        } else if (child->c > c) {
          // add child
          size += sizeof(TrieNode);
          TrieNode *tmp = new TrieNode();
          tmp->c = c;
          tmp->parent = node;
          tmp->next = child;
          node->child = tmp;
          node = tmp;
        } else {
          size += sizeof(TrieNode);
          TrieNode *tmp = new TrieNode();
          tmp->c = c;
          tmp->parent = node;
          tmp->next = child->next;
          child->next = tmp;
          node = tmp;
        }
      }
    }
    node->count++;
  }

  void write_to_file(const string &filename) {
    Writer writer(filename, FILE_BUFFER_SIZE);
    TrieNode *node = root;
    while (node != nullptr) {
      if (node->count > 0) {
        string word = node->getWord(buffer);
        for (size_t i = 0; i < node->count; i++) {
          writer.writeWord(word);
        }
      }
      node = nextNode(node);
    }
  }

  void clear() {
    delete root;
    root = new TrieNode();
    size = 0;
  }

  ~Trie() { delete root; }
};

int sort_using_trie(const string &input_file_name, int key_count) {
  Trie trie;
  Reader input_file(input_file_name, FILE_BUFFER_SIZE);
  int k = 0;
  int file_count = 0;
  while (!input_file.isEof() && k < key_count) {
    string line = input_file.nextWord();
    trie.insert(line);
    if (trie.size > MEM_LIMIT_MB * MB) {
      trie.write_to_file(get_file_name(0, ++file_count));
      trie.clear();
    }
    k++;
  }
  if (trie.size > 0) {
    trie.write_to_file(get_file_name(0, ++file_count));
  }
  return file_count;
}

/**
 * Reads input_file in chunks, sorts them and writes the mto temporary files
 */
int sort_in_runs(const string &input_file_name, int key_count) {
  size_t chars_count = 0;
  vector<string> arr;
  arr.reserve(MEM_LIMIT_MB * MB / (MAX_KEY_LENGTH / 2));
  Reader input_file(input_file_name, FILE_BUFFER_SIZE);
  string line;
  int i = 0;
  int k = 0;
  int file_count = 0;
  while (!input_file.isEof()) {
    k++;
    if (k > key_count) {
      break;
    }
    string line = input_file.nextWord();
    size_t line_length = line.length();
    size_t arr_cap = arr.capacity();
    size_t expected_usage = chars_count + arr_cap + line_length;
    if (arr.size() * sizeof(string) == arr_cap) {
      expected_usage += arr_cap;
    }
    if (expected_usage < MEM_LIMIT_MB * MB) {
      chars_count += line_length;
      arr.emplace_back(std::move(line));
      i++;
    } else {
      file_count++;
      sort(arr.begin(), arr.begin() + i);
      string outfile_name = get_file_name(0, file_count);
      Writer outfile(outfile_name, FILE_BUFFER_SIZE);
      for (int j = 0; j < i; j++) {
        outfile.writeWord(arr[j]);
      }
      arr.clear();
      chars_count = line_length;
      arr.emplace_back(std::move(line));
      i = 1;
    }
  }
  if (i > 0) {
    file_count++;
    sort(arr.begin(), arr.begin() + i);
    string outfile_name = get_file_name(0, file_count);
    Writer outfile(outfile_name, FILE_BUFFER_SIZE);
    for (int j = 0; j < i; j++) {
      outfile.writeWord(arr[j]);
    }
  }
  return file_count;
}

/**
 * Merges the temporary files into one output file
 */
void merge_runs(const vector<string> &sorted_files, const int file_count,
                const string &output_file_name) {
  // MBs per file
  const size_t mem_per_file = FILE_BUFFER_SIZE;

  vector<unique_ptr<Reader>> files;
  files.reserve(file_count);
  vector<string> lines(file_count, "");
  for (int i = 0; i < file_count; i++) {
    files.push_back(make_unique<Reader>(sorted_files[i], mem_per_file));
    lines[i] = files[i]->nextWord();
  }
  Writer outfile(output_file_name, mem_per_file);

  vector<bool> completed(file_count, false);
  for (int i = 0; i < file_count; i++) {
    completed[i] = false;
  }
  int completed_count = 0;

  while (completed_count < file_count) {
    int min_index = -1;
    // find the smallest word
    for (int i = 0; i < file_count; i++) {
      if (!completed[i] && (min_index == -1 || lines[i] < lines[min_index])) {
        min_index = i;
      }
    }
    // write the smallest word to the output file
    outfile.writeWord(lines[min_index]);
    if (files[min_index]->isEof()) {
      // mark the file as completed
      completed[min_index] = true;
      completed_count++;
    } else {
      // read the next word from the file
      lines[min_index] = files[min_index]->nextWord();
    }
  }
}

/**
 * External merge sort
 */
int external_merge_stop_withstop(const char *input, const char *output,
                                 const long key_count, const int k = 2,
                                 const int num_merges = 0) {
  try {
    auto start = std::chrono::high_resolution_clock::now();
    // sort the input file in runs
#if USE_TRIE
    int file_count = sort_using_trie(input, key_count);
#else
    int file_count = sort_in_runs(input, key_count);
#endif
    std::cout << "Sorted runs: " << file_count << std::endl;
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Time taken to sort runs: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                     .count()
              << " ms" << std::endl;
    start = std::chrono::high_resolution_clock::now();

    int merge_count = 0;

    bool writing_to_output = false;
    if (file_count == 0) {
      writing_to_output = true;
      // empty file
      std::ofstream outfile(output);

    } else if (file_count == 1) {
      writing_to_output = true;
      // no need to merge, rename (this can also move)
      if (rename(get_file_name(0, 1).c_str(), output) != 0) {
        // can't rename, copy
        merge_runs({get_file_name(0, 1)}, 1, output);
      }
    } else {
      int stage = 1;
      int run = 1;

      vector<string> sorted_files(k, "");

      while (merge_count < num_merges || num_merges == 0) {
        // merge the runs
        for (int i = 0; i < file_count; i += k) {
          if ((merge_count >= num_merges) && (num_merges != 0)) {
            break;
          }
          int j;
          for (j = 0; j < k; j++) {
            if (i + j < file_count) {
              sorted_files[j] = get_file_name(stage - 1, i + j + 1);
            } else {
              break;
            }
          }
          string merged_file_name;
          if (file_count <= k) {
            // last merge is to the output file
            merged_file_name = output;
            writing_to_output = true;
          } else {
            merged_file_name = get_file_name(stage, run);
          }
          if (j == 1 && USE_SYMLINK && !writing_to_output) {
            remove(merged_file_name.c_str());
            // expensive copy, create symlink instead
            if (symlink(sorted_files[0].c_str(), merged_file_name.c_str()) !=
                0) {
              // can't create symlink, copy
              merge_runs(sorted_files, j, merged_file_name);
            }
          } else {
            if (j > 1) {
              merge_count++;
            }
            merge_runs(sorted_files, j, merged_file_name);
          }
          std::cout << (j > 1 ? "Merged " : "Copied ") << j << " file(s) into "
                    << merged_file_name << std::endl;
          run++;
        }
        stage++;
        run = 1;
        file_count = (file_count + k - 1) / k;
        if (file_count == 1) {
          break;
        }
      }
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Time taken to merge runs: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                     .count()
              << " ms" << std::endl;
    return merge_count;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }
}


int main(int argc, char **argv) {
  string input_file_name = argv[1];
  string output_file_name = argv[2];

  long key_count = atoi(argv[3]);
  int k = atoi(argv[4]);
  int num_merges = atoi(argv[5]);

  std::cout << "Memory limit: " << MEM_LIMIT_MB << " MB" << std::endl;

  int res = external_merge_stop_withstop(input_file_name.c_str(),
                               output_file_name.c_str(), key_count, k,
                               num_merges);
//   std::cout<<"res: "<<res<<endl;
}
