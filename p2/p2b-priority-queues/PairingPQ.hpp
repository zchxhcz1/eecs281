// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C42CF4E

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include <deque>
#include <utility>
#include <queue>
#include "Eecs281PQ.hpp"

// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
    public:
        // Description: Custom constructor that creates a node containing
        //              the given value.
        explicit Node(const TYPE &val)
            : elt { val } {}

        // Description: Allows access to the element at that Node's position.
        //              There are two versions, getElt() and a dereference
        //              operator, use whichever one seems more natural to you.
        // Runtime: O(1) - this has been provided for you.
        const TYPE &getElt() const { return elt; }
        const TYPE &operator*() const { return elt; }

        // The following line allows you to access any private data
        // members of this Node class from within the PairingPQ class.
        // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
        // function).
        friend PairingPQ;

    private:
        TYPE elt;
        Node *child = nullptr;
        Node *sibling = nullptr;
        Node *parent = nullptr;
        // TODO: Add and initialize one extra pointer (parent or previous) as desired.
    };  // Node


    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR())
        : BaseClass { comp }, root{nullptr}, count{0}{
        //root = nullptr;可能不需要
        //count = 0;
        // TODO: Implement this function.
    }  // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR())
        : BaseClass { comp }, root{nullptr}, count{0}{
        // TODO: Implement this function.
        for (auto it = start; it != end; ++it){
            PairingPQ::push(*it);
        }
    }  // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other)
        : BaseClass { other.compare }, root{nullptr}, count{0} {
        // TODO: Implement this function.
        // NOTE: The structure does not have to be identical to the original,
        //       but it must still be a valid pairing heap.
        if (other.empty()) return;
        std::queue<Node*> nodeQ;
        Node *current = other.root;
        nodeQ.push(current);
        while (!nodeQ.empty()){
            current = nodeQ.front();
            nodeQ.pop();
            if (current->sibling) nodeQ.push(current->sibling);
            if (current->child) nodeQ.push(current->child);
            PairingPQ::push(current->elt);
        }
    }  // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        // TODO: Implement this function.
        // HINT: Use the copy-swap method from the "Arrays and Containers"
        // lecture.
        PairingPQ temp(rhs);
        std::swap(temp.root, root);
        std::swap(temp.count, count);
        return *this;
    }  // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        // TODO: Implement this function.
        if (!root) return;
        std::queue<Node*> nodeQ;
        Node *current = root;
        nodeQ.push(current);
        while (!nodeQ.empty()){
            current = nodeQ.front();
            nodeQ.pop();
            if (current->sibling) nodeQ.push(current->sibling);
            if (current->child) nodeQ.push(current->child);
            delete current;
        }


    }  // ~PairingPQ()


    // Description: Move constructor and assignment operators don't need any
    //              code, the members will be reused automatically.
    PairingPQ(PairingPQ &&) noexcept = default;
    PairingPQ &operator=(PairingPQ &&) noexcept = default;


    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant.  You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.
        std::queue<Node*> nodeQ;
        Node *current = root;
        nodeQ.push(current);
        while (!nodeQ.empty()){
            current = nodeQ.front();
            nodeQ.pop();
            if (current->sibling) nodeQ.push(current->sibling);
            if (current->child) nodeQ.push(current->child);
            current->sibling = nullptr;
            current->parent = nullptr;
            current->child = nullptr;
            root = meld(root, current);
        }
    }  // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely
    //              in the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) { addNode(val); }  // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    // an element when the pairing heap is empty. Though you are welcome to
    // if you are familiar with them, you do not need to use exceptions in
    // this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        if (this->empty()) return;
        if (count == 1){
            delete root;
            root = nullptr;
            count--;
            return;
        }
        std::queue<Node*> nodeQ;
        Node *current = root->child;
        Node *temp;
        while(current){
            temp = current->sibling;
            current->sibling = nullptr;
            current->parent = nullptr;
            nodeQ.push(current);
            current = temp;
        }
        delete root;
        while (nodeQ.size() > 1){
            Node *first = nodeQ.front();
            nodeQ.pop();
            Node *second = nodeQ.front();
            nodeQ.pop();
            nodeQ.push(meld(first, second));
        }
        root = nodeQ.front();
        count--;
    }  // pop(),可能有问题，要把root变成null?


    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        // TODO: Implement this function

        return root->elt;  // TODO: Delete or change this line
    }  // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    [[nodiscard]] virtual std::size_t size() const {
        // TODO: Implement this function
        return this->count;  // TODO: Delete or change this line
    }  // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    [[nodiscard]] virtual bool empty() const {
        // TODO: Implement this function
        return (count == 0);  // TODO: Delete or change this line
    }  // empty()


    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value.  Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //              extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node *node, const TYPE &new_value) {
        // TODO: Implement this function
        node->elt = new_value;
        if (this->compare(new_value, node->parent->getElt())){
            return;
        }
        if (node ==  node->parent->child){
            node->parent->child = node->sibling;
            node->sibling = nullptr;
            node->parent = nullptr;
        } else {
            Node *current = node->parent->child;
            while (current->sibling != node){
                current = current->sibling;
            }
            current->sibling = node->sibling;
            node->sibling = nullptr;
            node->parent = nullptr;
        }
        root=meld(root, node);
    }  // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // Runtime: O(1)
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    Node *addNode(const TYPE &val) {
        // TODO: Implement this function
        Node *newnode = new Node(val);
        root = meld(newnode, root);
        count++;
        return newnode;  // TODO: Delete or change this line
    }  // addNode()


private:
    // TODO: Add any additional member variables or member functions you
    // require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap
    // papers).
    Node *meld(Node *a, Node *b) {
        if (!a) return b;
        if (!b) return a;
        if (a == b) return a;
        if (this->compare(a->getElt(), b->getElt())) {
            a->parent = b;
            if (b->child){
                a->sibling = b->child;
            }
            b->child = a;
            return b;
        } else {
            b->parent = a;
            if (a->child){
                b->sibling = a->child;
            }
            a->child = b;
            return a;
        } 
    }

    Node *root;
    std::size_t count;
    // NOTE: For member variables, you are only allowed to add a "root
    //       pointer" and a "count" of the number of nodes. Anything else
    //       (such as a deque) should be declared inside of member functions
    //       as needed.
};


#endif  // PAIRINGPQ_H
