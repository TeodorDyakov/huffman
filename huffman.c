#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SZ 128
#define MAX_CODE_LEN 64

int counts[MAX_SZ];
char codes[MAX_SZ][MAX_CODE_LEN];

typedef struct Node {
    char character;
    int count;
    struct Node *left;
    struct Node *right;
} Node;

void calculate_prob(char *str, int *counts) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        counts[*str++]++;
    }
}

int comparator(const void *a, const void *b) {
    Node *n1 = (Node *)a;
    Node *n2 = (Node *)b;
    return (n1->count - n2->count);
}

Node *find_and_remove_min(Node *nodes, int sz) {
    int idx = 0;
    Node *minNode = &nodes[0];
    for (int i = 0; i < sz; i++) {
        if (nodes[i].count < minNode->count) {
            minNode = &nodes[i];
            idx = i;
        }
    }
    Node *node = (Node *)malloc(sizeof(Node));
    memcpy(node, minNode, sizeof(Node));
    nodes[idx] = nodes[sz - 1];
    return node;
}

void dfs(Node *node, char *path, int sz) {
    if (node == NULL) {
        return;
    }
    if (node->left == NULL && node->right == NULL) {
        strncpy(codes[node->character], path, sz);
        return;
    }
    path[sz] = '0';
    dfs(node->left, path, sz + 1);
    path[sz] = '1';
    dfs(node->right, path, sz + 1);
}

char *fileToString(char *file_name) {
    FILE *file = fopen(file_name, "r");
    if (file == NULL) return NULL;

    fseek(file, 0, SEEK_END);
    long int size = ftell(file);
    fclose(file);

    file = fopen(file_name, "r");
    char *str = (char *)malloc(size);
    int bytes_read = fread(str, sizeof(char), size, file);
    fclose(file);
    return str;
}

void writeCodeBook(FILE* file){
    int non_zero_chars_cnt = 0;

    for (int i = 0; i < MAX_SZ; i++) {
        if (counts[i]) {
            non_zero_chars_cnt++;
        }
    }

    fprintf(file, "%d", non_zero_chars_cnt);

    for (int i = 0; i < MAX_SZ; i++) {
        if (codes[i][0] == '\0') {
            continue;
        }
        putc((char)i, file);
        putc(':', file);
        fwrite(codes[i], sizeof(char), strlen(codes[i]), file);
        putc('\n', file);
    }
}

void writeBitToFile(unsigned char bit, FILE *file, int flush) {
    static int count = 0;
    static char buffer = 0;
    
    if(flush){
        buffer <<= (8 - count); 
        fwrite(&buffer, sizeof(buffer), 1, file);
        return;
    }

    buffer <<= 1;          // Make room for next bit.
    if (bit) buffer |= 1;  // Set if necessary.
    count++;

    if (count == 8) {
        fwrite(&buffer, sizeof(buffer), 1, file);  // Error handling elided.
        buffer = 0;
        count = 0;
    } 
}

int main(int argc, char **argv) {
    char *str = NULL;

    if (argc > 1) {
        str = fileToString(argv[1]);
    } else {
        return 1;
    }

    calculate_prob(str, counts);
    int node_count = 0;

    for (int i = 0; i < MAX_SZ; i++) {
        if (counts[i] != 0) {
            node_count++;
        }
    }
    const int SZ = node_count;
    Node nodes[SZ];

    for (int i = 0, idx = 0; i < MAX_SZ; i++) {
        if (counts[i] != 0) {
            struct Node node = {(char)i, counts[i], NULL, NULL};
            nodes[idx++] = node;
        }
    }

    qsort(nodes, SZ, sizeof(Node), comparator);

    int sz = SZ;
    Node *root = NULL;

    while (sz != 1) {
        struct Node *min = find_and_remove_min(nodes, sz);
        struct Node *min_2 = NULL;
        sz--;
        min_2 = find_and_remove_min(nodes, sz);
        Node new_node = {'\0', min->count + min_2->count, min, min_2};
        nodes[sz - 1] = new_node;
        root = &new_node;
    }

    char path[MAX_CODE_LEN];
    memset(path, '\0', MAX_CODE_LEN);

    dfs(root, path, 0);

    FILE *file = NULL;

    if (argc > 2) {
        file = fopen(argv[2], "w+");
    } else {
        return 1;
    }

    writeCodeBook(file);

    for (char *s = str; *s; s++) {
        char *code = codes[*s];
        int len = strlen(code);
        
        for (int i = 0; i < len; i++) {
            unsigned char bit = code[i] - '0';
            writeBitToFile(bit, file, 0);
        }
    }
    writeBitToFile(0, file, 1);
}
