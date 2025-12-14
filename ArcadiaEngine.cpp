// ArcadiaEngine.cpp - STUDENT TEMPLATE
// TODO: Implement all the functions below according to the assignment requirements

#include "ArcadiaEngine.h"
#include <algorithm>
#include <queue>
#include <numeric>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>

using namespace std;

// =========================================================
// PART A: DATA STRUCTURES (Concrete Implementations)
// =========================================================

// --- 1. PlayerTable (Double Hashing) ---

class ConcretePlayerTable : public PlayerTable {
private:
    // TODO: Define your data structures here
    // Hint: You'll need a hash table with double hashing collision resolution

public:
    ConcretePlayerTable() {
        // TODO: Initialize your hash table
    }

    void insert(int playerID, string name) override {
        // TODO: Implement double hashing insert
        // Remember to handle collisions using h1(key) + i * h2(key)
    }

    string search(int playerID) override {
        // TODO: Implement double hashing search
        // Return "" if player not found
        return "";
    }
};

// --- 2. Leaderboard (Skip List) ---

class ConcreteLeaderboard : public Leaderboard {
private:
    // TODO: Define your skip list node structure and necessary variables
    // Hint: You'll need nodes with multiple forward pointers
    struct Node{
        int ID;
        int score;
        int lvl;
        vector<Node*> next;

        Node(int id, int sc, int level) : ID(id), score(sc), lvl(level), next(level, nullptr) {}
    };

    static const int MAX = 16;
    static constexpr float P = 0.5f;

    Node* head;
    int currentLevel;

    int randomLevel(){
        int level = 1;
        while( ((float)rand() / RAND_MAX) < P && level < MAX){
            level++;
        }
        return level;
    }

    bool lessThan(Node* a , int id, int sc){
        if (a->score != sc){
            return a->score > sc;
        }
        return a->ID < id;
    }

    bool equalKey(Node* a, int id ,int sc){
        return a->score == sc && a->ID == id;
    }

    Node* findWithScore(int id, int sc, vector<Node*>& update){
        Node* x = head;
        for(int lvl = currentLevel -1; lvl >=0; lvl--){
            while (x->next[lvl] && lessThan(x->next[lvl], id, sc)){
                x = x->next[lvl];
            }
            update[lvl] = x;
        }
        x = x->next[0];
        if(x && equalKey(x,id,sc)) return x;
        return nullptr;
    }

    void deleteNode(Node* target){
        vector<Node*> update(MAX, nullptr);
        Node* x = head;
        int id = target->ID;
        int sc = target->score;
        for(int lvl = currentLevel - 1; lvl >=0; --lvl){
            while(x->next[lvl] && lessThan(x->next[lvl], id, sc)){
                x = x->next[lvl];
            }
            update[lvl] = x;
        }
        for(int lvl = 0; lvl < currentLevel; ++lvl){
            if( update[lvl]->next[lvl]==target){
                update[lvl]->next[lvl] = target->next[lvl];
            }
        }
        delete target;
        while( currentLevel > 1 && head->next[currentLevel- 1] == nullptr){
            currentLevel--;
        }
    }

public:
    ConcreteLeaderboard() {
        // TODO: Initialize your skip list
        head = new Node(-1,0, MAX);
        currentLevel = 1;
        srand(1);
    }

    void addScore(int playerID, int score) override {
        // TODO: Implement skip list insertion
        // Remember to maintain descending order by score
        Node* cur = head->next[0];
        Node* found = nullptr;
        while(cur){
            if(cur->ID == playerID){
                found = cur;
                break;
            }
            cur = cur->next[0];
        }
        if(found){
            deleteNode(found);
        }
        vector<Node*> update(MAX, nullptr);
        Node* x = head;
        for(int lvl = currentLevel - 1; lvl >=0; --lvl){
            while(x->next[lvl] && 
                (x->next[lvl]->score > score ||
                (x->next[lvl]->score == score && 
                x->next[lvl]->ID < playerID)))
            {
                x = x->next[lvl];
            }
            update[lvl] = x;
        }
        int lvl = randomLevel();
        if(lvl > currentLevel){
            for(int i = currentLevel; i < lvl; ++i){
                update[i] = head;
            }
            currentLevel = lvl;
        }
        Node* newNode = new Node(playerID, score, lvl);
        for(int i = 0; i < lvl; ++i){
            newNode->next[i] = update[i]->next[i];
            update[i]->next[i] = newNode;
        }
    }

    void removePlayer(int playerID) override {
        // TODO: Implement skip list deletion
        Node* cur = head->next[0];
        Node* target = nullptr;
        while(cur){
            if(cur->ID == playerID){
                target = cur;
                break;
            }
            cur = cur->next[0];
        }
        if (!target) return;
        deleteNode(target);
    }

    vector<int> getTopN(int n) override {
        // TODO: Return top N player IDs in descending score order
        vector<int> result;
        Node* cur = head->next[0];
        while(cur && (int)result.size() < n){
            result.push_back(cur->ID);
            cur = cur->next[0];
        }
        return result;
    }
};

// --- 3. AuctionTree (Red-Black Tree) ---
enum Color{
    Red, Black
};

struct Node {
    int ID;
    int price;
    Color color;

    Node* left;
    Node* right;
    Node* parent;

    Node(int id, int p)
        : ID(id),price(p),color(Red), left(nullptr), right(nullptr), parent(nullptr)
        {

        }
    
};
class ConcreteAuctionTree : public AuctionTree {
private:
    // TODO: Define your Red-Black Tree node structure
    // Hint: Each node needs: id, price, color, left, right, parent pointers
    Node* root;

    // helper functions
    void leftRotate(Node* x){
        Node* y = x->right;
        x->right = y->left;

        if(y->left)
            y->left->parent = x;

        y->parent = x->parent;

        if(!x->parent)
            root = y;
        else if(x == x->parent->left)
            x->parent->left = y;
        else 
            x->parent->right = y;

        y->left = x;
        x->parent = y;
        
    }
    void rightRotate(Node* x){
        Node* y = x->left;
        x->left = y->right;

        if(y->right)
            y->right->parent = x;

        y->parent = x->parent;
        
        if(!x->parent)
            root = y;
        else if( x == x->parent->right )
            x->parent->right = y;
        else
            x->parent->left = y;

        y->right = x;
        x->parent = y;

    }

    void insertFixUp(Node* z){
        while(z->parent != nullptr && z->parent->color ==Red){
            if(z->parent == z->parent->parent->left ){ // parent is left child
                Node* y = z->parent->parent->right; // uncle
                // Case 1 : z has a red uncle
                if(y != nullptr && y->color == Red){
                    z->parent->color = Black;
                    y->color = Black;
                    z->parent->parent->color = Red;
                    z = z->parent->parent;
                }else{
                    // Case 2 : z has a black uncle
                    // triangle left-right
                    if(z == z->parent->right ){
                        z = z->parent;
                        leftRotate(z);
                    }
                    // line case left-left
                    z->parent->color = Black;
                    z->parent->parent->color = Red;
                    rightRotate(z->parent->parent); 
                }     
            }else{ // parent is right child
                Node* y = z->parent->parent->left; // uncle
                if(y!=nullptr && y->color == Red){
                    // Case 1 
                    z->parent->color = Black;
                    y->color = Black;
                    z->parent->parent->color = Red;
                    z = z->parent->parent;
                }else{
                    // case 2
                    // triangle right-left
                    if( z == z->parent->left){
                        z = z->parent;
                        rightRotate(z);
                    }
                    // line case right-right
                    z->parent->color = Black;
                    z->parent->parent->color = Red;
                    leftRotate(z->parent->parent); 
                }
            }
        }
        root->color = Black;
        
    }
    void deleteFixUp(Node* x){
        while (x != root && x->color == Black)
        {
            // ---------- x is left child ----------
            if (x == x->parent->left) {

                Node* w = x->parent->right; // sibling

                // no NIL node
                if (w == nullptr) {
                    x = x->parent;
                    continue;
                }

                // Case 1: sibling is RED
                if (w->color == Red) {
                    w->color = Black;
                    x->parent->color = Red;
                    leftRotate(x->parent);
                    w = x->parent->right; // update sibling
                }

                // Case 2: sibling BLACK, both children BLACK
                if ((w->left == nullptr || w->left->color == Black) &&
                    (w->right == nullptr || w->right->color == Black)) {

                    w->color = Red;
                    x = x->parent;
                }
                else {
                    // Case 3: NEAR RED, FAR BLACK
                    if (w->right == nullptr || w->right->color == Black) {

                        if (w->left)
                            w->left->color = Black;

                        w->color = Red;
                        rightRotate(w);
                        w = x->parent->right;
                    }

                    // Case 4: FAR RED
                    w->color = x->parent->color;
                    x->parent->color = Black;

                    if (w->right)
                        w->right->color = Black;

                    leftRotate(x->parent);
                    x = root;
                }
            }

            // ---------- x is right child  ----------
            else {

                Node* w = x->parent->left; // sibling

                // no NIL node
                if (w == nullptr) {
                    x = x->parent;
                    continue;
                }

                // Case 1: sibling is RED
                if (w->color == Red) {
                    w->color = Black;
                    x->parent->color = Red;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }

                // Case 2: sibling BLACK, both children BLACK
                if ((w->right == nullptr || w->right->color == Black) &&
                    (w->left == nullptr || w->left->color == Black)) {

                    w->color = Red;
                    x = x->parent;
                }
                else {
                    // Case 3: NEAR RED, FAR BLACK
                    if (w->left == nullptr || w->left->color == Black) {

                        if (w->right)
                            w->right->color = Black;

                        w->color = Red;
                        leftRotate(w);
                        w = x->parent->left;
                    }

                    // Case 4: FAR RED
                    w->color = x->parent->color;
                    x->parent->color = Black;

                    if (w->left)
                        w->left->color = Black;

                    rightRotate(x->parent);
                    x = root;
                }
            }
        }

        // ensure x is BLACK
        x->color = Black;
    }


    Node* bstInsert(Node* root, Node* node){
        Node* y =nullptr; // parent
        Node* curr = root; // current

        while (curr) // search for right place
        {
            y = curr;
            if(node->ID < curr->ID) curr = curr->left;
            else curr = curr->right; 
        }

        node->parent = y;
        if(!y) this->root = node;
        else if(node->ID < y->ID) y->left = node;
        else y->right = node;

        return node;     
    }

    Node* findByID(Node* x, int itemID){
        if(!x) return nullptr;  
        if(x->ID == itemID) return x;

        Node* left = findByID(x->left, itemID);
        if(left) return left;

        return findByID(x->right,itemID);

    }

    Node* minimum(Node* x){
        while (x->left)
            x =x->left;
        return x;
         
    }
    void transplant(Node* u, Node* v){
        if(!u->parent)
            root = v;
        else if( u == u->parent->left)
            u->parent->left = v;
        else    
            u->parent->right = v;
        if(v)
            v->parent = u->parent;
    }

public:
    ConcreteAuctionTree() {
        // TODO: Initialize your Red-Black Tree
        root = nullptr;
    }

    void insertItem(int itemID, int price) override {
        // TODO: Implement Red-Black Tree insertion
        // Remember to maintain RB-Tree properties with rotations and recoloring
        Node* x = new Node (itemID,price);
        bstInsert(root,x);
        insertFixUp(x);

    }

    void deleteItem(int itemID) override {
        // TODO: Implement Red-Black Tree deletion
        // This is complex - handle all cases carefully
        Node* z = findByID(root,itemID); // fin the node to be deleted
        if(!z) return;

        Node*y = z;
        Node*x = nullptr;
        Color yOgColor = y->color;

        if(!z->left){ // only 1 child
            x = z->right;
            transplant(z,z->right);
        }else if(!z->right){ 
            x = z->left;
            transplant(z,z->left);
        }else{ // 2 childs
            y = minimum(z->right); // successor
            yOgColor = y->color;
            x = y->right;
            if(y->parent == z) {
                if(x) x->parent = y;
            }else{
                transplant(y,y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z,y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
            
        }
        delete z;

        if(yOgColor == Black && x) 
            deleteFixUp(x);
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================

int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {
    // TODO: Implement partition problem using DP
    // Goal: Minimize |sum(subset1) - sum(subset2)|
    // Hint: Use subset sum DP to find closest sum to total/2
    return 0;
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>>& items) {
    // TODO: Implement 0/1 Knapsack using DP
    // items = {weight, value} pairs
    // Return maximum value achievable within capacity
    return 0;
}

long long InventorySystem::countStringPossibilities(string s) {
    // TODO: Implement string decoding DP
    // Rules: "uu" can be decoded as "w" or "uu"
    //        "nn" can be decoded as "m" or "nn"
    // Count total possible decodings
    return 0;
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool WorldNavigator::pathExists(int n, vector<vector<int>>& edges, int source, int dest) {
    // TODO: Implement path existence check using BFS or DFS
    // edges are bidirectional
    return false;
}

long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                                       vector<vector<int>>& roadData) {
    // TODO: Implement Minimum Spanning Tree (Kruskal's or Prim's)
    // roadData[i] = {u, v, goldCost, silverCost}
    // Total cost = goldCost * goldRate + silverCost * silverRate
    // Return -1 if graph cannot be fully connected
    return -1;
}

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>>& roads) {
    // TODO: Implement All-Pairs Shortest Path (Floyd-Warshall)
    // Sum all shortest distances between unique pairs (i < j)
    // Return the sum as a binary string
    // Hint: Handle large numbers carefully
    return "0";
}

// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================

int ServerKernel::minIntervals(vector<char>& tasks, int n) {
    // TODO: Implement task scheduler with cooling time
    // Same task must wait 'n' intervals before running again
    // Return minimum total intervals needed (including idle time)
    // Hint: Use greedy approach with frequency counting
    return 0;
}

// =========================================================
// FACTORY FUNCTIONS (Required for Testing)
// =========================================================

extern "C" {
    PlayerTable* createPlayerTable() { 
        return new ConcretePlayerTable(); 
    }

    Leaderboard* createLeaderboard() { 
        return new ConcreteLeaderboard(); 
    }

    AuctionTree* createAuctionTree() { 
        return new ConcreteAuctionTree(); 
    }
}