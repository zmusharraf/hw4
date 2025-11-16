#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
  void rotateLeft(AVLNode<Key,Value>* x);
  void rotateRight(AVLNode<Key,Value>* x);
  void insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);
  void removeFix(AVLNode<Key,Value>* p, int diff);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if(this->root_ == nullptr){
      this->root_ = new AVLNode<Key,Value>(new_item.first, new_item.second, nullptr);
      return;
    }

    Node<Key,Value>* cur = this->root_;
    Node<Key,Value>* parent = nullptr;

    while(cur != nullptr){
      parent = cur;
      if(new_item.first < cur->getKey()){
        cur = cur->getLeft();
      } else if(cur->getKey() < new_item.first){
        cur = cur->getRight();
      } else {
        cur->setValue(new_item.second);
        return;
      }
    }

    AVLNode<Key,Value>* p = static_cast<AVLNode<Key,Value>*>(parent);
    AVLNode<Key,Value>* n = new AVLNode<Key,Value>(new_item.first, new_item.second, p);

    if(new_item.first < p->getKey()){
      p->setLeft(n);
      p->updateBalance(-1);
    } else {
      p->setRight(n);
      p->updateBalance(1);
    }

    if(p->getBalance() == 0){
      return;
    }

    insertFix(p, n);
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key,Value>* n = static_cast<AVLNode<Key,Value>*>(this->internalFind(key));
    if(n == nullptr){
      return;
    }

    if(n->getLeft() != nullptr && n->getRight() != nullptr){
      AVLNode<Key,Value>* pred = n->getLeft();
      while(pred->getRight() != nullptr){
        pred = pred->getRight();
      }
      nodeSwap(n, pred);
    }

    AVLNode<Key,Value>* child;
    if(n->getLeft() != nullptr){
      child = n->getLeft();
    } else {
      child = n->getRight();
    }
    AVLNode<Key,Value>* p = n->getParent();
    int diff = 0;

    if(child != nullptr){
      child->setParent(p);
    }

    if(p == nullptr){
      this->root_ = child;
    } else if(p->getLeft() == n){
      p->setLeft(child);
      diff = 1;
    } else {
      p->setRight(child);
      diff = -1;
    }

    delete n;

    if(p != nullptr){
      removeFix(p, diff);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* x){
  AVLNode<Key,Value>* y = x->getRight();
  AVLNode<Key,Value>* beta = y->getLeft();
  AVLNode<Key,Value>* xp = x->getParent();

  y->setParent(xp);
  if(xp == nullptr){
    this->root_ = y;
  } else if(xp->getLeft() == x){
    xp->setLeft(y);
  } else {
    xp->setRight(y);
  }

  y->setLeft(x);
  x->setParent(y);

  x->setRight(beta);
  if(beta != nullptr){
    beta->setParent(x);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* x){
  AVLNode<Key,Value>* y = x->getLeft();
  AVLNode<Key,Value>* beta = y->getRight();
  AVLNode<Key,Value>* xp = x->getParent();

  y->setParent(xp);
  if(xp == nullptr){
    this->root_ = y;
  } else if(xp->getLeft() == x){
    xp->setLeft(y);
  } else {
    xp->setRight(y);
  }

  y->setRight(x);
  x->setParent(y);

  x->setLeft(beta);
  if(beta != nullptr){
    beta->setParent(x);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n){
  if(p == nullptr){
    return;
  }
  AVLNode<Key,Value>* g = p->getParent();

  if(g == nullptr){
    int pb = p->getBalance();
    if(pb == -2){
      AVLNode<Key,Value>* L = p->getLeft();
      if(n == L || n == L->getLeft()){
        rotateRight(p);
        static_cast<AVLNode<Key,Value>*>(p)->setBalance(0);
        L->setBalance(0);
      } else {
        AVLNode<Key,Value>* LR = L->getRight();
        int b = LR->getBalance();

        rotateLeft(L);
        rotateRight(p);

        if(b == -1){
          p->setBalance(1);
          L->setBalance(0);
        } else if(b == 0){
          p->setBalance(0);
          L->setBalance(0);
        } else {
          p->setBalance(0);
          L->setBalance(-1);
        }
        LR->setBalance(0);
      }
    } else if(pb == 2){
      AVLNode<Key,Value>* R = p->getRight();
      if(n == R || n == R->getRight()){
        rotateLeft(p);
        static_cast<AVLNode<Key,Value>*>(p)->setBalance(0);
        R->setBalance(0);
      } else {
        AVLNode<Key,Value>* RL = R->getLeft();
        int b = RL->getBalance();

        rotateRight(R);
        rotateLeft(p);

        if(b == 1){
          p->setBalance(-1);
          R->setBalance(0);
        } else if(b == 0){
          p->setBalance(0);
          R->setBalance(0);
        } else {
          p->setBalance(0);
          R->setBalance(1);
        }
        RL->setBalance(0);
      }
    }
    return;
  }

  if(p == g->getLeft()){
    g->updateBalance(-1);
    if(g->getBalance() == 0){
      return;
    } else if(g->getBalance() == -1){
      insertFix(g, p);
    } else { // -2
      if(n == p->getLeft()){
        // LL
        rotateRight(g);
        p->setBalance(0);
        g->setBalance(0);
      } else {
        // LR
        AVLNode<Key,Value>* c = p->getRight();
        int b = c->getBalance();

        rotateLeft(p);
        rotateRight(g);
        if(b == -1){
          p->setBalance(0);
          g->setBalance(1);
        } else if(b == 0){
          p->setBalance(0);
          g->setBalance(0);
        } else {
          p->setBalance(-1);
          g->setBalance(0);
        }
        c->setBalance(0);
      }
    }
  } else { 
    g->updateBalance(1);
    if(g->getBalance() == 0){
      return;
    } else if(g->getBalance() == 1){
      insertFix(g, p);
    } else { // +2
      if(n == p->getRight()){
        //RR
        rotateLeft(g);
        p->setBalance(0);
        g->setBalance(0);
      } else {
        //RL
        AVLNode<Key,Value>* c = p->getLeft();
        int b = c->getBalance();

        rotateRight(p);
        rotateLeft(g);
        if(b == 1){
          p->setBalance(0);
          g->setBalance(-1);
        } else if(b == 0){
          p->setBalance(0);
          g->setBalance(0);
        } else {
          p->setBalance(1);
          g->setBalance(0);
        }
        c->setBalance(0);
      }
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* p, int diff){
  while(p != nullptr){
    p->updateBalance(diff);

    if(p->getBalance() == -1 || p->getBalance() == 1){
      return;
    }
    if(p->getBalance() == 0){
      AVLNode<Key,Value>* gp = p->getParent();
      if(gp == nullptr){
        return;
      }
      if(p == gp->getLeft()){
        p = gp;
        diff = 1;
      } else {
        p = gp;
        diff = -1;
      }
    } else if(p->getBalance() == -2){
      AVLNode<Key,Value>* L = p->getLeft();
      if(L->getBalance() <= 0){
        rotateRight(p);
        if(L->getBalance() == 0){
          p->setBalance(-1);
          L->setBalance(1);
          return;
        } else {
          p->setBalance(0);
          L->setBalance(0);
          AVLNode<Key,Value>* gp = L->getParent();
          if(gp == nullptr){
            return;
          } if(L == gp->getLeft()){
            p = gp;
            diff = 1;
          } else {
            p = gp;
            diff = -1;
          }
        }
      } else {
        AVLNode<Key,Value>* LR = L->getRight();
        int b = LR->getBalance();
        rotateLeft(L);
        rotateRight(p);
        if(b == -1){
          p->setBalance(1);
          L->setBalance(0);
        } else if(b == 0){
          p->setBalance(0);
          L->setBalance(0);
        } else {
          p->setBalance(0);
          L->setBalance(-1);
        }
        LR->setBalance(0);
        AVLNode<Key,Value>* gp = LR->getParent();
        if(gp == nullptr){
          return;
        }
        if(LR == gp->getLeft()){
          p = gp;
          diff = 1;
        } else {
          p = gp;
          diff = -1;
        }
      }
    } else if(p->getBalance() == 2){
      AVLNode<Key,Value>* R = p->getRight();
      if(R->getBalance() >= 0){
        rotateLeft(p);
        if(R->getBalance() == 0){
          p->setBalance(1);
          R->setBalance(-1);
          return;
        } else {
          p->setBalance(0);
          R->setBalance(0);
          AVLNode<Key,Value>* gp = R->getParent();
          if(gp == nullptr){
            return;
          }
          if(R == gp->getLeft()){
            p = gp;
            diff = 1;
          } else {
            p = gp;
            diff = -1;
          }
        }
      } else {
        AVLNode<Key,Value>* RL = R->getLeft();
        int b = RL->getBalance();
        rotateRight(R);
        rotateLeft(p);
        if(b == 1){
          p->setBalance(-1);
          R->setBalance(0);
        } else if(b == 0){
          p->setBalance(0);
          R->setBalance(0);
        } else {
          p->setBalance(0);
          R->setBalance(1);
        }
        RL->setBalance(0);
        AVLNode<Key,Value>* gp = RL->getParent();
        if(gp == nullptr){
          return;
        }
        if(RL == gp->getLeft()){
          p = gp;
          diff = 1;
        } else {
          p = gp;
          diff = -1;
        }
      }
    }
  }
}

#endif
