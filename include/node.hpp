#ifndef __NODE_HPP__
#define __NODE_HPP__

#include <iostream>
#include <memory>

template<class T>
using UPtr = std::unique_ptr<T>;

template<class T>
class Node {
public:
	Node(T* ptr) {
        data.reset(ptr);
    }

    Node(UPtr<T>& ptr) :
        data(std::move(ptr))
    { }
    
    Node(const Node& n) = delete;
    Node& operator=(const Node& other) = delete;

    Node(Node&& n) = default;
    Node& operator=(Node&& n) = default;

    ~Node() = default;

    template<class... Types>
    static auto makeNode(Types&&... args) {
        return std::make_unique<Node<T>>(
            new T(std::forward<Types>(args)...));
    }

	const UPtr<Node<T>>& getLeft() const {
		return left;
	}

	const UPtr<Node<T>>& getRight() const {
		return right;
	}

	UPtr<T>& getContent() {
		return data;
	}

    void setLeft(UPtr<Node<T>> p) {
        left = std::move(p);
    }

    void setRight(UPtr<Node<T>> p) {
        right = std::move(p);
    }

    void setContent(const UPtr<T> p) {
        data = std::move(p);
    }

    inline bool isLeaf() {
        return (getLeft() == nullptr && getRight() == nullptr);
    }

private:
	UPtr<T> data;
	UPtr<Node<T>> left;
	UPtr<Node<T>> right;
};

#endif __NODE_HPP__