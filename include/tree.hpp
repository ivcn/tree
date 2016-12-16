#ifndef __TREE_HPP__
#define __TREE_HPP__

#include <node.hpp>

template<class T>
using UPtrNode = std::unique_ptr<Node<T>>;

template<class Type>
class Tree {
public:
    Tree() : root(nullptr)
    { }

    Tree(UPtrNode<Type> node) : root(node)
    { }

    Tree(const Tree& other) = delete;
    Tree& operator= (const Tree& other) = delete;

    Tree(Tree&& other) = default;
    Tree& operator=(Tree&& other) = default;

    ~Tree() = default;

    void insert(const Type& el) {
        addNode(getRoot(), std::move(Node<Type>::makeNode(el)));
    }

    void remove(const Type& el) {
        if (!getRoot()) {
            throw runtime_error("Trying to remove from empty tree");
        }
        if (getRoot()->getContent() == el) {
            auto detachedRightSubtree = std::move(getRoot()->getRight());
            auto detachedLeftSubtree = std::move(getRoot()->getLeft());
            setRoot(std::move(detachedLeftSubtree));
            addNode(getRoot(), std::move(detachedRightSubtree));
            return;
        }
        auto currentRoot = getRoot().get();
        while (currentRoot != nullptr) {
            auto child = el <= currentRoot->getContent() ?
                currentRoot->getLeft().get() :
                currentRoot->getRight().get();
            if (child != nullptr && child->getContent() == el)
                break;
            else
                currentRoot = child;
        }
        if (currentRoot == nullptr) {
            throw std::runtime_error("Element not found");
        }
        //detach target node from tree
        auto target = currentRoot->getLeft()->getContent() == el ?
            std::move(currentRoot->getLeft()) :
            std::move(currentRoot->getRight());

        if (target->isLeaf()) {
            target.reset();
            return;
        }
        // always set left child as a new root of subtree.
        if (target->getContent() <= currentRoot->getContent()) {
            currentRoot->setLeft(std::move(target->getLeft()));
        }
        else {
            currentRoot->setRight(std::move(target->getLeft()));
        }
        // right subtree of targer node will go down 
        // and find a good place for yourself
        if (target->getRight()) {
            addNode(currentRoot->getLeft(), std::move(target->getRight()));
        }
    }


    UPtrNode<Type>& getRoot() {
        return root;
    }

    void setRoot(UPtrNode<Type> node) {
        root = std::move(node);
    }

private:

    void addNode(UPtrNode<Type>& subroot, UPtrNode<Type> newNode) {
        if (!subroot) {
            subroot = std::move(newNode);
            return;
        }
        auto currentRoot = subroot.get();
        while (currentRoot != nullptr) {
            auto branch = currentRoot->getContent() <= newNode->getContent() ?
                currentRoot->getLeft().get() :
                currentRoot->getRight().get();
            if (branch == nullptr)
                break;
            else
                currentRoot = branch;
        }
        if (currentRoot->getContent() <= newNode->getContent()) {
            currentRoot->setLeft(std::move(newNode));
        }
        else {
            currentRoot->setRight(std::move(newNode));
        }
    }

    UPtrNode<Type> root;
};
#endif __TREE_HPP__