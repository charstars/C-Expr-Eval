/**
 *	 Implementation for an infix integer algebraic expression evaluator.
 *     Converts in-fix expression to postfix notation, evaluates expression.
 *     Infix expression is provided as a single, quoted command-line argument.
 *
 *  COMP220: Lab 7 Starter Project - Stacks & Queues
 *  Author:  Joseph Fall
 *  Date:    Mar. 6, 2018
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "ds/stack.h"
#include "ds/queue.h"
#include "intStack/istack.h"

// Helper Functions
Queue_t tokenize(char* expression);
bool isOperand(char* t);
int operandValue(char* t);
bool isOperator(char* token);
bool isOpenBracket(char* token);
bool isCloseBracket(char* token);
bool isBracket(char* token);
int precedence(char* token);
void isQueueEmpty(Queue_t expression);
int operations(IntStack_t* stack, char* token);

// Experession Evaluator Functions
Queue_t toPostfix(Queue_t infix_tokens);
int evalExpr(Queue_t postfix_tokens);


//**********************
//  MAIN
//**********************
int main(int argc, char* argv[]) {
   if (argc != 2) {
      printf("Usage: eval \"4 + 5 * ( x - a ) / 3\" \n\n");
      exit(-1);
   }
   Queue_t infix_expression;
   Queue_t postfix_expression;
   
   infix_expression = tokenize(argv[1]);
   printf("Infix Expression: ");
   qPrint(infix_expression);
   
   postfix_expression = toPostfix(infix_expression);
   printf("Postfix Expression: ");
   qPrint(postfix_expression);
   
   printf("Answer: %d\n", evalExpr(postfix_expression));

   qDestroy(&infix_expression);
   qDestroy(&postfix_expression);
   return 0;
}


//**********************
// Helpers
//**********************

// POST: returns the single character symbol at the start of t, or NULL
char symbol(char* t)
{
	return strlen(t) > 0 ? t[0] : '\0';
}

// POST: returns true if c is a valid operand symbol, false otherwise
bool isOperand(char* t)
{
	int i;
	for (i=0; i<strlen(t); i++) {
		if (! (isdigit(t[i]) || isalpha(t[i])))
			return false;	// valid operands must be alpha-numeric
	}
	return strlen(t) > 0;
}

// PRE: IsOperand(t)
// POST: returns numeric value of operand t
int operandValue(char* t)
{
	assert(isOperand(t));
	char c = symbol(t);
	// if c is a digit, return its numeric value
	if (isdigit(c)) {
		return atoi(t);
	}
	else 
	{	// c is an identifier, do a lookup to find its value
		//   TODO: implement proper identifier lookup table.  For now, return 0 for a, 1 for b, etc..
		return (tolower(c) - 'a');
	}
}

// POST: returns true if t represents a valid operator symbol, false otherwise
bool isOperator(char* t)
{
	char c = symbol(t);
	return strlen(t) == 1 && (c=='+' || c=='-' || c=='*' || c=='/' );
}

// POST: returns true if t is the open bracket symbol, false otherwise
bool isOpenBracket(char* t)
{
	char c = symbol(t);
	return strlen(t) == 1 && c=='(';
}

// POST: returns true if t is the close bracket symbol, false otherwise
bool isCloseBracket(char* t)
{
	char c = symbol(t);
	return strlen(t) == 1 && c==')';
}

// POST: returns true if t is a bracket symbol, false otherwise
bool isBracket(char* t)
{
	return isOpenBracket(t) || isCloseBracket(t);
}

// PRE: isOperator(t) || isBracket(t)
// POST: returns relative precedence of the operator, op, as follows:
//		lowest to highest : ( ) + - * /
int precedence(char* t)
{
	assert( isOperator(t) || isBracket(t) );
	char op = t[0];
	if (op == '(' || op == ')' )
		return 0;
	else if (op == '+' || op == '-')
		return 1;
	else if (op == '*' || op == '/' )
		return 2;

	assert (false);	// should never happen
	return 0;
}

// PRE: expression is a set of space-separated tokens
// POST: Side-effect - spaces replaced by '\0' in original expression
// RETURN: pointers to each token in original expression returned in sequence
// Caller is responsible for calling qDestroy on returned Queue object.
Queue_t tokenize(char* expression)
{
   const char* sep = " "; // each token separaterd by single space!
   char* t;
   Queue_t tokens = qCreate();
  
   t = strtok(expression, sep); // initialize strtok with expression.
   
   /* parse rest of tokens */
   while( t != NULL ) {
      qEnqueue(&tokens, t);
      t = strtok(NULL, sep); // next token
   }
   return tokens;
}

//********************************************
//  In-fix experession --> Post-fix expression
//
// NOTE: a valid expression for this module may only contain:
//		 -- integer or symbolic operands
//		 -- the operators * + - /  (no unary operators)
//		 -- matching brackets in pairs ( and )
//*********************************************

// PRE: infix_tokens contains a valid in-fix algebraic expression, as defined above.
// POST: returns a Queue containing the expression tokens in post-fix sequence.
// Caller is responsible for calling qDestroy on returned Queue object.
Queue_t toPostfix(Queue_t infix_tokens)
{
   int i;
	char* token;
	Queue_t expression = qCreate();
	Stack_t operators = stackCreate();

	while (!qIsEmpty(infix_tokens)) {
		token = qDequeue(&infix_tokens);

		if (isOperator(token) ) {  // Math Operator
		  // any operators of equal or higher precedence on stack need be evaluated first... 
			while ((! stackIsEmpty(operators)) &&
			         precedence(token) <= precedence(stackTop(operators)) )
			   qEnqueue(&expression, stackPop(&operators));
			// then push the new operator onto the stack for evaluation once we have all operands
			stackPush(&operators, token);
		}
		else if (isOpenBracket(token)) { // Open Bracket
		  // push open brackets on stack so sub-expression is evaluated as a group
			stackPush(&operators, token);
	  }
		else if (isCloseBracket(token)) { // Close Bracket
		  // pop operators for sub-expression and place them in queue for evaluation
			while (!stackIsEmpty(operators) && ! isOpenBracket(stackTop(operators))) {
			   qEnqueue(&expression, stackPop(&operators));
			}
			// TODO: add some better error handling here
			assert(! stackIsEmpty(operators) );  // there MUST be a matching Open bracket
			stackPop(&operators);		// remove the matching open bracket.
		}
		else	{ // Operand
			// TODO: add some better error handling here
			assert( isOperand(token) );  // token MUST otherwise be a valid operand
			// Simply queue up the operand for evaluation
			qEnqueue(&expression, token);
		}
	}
	// Finally, add any remaining (usually high precedence) operators to expression.
	while (! stackIsEmpty(operators)) {
			// TODO: add some better error handling here
		assert( ! isOpenBracket(stackTop(operators) ));
		
		qEnqueue(&expression, stackPop(&operators));
	}
	assert( stackIsEmpty(operators) );
	stackDestroy(&operators);
	return expression;
}

//*********************************************
//  Evaluate Post-fix Expression
//*********************************************

// PRE: postfix contains a valid post-fix algebraic expression, as defined above
// 		  Variable substitutions still need to be performed.
// POST: returns the result of evaluating the post-fix expression.
int evalExpr(Queue_t expression) //queue=characters
{
	IntStack_t newStack = istackCreate();
	while(!qIsEmpty(expression))
	{
		char* token = qDequeue(&expression);
		if(isOperand(token)){
			int value = operandValue(token);
			istackPush(&newStack, value);
		}
		else{
			assert(isOperator(token));
			operations(&newStack, token);
		}
	}
	return(istackPop(&newStack));
}


int operations(IntStack_t* stack, char* token)
{
	int value;
	int op1 = istackPop(stack);
	int op2 = istackPop(stack);
	switch(token[0])
	{
		case '+':
			value = op2 + op1;
			break;
		case '-':
			value = op2 - op1;
			break;
		case '/':
			value = op2 / op1;
			break;
		case '*':
			value = op2 * op1;
			break;
		case '%':
			value = op2 % op1;
			break;
	}
	istackPush(stack, value);
}


