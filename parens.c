#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    StatusUnknown,
    StatusOpeningParens,
    StatusClosingParens,
    StatusOther,
} Status;

struct Node {
    Status status;
    struct Node* parent;
    struct Node* children;
    int32_t child_count;
};
typedef struct Node Node;

Node* node_make() {
    Node* node = (Node*)calloc(1, sizeof(Node));
    return node;
}

void node_add_child(Node* parent, Node const* child) {
    if (!parent || !child) return;

    parent->children = realloc(parent->children, sizeof(parent->children[0]) *
                                                     (parent->child_count + 1));
    parent->child_count += 1;

    Node* new_child = &(parent->children[parent->child_count - 1]);
    *new_child = *child;
    new_child->parent = parent;
}

void node_free(Node* node) {
    if (!node) return;
    node_free(node->children);

    free(node);
}

void node_print(Node const* node) {
    if (!node) return;

    printf("( %d ", node->status);

    int32_t i;
    for (i = 0; i < node->child_count; ++i) node_print(&(node->children[i]));

    printf(" ) ");
}

int main(int argc, char* argv[]) {
    if (argc != 2) return 1;

    const char* input = argv[1];
    Node* root = node_make();
    Node child1 = {.status = StatusUnknown, .children = NULL, .child_count = 0};
    node_add_child(root, &child1);
    Node child2 = {
        .status = StatusClosingParens, .children = NULL, .child_count = 0};
    node_add_child(root, &child2);

    node_print(root);

    node_free(root);
}
