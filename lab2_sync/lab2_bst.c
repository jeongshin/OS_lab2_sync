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

queue *create_queue()
{
    queue *new_queue = (queue *)malloc(sizeof(queue));
    new_queue->count = 0;
    new_queue->front = NULL;
    new_queue->rear = NULL;
    return new_queue;
}

tnptr pop(queue *queue)
{
    if (queue->count == 0)
        return;
    else
    {
        qnptr temp = queue->front;
        queue->front = temp->next;
        tnptr node = &temp->tnode;
        free(temp);
        queue->count--;
        return node;
    }
}

void push(queue *queue, tnptr tnode)
{
    qnptr qnode = (qnptr)malloc(sizeof(qnptr));
    qnode->tnode = tnode;
    qnode->next = NULL;
    if (qnode->count == 0)
    {
        queue->front = qnode;
        queue->rear = qnode;
    }
    else
    {
        queue->rear->next = qnode;
        queue->rear = qnode;
    }
    queue->count++;
    return;
}

void destroy_queue(queue *queue)
{
    while (queue->count != 0)
    {
        pop(queue);
    }
    free(queue);
    return;
}
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
    printf("%d", node->key);
    traversal_node_inorder((lab2_node *)node->right);
}

int lab2_node_print_inorder(lab2_tree *tree)
{
    lab2_node *node = tree->root;
    traversal_node_inorder(node);
    return 0;
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
    if (tree == NULL)
    {
        perror("Error: tree malloc error occurred!");
        return -1;
    }
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
    if (node == NULL)
    {
        perror("Error: node malloc error occurred!");
        return -1;
    }
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
    lab2_node *root = tree->root;
    if (root == NULL)
    {
        perror("Error: empty tree node removal!");
        return 0;
    }
    if (root->left == NULL && root->right == NULL)
    {
        if (root->key == key)
            return 0;
    }
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
    // You need to implement lab2_node_remove_fg function.
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
}

/*
 * TODO
 *  Implement function which delete struct lab2_tree
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param lab2_tree *tree  : bst which you want to delete. 
 *  @return                 : status(success or fail)
 */
void lab2_tree_delete(lab2_tree *tree)
{
    queue *queue = create_queue();
    lab2_node *root = tree->root;
    lab2_node *curr = NULL;
    if (root == NULL)
    {
        perror("Warning: delete empty tree");
        return;
    }
    push(queue, root);
    while (queue->count > 0)
    {
        curr = pop(queue);
        if (curr->left)
            push(queue, curr->left);
        if (curr->right)
            push(queue, curr->right);
        free(curr);
    }
    root = NULL;
}
