#include "stdlib.h"
#include "stdio.h"


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


/* ######################### Suffix Tree Algorithm ########################## */

/*
The DescendQ function checks if it is possible to descend from a given point p to 
a child node with a given character c. It takes a pointer to a struct point p and 
a character c as input and returns 1 if there is a child node of p with a label 
that starts with c, and 0 otherwise. If p is the root, it returns 1, since any 
character can be the first character of a string in the tree.
*/
int DescendQ(struct point* p, char c) {
    if (p->above == NULL) { // special case for root
        return 1;
    }
    struct node* u = p->below;
    int i = u->head;
    while (i < n_nodes && i <= u->head + u->depth) {
        if (string[i] == c) {
            return 1;
        }
        i++;
    }
    return 0;
}

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
void Descend(struct point* p, char c) {
    if (p->above == NULL) { // special case for root
        p->below = nodes[0].child;
        p->depth = 0;
    } else {
        struct node* u = p->below;
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
int AddLeaf(Point p, Node root, int i) {
  int a = 0; // number of nodes added
  Node r = p->above;
  int j = p->depth + 1;
  Node s = NULL; // new internal node to be created (if necessary)
  Node t = NULL; // new leaf to be created
  while(j <= p->below->depth) {
    // find the edge to descend on
    Node q = r->child;
    while(string[q->head] != string[j]) {
      q = q->brother;
    }
    int k = q->head;
    // if T[k+1]...T[q->sdep] is a prefix of T[i+1]...T[n]
    if(j + q->depth - k <= p->below->depth) {
      r = q;
      j = q->depth;
    } else {
      // split the edge and create a new internal node
      Node u = (Node) calloc(1, sizeof(struct node));
      a++;
      u->head = k;
      u->depth = j + q->depth - k;
      u->child = q;
      u->brother = r->child;
      u->suffix_link = NULL;
      u->hook = &root;
      r->child = u;
      r = u;
      j = u->depth;
      if(s != NULL) {
        s->suffix_link = u;
      }
      s = u;
    }
  }
  // create a new leaf node
  if(s != NULL) {
    s->suffix_link = r;
  }
  t = (Node) calloc(1, sizeof(struct node));
  a++;
  t->head = i;
  t->depth = n_nodes;
  t->child = NULL;
  t->brother = r->child;
  t->suffix_link = NULL;
  t->hook = &root;
  r->child = t;
  printf("Leaf ");
  show_node(t);
  return a;
}


/**
 * Updates the suffix link of the last visited node.
 */
void SuffixLink(struct point* p) {
  if (last != NULL) {
    last->suffix_link = p->above;
  }

  last = p->above;
}

void ukkonen_algorithm() {
  
  int i = 0;
  Point p = (Point) malloc(sizeof(struct point)); // create a new point

  p->above = NULL;
  p->below = nodes[0].child;
  p->depth = 0;
  
  while(i <= 2 * n_nodes) {

      char c = (i < n_nodes) ? string[i] : '$'; // get the next character or the end symbol $
      printf("Letter %c\n", c);

      // Check if there is a child node that matches the current character
      while(!DescendQ(p, c)) {
        n_tree_nodes += AddLeaf(p, &(nodes[n_tree_nodes]), i);
        SuffixLink(p);
      }

      // Move down the tree to the child node that matches the current character
      Descend(p, c);

      i++;
  }
}


/* ################################# Funcs ################################# */


/**
 * @brief Reads input from stdin and builds structures to be used.
 */
void init_nodes() {
  /* Used as parameters in getline to store input and size of input */ 
  size_t len = 0;
  char* input = NULL;

  /* Reads size of dna string and the dna sequence */
  getline(&input, &len, stdin);

  /* Allocates memory for the string */
  sscanf(input, "%d %*s", &n_nodes);
  string = (char*) calloc(n_nodes + 1, sizeof(char));

  /* Stores dna sequence in the string */
  sscanf(input, "%*d %s", string);

  /* Allocates necessary memory for the suffix trees nodes */
  nodes = (Node) calloc(2 * ((2 * n_nodes) + 1), sizeof(struct node));

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

  /* Cleans up the program by freeing all the allocated memory */
  free_program_memory();

  exit(0);
}
