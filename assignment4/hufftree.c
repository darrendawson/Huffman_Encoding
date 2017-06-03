# include "hufftree.h"

//=======================================================================
// TREE
//=======================================================================

//---newNode-------------------------------------------------------------

// create a new node
treeNode *newNode(uint8_t s, bool l, uint64_t f)
{
  treeNode *node = (treeNode*)calloc(1, sizeof(treeNode));
  node->symbol = s;
  node->frequency = f;
  node->leaf = l;
  
  return node;
}

//---deleteNode----------------------------------------------------------

// deletes a node
void deleteNode(treeNode *node)
{
  free(node);
  return;
}

//---deleteTree----------------------------------------------------------

// post-order traverse the tree and delete it
void deleteTree(treeNode *node)
{
  // walk the tree and delete it
  if (node->leaf)
  {
    deleteNode(node);
    return;
  }

  // call recursively
  deleteTree(node->left);
  deleteTree(node->right);

  deleteNode(node);
  return;
}

//---join----------------------------------------------------------------

// join two subtrees
treeNode *join(treeNode *l, treeNode *r)
{
  treeNode *parent = newNode('*', false,
			     l->frequency + r->frequency);
  parent->left = l;
  parent->right = r;

  return parent;
}

//---generateTreeInstructions--------------------------------------------

// returns a String containing instructions to rebuild tree
// L means leaf, J means join
void generateTreeInstructions(treeNode *node, char *instructions)
{
  if (node->leaf)
  {
    strcat(instructions, "L");
    strcat(instructions, (char*)&(node->symbol));
    return;
  }

  // post order recursive calls
  generateTreeInstructions(node->left, instructions);
  generateTreeInstructions(node->right, instructions);

  // bubble back up
  strcat(instructions, "J"); // need to join the two children
  return;
}

//---rebuildTree---------------------------------------------------------

// rebuilds a tree by parsing instructions and using a stack
// L: char is a leaf, push to stack
// J: pop off and join 2 treeNodes from the stack
treeNode *rebuildTree(char *instructions, int size)
{
  int index = 0;
  huffStack *stack = newHuffStack(1000);
  treeNode *node;
  
  while (index < size)
  {
    if (instructions[index] == 'L')
    {
      // it's a leaf
      index++; // move to char we care about
      node = newNode(instructions[index], true, 0);
      pushHuffStack(stack, node);
      index++;
    }
    else if (instructions[index] == 'J')
    {
      // join children 
      node = join(popHuffStack(stack), popHuffStack(stack));
      pushHuffStack(stack, node);
      index++;
    }
    else
    {
      // something went wrong
      printf("Couldn't parse instructions\n");
    }
  }
  deleteHuffStack(stack);
  return node;
}


//=======================================================================
// STACK
//=======================================================================

//---newHuffStack--------------------------------------------------------

// creates a new stack of designated size
huffStack *newHuffStack(uint32_t newSize)
{
  huffStack *newStack;
  newStack = (huffStack*)calloc(1, sizeof(huffStack));
  newStack->entries = (treeNode**)calloc(newSize, sizeof(treeNode*));
  
  newStack->size = newSize;
  newStack->top = 0;

  return newStack;
}

//---deleteHuffStack-----------------------------------------------------

// deletes a stack
void deleteHuffStack(huffStack *stack)
{
  free(stack->entries);
  free(stack);
  return;
}

//---pushHuffStack-------------------------------------------------------

// pushes to top of stack
void pushHuffStack(huffStack *stack, treeNode *node)
{
  // if not full
  if (!fullHuffStack(stack))
  {
    stack->entries[stack->top] = node;
    stack->top = (stack->top)+1;
  }
  return;
}

//---popHuffStack--------------------------------------------------------

// pops off top of stack
treeNode *popHuffStack(huffStack *stack)
{
  treeNode *node = (treeNode*)NULL;

  // if not empty
  if (!emptyHuffStack(stack))
  {
    node = stack->entries[(stack->top)-1]; // get value
    stack->entries[(stack->top)-1] = NULL; // nullify value
    stack->top -= 1;                       // pop it
  }
  return node;
}

//---emptyHuffStack------------------------------------------------------

// returns true if stack is empty
bool emptyHuffStack(huffStack *stack)
{
  return (stack->top == 0);
}

//---fullHuffStack-------------------------------------------------------

// returns true if stack is full
bool fullHuffStack(huffStack *stack)
{
  return (stack->top == stack->size);
}


//=======================================================================
// DEBUGGING TOOLS
//=======================================================================

/*
//---dumpTree------------------------------------------------------------

// dumps a huffman tree onto a file
void dumpTree(treeNode *t, int file)
{

}

//---stepTree------------------------------------------------------------

// step through a tree following the code
int32_t *stepTree(treeNode *root, treeNode *t, uint32_t code)
{

}
*/

//---printTree-----------------------------------------------------------

// prints the tree for debugging purposes
// code written by Darrell Long
void printTree(treeNode *t, int depth)
{
  if (t && t->leaf)
  {
    if (isalnum(t->symbol))
    {
      spaces(4 * depth); printf("%c (%lu)\n", t->symbol, t->frequency);
    }
    else
    {
      spaces(4 * depth); printf("%X (%lu)\n", t->symbol, t->frequency);
    }
  }
  else if (t)
  {
    spaces(4 * depth); printf("$ (%lu)\n", t->frequency);
    printTree(t->left, depth + 1);
    printTree(t->right, depth + 1);
  }
  return;
}
