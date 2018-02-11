#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint32_t u32;

typedef enum {
    StatusUnknown,
    StatusOpeningParens,
    StatusPairParens,
    StatusInvalid,
} Status;

struct Node {
    Status status;
    u32 child_count;
    struct Node* parent;
    struct Node* children;
};
typedef struct Node Node;

Node* node_make(void);
Node* node_add_child(Node* parent, Node const* child);
void node_free(Node* node);
void node_print(Node const* node);
Node* node_parent(Node* node);
Node* node_add_sibling(Node* node, Node* sibling);
Node* parse_char(char c, Node* node);
void parse_string(char const* str, Node* root);
bool node_has_status(Node const* node, Status status);
bool is_parens_expr_valid(char* str);

Node* node_make() {
    Node* node = calloc(1, sizeof(Node));
    return node;
}

Node* node_add_child(Node* parent, Node const* child) {
    if (!parent || !child) return NULL;

    parent->children = realloc(parent->children, sizeof(parent->children[0]) *
                                                     (parent->child_count + 1));
    parent->child_count += 1;

    Node* new_child = &(parent->children[parent->child_count - 1]);
    *new_child = *child;
    new_child->parent = parent;

    return new_child;
}

void node_free(Node* node) {
    if (!node) return;
    node_free(node->children);

    free(node);
}

void node_print(Node const* node) {
    if (!node) return;

    printf("( %d ", node->status);

    u32 i;
    for (i = 0; i < node->child_count; ++i) node_print(&(node->children[i]));

    printf(" ) ");
}

Node* node_parent(Node* node) { return node->parent ? node->parent : node; }

Node* node_add_sibling(Node* node, Node* sibling) {
    return node_add_child(node_parent(node), sibling);
}

Node* parse_char(char c, Node* node) {
    if (c == '(' && node->status == StatusUnknown) {
        node->status = StatusOpeningParens;
        return node;
    } else if (c == '(' && node->status == StatusOpeningParens) {
        Node child = {
            .status = StatusOpeningParens, .children = NULL, .child_count = 0};
        return node_add_child(node, &child);
    } else if (c == '(' && node->status == StatusPairParens) {
        Node sibling = {
            .status = StatusOpeningParens, .children = NULL, .child_count = 0};
        return node_add_sibling(node, &sibling);
    } else if (c == ')' && node->status == StatusOpeningParens) {
        node->status = StatusPairParens;
        return node;
    } else if (c == ')' && node->status == StatusPairParens) {
        return parse_char(c, node_parent(node));
    } else if (c == ')') {
        node->status = StatusInvalid;
        return node;
    } else {
        return node;
    }
}

void parse_string(char const* str, Node* root) {
    size_t i = 0;
    char c = str[0];
    Node* node = root;

    while (c != '\0') {
        node = parse_char(c, node);
        i++;
        c = str[i];
    }
}

bool node_has_status(Node const* node, Status status) {
    if (node->status == status) return true;
    u32 i;
    for (i = 0; i < node->child_count; ++i) {
        if (node_has_status(&(node->children[i]), status)) return true;
    }
    return false;
}

bool is_parens_expr_valid(char* str) {
    Node* root = node_make();
    Node child1 = {.status = StatusUnknown, .children = NULL, .child_count = 0};
    parse_string(str, node_add_child(root, &child1));
    node_print(root);

    bool is_valid = node_has_status(root, StatusInvalid);

    node_free(root);

    return is_valid;
}

int main(int argc, char* argv[]) {
    printf("%d", sizeof(Status));
    if (argc != 2) return 1;

    return !is_parens_expr_valid(argv[1]);
}
