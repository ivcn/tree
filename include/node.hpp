#ifndef __NODE_HPP__
#define __NODE_HPP__

#include <memory>

template<class T>
using UPtr = std::unique_ptr<T>;

template<class T>
class Node {
public:
	Node(T* ptr = nullptr) {
        data.reset(ptr);
    }

    Node(UPtr<T>& ptr) :
        data(std::move(ptr))
    { }
    
    Node(const Node& n) = delete;
    Node& operator=(const Node& other) = delete;

    Node(Node&& n) = default;
    Node& operator=(Node&& n) = default;

    virtual ~Node() { }

    template<class... Types>
    static auto makeNode(Types&&... args) {
        return std::make_unique<Node<T>>(
            new T(std::forward<Types>(args)...));
    }

    bool isEmpty() const {
        return data == nullptr;
    }

    bool hasLeft() {
        return left != nullptr;
    }

    bool hasRight() {
        return right != nullptr;
    }
    
    bool isParent(const Node<T>* node) {
        return left.get() == node || right.get() == node;
    }

	UPtr<Node<T>>& getLeft() {
		return left;
	}

	UPtr<Node<T>>& getRight() {
		return right;
	}

	const T& getContent() const {
        if (!data)
            throw std::runtime_error("Reading content of empty node");
        return *data;
	}

    void setLeft(UPtr<Node<T>> p) {
        left = std::move(p);
    }

    void setRight(UPtr<Node<T>> p) {
        right = std::move(p);
    }

    void setContent(UPtr<T> p) {
        data = std::move(p);
    }

    inline bool isLeaf() {
        return !hasLeft() && !hasRight();
    }

private:
	UPtr<T> data;
	UPtr<Node<T>> left;
	UPtr<Node<T>> right;
};

#endif __NODE_HPP__