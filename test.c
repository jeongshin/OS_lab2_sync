#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

typedef struct lab2_node
{
    struct lab2_tree *left;
    struct lab2_tree *right;
    int key;
} lab2_node;

/*
 * lab2_tree
 *
 *  struct lab2_node *root  : root node of bst.
 */
typedef struct lab2_tree
{
    struct lab2_node *root;
} lab2_tree;

typedef struct thread_arg
{
    pthread_t thread;
    lab2_tree *tree;
    int node_count;
    int num_iterations;
    int is_sync;
    int *data_set;
    int start;
    int end;
} thread_arg;

lab2_node *lab2_node_create(int key)
{
    struct lab2_node *node = malloc(sizeof(struct lab2_node));
    assert(node != NULL);
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    return node;
}

lab2_tree *lab2_tree_create()
{
    lab2_tree *tree;
    tree = malloc(sizeof(lab2_tree));
    assert(tree != NULL);
    tree->root = NULL;
    return tree;
}

int lab2_node_insert(lab2_tree *tree, lab2_node *new_node)
{
    if (tree == NULL)
    {
        perror("Error: Empty tree insertion!");
        exit(-1);
    }
    lab2_node *curr = tree->root;
    lab2_node *parent = NULL;

    int key = new_node->key;

    if (tree->root == NULL)
    {
        tree->root = new_node;
        return 1;
    }

    while (curr != NULL)
    {
        parent = curr;
        if (key < curr->key)
            curr = (lab2_node *)curr->left;
        else
            curr = (lab2_node *)curr->right;
    }

    if (key < parent->key)
    {
        parent->left = (struct lab2_tree *)new_node;
    }
    else
    {
        parent->right = (struct lab2_tree *)new_node;
    }

    return 1;
}

void traversal_node_inorder(lab2_node *node)
{
    if (node == NULL)
        return;
    traversal_node_inorder((lab2_node *)node->left);
    printf("%d", node->key);
    traversal_node_inorder((lab2_node *)node->right);
}

int lab2_node_print_inorder(lab2_tree *tree)
{
    if (tree == NULL)
    {
        perror("Error: Empty tree node traversal!");
        exit(-1);
    }
    lab2_node *node = tree->root;
    traversal_node_inorder(node);
    return 0;
}

int lab2_node_remove(lab2_tree *tree, int key)
{
    if (tree == NULL)
    {
        perror("Error: Empty tree node deletion!");
        exit(-1);
    }
    lab2_node *parent = NULL;
    lab2_node *curr = tree->root;
    /*Searching for node to delete*/
    while (curr != NULL && curr->key != key)
    {
        parent = curr;
        if (key < curr->key)
            curr = (lab2_node *)curr->left;
        else
            curr = (lab2_node *)curr->right;
    }
    /*Node with no child (leaf node)*/
    if (curr->left == NULL && curr->right == NULL)
    {
        if (parent->left == curr)
            parent->left = NULL;
        else
            parent->right = NULL;
        free(curr);
        return 1;
    }
    /*Node with left child only*/
    else if (curr->left != NULL && curr->right == NULL)
    {
        if (parent->left == curr)
            parent->left = curr->left;
        else
            parent->right = curr->left;
        free(curr);
        return 1;
    }
    /*Node with right child only*/
    else if (curr->left == NULL && curr->right != NULL)
    {
        if (parent->left == curr)
            parent->left = curr->right;
        else
            parent->right = curr->right;
        free(curr);
        return 1;
    }
    /*Node with right & left child*/
    else
    {
        lab2_node *succ = (lab2_node *)curr->right;
        while (succ->left != NULL)
            succ = (lab2_node *)succ->left;
        parent = curr;
        curr->key = succ->key;
        if (parent->left == succ)
            parent->left = NULL;
        else
            parent->right = NULL;
        free(succ);
        return 1;
    }
}

void _lab2_tree_delete(lab2_node *node)
{
    if (node == NULL)
        return;
    _lab2_tree_delete((lab2_node *)node->left);
    _lab2_tree_delete((lab2_node *)node->right);
    free(node);
}

void lab2_tree_delete(lab2_tree *tree)
{
    _lab2_tree_delete(tree->root);
    free(tree);
}

int main()
{
    lab2_tree *tree = lab2_tree_create();
    lab2_node *node = lab2_node_create(3);
    lab2_node *node1 = lab2_node_create(1);
    lab2_node *node2 = lab2_node_create(5);
    lab2_node_insert(tree, node);
    lab2_node_insert(tree, node1);
    lab2_node_insert(tree, node2);
    printf("before remove");
    lab2_node_print_inorder(tree);
    printf("\nafter remove\n");
    lab2_node_remove(tree, 3);
    lab2_node_print_inorder(tree);
    lab2_tree_delete(tree);
    lab2_node_print_inorder(tree);

    //    lab2_tree_delete(tree);
    //    printf("\nafter remove");
    //    lab2_node_print_inorder(tree);

    return 0;
}