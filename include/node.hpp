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

    ~Node() = default;

    template<class... Types>
    static auto makeNode(Types&&... args) {
        return std::make_unique<Node<T>>(
            new T(std::forward<Types>(args)...));
    }

	const UPtr<Node<T>>& getLeft() {
		return left;
	}

	const UPtr<Node<T>>& getRight() {
		return right;
	}

	UPtr<T>& getContent() {
		return data;
	}

    void setLeft(const UPtr<Node<T>>& p) {
        left = std::move(p);
    }

    void setRight(const UPtr<Node<T>>& p) {
        right = std::move(p);
    }

    void setContent(const UPtr<T>& p) {
        data = std::move(p);
    }

private:
	UPtr<T> data;
	UPtr<T> left;
	UPtr<T> right;
};

#endif __NODE_HPP__