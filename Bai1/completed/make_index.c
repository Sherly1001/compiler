#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct array {
    int row, col;
    struct array *next;
} array;

void add_val(array **arr, int row, int col) {
    if (*arr) {
        array *new = malloc(sizeof(array));
        new->row = row;
        new->col = col;
        new->next = *arr;
        *arr = new;
    } else {
        *arr = malloc(sizeof(array));
        (*arr)->row = row;
        (*arr)->col = col;
        (*arr)->next = NULL;
    }
}

typedef struct word_tree {
    char *word;
    array *arr;
    struct word_tree *left;
    struct word_tree *right;
} word_tree;

void add_word(word_tree **wl, const char *word, int row, int col) {
    if (*wl) {
        int cmp = strcmp((*wl)->word, word);
        if (cmp == 0) add_val(&(*wl)->arr, row, col);
        else if (cmp > 0) add_word(&(*wl)->left, word, row, col);
        else add_word(&(*wl)->right, word, row, col);
    } else {
        *wl = malloc(sizeof(word_tree*));
        (*wl)->word = malloc(sizeof(char) * 20);
        strcpy((*wl)->word, word);
        (*wl)->arr = NULL;
        add_val(&(*wl)->arr, row, col);
        (*wl)->left = NULL;
        (*wl)->right = NULL;
    }
}

typedef struct word_list {
    char *word;
    struct word_list *next;
} word_list;

void add_stopw(word_list **arr, const char *word) {
    if (*arr) {
        word_list *new = malloc(sizeof(word_list));
        new->word = malloc(sizeof(char) * 20);
        strcpy(new->word, word);
        new->next = *arr;
        *arr = new;
    } else {
        *arr = malloc(sizeof(word_list));
        (*arr)->word = malloc(sizeof(char) * 20);
        strcpy((*arr)->word, word);
        (*arr)->next = NULL;
    }
}

word_list *load_word_list(const char *filename) {
    FILE *f = fopen(filename, "r");
    char w[20];
    word_list *stopw = NULL;
    while (fscanf(f, " %[^\n]s\n", w) > 0) add_stopw(&stopw, w);
    fclose(f);
    return stopw;
}

int is_mean_word(const char *word, word_list *mean_word) {
    for (word_list *w = mean_word; w; w = w->next) {
        if (!strncmp(w->word, word, strlen(word))) return 1;
    }
    return 0;
}

int is_stop_word(const char *word, word_list *stopw) {
    for (word_list *w = stopw; w; w = w->next) {
        if (!strncmp(w->word, word, strlen(word))) return 1;
    }
    return 0;
}

word_tree *load_word_tree(const char *filename, word_list *stopw, word_list *mean_name) {
    FILE *f = fopen(filename, "r");
    char s[200];
    int row = 1;
    word_tree *wt = NULL;

    while (fgets(s, 200, f)) {
        int i = 0, j = 0;
        while (s[i]) {
            if (!isalpha(s[i])) {
                ++i;
                continue;
            }
            j = i + 1;
            while (isalpha(s[j]) || s[j] == '.') ++j;

            char tmp = s[j], word[20];
            s[j] = '\0';
            strcpy(word, s + i);
            s[j] = tmp;

            if (is_mean_word(word, mean_name)) {
                add_word(&wt, word, row, i + 1);
                i = j;
                continue;
            }

            for (char *w = word; *w; ++w) *w = tolower(*w);
            if (word[j - i - 1] == '.') word[j - i - 1] = '\0';

            if (!is_stop_word(word, stopw)) add_word(&wt, word, row, i + 1);

            i = j;
        }
        ++row;
    }

    fclose(f);
    return wt;
}

void print_index(array *arr, int count) {
    if (arr) {
        print_index(arr->next, count + 1);
        printf("(%d, %d) ", arr->row, arr->col);
    } else {
        printf("%d, ", count);
    }
}

void show_index(word_tree *index) {
    if (!index) return;
    show_index(index->left);
    printf("%s ", index->word);
    print_index(index->arr, 0);
    printf("\n");
    show_index(index->right);
}

int main() {
    word_list *stopw = load_word_list("stopw.txt");
    word_list *mean_name = load_word_list("mean_name.txt");
    word_tree *index = load_word_tree("vanban.txt", stopw, mean_name);
    show_index(index);
}
