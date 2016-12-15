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

    UPtrNode<Type>& getRoot() {
        return root;
    }

    void setRoot(UPtrNode<Type> node) {
        root = std::move(node);
    }

    void addNode(UPtrNode<Type> newNode) {
        if (!getRoot()) {
            setRoot(std::move(newNode));
            return;
        }
        auto currentRoot = getRoot().get();
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

private:
    UPtrNode<Type> root;
};
#endif __TREE_HPP__