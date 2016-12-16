#include <node.hpp>
#include <tree.hpp>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace std;

struct SomeClass {
    SomeClass(int _a = 0, int _b = 0, double _c = 0.0, const std::string& _d = "nothing") :
        a(_a), b(_b), c(_c), d(_d)
    { }

    bool operator==(const SomeClass& that) const {
        return this->a == that.a &&
            this->b == that.b &&
            this->c == that.c &&
            this->d == that.d;
    }

    bool operator<= (const SomeClass& that) const {
        return this->a <= that.a;
    }

    int a;
    int b;
    double c;
    std::string d;

};

TEST_CASE("Constructing and moving copying of class Node are working correctly", "[Node]") {
    REQUIRE(Node<SomeClass>().isEmpty() == true);

    Node<SomeClass> node(new SomeClass());
    REQUIRE(node.getContent() == SomeClass());
    REQUIRE(node.getLeft() == nullptr);
    REQUIRE(node.getRight() == nullptr);

    auto pSC = new SomeClass(1, 2, 3, "4");
    std::unique_ptr<SomeClass> upSC(pSC);
    Node<SomeClass> node2(upSC);
    REQUIRE(*pSC == node2.getContent());
    REQUIRE(node2.getLeft() == nullptr);
    REQUIRE(node2.getRight() == nullptr);

    auto p = new SomeClass(1, 1, 1, "1");
    Node<SomeClass> n1(p);
    Node<SomeClass> n2(std::move(n1));
    REQUIRE(n1.isEmpty() == true);
    REQUIRE(n1.getLeft() == nullptr);
    REQUIRE(n1.getRight() == nullptr);
    REQUIRE(*p == n2.getContent());
    REQUIRE(n2.getLeft() == nullptr);
    REQUIRE(n2.getRight() == nullptr);
    
    Node<SomeClass> n3;
    n3 = std::move(n2);
    REQUIRE(n2.isEmpty() == true);
    REQUIRE(n2.getLeft() == nullptr);
    REQUIRE(n2.getRight() == nullptr);
    REQUIRE(*p == n3.getContent());
    REQUIRE(n3.getLeft() == nullptr);
    REQUIRE(n3.getRight() == nullptr);
}

TEST_CASE("Node<T>::makeNode is correctly creates objects", "[Node<T>::makeNode]") {
    auto upn = Node<SomeClass>::makeNode(1, 2, 3, "abc");
    REQUIRE(upn->getContent().a == 1);
    REQUIRE(upn->getContent().b == 2);
    REQUIRE(upn->getContent().c == 3);
    REQUIRE(upn->getContent().d == "abc");

    auto upn2 = Node<SomeClass>::makeNode(1, 2);
    REQUIRE(upn2->getContent().a == 1);
    REQUIRE(upn2->getContent().b == 2);
    REQUIRE(upn2->getContent().c == 0);
    REQUIRE(upn2->getContent().d == "nothing");

    auto upn3 = Node<SomeClass>::makeNode();
    REQUIRE(upn3->getContent().a == 0);
    REQUIRE(upn3->getContent().b == 0);
    REQUIRE(upn3->getContent().c == 0);
    REQUIRE(upn3->getContent().d == "nothing");
}

TEST_CASE("Cons of Tree class are working correctly", "Tree()") {
    Tree<SomeClass> t;
    REQUIRE(t.getRoot() == nullptr);
    auto sc = SomeClass(3, 3, 3.0, "qwe");
    t.insert(sc);
    REQUIRE(t.getRoot()->getContent() == sc);
    t.remove(sc);
    REQUIRE(t.getRoot() == nullptr);
}