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


/*
The DescendQ function checks if it is possible to descend from a given point p to
a child node with a given character c. It takes a pointer to a struct point p and
a character c as input and returns 1 if there is a child node of p with a label
that starts with c, and 0 otherwise. If p is the root, it returns 1, since any
character can be the first character of a string in the tree.
*/
int DescendQ(Point point, char string_char) {

/*    if (point->above == &nodes[0]) { // special case for root
        return 1;
    }*/

    Node u = point->below;
    int i = u->head;

    while (i < n_tree_nodes && i <= u->head + u->depth) {
        if (string[i] == string_char) {
            return 1;
        }
        i++;
    }

    return 0;
}

// Chamo SuffixLink na raiz vai para sentinela
// Suffix link da raiz é a sentinela
// O point tem o above e o below igual ao nó em que estou
// Quando chamo o AddLeaf, se for a criar um nó interno, coloco o suffix do last a apontar para o novo nó e depois mudo o last para o novo nó criado
// AddLeaf adds at most 2 nodes

/*
The Descend function descends from a given point p to a child
node with a given character c. It takes a pointer to a struct point p and a
character c as input and updates p to point to the child node that has a
label starting with c. If there is no such child node, p is updated to point
to the next sibling node, and the string depth is updated accordingly. If there
is no next sibling node, p is updated to point to NULL. If the new node is not a
leaf node, the function updates the last visited node to p->b, so that the SuffixLink
function can set the suffix link of the current node later.
*/
void Descend(Point p, char c) {
    if (p->above == &nodes[0]) { // special case for root

        p->below = nodes[0].child;
        p->depth = 0;

    } else {

        Node u = p->below;
        int i = u->head;

        while (i < n_nodes && i <= u->head + u->depth) {
            if (string[i] == c) {
                p->depth++;
                return;
            }
            i++;
        }

        p->below = u->brother;
        p->depth = u->depth + 1;

    }

    while (p->below != NULL && p->below->head + p->depth >= n_nodes) {
        p->below = p->below->suffix_link;
        p->depth = p->below->depth;
    }

}


/*
The AddLeaf function adds a new leaf node to the tree. It takes a pointer to
a struct point p, a double pointer to a struct node h (which keeps track of the
last node that was added to the tree), and an integer i (which is the index of
the first character of the suffix to add to the tree). The function starts at the
node pointed to by p->b and compares the characters in the label of that node with
the characters in the suffix starting at index i in the input string T. If the
characters match up to the end of the label of the node, the suffix is already in
the tree and the function returns 0. Otherwise, the function creates a new node v
with a label starting at index i and a string depth of n - i, where n is the length
of the input string. The function updates the last visited node to the sibling of u,
which will be the next node visited when adding the next suffix. Finally, the function
sets the suffix link of the new node to the root, updates the hook of the new node to h,
and adds the new node to the linked list of siblings of the original node u. The
function returns 1 to indicate that a new node was added to the tree.
*/
int AddLeaf(Point p, Node string_idx_node, int string_idx) {
    int n_nodes_added = 0; // number of nodes added
    int next_node = 2 + n_tree_nodes + n_nodes_added;

    Node parent = p->above;
    int new_depth = p->depth + 1;

    while(new_depth <= p->below->depth) {  // Internal nodes

        // find the edge to descend on
        Node q = parent->child;
        while(string[q->head] != string[new_depth]) {
            q = q->brother;
        }

        int k = q->head;

        // if T[k+1]...T[q->sdep] is n_nodes_added prefix of T[i+1]...T[n]
        if(new_depth + q->depth - k <= p->below->depth) {
            parent = q;
            new_depth = q->depth;
        } else {

            // split the edge and create n_nodes_added new internal node
            n_nodes_added++;

            nodes[next_node].head = k;
            // nodes[next_node].depth = new_depth + q->depth - k;
            nodes[next_node].depth = n_nodes * 2 + 1 - n_tree_nodes - n_nodes_added;
            nodes[next_node].child = q;
            nodes[next_node].brother = parent->child;
            nodes[next_node].suffix_link = parent;  // Suffix links is parent node
            nodes[next_node].hook = &string_idx_node;

            parent->child = &nodes[next_node];

            parent = &nodes[next_node];
            new_depth = nodes[next_node].depth;

            /* Updates last's suffix to the newly created internal node */
            last->suffix_link = &nodes[next_node];
            last = &nodes[next_node];

            n_nodes_added++;
            printf("Internal ");
            show_node(&nodes[2 + n_tree_nodes + n_nodes_added]);
        }
    }

    // create n_nodes_added new leaf node
    // if(s != NULL) {
    //     s->suffix_link = parent;
    // }

    next_node = next_node + n_nodes_added;

    nodes[next_node].head = string_idx;
    // nodes[next_node].depth = n_nodes
    nodes[next_node].depth = n_nodes * 2 + 1 - n_tree_nodes - n_nodes_added;
    nodes[next_node].child = NULL;
    nodes[next_node].brother = parent->child;
    nodes[next_node].suffix_link = NULL;
    nodes[next_node].hook = &string_idx_node;
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
    if (last != NULL) {
        last->suffix_link = p->above;
    }

    last = p->above;

    // Tenho de dar update ao pointer aqui para ir para o suffix link
    // Escolho o suffix link do ultimo criado
    // Depois tenho de fazer um loop a descer comparando apenas as primeiras letras das edges
    // este truque é o skip count trick
    // para o fazer, tenho de usar o T[head do nó atual + string depth do nó anterior]
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
            SuffixLink(point);
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
    traverse_tree(nodes, n_nodes, string);

    /* Prints all node's final state */
    print_final();

    /* Cleans up the program by freeing all the allocated memory */
    free_program_memory();

    exit(0);
}
