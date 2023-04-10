#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXN 100000 // maximum size of input sequence

struct node {
    int head; // path-label start at &(Ti[head])
    int sdep; // string-depth, from the root down to node
    struct node* child; // pointer to the first node of the linked list
    struct node* brother; // next node in the linked list
    struct node* slink; // suffix-link
    struct node** hook; // what keeps this node linked?
};

struct point {
    struct node* a; // node above
    struct node* b; // node below
    int s; // string-depth
};

struct node* last; // Global variable to keep track of the last visited node
int n; // size of input sequence
char T[MAXN]; // input sequence
struct node* root; // root of the suffix tree
int a; // number of nodes in the tree

/*
The DescendQ function checks if it is possible to descend from a given point p to 
a child node with a given character c. It takes a pointer to a struct point p and 
a character c as input and returns 1 if there is a child node of p with a label 
that starts with c, and 0 otherwise. If p is the root, it returns 1, since any 
character can be the first character of a string in the tree.
*/
int DescendQ(struct point* p, char c) {
    if (p->a == NULL) { // special case for root
        return 1;
    }
    struct node* u = p->b;
    int i = u->head;
    while (i < n && i <= u->head + u->sdep) {
        if (T[i] == c) {
            return 1;
        }
        i++;
    }
    return 0;
}

/**
 * Updates the suffix links of the given node, and of its ancestors if needed.
 */
// void SuffixLink(struct node* p) {
//   if (last != NULL) last->slink = p;  // set link of previous node
//   last = p;
//   if (p->hook == NULL) return;  // root node
//   struct node* q = p->hook->suffix_link;  // follow link of parent
//   while (q != NULL && !DescendQ(q, T[p->head+p->sdep])) {
//     q = q->slink;  // follow links of ancestors
//   }
//   if (q == NULL) {
//     p->slink = root;  // suffix links to the root
//   } else {
//     p->slink = Descend(q, T[p->head+p->sdep]);  // found edge
//   }
// }

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
    if (p->a == NULL) { // special case for root
        p->b = root->child;
        p->s = 0;
    } else {
        struct node* u = p->b;
        int i = u->head;
        while (i < n && i <= u->head + u->sdep) {
            if (T[i] == c) {
                p->s++;
                return;
            }
            i++;
        }
        p->b = u->brother;
        p->s = u->sdep + 1;
    }
    while (p->b != NULL && p->b->head + p->s >= n) {
        p->b = p->b->slink;
        p->s = p->b->sdep;
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
int AddLeaf(struct point* p, struct node** h, int i) {
    struct node* u = p->b;
    int j = u->head;
    while (j < n && j <= u->head + u->sdep) {
        if (T[j] != T[i]) {
            break;
        }
        j++;
        i++;
    }
    if (j == u->head + u->sdep + 1) { // already in tree
        return 0;
    }
    struct node* v = (struct node*) malloc(sizeof(struct node));
    v->head = i;
    v->sdep = n - i;
    v->child = NULL;
    v->brother = NULL;
    v->slink = root;
    v->hook = h;
    u->brother = v;
    *h = v;
    a++;
    return 1;
}


int AddLeaf(node* p, node** root, int i) {
    int remainder = 0;
    node* lastInternalNode = NULL;

    while (true) {
        point next = DescendQ(p, i);

        if (next.s == -1) { // no edge from p with label starting with T[i]
            node* leaf = CreateNode(p, i, 2 * n);
            leaf->hook = root; // leaf node is linked to the root node
            leaf->slink = lastInternalNode;

            if (lastInternalNode != NULL) {
                lastInternalNode->slink = p;
            }
            printf("Leaf ");
            shownode(leaf);
            return 1; // newly added node
        } else {
            node* q = next.a;
            int k = next.s;

            char edgeLabelStart = T[q->head + k];
            char newLeafLabelStart = T[i + k];

            if (edgeLabelStart == newLeafLabelStart) { // edge leading to a leaf node, simply return
                printf("Leaf ");
                shownode(q);
                return 0;
            } else { // create new internal node and add new leaf
                node* internalNode = CreateNode(q, q->head, k);
                internalNode->hook = root; // internal node is linked to the root node
                internalNode->slink = lastInternalNode;

                if (lastInternalNode != NULL) {
                    lastInternalNode->slink = internalNode;
                }

                node* leaf = CreateNode(internalNode, i + k, 2 * n);
                leaf->hook = root; // leaf node is linked to the root node
                printf("Internal ");
                shownode(internalNode);
                printf("Leaf ");
                shownode(leaf);

                q->head += k;
                internalNode->child = q;
                internalNode->brother = q->brother;
                q->brother = internalNode;

                remainder--;
                if (p == root) {
                    k++;
                }
                p = p->slink;
                if (p == NULL) {
                    p = root;
                }
                if (remainder > 0) {
                    break;
                }
                lastInternalNode = internalNode;
            }
        }
    }

    return 0;
}


/**
 * Updates the suffix link of the last visited node.
 */
void SuffixLink(struct node* p) {
  if (last != NULL) {
    last->slink = p;
  }
  last = p;
}

void ukkonen_algorithm() {
  // Define the input DNA sequence T
  char* T = "ATCGAATCGATCGGCTAGC";
  
  // Initialize the current node as the root node
  struct node* p = &root;
  
  // Initialize the index i to 0
  int i = 0;
  
  // Iterate over the input sequence T
  while (i <= 2*n) {
    // Print the current character
    printf("Letter %c\n", '\0' == T[i] ? '$' : T[i]);
    
    // Check if there is a child node that matches the current character
    while (!DescendQ(p, T[i])) {
      // Add a new leaf node to the tree and update the node pointer
      a += AddLeaf(p, &(root[a]), i);
      
      // Create suffix links between nodes
      SuffixLink(p);
    }
    
    // Move down the tree to the child node that matches the current character
    Descend(p, T[i]);
    
    // Increment the index i
    i++;
  }
}

int main() {

  ukkonen_algorithm();

  return 0;
}
