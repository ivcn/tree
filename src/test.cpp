#include <node.hpp>
#include <tree.hpp>

#include <array>
#include <cassert>
#include <iostream>

using namespace std;

struct BigStruct {
    BigStruct(int _a = 0, int _b = 0, int _c = 0, int _d = 0) : 
        a(_a), b(_b), c(_c), d(_d) 
    { }

    int a;
    int b;
    int c;
    int d;
};

int main() {
    auto p = Node<BigStruct>::makeNode(1, 2, 3, 4);
    assert(p->getContent()->a == 1);
    assert(p->getContent()->b == 2);
    assert(p->getContent()->c == 3);
    assert(p->getContent()->d == 4);
    
    auto bs1 = new BigStruct();
    auto pbs1 = new Node<BigStruct>(bs1);
    assert(pbs1->getContent().get() == bs1);

    auto n1 = make_unique<Node<BigStruct>>(new BigStruct());
    auto n2 = make_unique<Node<BigStruct>>(new BigStruct());
    n2.reset(pbs1);
    assert(n2->getContent().get() == pbs1->getContent().get());
    assert(n2->getLeft().get() == pbs1->getLeft().get());
    assert(n2->getRight().get() == pbs1->getRight().get());
    n2 = std::move(n1);
    assert(n2->getContent()->a == 0);
    assert(n2->getContent()->b == 0);
    assert(n2->getContent()->c == 0);
    assert(n2->getContent()->d == 0);
    assert(!n1);

    Tree<BigStruct> t;
    assert(t.getRoot() == nullptr);
    auto node = Node<BigStruct>::makeNode(3, 3, 3, 3);
    t.addNode(std::move(node));
    assert(t.getRoot()->getContent()->a == 3);
    assert(t.getRoot()->getContent()->b == 3);
    assert(t.getRoot()->getContent()->c == 3);
    assert(t.getRoot()->getContent()->d == 3);
}