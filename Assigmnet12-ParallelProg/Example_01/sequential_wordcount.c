#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>

#define MAX_WORD_LEN 64
#define HASH_SIZE 1000003
#define BUFFER_SIZE 8192

typedef struct WordNode {
    char word[MAX_WORD_LEN];
    unsigned int count;
    struct WordNode* next;
} WordNode;

WordNode* hash_table[HASH_SIZE];
unsigned long total_words = 0;
unsigned long total_unique_words = 0;

unsigned int hash_djb2(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

void initialize_table() {
    memset(hash_table, 0, sizeof(hash_table));
    total_words = 0;
    total_unique_words = 0;
}

void insert_word(const char* word) {
    unsigned int index = hash_djb2(word);
    WordNode* current = hash_table[index];
    
    while (current != NULL) {
        if (strcmp(current->word, word) == 0) {
            current->count++;
            total_words++;
            return;
        }
        current = current->next;
    }
    
    WordNode* new_node = (WordNode*)malloc(sizeof(WordNode));
    strncpy(new_node->word, word, MAX_WORD_LEN - 1);
    new_node->word[MAX_WORD_LEN - 1] = '\0';
    new_node->count = 1;
    new_node->next = hash_table[index];
    hash_table[index] = new_node;
    
    total_unique_words++;
    total_words++;
}

int process_word(const char* input, char* output) {
    int j = 0;
    for (int i = 0; input[i] && j < MAX_WORD_LEN - 1; i++) {
        if (isalpha(input[i])) {
            output[j++] = tolower(input[i]);
        }
    }
    output[j] = '\0';
    return (j > 1);
}

void process_file(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) return;
    
    char buffer[BUFFER_SIZE];
    char processed_word[MAX_WORD_LEN];
    
    while (fgets(buffer, sizeof(buffer), file)) {
        char* token = strtok(buffer, " \t\n\r\f\v,.;:!?\"'()[]{}");
        while (token != NULL) {
            if (process_word(token, processed_word)) {
                insert_word(processed_word);
            }
            token = strtok(NULL, " \t\n\r\f\v,.;:!?\"'()[]{}");
        }
    }
    
    fclose(file);
}

int is_text_file(const char* filename) {
    int len = strlen(filename);
    if (len < 4) return 0;
    return (strcasecmp(filename + len - 4, ".txt") == 0);
}

void process_directory(const char* dirpath, int* total_files) {
    DIR* dir = opendir(dirpath);
    if (!dir) {
        fprintf(stderr, "Error: Cannot open directory: %s\n", dirpath);
        exit(1);
    }
    
    *total_files = 0;
    struct dirent* entry;
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        
        char filepath[512];
        snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, entry->d_name);
        
        struct stat st;
        if (stat(filepath, &st) == 0 && S_ISREG(st.st_mode) && is_text_file(entry->d_name)) {
            process_file(filepath);
            (*total_files)++;
            
            if (*total_files % 100 == 0) {
                printf("\rProgress: %d files | Words: %lu | Unique: %lu     ",
                       *total_files, total_words, total_unique_words);
                fflush(stdout);
            }
        }
    }
    
    closedir(dir);
}

int compare_words(const void* a, const void* b) {
    WordNode* node_a = *(WordNode**)a;
    WordNode* node_b = *(WordNode**)b;
    if (node_a->count > node_b->count) return -1;
    if (node_a->count < node_b->count) return 1;
    return strcmp(node_a->word, node_b->word);
}

void print_results(int top_n) {
    WordNode** words = (WordNode**)malloc(total_unique_words * sizeof(WordNode*));
    
    unsigned long word_count = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        WordNode* current = hash_table[i];
        while (current != NULL) {
            words[word_count++] = current;
            current = current->next;
        }
    }
    
    qsort(words, word_count, sizeof(WordNode*), compare_words);
    
    printf("\n\n=== Top %d Most Frequent Words ===\n", top_n);
    printf("%-5s %-30s %15s\n", "Rank", "Word", "Count");
    printf("----------------------------------------------------\n");
    
    for (unsigned long i = 0; i < top_n && i < word_count; i++) {
        printf("%-5lu %-30s %15u\n", i+1, words[i]->word, words[i]->count);
    }
    
    free(words);
}

void cleanup() {
    for (int i = 0; i < HASH_SIZE; i++) {
        WordNode* current = hash_table[i];
        while (current != NULL) {
            WordNode* next = current->next;
            free(current);
            current = next;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_directory> [top_n]\n", argv[0]);
        return 1;
    }
    
    const char* input_dir = argv[1];
    int top_n = (argc >= 3) ? atoi(argv[2]) : 20;
    
    printf("=== Sequential Word Count ===\n");
    printf("Input directory: %s\n\n", input_dir);
    
    clock_t start = clock();
    time_t start_time = time(NULL);
    
    int total_files = 0;
    initialize_table();
    process_directory(input_dir, &total_files);
    
    printf("\n\nProcessing complete!\n");
    printf("Total files: %d\n", total_files);
    printf("Total words: %lu\n", total_words);
    printf("Unique words: %lu\n\n", total_unique_words);
    
    printf("Sorting results...\n");
    print_results(top_n);
    
    clock_t end = clock();
    time_t end_time = time(NULL);
    double wall_time = difftime(end_time, start_time);
    
    printf("\n=== Performance Summary ===\n");
    printf("Wall time: %.2f seconds (%.2f minutes)\n", wall_time, wall_time/60);
    printf("Words processed: %lu\n", total_words);
    printf("Processing rate: %.0f words/second\n", total_words / wall_time);
    printf("Unique words found: %lu\n", total_unique_words);
    
    cleanup();
    
    return 0;
}