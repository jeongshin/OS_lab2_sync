/*
*	Operating System Lab
*	    Lab2 (Synchronization)
*	    Student id : 32162417
*	    Student name : 신정웅
*
*   lab2_bst.c :
*       - thread-safe bst code.
*       - coarse-grained, fine-grained lock code
*
*   Implement thread-safe bst for coarse-grained version and fine-grained version.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>

#include "lab2_sync_types.h"

/*
 * TODO
 *  Implement funtction which traverse BST in in-order
 *  
 *  @param lab2_tree *tree  : bst to print in-order. 
 *  @return                 : status (success or fail)
 */
void traversal_node_inorder(lab2_node *node)
{
    if (node == NULL)
        return;
    traversal_node_inorder((lab2_node *)node->left);
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
}

/*
 * TODO
 *  Implement function which creates struct lab2_tree
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_tree )
 * 
 *  @return                 : bst which you created in this function.
 */
lab2_tree *lab2_tree_create()
{
    lab2_tree *tree;
    tree = malloc(sizeof(lab2_tree));
    assert(tree != NULL);
    pthread_mutex_init(&tree->mutex, NULL);
    tree->root = NULL;
    return tree;
}

/*
 * TODO
 *  Implement function which creates struct lab2_node
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param int key          : bst node's key to creates
 *  @return                 : bst node which you created in this function.
 */
lab2_node *lab2_node_create(int key)
{
    struct lab2_node *node = malloc(sizeof(struct lab2_node));
    assert(node != NULL);
    pthread_mutex_init(&node->mutex, NULL);
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* 
 * TODO
 *  Implement a function which insert nodes from the BST. 
 *  
 *  @param lab2_tree *tree      : bst which you need to insert new node.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                 : status (success or fail)
 */
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
/* 
 * TODO
 *  Implement a function which insert nodes from the BST in fine-garined manner.
 *
 *  @param lab2_tree *tree      : bst which you need to insert new node in fine-grained manner.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                     : status (success or fail)
 */
int lab2_node_insert_fg(lab2_tree *tree, lab2_node *new_node)
{
    lab2_node *parent = NULL;
    lab2_node *curr = tree->root;
    int key = new_node->key;
    if (curr == NULL)
    {
        pthread_mutex_lock(&tree->mutex);
        curr = new_node;

        pthread_mutex_unlock(&tree->mutex);
        return 1;
    }

    while (curr != NULL)
    {
        pthread_mutex_lock(&curr->mutex);
        parent = curr;
        if (key < curr->key)
            curr = curr->left;
        else
            curr = curr->right;
        pthread_mutex_unlock(&curr->mutex);
    }

    pthread_mutex_lock(&tree->mutex);
    if (key < parent->key)
    {
        parent->left = new_node;
    }
    else
    {
        parent->right = new_node;
    }
    pthread_mutex_unlock(&tree->mutex);
    return 1;
}

/* 
 * TODO
 *  Implement a function which insert nodes from the BST in coarse-garined manner.
 *
 *  @param lab2_tree *tree      : bst which you need to insert new node in coarse-grained manner.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                     : status (success or fail)
 */
int lab2_node_insert_cg(lab2_tree *tree, lab2_node *new_node)
{
    pthread_mutex_lock(&tree->mutex);
    lab2_node *parent = NULL;
    lab2_node *curr = tree->root;
    int key = new_node->key;
    if (curr == NULL)
    {
        curr = new_node;

        pthread_mutex_unlock(&tree->mutex);
        return 1;
    }

    while (curr != NULL)
    {
        parent = curr;
        if (key < curr->key)
            curr = curr->left;
        else
            curr = curr->right;
    }

    if (key < parent->key)
    {
        parent->left = new_node;
    }
    else
    {
        parent->right = new_node;
    }
    pthread_mutex_unlock(&tree->mutex);
    return 1;
}

/* 
 * TODO
 *  Implement a function which remove nodes from the BST.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */

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
    if (curr == NULL)
        return 0;
    /*Node with no child (leaf node)*/
    if (curr->left == NULL && curr->right == NULL)
    {
        if (parent->left == curr)
            parent->left = NULL;
        else
            parent->right = NULL;

        free(curr);
    }
    /*Node with left child only*/
    else if (curr->left != NULL && curr->right == NULL)
    {
        if (parent->left == curr)
            parent->left = curr->left;
        else
            parent->right = curr->left;

        free(curr);
    }
    /*Node with right child only*/
    else if (curr->left == NULL && curr->right != NULL)
    {
        if (parent->left == curr)
            parent->left = curr->right;
        else
            parent->right = curr->right;

        free(curr);
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
    }
    return 1;
}

/* 
 * TODO
 *  Implement a function which remove nodes from the BST in fine-grained manner.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node in fine-grained manner from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */
int lab2_node_remove_fg(lab2_tree *tree, int key)
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
        pthread_mutex_lock(&curr->mutex);
        if (key < curr->key)
            curr = (lab2_node *)curr->left;
        else
            curr = (lab2_node *)curr->right;
        pthread_mutex_unlock(&curr->mutex);
    }
    if (curr == NULL)
        return 0;
    /*Node with no child (leaf node)*/
    if (curr->left == NULL && curr->right == NULL)
    {
        pthread_mutex_lock(&parent->mutex);
        if (parent->left == curr)
            parent->left = NULL;
        else
            parent->right = NULL;
        pthread_mutex_unlock(&parent->mutex);

        free(curr);
    }
    /*Node with left child only*/
    else if (curr->left != NULL && curr->right == NULL)
    {
        pthread_mutex_lock(&parent->mutex);
        if (parent->left == curr)
            parent->left = curr->left;
        else
            parent->right = curr->left;

        pthread_mutex_unlock(&parent->mutex);
        free(curr);
    }
    /*Node with right child only*/
    else if (curr->left == NULL && curr->right != NULL)
    {
        pthread_mutex_lock(&parent->mutex);
        if (parent->left == curr)
            parent->left = curr->right;
        else
            parent->right = curr->right;

        pthread_mutex_unlock(&parent->mutex);
        free(curr);
    }
    /*Node with right & left child*/
    else
    {
        lab2_node *succ = (lab2_node *)curr->right;
        pthread_mutex_lock(&parent->mutex);
        while (succ->left != NULL)
            succ = (lab2_node *)succ->left;
        parent = curr;
        curr->key = succ->key;
        if (parent->left == succ)
            parent->left = NULL;
        else
            parent->right = NULL;

        pthread_mutex_unlock(&parent->mutex);
        free(succ);
    }
    return 1;
}

/* 
 * TODO
 *  Implement a function which remove nodes from the BST in coarse-grained manner.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node in coarse-grained manner from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */
int lab2_node_remove_cg(lab2_tree *tree, int key)
{
    if (tree == NULL)
    {
        perror("Error: Empty tree node deletion!");
        exit(-1);
    }
    pthread_mutex_lock(&tree->mutex);
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
    if (curr == NULL)
        return 0;
    /*Node with no child (leaf node)*/
    if (curr->left == NULL && curr->right == NULL)
    {
        if (parent->left == curr)
            parent->left = NULL;
        else
            parent->right = NULL;
        free(curr);
    }
    /*Node with left child only*/
    else if (curr->left != NULL && curr->right == NULL)
    {
        if (parent->left == curr)
            parent->left = curr->left;
        else
            parent->right = curr->left;
        free(curr);
    }
    /*Node with right child only*/
    else if (curr->left == NULL && curr->right != NULL)
    {
        if (parent->left == curr)
            parent->left = curr->right;
        else
            parent->right = curr->right;
        free(curr);
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
    }
    pthread_mutex_unlock(&tree->mutex);
    return 1;
}

/*
 * TODO
 *  Implement function which delete struct lab2_tree
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param lab2_tree *tree  : bst which you want to delete. 
 *  @return                 : status(success or fail)
 */
void _lab2_tree_delete(lab2_node *node)
{
    if (node == NULL)
        return;
    _lab2_tree_delete((lab2_node *)node->left);
    _lab2_tree_delete((lab2_node *)node->right);
    //    printf("\n delete node %d", node->key);
    free(node);
}

void lab2_tree_delete(lab2_tree *tree)
{
    if (tree == NULL)
    {
        perror("Error : empty tree deletion");
        exit(-1);
    }
    _lab2_tree_delete(tree->root);
    tree->root = NULL;
    free(tree);
}