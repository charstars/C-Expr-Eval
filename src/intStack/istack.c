/*
 *  Integer Stack ADT : basic test driver
 *
 *  COMP220: Lab 7
 *  Author:  Charmaine Neufeld
 *  Date:    Feb. 8, 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#include "istack.h"

Node2_t* Node2Create(int value);
void Node2Print(Node2_t Node2);
void stackInsertAfter(IntStack_t* stack, Node2_t* cursor, int item);
void stackLinkAfter(IntStack_t* stack, Node2_t* cursor, Node2_t* newNode2);


Node2_t* Node2Create(int value)
{
	Node2_t* Node2 = malloc(sizeof(Node2_t));
    Node2->data = value;
    Node2->next = NULL;
    return Node2;
}

IntStack_t istackCreate( )
{
    Node2_t* dummy = Node2Create(INT_MIN);
	IntStack_t stack = {dummy};
	return stack;
}

void istackDestroy(IntStack_t *stack)
{
	while (stack->head->next != NULL){
		istackPop(stack);
	}
}

void Node2Print(Node2_t Node2)
{
    printf("|\t%d\t|", Node2.data);
}


void istackPrint(IntStack_t stack)
{
	Node2_t* cur = stack.head->next;
	printf("\nStack:\n");
	while (cur != NULL){
    	Node2Print(*cur);
    	printf("\n");
    	cur = cur->next;
	}
	printf(" ---------------");
	printf("\n\n");
}

bool istackIsEmpty(IntStack_t stack)
{
    return stack.head->next == NULL;
}

int istackTop(IntStack_t stack)
{
    assert(!istackIsEmpty(stack));
	return stack.head->next->data;
}

int istackPop(IntStack_t *stack)
{
    assert(!istackIsEmpty(*stack));
	Node2_t* Node2 = stack->head->next;
	stack->head->next = Node2->next;
	int value = Node2->data;
	free(Node2);
	return value;
    
}

int istackPush( IntStack_t *stack, int item)
{
	stackInsertAfter(stack, stack->head, item);
}

void stackInsertAfter(IntStack_t* stack, Node2_t* cursor, int value)
{
	Node2_t* newNode2 = Node2Create(value);
	stackLinkAfter(stack, cursor, newNode2);
}

void stackLinkAfter (IntStack_t* stack, Node2_t* cursor, Node2_t* newNode2)
{
    newNode2->next = cursor->next;
    cursor->next = newNode2;
}