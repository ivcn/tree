#include <node.hpp>

#include <array>
#include <iostream>

using namespace std;

struct BigStruct {
    BigStruct(int _a = 0, int _b = 0, int _c = 0) : a(_a), b(_b), c(_c) {}
    int a;
    int b;
    int c;
};

int main() {
    auto p = Node<BigStruct>::makeNode(1, 2, 3);
    auto bs1 = new BigStruct();
    auto pbs1 = new Node<BigStruct>(bs1);
    auto n1 = make_unique<Node<BigStruct>>(new BigStruct());
    auto n2 = make_unique<Node<BigStruct>>(new BigStruct());
    n2.reset(pbs1);
    n2 = std::move(p);
}