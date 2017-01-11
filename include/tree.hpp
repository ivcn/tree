#ifndef __TREE_HPP__
#define __TREE_HPP__

#include <node.hpp>

#include <functional>
#include <stack>

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

    void insert(const Type& el) {
        addNode(getRoot(), std::move(Node<Type>::makeNode(el)));
    }

    bool empty() {
        return getRoot() == nullptr;
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

    enum class TraverseType {
        PreOrder,
        InOrder,
        PostOrder
    };

    void traverse(TraverseType type, std::function<void(Type)> action) {
        if (type == TraverseType::PreOrder) {
            preOrderTraversal(action);
        }
        else if (type == TraverseType::InOrder) {
            inOrderTraversal(action);
        }
        else { // type == TraverseType::PostOrder
            postOrderTraversal(action);
        }
    }

private:

    void addNode(UPtrNode<Type>& subroot, UPtrNode<Type> newNode) {
        if (!subroot) {
            subroot = std::move(newNode);
            return;
        }
        auto currentRoot = subroot.get();
        while (currentRoot != nullptr) {
            auto branch = newNode->getContent() <= currentRoot->getContent() ?
                currentRoot->getLeft().get() :
                currentRoot->getRight().get();
            if (branch == nullptr)
                break;
            else
                currentRoot = branch;
        }
        if (newNode->getContent() <= currentRoot->getContent()) {
            currentRoot->setLeft(std::move(newNode));
        }
        else {
            currentRoot->setRight(std::move(newNode));
        }
    }

    void preOrderTraversal(std::function<void(Type)> visit) {
        std::stack<Node<Type>*> stack;
        stack.push(getRoot().get());
        while (!stack.empty()) {
            auto node = stack.top();
            stack.pop();
            visit(node->getContent());
            if (node->hasRight())
                stack.push(node->getRight().get());
            if (node->hasLeft())
                stack.push(node->getLeft().get());
        }
    }

    void inOrderTraversal(std::function<void(Type)> visit) {
        std::stack<Node<Type>*> stack;
        auto node = getRoot().get();
        while (!stack.empty() || node != nullptr) {
            if (node != nullptr) {
                stack.push(node);
                node = node->getLeft().get();
            }
            else {
                node = stack.top();
                stack.pop();
                visit(node->getContent());
                node = node->getRight().get();
            }
        }
    }

    void postOrderTraversal(std::function<void(Type)> visit) {
        std::stack<Node<Type>*> stack;
        stack.push(getRoot().get());
        auto node = stack.top();
        while (!stack.empty()) {
            if (node != nullptr) {
                if(node->hasRight())
                    stack.push(node->getRight().get());
                if(node->hasLeft())
                    stack.push(node->getLeft().get());
                node = node->getLeft().get();
            }
            else {
                // we came to leaf
                if (stack.top() == nullptr) {
                    // there is no such child.
                    // Proceed to search. Sometimes we'll find some left child 
                    continue;
                }
                visit(stack.top()->getContent());
                auto tmp = stack.top();
                stack.pop();
                if (stack.empty())
                    break;
                if (stack.top()->isLeaf() || stack.top()->isParent(tmp)) {
                    visit(stack.top()->getContent());
                    stack.pop();
                }
                else {
                    // visit right subtree
                    node = stack.top();
                }
            }
        }
    }

    UPtrNode<Type> root;
};
#endif __TREE_HPP__