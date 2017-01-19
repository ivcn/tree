#ifndef __TREE_HPP__
#define __TREE_HPP__

#include <node.hpp>

#include <functional>
#include <stack>
#include <type_traits>

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

    void traverse(TraverseType type, std::function<void(Type)> visit) {
        if (type == TraverseType::PreOrder) {
            preOrderTraversal(visit);
        }
        else if (type == TraverseType::InOrder) {
            inOrderTraversal(visit);
        }
        else { // type == TraverseType::PostOrder
            postOrderTraversal(visit);
        }
    }

    template<typename T>
    class is_serializable {
    private:
        template<typename U>
        static int test(...);

        template<typename U>
        static auto test(const U&) -> decltype(declval<U>().serialize(declval<std::ostream>()));

    public:
        static constexpr bool value = std::is_same<void, decltype(test(T()))>::value;
    };

    template<typename U = Type>
    typename std::enable_if<is_serializable<U>::value, void>::type
    serialize(std::ostream& stream)
    {
        serialize_impl(getRoot().get(), stream);
    }

    template<typename T>
    class is_deserializable {
    private:
        template<typename U>
        static int test(...);

        template<typename U>
        static auto test(const U&) ->
            decltype(declval<U>().deserialize(declval<std::istream>()));

    public:
        static constexpr bool value =
            std::is_same<void, decltype(test(T()))>::value;
    };

    template<typename U = Type>
    typename std::enable_if<is_deserializable<U>::value, void>::type
    deserialize(std::istream& stream)
    {
        deserialize_impl(std::move(getRoot()), stream);
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
        auto node = getRoot().get();
        auto lastVisitedNode = static_cast<Node<Type>*>(nullptr);
        while (!stack.empty() || node != nullptr) {
            if (node != nullptr) {
                stack.push(node);
                node = node->getLeft().get();
            }
            else {
                auto peekNode = stack.top();
                if (peekNode->getRight().get() != nullptr  && lastVisitedNode != peekNode->getRight().get())
                    node = peekNode->getRight().get();
                else {
                    visit(peekNode->getContent());
                    lastVisitedNode = stack.top();
                    stack.pop();
                }
            }
        }

    }

    void serialize_impl(Node<Type>* subroot, std::ostream& stream) {
        if (subroot == nullptr) {
            stream << "{ _NULL_ }";
            return;
        }
        stream << "{ ";
        subroot->getContent().serialize(stream);
        stream << " }";
        serialize_impl(subroot->getLeft().get(), stream);
        serialize_impl(subroot->getRight().get(), stream);
    }

    void deserialize_impl(UPtrNode<Type>& subroot, std::istream& stream) {
        int c;
        if ((c = stream.get()) != EOF) {
            if (c == '{') {
                stream.get(); //skip ws
                if (stream.peek() == '_') {
                    auto pos = stream.tellg();
                    if (
                        stream.get() == '_' &&
                        stream.get() == 'N' &&
                        stream.get() == 'U' &&
                        stream.get() == 'L' &&
                        stream.get() == 'L' &&
                        stream.get() == '_'
                        ) {
                        stream.get(); //skip ws
                        stream.get(); //skip right brace
                        subroot.reset();
                        return;
                    }
                    else {
                        stream.seekg(pos);
                    }
                }
                subroot = std::move(Node<Type>::makeNode());
                auto tmp = std::make_unique<Type>();
                tmp->deserialize(stream);
                stream.get(); //skip ws
                stream.get(); // skip right brace
                subroot->setContent(std::move(tmp));
                deserialize_impl(subroot->getLeft(), stream);
                deserialize_impl(subroot->getRight(), stream);
            }
            else {
                throw std::runtime_error("Deserialization failed");
            }
        }
    }

    UPtrNode<Type> root;
};
#endif __TREE_HPP__