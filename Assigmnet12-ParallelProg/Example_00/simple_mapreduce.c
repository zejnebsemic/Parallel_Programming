#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS 100

typedef struct {
    char word[20];
    int count;
} WordCount;

void map(const char* text, WordCount* results, int* result_count) {
    char word[20];
    int i = 0, j = 0;
    
    while (text[i] != '\0') {
        if (isalpha(text[i])) {
            word[j++] = tolower(text[i]);
        } else if (j > 0) {
            word[j] = '\0';
            strcpy(results[*result_count].word, word);
            results[*result_count].count = 1;
            (*result_count)++;
            j = 0;
        }
        i++;
    }
    
    if (j > 0) {
        word[j] = '\0';
        strcpy(results[*result_count].word, word);
        results[*result_count].count = 1;
        (*result_count)++;
    }
}

void shuffle_sort(WordCount* data, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(data[i].word, data[j].word) > 0) {
                WordCount temp = data[i];
                data[i] = data[j];
                data[j] = temp;
            }
        }
    }
}

void reduce(WordCount* sorted_data, int count, WordCount* final, int* final_count) {
    if (count == 0) return;
    
    strcpy(final[0].word, sorted_data[0].word);
    final[0].count = sorted_data[0].count;
    *final_count = 1;
    
    for (int i = 1; i < count; i++) {
        if (strcmp(sorted_data[i].word, final[*final_count - 1].word) == 0) {
            final[*final_count - 1].count += sorted_data[i].count;
        } else {
            strcpy(final[*final_count].word, sorted_data[i].word);
            final[*final_count].count = sorted_data[i].count;
            (*final_count)++;
        }
    }
}

int main() {
    const char* document1 = "Hello world hello";
    const char* document2 = "world of data";
    const char* document3 = "hello data";
    
    printf("=== Simple MapReduce Example ===\n\n");
    
    printf("Input Documents:\n");
    printf("  Doc 1: \"%s\"\n", document1);
    printf("  Doc 2: \"%s\"\n", document2);
    printf("  Doc 3: \"%s\"\n\n", document3);
    
    WordCount map_output[MAX_WORDS];
    int map_count = 0;
    
    printf("--- MAP PHASE ---\n");
    printf("Processing each document, emit (word, 1) for each word:\n\n");
    
    int start = map_count;
    map(document1, map_output, &map_count);
    printf("Doc 1 output:\n");
    for (int i = start; i < map_count; i++) {
        printf("  (%s, %d)\n", map_output[i].word, map_output[i].count);
    }
    
    start = map_count;
    map(document2, map_output, &map_count);
    printf("\nDoc 2 output:\n");
    for (int i = start; i < map_count; i++) {
        printf("  (%s, %d)\n", map_output[i].word, map_output[i].count);
    }
    
    start = map_count;
    map(document3, map_output, &map_count);
    printf("\nDoc 3 output:\n");
    for (int i = start; i < map_count; i++) {
        printf("  (%s, %d)\n", map_output[i].word, map_output[i].count);
    }
    
    printf("\n--- SHUFFLE & SORT PHASE ---\n");
    printf("Group all pairs by key (word):\n\n");
    
    shuffle_sort(map_output, map_count);
    
    printf("After sorting by word:\n");
    for (int i = 0; i < map_count; i++) {
        printf("  (%s, %d)\n", map_output[i].word, map_output[i].count);
    }
    
    printf("\n--- REDUCE PHASE ---\n");
    printf("Sum counts for each unique word:\n\n");
    
    WordCount final_output[MAX_WORDS];
    int final_count = 0;
    
    reduce(map_output, map_count, final_output, &final_count);
    
    printf("Final word counts:\n");
    for (int i = 0; i < final_count; i++) {
        printf("  %s: %d\n", final_output[i].word, final_output[i].count);
    }
    
    printf("\n=== Complete ===\n");
    
    return 0;
}