#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_WORD_LEN 64
#define HASH_SIZE 1000003
#define BUFFER_SIZE 8192

typedef struct WordNode {
    char word[MAX_WORD_LEN];
    unsigned int count;
    struct WordNode* next;
} WordNode;

typedef struct {
    char word[MAX_WORD_LEN];
    unsigned int count;
} WordCount;

WordNode* hash_table[HASH_SIZE];
unsigned long local_words = 0;
unsigned long local_unique = 0;

unsigned int hash_djb2(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

void initialize_table() {
    memset(hash_table, 0, sizeof(hash_table));
    local_words = 0;
    local_unique = 0;
}

void insert_word(const char* word) {
    unsigned int index = hash_djb2(word);
    WordNode* current = hash_table[index];
    
    while (current != NULL) {
        if (strcmp(current->word, word) == 0) {
            current->count++;
            local_words++;
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
    
    local_unique++;
    local_words++;
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

void gather_filenames(const char* dirpath, char*** all_files, int* total_files, int rank) {
    *total_files = 0;
    
    if (rank == 0) {
        DIR* dir = opendir(dirpath);
        if (!dir) {
            fprintf(stderr, "Error: Cannot open directory: %s\n", dirpath);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        
        int capacity = 10000;
        *all_files = (char**)malloc(capacity * sizeof(char*));
        
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] == '.') continue;
            
            char filepath[512];
            snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, entry->d_name);
            
            struct stat st;
            if (stat(filepath, &st) == 0 && S_ISREG(st.st_mode) && is_text_file(entry->d_name)) {
                if (*total_files >= capacity) {
                    capacity *= 2;
                    *all_files = (char**)realloc(*all_files, capacity * sizeof(char*));
                }
                (*all_files)[*total_files] = strdup(filepath);
                (*total_files)++;
            }
        }
        closedir(dir);
    }
}

void distribute_files(char** all_files, int total_files, int rank, int size, 
                      char*** my_files, int* my_file_count) {
    int files_per_process = total_files / size;
    int remainder = total_files % size;
    
    int start_idx = rank * files_per_process + (rank < remainder ? rank : remainder);
    *my_file_count = files_per_process + (rank < remainder ? 1 : 0);
    
    *my_files = (char**)malloc(*my_file_count * sizeof(char*));
    for (int i = 0; i < *my_file_count; i++) {
        (*my_files)[i] = all_files[start_idx + i];
    }
}

int hash_table_to_array(WordCount** array) {
    *array = (WordCount*)malloc(local_unique * sizeof(WordCount));
    
    int index = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        WordNode* current = hash_table[i];
        while (current != NULL) {
            strcpy((*array)[index].word, current->word);
            (*array)[index].count = current->count;
            index++;
            current = current->next;
        }
    }
    
    return index;
}

void reduce_phase(WordCount* local_data, int local_count, int rank, int size) {
    int* recv_counts = NULL;
    int* displs = NULL;
    
    if (rank == 0) {
        recv_counts = (int*)malloc(size * sizeof(int));
        displs = (int*)malloc(size * sizeof(int));
    }
    
    MPI_Gather(&local_count, 1, MPI_INT, recv_counts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    int total_count = 0;
    if (rank == 0) {
        displs[0] = 0;
        total_count = recv_counts[0];
        for (int i = 1; i < size; i++) {
            displs[i] = displs[i-1] + recv_counts[i-1];
            total_count += recv_counts[i];
        }
    }
    
    WordCount* all_data = NULL;
    if (rank == 0) {
        all_data = (WordCount*)malloc(total_count * sizeof(WordCount));
    }
    
    MPI_Datatype MPI_WORD_COUNT;
    int blocklengths[2] = {MAX_WORD_LEN, 1};
    MPI_Aint offsets[2];
    offsets[0] = offsetof(WordCount, word);
    offsets[1] = offsetof(WordCount, count);
    MPI_Datatype types[2] = {MPI_CHAR, MPI_UNSIGNED};
    MPI_Type_create_struct(2, blocklengths, offsets, types, &MPI_WORD_COUNT);
    MPI_Type_commit(&MPI_WORD_COUNT);
    
    MPI_Gatherv(local_data, local_count, MPI_WORD_COUNT, 
                all_data, recv_counts, displs, MPI_WORD_COUNT, 0, MPI_COMM_WORLD);
    
    MPI_Type_free(&MPI_WORD_COUNT);
    
    if (rank == 0) {
        initialize_table();
        
        for (int i = 0; i < total_count; i++) {
            unsigned int index = hash_djb2(all_data[i].word);
            WordNode* current = hash_table[index];
            
            int found = 0;
            while (current != NULL) {
                if (strcmp(current->word, all_data[i].word) == 0) {
                    current->count += all_data[i].count;
                    found = 1;
                    break;
                }
                current = current->next;
            }
            
            if (!found) {
                WordNode* new_node = (WordNode*)malloc(sizeof(WordNode));
                strcpy(new_node->word, all_data[i].word);
                new_node->count = all_data[i].count;
                new_node->next = hash_table[index];
                hash_table[index] = new_node;
                local_unique++;
            }
        }
        
        free(all_data);
        free(recv_counts);
        free(displs);
    }
}

int compare_words(const void* a, const void* b) {
    WordNode* node_a = *(WordNode**)a;
    WordNode* node_b = *(WordNode**)b;
    if (node_a->count > node_b->count) return -1;
    if (node_a->count < node_b->count) return 1;
    return strcmp(node_a->word, node_b->word);
}

void print_results(int top_n) {
    WordNode** words = (WordNode**)malloc(local_unique * sizeof(WordNode*));
    
    unsigned long word_count = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        WordNode* current = hash_table[i];
        while (current != NULL) {
            words[word_count++] = current;
            current = current->next;
        }
    }
    
    qsort(words, word_count, sizeof(WordNode*), compare_words);
    
    printf("\n=== Top %d Most Frequent Words ===\n", top_n);
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
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (argc < 2) {
        if (rank == 0) {
            fprintf(stderr, "Usage: %s <input_directory> [top_n]\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    
    const char* input_dir = argv[1];
    int top_n = (argc >= 3) ? atoi(argv[2]) : 20;
    
    double start_time = MPI_Wtime();
    
    if (rank == 0) {
        printf("=== MPI Parallel Word Count ===\n");
        printf("Input directory: %s\n", input_dir);
        printf("Processes: %d\n\n", size);
    }
    
    char** all_files = NULL;
    int total_files = 0;
    
    gather_filenames(input_dir, &all_files, &total_files, rank);
    MPI_Bcast(&total_files, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank != 0) {
        all_files = (char**)malloc(total_files * sizeof(char*));
        for (int i = 0; i < total_files; i++) {
            all_files[i] = (char*)malloc(512);
        }
    }
    
    for (int i = 0; i < total_files; i++) {
        int len = (rank == 0) ? strlen(all_files[i]) + 1 : 0;
        MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (rank != 0) {
            all_files[i] = (char*)realloc(all_files[i], len);
        }
        MPI_Bcast(all_files[i], len, MPI_CHAR, 0, MPI_COMM_WORLD);
    }
    
    char** my_files;
    int my_file_count;
    distribute_files(all_files, total_files, rank, size, &my_files, &my_file_count);
    
    if (rank == 0) {
        printf("Found %d files\n", total_files);
        printf("Processing...\n");
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    initialize_table();
    for (int i = 0; i < my_file_count; i++) {
        process_file(my_files[i]);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    unsigned long global_words = 0;
    MPI_Reduce(&local_words, &global_words, 1, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        printf("\nProcessing complete!\n");
        printf("Total files: %d\n", total_files);
        printf("Total words: %lu\n", global_words);
        printf("Unique words: %lu\n\n", local_unique);
        printf("Collecting results...\n");
    }
    
    WordCount* local_data;
    int local_count = hash_table_to_array(&local_data);
    
    reduce_phase(local_data, local_count, rank, size);
    
    if (rank == 0) {
        printf("Sorting results...\n");
        print_results(top_n);
    }
    
    double end_time = MPI_Wtime();
    
    if (rank == 0) {
        printf("\n=== Performance Summary ===\n");
        printf("Wall time: %.2f seconds (%.2f minutes)\n", 
               end_time - start_time, (end_time - start_time)/60);
        printf("Words processed: %lu\n", global_words);
        printf("Processing rate: %.0f words/second\n", 
               global_words / (end_time - start_time));
        printf("Unique words found: %lu\n", local_unique);
    }
    
    free(local_data);
    cleanup();
    
    for (int i = 0; i < total_files; i++) {
        free(all_files[i]);
    }
    free(all_files);
    free(my_files);
    
    MPI_Finalize();
    return 0;
}