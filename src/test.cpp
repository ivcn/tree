#include <node.hpp>
#include <tree.hpp>

#include <array>
#include <sstream>

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

    void serialize(std::ostream& stream) const {
        stream << this->a << " "
            << this->b << " "
            << this->c << " "
            << this->d;
    }

    void deserialize(std::istream& stream) {
        stream >> this->a;
        stream >> this->b;
        stream >> this->c;
        stream >> this->d;
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

TEST_CASE("Insertion to empty tree is working", "[Tree]") {
    Tree<SomeClass> t;
    REQUIRE(t.empty());
    auto sc = SomeClass(3, 3, 3.0, "qwe");
    t.insert(sc);
    REQUIRE(t.getRoot()->getContent() == sc);
    t.remove(sc);
    REQUIRE(t.empty());
}

TEST_CASE("Insertion and removing are working", "[Tree::insert, Tree::remove]") {
    Tree<SomeClass> t;
    auto sc3 = SomeClass(3, 3, 3.0, "qwe");
    auto sc2 = SomeClass(2, 3, 3.0, "qwe");
    auto sc4 = SomeClass(4, 3, 3.0, "qwe");
    auto sc5 = SomeClass(5, 0, 5.5, "asd");
    t.insert(sc3);
    t.insert(sc2);
    REQUIRE(!t.getRoot()->isLeaf());
    REQUIRE(t.getRoot()->hasLeft());
    REQUIRE(!t.getRoot()->hasRight());
    REQUIRE(t.getRoot()->getLeft()->getContent().a == 2);
    t.insert(sc4);
    REQUIRE(t.getRoot()->hasRight());
    REQUIRE(t.getRoot()->getRight()->isLeaf());
    REQUIRE(t.getRoot()->getRight()->getContent().a == 4);
    t.insert(sc5);
    REQUIRE(!t.getRoot()->getRight()->isLeaf());
    REQUIRE(t.getRoot()->getRight()->hasRight());
    REQUIRE(t.getRoot()->getRight()->getRight()->getContent().a == 5);
    /*
            3
           / \
          2   4
               \
                5
    */
    t.remove(sc3);
    REQUIRE(t.getRoot()->getContent().a == 2);
    REQUIRE(!t.getRoot()->hasLeft());
    REQUIRE(t.getRoot()->hasRight());
    REQUIRE(t.getRoot()->getRight()->getContent().a == 4);
    REQUIRE(t.getRoot()->getRight()->getRight()->getContent().a == 5);
    REQUIRE(t.getRoot()->getRight()->getRight()->isLeaf());

    /*      2
             \
              4
               \
                5
    */
}

TEST_CASE("Test tree traversals", "[Tree::traverse]") {
    Tree<SomeClass> t;
    for (int i = 0; i < 5; i++) {
        t.insert(SomeClass(i));
    }
    /*
                0
                 \
                  1
                   \
                    3
                     \
                      4
    */
    std::stringstream str;
    t.traverse(Tree<SomeClass>::TraverseType::PreOrder, [&str](SomeClass sc) {str << sc.a;});
    REQUIRE(str.str() == "01234");
    str.str("");
    str.clear();
    t.traverse(Tree<SomeClass>::TraverseType::InOrder, [&str](SomeClass sc) {str << sc.a;});
    REQUIRE(str.str() == "01234");
    str.str("");
    str.clear();
    t.traverse(Tree<SomeClass>::TraverseType::PostOrder, [&str](SomeClass sc) {str << sc.a;});
    REQUIRE(str.str() == "43210");
    str.str("");
    str.clear();

    Tree<SomeClass> t2;
    std::array<int,15> numbers = {8,4,12,2,6,10,14,1,3,5,7,9,11,13,15};
    for (auto n : numbers) {
        t2.insert(n);
    }
    /*
                 _____8_____
                /           \
               4            12
             /   \        /    \
            2     6     10      14
           / \   / \   /  \    /  \
          1   3 5   7 9   11 13   15
    */
    t2.traverse(Tree<SomeClass>::TraverseType::PreOrder, [&str](SomeClass sc) {str << "(" << sc.a << ")"; });
    REQUIRE(str.str() == "(8)(4)(2)(1)(3)(6)(5)(7)(12)(10)(9)(11)(14)(13)(15)");
    str.str("");
    str.clear();
    t2.traverse(Tree<SomeClass>::TraverseType::InOrder, [&str](SomeClass sc) {str << "(" << sc.a << ")"; });
    REQUIRE(str.str() == "(1)(2)(3)(4)(5)(6)(7)(8)(9)(10)(11)(12)(13)(14)(15)");
    str.str("");
    str.clear();
    t2.traverse(Tree<SomeClass>::TraverseType::PostOrder, [&str](SomeClass sc) {str << "(" << sc.a << ")"; });
    REQUIRE(str.str() == "(1)(3)(2)(5)(7)(6)(4)(9)(11)(10)(13)(15)(14)(12)(8)");
}

TEST_CASE("Testing serialization/deserialization", "[Tree::serialize, Tree::deserialize]") {
    Tree<SomeClass> t;
    std::array<int, 15> numbers = { 8,4,12,2,6,10,14,1,3,5,7,9,11,13,15 };
    for (auto n : numbers) {
        t.insert(n);
    }

    std::ostringstream sout;
    t.serialize(sout);

    Tree<SomeClass> t2;
    std::istringstream sin(sout.str());
    t2.deserialize(sin);

    std::stringstream str;
    t2.traverse(Tree<SomeClass>::TraverseType::PreOrder, [&str](SomeClass sc) {str << "(" << sc.a << ")"; });
    REQUIRE(str.str() == "(8)(4)(2)(1)(3)(6)(5)(7)(12)(10)(9)(11)(14)(13)(15)");
}