#include <iostream>
#include <unordered_map>
#include <cmath>

using namespace std;

struct node{
    int key;
    int degree;
    node* parent,* child;
    node* left,* right;
    bool marked;
};

class FibonacciHeap{
public:
    node* mini;
    int noNodes;

    FibonacciHeap();
    ~FibonacciHeap() {
        _clear(mini);
    }
    void insertNode(int newKey);
    void merge(FibonacciHeap& heap2);
    int  extractMin();
    void Display();

private:
    node* _createNode(int newKey);
    void _insertNode(node* nod);
    node* _merge(node* a, node* b);
//    removing a node from the circular list he s in
    void _removeFromCircularList(node* nod);
    void _makeChild(node *child, node *parent);
    void _consolidate();
    void _unparentAll(node* nod);
    node* _extractMin_node();
    void _clear(node* nod);

};

FibonacciHeap::FibonacciHeap() {
    mini = nullptr;
    noNodes = 0;
}



void FibonacciHeap::insertNode(int newKey) {
    node* nod = _createNode(newKey);
    _insertNode(nod);

}

//    union
void FibonacciHeap::merge(FibonacciHeap &heap2) {
    mini = _merge(mini, heap2.mini);
    noNodes += heap2.noNodes;
    heap2.mini = nullptr;
    heap2.noNodes = 0;

}

int FibonacciHeap::extractMin() {
    node* minNode = _extractMin_node();
    int ret = minNode->key;
    delete minNode;
    return ret;
}

node *FibonacciHeap::_createNode(int newKey) {

    node *nod = new node;
    nod->key = newKey;
    nod->child = nod->parent = nullptr;
    nod->degree = 0;
    nod->marked = false;
    nod->left = nod->right = nod;   // we assume is the only one one the level

    return nod;
}

void FibonacciHeap::_insertNode(node *nod) {
    mini = _merge(mini, nod);
    noNodes++;
}


//removing nod from the circular list he s in
void FibonacciHeap::_removeFromCircularList(node *nod) {
    if(nod->right == nod){
        // the list only has one node
        return;
    }

    node* leftNode;
    node* rightNode;

    leftNode = nod->left;
    rightNode = nod->right;

    leftNode->right = rightNode;
    rightNode->left = leftNode;
}


node *FibonacciHeap::_merge(node *a, node *b) {
    if(a == nullptr){
        return b;
    }
    if(b == nullptr){
        return a;
    }

//    we swap the nodes so a is smaller
    if(b->key < a->key){
        node* aux = a;
        a = b;
        b = aux;
    }
//           <- ->
//    a->left  a ->right => aRight
//    bLeft <= left<-b  b->right

    node* aRight = a->right;
    node* bLeft = b->left;

//    b will be to the right of a
    a->right = b;
    b->left = a;

//    a->right => b
//    a <= left<-b

//    what was to the right of a, now will be to the right of b
//    what was to the left of b, now will be to the left of a
    aRight->left = bLeft;
    bLeft->right = aRight;

    return a;
}

node *FibonacciHeap::_extractMin_node() {
    node* min = mini;

    if(min != nullptr){
//        the heap is not empty
//        unlink the children of the mini node
        _unparentAll(min->child);
//        merge the children (child circular list) to the root list
        _merge(min, min->child);
        _removeFromCircularList(min);

        if(min == min->right){
//            only one node in the heap -> the heap will be empty after extraction
            mini = nullptr;
        }
        else{
            mini = min->right;  // change the minimum
            _consolidate();
        }
        noNodes--;
    }

    return min;
}

void FibonacciHeap::_unparentAll(node *nod) {
    if(nod == nullptr){
//        the node doesn't exist
        return;
    }
    node* aux = nod;

    aux->parent = nullptr;
    aux = aux->right;

    while(aux != nod){

        aux->parent = nullptr;
        aux = aux->right;
    }
}


void FibonacciHeap::_consolidate() {
    int degree = (log(noNodes)) / log(2);

//    vector of pointers  to the different degree heaps
//    node* d[degree + 1] = {nullptr};
    node* d[degree + 1];

    for(int i = 0; i <= degree; i++){
        d[i] = nullptr;
    }

    d[mini->degree] = mini;

//    we check all the nodes in the root list for their degree
//    when two nodes have the same degree
//    we link them (parent-child ) relationship

    bool done = false;
    node* aux = mini;

    while(true){
        int deg = aux->degree;
        while(d[deg] != nullptr){
            node* otherNode = d[deg];

            if(aux == otherNode){
//                this means we checked all nodes and all root nodes have
//                different degrees
                done = true;
                break;
            }
            _makeChild(otherNode, aux);
//            by linking the 2 subtrees we create a new one with a bigger degree
//            and their old degree doesn't exist anymore in the root list
            d[deg] = nullptr;
            deg++;
        }
        if(done){
            break;
        }
        d[aux->degree] = aux;
        aux = aux->right;
    }
    mini = aux;
//    we need to update the min

    for(node* i = aux->right; i != aux; i = i->right){
        if(i->key < mini->key) {
            mini = i;
        }
    }

}

void FibonacciHeap::_makeChild(node *child, node *parent) {
//    linking tree 1 to tree 2

    node* aux;

//    making it so tree1 has a smaller value than tree2
    if(child->key > parent->key){
        aux = child;
        child = parent;
        parent = aux;
    }

    _removeFromCircularList(child);

//    we put it alone in a list
    child->left = child->right = child;

    child->parent = parent;
//    we add the child into the parent's list of children by merging them
    parent->child = _merge(parent->child, child);

    parent->degree++;
    child->marked = false;
}


void FibonacciHeap::_clear(node *nod) {
    if(nod){
        node* aux = nod;

        do{
//            we delete all the children, one by one
            node* aux1 = aux;
            aux = aux->right;
            _clear(aux1->child);
            delete aux1;
        }while(aux != nod);
    }
}

void FibonacciHeap::Display() {
    node* aux = mini;

    if(aux == nullptr){
        cout << "\nThe heap is empty\n";
        return;
    }
    cout << "The root nodes of the heap are: \n";
    do{
        cout << aux->key << " " ;
        aux = aux->right;
        if(aux != mini){
            cout << "-->";
        }

    } while (aux != mini && aux != nullptr);

    cout <<"\n";

}


//node *FibonacciHeap::link(node *tree1, node *tree2) {
////    linking tree 1 to tree 2
//
//    node* aux;
//
////    making it so tree1 has a smaller value than tree2
//    if(tree1->key > tree2->key){
//        aux = tree1;
//        tree1 = tree2;
//        tree2 = aux;
//    }
//
////    we cut off tree 2
//    (tree2->right)->left = tree2->left;
//    (tree2->left)->right = tree2->right;
//
////    we make tree 1 parent of tree 2
//    tree2->parent = tree1;
//
////    and tree 2 child of tree 1
////    if tree 1 has no child
//    if(tree1->child == nullptr){
//        tree1->child = tree2;
//        tree2->left = tree2->right = tree2;
//    }
//    else{
////        if tree 1 already has children
////        we add tree 2 to the list of children
//        node* nod;
//        nod = tree1->child;
////        to the right of the 'main' child
//        (nod->right)->left = tree2;
//        nod->right = tree2;
//
//        tree2->left = nod;
//        tree2->right = nod->right;
//
//    }
////    incrementing the degree
//    tree1->degree++;
//    return tree1;
//
//}


int main() {
    FibonacciHeap h1, h2;

    int task;

    while(true){

        cout << "1.Insert a node to one of the heaps\n";
        cout << "2.Print the minimum value from one of the heaps\n";
        cout << "3.Print and delete the minimum value\n";
        cout << "4.Merge the two heaps\n";
        cout << "5.Display the values in one of the heaps\n";
        cout << "9.Exit\n";

        cin >> task;

        if(task == 1){
            cout << "1 - Insert in heap1 , 2 - Insert in heap2\n";
            int h;
            cin >> h;

            int key;
            cout << "Insert value:";
            cin >> key;

            if(h == 1){
                h1.insertNode(key);
            } else{
                h2.insertNode(key);
            }
        }
        else if(task == 2){
            cout << "1 - Minimum in heap1 , 2 - Minimum in heap2\n";
            int h;
            cin >> h;

            if(h == 1){
                cout << "\n Minimum in heap 1 is: " << h1.mini->key << "\n";
            } else{
                cout << "\n Minimum in heap 2 is: " << h2.mini->key << "\n";
            }
        }else if(task == 3){
            cout << "1 - Minimum in heap1 , 2 - Minimum in heap2\n";
            int h;
            cin >> h;

            if(h == 1){
                cout << "\n Minimum in heap 1 is: " << h1.extractMin() << " (deleted)\n";
            } else{
                cout << "\n Minimum in heap 2 is: " << h2.extractMin() << " (deleted)\n";
            }
        }else if(task == 4)
                    h1.merge(h2);
        else if(task == 5) {
            cout << "1 - Display heap1 , 2 - Display heap2\n";
            int h;
            cin >> h;

            if(h == 1){
                h1.Display();
            } else{
                h2.Display();
            }
        }
        else if(task == 9){}
            break;

    }

    return 0;
}
