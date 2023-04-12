#include "stdlib.h"
#include "stdio.h"
#include "string.h"


/* ################################ Globals ################################ */


/**
 * @brief Node pointer alias.
 */
typedef struct node* Node;

/**
 * @brief Point pointer alias
 */
typedef struct point* Point;

/**
 * @brief Suffix tree internal node representation.
 *
 * @param head the path-label start at &(Ti[head])
 * @param depth string depth from the root down to node
 * @param child first child (singly linked list) points to the first element
 * @param brother brother node (singly linked list) points to the next element
 * @param suffix_link suffix link
 * @param hook what keeps this node linked
 */
struct node {
    int head;
    int depth;
    Node child;
    Node brother;
    Node suffix_link;
    Node* hook;
};

/**
 * @brief Represents a position between two letters in a given branch.
 *
 * @param above node above
 * @param below node below
 * @param depth string depth
 */
struct point {
    Node above;
    Node below;
    int depth;
};

/**
 * @brief DNA sequence string.
 */
char* string = NULL;

/**
 * @brief Size of input string.
 */
int n_nodes = 0;

/**
 * @brief Holds all the suffix trees nodes.
 */
Node nodes = NULL;

/**
 * @brief Number of nodes in suffix tree.
 */
int n_tree_nodes = 0;

/**
 * @brief Global variable to keep track of the last visited node.
 */
Node last = NULL;


/* ################################ Helpers ################################ */


/**
 * @brief
 *
 * @param v
 * @param A
 * @return int
 */
int ptr2loc(Node v, Node A) {
    int r;
    r = -1;
    if(NULL != v)
        r = ((size_t) v - (size_t) A) / sizeof(struct node);
    return (int) r;
}

/**
 * @brief Shows input node's information.
 *
 * @param node node to be shown
 */
void show_node(Node node) {
    if(NULL == node)
        printf("NULL\n");
    else {
        printf("node: %d ", ptr2loc(node, nodes));
        printf("head: %d ", node->head);
        printf("sdep: %d ", node->depth);
        printf("child: %d ", ptr2loc(node->child, nodes));
        printf("brother: %d ", ptr2loc(node->brother, nodes));
        printf("slink: %d", ptr2loc(node->suffix_link, nodes));
        printf("\n");
    }
}

/**
 * @brief Allows updating the pointer reference between two nodes.
 *
 * @param n1 node that is pointing to n2
 * @param n2 node that is being pointed to
 */
void points_to(Node n1, Node n2) {
    n1 = n2;
    n2->hook = &(n1);
}

/**
 * @brief Frees all the allocated memory in all nodes.
 */
void free_program_memory() {
    free(string);
    free(nodes);
}


void traverse_tree(Node p, int n, char* T) {
    // If p is NULL, return
    if (p == NULL) {
        return;
    }
    // If string depth of p is equal to n, print the string found
    if (p->depth == n) {
        printf("%.*s\n", n, &T[p->head]);
        return;
    }
    // Initialize the smallest child to NULL and the smallest character to '~' (larger than any character)
    Node smallestChild = NULL;
    char smallestChar = '~';
    // Traverse all children of p
    for (Node child = p->child; child != NULL; child = child->brother) {
        // If the first character of the label of the child is smaller than smallestChar, update smallestChar and smallestChild
        if (T[child->head] < smallestChar) {
            smallestChar = T[child->head];
            smallestChild = child;
        }
    }
    // Recursively traverse the subtree rooted at the smallest child
    traverse_tree(smallestChild, n, T);
}




void print_final() {
    int i = 0;
    printf("Final version\n");
    while(i < n_tree_nodes) {
        show_node(&(nodes[i]));
        i++;
    }
}


/* ######################### Suffix Tree Algorithm ########################## */


int DescendQ(Point point, char string_char) {

    if (point->above == &nodes[1]) { // special case for sentinel
        return 1;
    }

    if (point->above != point->below) {
        return string[point->below->head + point->depth] == string_char;
    }

    Node n = point->below->child;
    while (n != NULL) {
        if (string[n->head + point->above->depth] == string_char) {
            return 1;
        }
        n = n->brother;
    }

//    Node below = point->below;
//    int below_head = below->head;
//
//    while (below_head < n_tree_nodes && below_head <= below->head + below->depth) {
//        if (string[below_head] == string_char) {
//            return 1;
//        }
//        below_head++;
//    }

    return 0;
}

// Quando faço descend no SuffixLink, vou ter de corrigir o point.depth com o nó depth e a apontar para o nó de baixo

// Chamo SuffixLink na raiz vai para sentinela
// Suffix link da raiz é a sentinela
// O point tem o above e o below igual ao nó em que estou
// Quando chamo o AddLeaf, se for a criar um nó interno, coloco o suffix do last a apontar para o novo nó e depois mudo o last para o novo nó criado
// AddLeaf adds at most 2 nodes

void Descend(Point p, char c) {
    if (p->above == &nodes[1]) { // special case for sentinel

        p->below = &nodes[0];
        p->above = &nodes[0];
        p->depth = 0;
        return;

    }

    // FIXME: Quando o above e o below do point são diferentes, tenho de verificar se posso descer
    if (p->above != p->below) {  // TODO: refactor this
        if (string[p->below->head + p->depth] == c) {
            p->depth++;

            if (p->depth == p->below->depth) {  // Passei a estar no nó
                p->above = p->below;
            }

        }  // Head do de baixo + depth do de cima
    }

    // Finds branch to descend at
    Node n = p->below->child;
    while (n != NULL) {
        if (string[n->head + p->above->depth] == c) {  // Head do de baixo + depth do de cima
            p->depth++;
            p->below = n;
            if (p->depth == p->below->depth) {
                p->above = p->below;
            }
            return;
        }
        n = n->brother;
    }

//    Node u = p->below;
//    int i = u->head;
//
//    while (i < n_nodes && i <= u->head + u->depth) {
//        if (string[i] == c) {
//            p->depth++;
//            return;
//        }
//        i++;
//    }
//
//    p->below = u->brother;
//    p->depth = u->depth + 1;
//
//    while (p->below != NULL && p->below->head + p->depth >= n_nodes) {
//        p->below = p->below->suffix_link;
//        p->depth = p->below->depth;
//    }

}

// Se uma leaf for adicionada, não interessa. Apenas nós internos são relevantes


int AddLeaf(Point p, Node string_idx_node, int string_idx) {
    int n_nodes_added = 0; // number of nodes added
    int next_node = 2 + n_tree_nodes;

    Node parent = p->above;
    int depth = p->depth;

    // If the pointer is not at a leaf, then we need to add an internal node
    if (p->depth + 1 <= p->below->depth) {

        // split the edge and create n_nodes_added new internal node
        n_nodes_added++;

        nodes[next_node].head = p->below->head;
        // nodes[next_node].depth = depth + q->depth - k;
        nodes[next_node].depth = depth;
        nodes[next_node].child = p->below;
        nodes[next_node].brother = p->below->brother;

        if (p->below->brother != NULL) {
            p->below->brother->hook = &nodes[next_node].brother;
        }

        nodes[next_node].suffix_link = NULL;  // Suffix links is parent node
        nodes[next_node].hook = p->below->hook;

        *nodes[next_node].hook = &nodes[next_node];

        p->below->brother = NULL;  // Passa a ser leaf

        parent->child = &nodes[next_node];

        // FIXME: Acho que não estou a colocar bem os nós brother e child

        /* Updates last's suffix to the newly created internal node */
        if (last != NULL) {
            last->suffix_link = &nodes[next_node];
        }
        last = &nodes[next_node];

        printf("Internal ");
        show_node(&nodes[2 + n_tree_nodes]);

        parent = last;

    }

//    while(depth <= p->below->depth) {  // Internal nodes
//
//        // find the edge to descend on
//        Node q = parent->child;
//        while(string[q->head] != string[depth]) {
//            q = q->brother;
//        }
//
//        int k = q->head;
//
//        // if T[k+1]...T[q->sdep] is n_nodes_added prefix of T[i+1]...T[n]
//        if(depth + q->depth - k <= p->below->depth) {
//            parent = q;
//            depth = q->depth;
//        } else {
//
//            // split the edge and create n_nodes_added new internal node
//            n_nodes_added++;
//
//            nodes[next_node].head = k;
//            // nodes[next_node].depth = depth + q->depth - k;
//            nodes[next_node].depth = p->depth;
//            nodes[next_node].child = q;
//            nodes[next_node].brother = parent->child;
//            nodes[next_node].suffix_link = NULL;  // Suffix links is parent node
//            nodes[next_node].hook = &string_idx_node;
//
//            parent->child = &nodes[next_node];
//
//            parent = &nodes[next_node];
//            depth = nodes[next_node].depth;
//
//            /* Updates last's suffix to the newly created internal node */
//            if (last != NULL) {
//                last->suffix_link = &nodes[next_node];
//            }
//            last = &nodes[next_node];
//
//            n_nodes_added++;
//            printf("Internal ");
//            show_node(&nodes[2 + n_tree_nodes + n_nodes_added]);
//        }
//    }

    // create n_nodes_added new leaf node
    // if(s != NULL) {
    //     s->suffix_link = parent;
    // }

    next_node = next_node + n_nodes_added;

    nodes[next_node].head = string_idx - p->depth;
    nodes[next_node].depth = n_nodes * 2 + 1 - nodes[next_node].head;
    nodes[next_node].child = NULL;
    nodes[next_node].brother = parent->child;
    nodes[next_node].suffix_link = NULL;

    nodes[next_node].hook = &parent->child;

    n_nodes_added++;

    parent->child = &nodes[next_node];

    printf("Leaf ");
    show_node(&nodes[next_node]);
    return n_nodes_added;
}

/**
 * Updates the suffix link of the last visited node.
 */
void SuffixLink(Point p) {

    /* Update last_internal_node to be the current node p->above */
    last = p->above;

    /* If last_internal_node is the root node, put point in sentinel and return */
    if (last == &nodes[0]) {
        p->above = &nodes[1];
        p->below = &nodes[1];
        return;
    }

    /* Move up one level in the tree */
    p->above = last->suffix_link;

    /* Continue moving up the tree while p->above is not the root and the edge from p->above to p->below is a leaf edge */
    while (p->above != &nodes[0] && p->above->child == NULL) {
        p->above = p->above->suffix_link;
    }

    /* If p->above is not the root, move down the tree to the first child of p->above that matches the first character of the edge from p->above to p->below */
    if (p->above != &nodes[0]) {
        Descend(p, string[p->below->head]);
        // p->above = p->below;
        // p->depth = p->below->depth;
    }
}

/**
 * @brief Runs the Ukkonen's Algorithm and builds a suffix tree.
 */
void ukkonen_algorithm() {

    int string_idx = 0;
    Point point = (Point) calloc(1, sizeof(struct point)); // create a new point

    /* Builds initial point */
    point->above = &nodes[0];
    point->below = &nodes[0];
    point->depth = 0;

    /* Loops over all characters in the duplicated string */
    while(string_idx <= 2 * n_nodes) {

        printf("Letter %c\n", '\0' == string[string_idx] ? '$' : string[string_idx]);

        // Check if there is a child node that matches the current character
        while(!DescendQ(point, string[string_idx])) {
            n_tree_nodes += AddLeaf(point, &(nodes[n_tree_nodes]), string_idx);
            SuffixLink(point);  // Vai-me colocar na sentinela e depois o Descend coloca-me na root
        }

        // Move down the tree to the child node that matches the current character
        Descend(point, string[string_idx]);

        string_idx++;
    }

    free(point);
}


/* ################################# Funcs ################################# */


/**
 * @brief Reads input from stdin and builds structures to be used.
 */
void init_nodes() {
    /* Used as parameters in getline to store input and size of input */
    size_t len = 0;
    char* input = NULL;
    char* tmp = NULL;

    /* Reads size of dna string and the dna sequence */
    getline(&input, &len, stdin);

    /* Allocates memory for the string */
    sscanf(input, "%d %*s", &n_nodes);
    tmp = (char*) calloc(n_nodes + 1, sizeof(char));
    string = (char*) calloc(2 * n_nodes + 1, sizeof(char));

    /* Stores dna sequence in the string */
    sscanf(input, "%*d %s", tmp);
    strcat(string, tmp);
    strcat(string, tmp);

    /* Allocates necessary memory for the suffix trees nodes */
    nodes = (Node) calloc(2 * ((2 * n_nodes) + 1), sizeof(struct node));

    /* Root's suffix link is the sentinel */
    nodes[0].suffix_link = &nodes[1];

    free(tmp);
    free(input);
}

/**
 * @brief Main driver code.
 *
 * @return int 0 for success or 1 for error
 */
int main() {

    /* Reads input from the stdin and builds structures */
    init_nodes();

    /* Applies ukkonen's algorithm to find the suffix tree */
    ukkonen_algorithm();

    /* Traverses all nodes and outputs final string */
    // traverse_tree(nodes, n_nodes, string);

    /* Prints all node's final state */
    // print_final();

    /* Cleans up the program by freeing all the allocated memory */
    free_program_memory();

    exit(0);
}
