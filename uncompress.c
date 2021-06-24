#include <stdio.h>
#include <stdlib.h>

typedef struct trie_node {
    struct trie_node* children[2];
    char symbol;
} trie_node;

trie_node* root = NULL;

void init_trie() { root = malloc(sizeof(trie_node)); }

void add_to_trie(char* str, char symbol) {
    trie_node* curr = root;

    for (; *str; *str++) {
        int bit = *str - '0';
        if (!(bit == 0 || bit == 1)) {
            break;
        }
        curr = curr->children[bit]
                   ? curr->children[bit]
                   : (curr->children[bit] = malloc(sizeof(trie_node)));
    }
    curr->symbol = symbol;
}

int is_leaf_node(trie_node* node) {
    return node->children[0] == NULL && node->children[1] == NULL;
}

int main(int argc, char* argv[]) {
    init_trie();

    char* line = NULL;
    int n_read = 0;
    size_t len = 0;
    ssize_t nread;

    FILE* file = fopen(argv[1], "r");
    int count = 0;
    fscanf(file, "%d", &count);

    for (int i = 0; i < count; i++) {
        char c = fgetc(file);
        char symbol = c;
        c = fgetc(file);
        n_read = getline(&line, &len, file);
        add_to_trie(line, symbol);
    }

    trie_node* curr = root;


    do {
        char c = fgetc(file);

        for (int i = 7; i >= 0; i--) {
            int bit = (c >> i) & 1;
            curr = curr->children[bit];
            if (curr == NULL) {
                curr = root;
            }

            if (is_leaf_node(curr)) {
                printf("%c", curr->symbol);
                curr = root;
            }
        }
    } while (!feof(file));

}