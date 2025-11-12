#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int depthOrFail(Node* n)
{
  if(n == nullptr){
    return 0; //empty subtree
  }

  if(n->left == nullptr && n->right == nullptr){
    return 1; //leaf depth 1
  }

  int leftDepth = depthOrFail(n->left);
  int rightDepth = depthOrFail(n->right);

  if(leftDepth == -1 || rightDepth == -1){
    return -1;
  }

  //both children exist and have different depths
  if(n->left != nullptr && n->right != nullptr && leftDepth != rightDepth){
    return -1;
  }

  if(leftDepth != 0){
    return leftDepth + 1;
  } else {
    return rightDepth + 1;
  }
}

bool equalPaths(Node * root)
{
    // Add your code below
    return depthOrFail(root) != -1;
}