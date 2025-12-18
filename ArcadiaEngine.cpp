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
    static const int TABLE_SIZE = 101;

    struct Entry {
        int playerID;
        string name;
        bool occupied;

        Entry() : playerID(0), name(""), occupied(false) {}
    };

    vector<Entry> table;

    int hash1(int key) {
        return key % TABLE_SIZE;
    }

    int hash2(int key) {
        return 1 + (key % (TABLE_SIZE - 1));
    }

public:
    ConcretePlayerTable() {
        table.resize(TABLE_SIZE);
    }

    void insert(int playerID, string name) override {
        int index1 = hash1(playerID);
        int step   = hash2(playerID);

        for (int i = 0; i < TABLE_SIZE; i++) {
            int index = (index1 + i * step) % TABLE_SIZE;

            if (table[index].occupied && table[index].playerID == playerID) {
                table[index].name = name; // UPDATE
                return;
            }

            if (!table[index].occupied) {
                table[index].playerID = playerID;
                table[index].name = name;
                table[index].occupied = true;
                return;
            }

        }

        cout << "Table is Full" << endl;

    }

    string search(int playerID) override {
        int index1 = hash1(playerID);
        int step   = hash2(playerID);

        for (int i = 0; i < TABLE_SIZE; i++) {
            int index = (index1 + i * step) % TABLE_SIZE;

            if (!table[index].occupied)
                return "";

            if (table[index].playerID == playerID)
                return table[index].name;
        }
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

public:
    ConcreteLeaderboard() {
        // TODO: Initialize your skip list
        head = new Node(-1,INT_MAX, MAX);
        currentLevel = 1;
        srand(1);
    }
    ~ConcreteLeaderboard() {
        Node* cur = head;
        while (cur != nullptr) {
            Node* next = cur->next[0];
            delete cur;
            cur = next;
        }
    }

    void addScore(int playerID, int score) override {
        // TODO: Implement skip list insertion
        // Remember to maintain descending order by score
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
        vector<Node*> update(MAX, nullptr);
        Node* x = head;

        for(int lvl = currentLevel - 1; lvl >=0; --lvl){
            while(x->next[lvl] && 
                (x->next[lvl]->score > target->score ||
                (x->next[lvl]->score == target->score && x->next[lvl]->ID < target->ID)))
                {
                    x = x->next[lvl];
                }
            update[lvl] = x;
        }
        for(int lvl = 0; lvl < currentLevel; ++lvl){
            if(update[lvl]->next[lvl] == target){
                update[lvl]->next[lvl] = target->next[lvl];
            }
        }
        delete target;
        while(currentLevel > 1 && head->next[currentLevel - 1] == nullptr){
            currentLevel--;
        }
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

    void deleteSubtree(Node* node) {
        if (!node) return;
        deleteSubtree(node->left);
        deleteSubtree(node->right);
        delete node;
    }
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

    ~ConcreteAuctionTree() {
        deleteSubtree(root);
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
    int totalSum = 0;
    for (int c : coins)
        totalSum += c;

    int target = totalSum / 2;

    vector<bool> dp(target + 1, false);
    dp[0] = true;

    for (int coin : coins) {
        for (int j = target; j >= coin; j--) {
            dp[j] = dp[j] || dp[j - coin];
        }
    }

    int best = 0;
    for (int j = target; j >= 0; j--) {
        if (dp[j]) {
            best = j;
            break;
        }
    }

    return abs(totalSum - 2 * best);
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>>& items) {
    // TODO: Implement 0/1 Knapsack using DP
    // items = {weight, value} pairs
    // Return maximum value achievable within capacity

    int n = items.size();
    vector<vector<int>> dp(n + 1, vector<int>(capacity + 1, 0));
    for (int i = 1; i <= n; i++) {
        int weight = items[i - 1].first;
        int value = items[i - 1].second;

        for (int w = 1; w <= capacity; w++) {
            if (weight <= w) {
                dp[i][w] = max(dp[i - 1][w], dp[i - 1][w - weight] + value);
            }
            else {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }
        return dp[n][capacity];
}

long long InventorySystem::countStringPossibilities(string s) {
    // TODO: Implement string decoding DP
    // Rules: "uu" can be decoded as "w" or "uu"
    //        "nn" can be decoded as "m" or "nn"
    // Count total possible decodings
    if (s.empty()) return 1;

    for (char c : s) {
        if (c == 'w' || c == 'm')
            return 0;
    }

    const long long MOD = 1000000007;
    int n = s.size();

    vector<long long> dp(n + 1, 0);
    dp[n] = 1; // base case

    for (int i = n - 1; i >= 0; i--) {
        // take single character
        dp[i] = dp[i + 1];

        // take pair if valid
        if (i + 1 < n) {
            if ((s[i] == 'u' && s[i + 1] == 'u') ||
                (s[i] == 'n' && s[i + 1] == 'n')) {
                dp[i] = (dp[i] + dp[i + 2]) % MOD;
            }
        }
    }

    return dp[0];
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool WorldNavigator::pathExists(int n, vector<vector<int>>& edges, int source, int dest) {
    if (source < 0 || dest < 0 || source >= n || dest >= n)
        return false;

    if (source == dest)
        return true;

    vector<vector<int>> adj(n);
    for (auto& e : edges) {
        int u = e[0];
        int v = e[1];
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    vector<bool> visited(n, false);
    queue<int> q;
    q.push(source);
    visited[source] = true;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v : adj[u]) {
            if (!visited[v]) {
                if (v == dest) return true;
                visited[v] = true;
                q.push(v);
            }
        }
    }
    
    return false;
}

long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,vector<vector<int>>& roadData) {
    // TODO: Implement Minimum Spanning Tree (Kruskal's or Prim's)
    // roadData[i] = {u, v, goldCost, silverCost}
    // Total cost = goldCost * goldRate + silverCost * silverRate
    // Return -1 if graph cannot be fully connected
    vector<tuple<long long, int, int>> edges;
    for (auto& r : roadData) {
        long long cost = r[2] * goldRate + r[3] * silverRate;
        edges.push_back({cost, r[0], r[1]});
    }
    sort(edges.begin(), edges.end());
    vector<int> parent(n), rank(n, 0);
    for (int i = 0; i < n; i++) parent[i] = i;
    auto find = [&](int x) {
        while (parent[x] != x) {
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
    };
    auto unite = [&](int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (rank[a] < rank[b]) swap(a, b);
        parent[b] = a;
        if (rank[a] == rank[b]) rank[a]++;
        return true;
    };
    long long total = 0;
    int used = 0;
    for (auto& [cost, u, v] : edges) {
        if (unite(u, v)) {
            total += cost;
            used++;
            if (used == n - 1) break;
        }
    }
    return (used == n - 1) ? total : -1;
}

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>>& roads) {
    // TODO: Implement All-Pairs Shortest Path (Floyd-Warshall)
    // Sum all shortest distances between unique pairs (i < j)
    // Return the sum as a binary string
    // Hint: Handle large numbers carefully
    const long long INF = LLONG_MAX / 4;
    // distance matrix
    vector<vector<long long>> dist(n, vector<long long>(n, INF));
    // distance to self = 0
    for (int i = 0; i < n; i++)
        dist[i][i] = 0;
    // initialize roads
    for (auto& r : roads) {
        int u = r[0];
        int v = r[1];
        long long w = 1LL << r[2];
        dist[u][v] = min(dist[u][v], w);
        dist[v][u] = min(dist[v][u], w);
    }
    // Floyd-Warshall
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            if (dist[i][k] == INF) continue;
            for (int j = 0; j < n; j++) {
                if (dist[k][j] == INF) continue;
                dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
            }
        }
    }
    // sum all i < j
    long long sum = 0;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (dist[i][j] != INF)
                sum += dist[i][j];
        }
    }
    // convert sum to binary
    if (sum == 0) return "0";
    string binary = "";
    while (sum > 0) {
        binary.push_back((sum & 1) + '0');
        sum >>= 1;
    }
    reverse(binary.begin(), binary.end());
    return binary;
}

// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================

int ServerKernel::minIntervals(vector<char>& tasks, int n) {
    // TODO: Implement task scheduler with cooling time
    // Same task must wait 'n' intervals before running again
    // Return minimum total intervals needed (including idle time)
    // Hint: Use greedy approach with frequency counting
    
    // freq for each task

    vector<int> freq(26,0);
    for (char c : tasks){
        freq[c-'A']++;
    }

    // max freq
    int maxFreq = 0;
    for(int f : freq){
        maxFreq = max(maxFreq,f);
    }

    // how many tasks have maxfreq
    int maxCount = 0;
    for(int f : freq){
        if( f == maxFreq)
            maxCount++;
    }

    // greedy formla
    int intervals = max((int)(tasks.size()),((maxFreq - 1) * (n + 1) + maxCount));

    return intervals;


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

/**
 * main_test_student.cpp
 * Basic "Happy Path" Test Suite for ArcadiaEngine
 * Use this to verify your basic logic against the assignment examples.
 */

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <functional>
#include "ArcadiaEngine.h" 

using namespace std;

// ==========================================
// FACTORY FUNCTIONS (LINKING)
// ==========================================
// These link to the functions at the bottom of your .cpp file
extern "C" {
    PlayerTable* createPlayerTable();
    Leaderboard* createLeaderboard();
    AuctionTree* createAuctionTree();
}

// ==========================================
// TEST UTILITIES
// ==========================================
class StudentTestRunner {
	int count = 0;
    int passed = 0;
    int failed = 0;

public:
    void runTest(string testName, bool condition) {
		count++;
        cout << "TEST: " << left << setw(50) << testName;
        if (condition) {
            cout << "[ PASS ]";
            passed++;
        } else {
            cout << "[ FAIL ]";
            failed++;
        }
        cout << endl;
    }

    void printSummary() {
        cout << "\n==========================================" << endl;
        cout << "SUMMARY: Passed: " << passed << " | Failed: " << failed << endl;
        cout << "==========================================" << endl;
		cout << "TOTAL TESTS: " << count << endl;
        if (failed == 0) {
            cout << "Great job! All basic scenarios passed." << endl;
            cout << "Now make sure to handle edge cases (empty inputs, collisions, etc.)!" << endl;
        } else {
            cout << "Some basic tests failed. Check your logic against the PDF examples." << endl;
        }
    }
};

StudentTestRunner runner;

// ==========================================
// PART A: DATA STRUCTURES
// ==========================================

void test_PartA_DataStructures() {
    cout << "\n--- Part A: Data Structures ---" << endl;

    // 1. PlayerTable (Double Hashing)
    // Requirement: Basic Insert and Search
    PlayerTable* table = createPlayerTable();
    runner.runTest("PlayerTable: Insert 'Alice' and Search", [&]() {
        table->insert(101, "Alice");
        return table->search(101) == "Alice";
    }());
    delete table;

    // 2. Leaderboard (Skip List)
    Leaderboard* board = createLeaderboard();

    // Test A: Basic High Score
    runner.runTest("Leaderboard: Add Scores & Get Top 1", [&]() {
        board->addScore(1, 100);
        board->addScore(2, 200); // 2 is Leader
        vector<int> top = board->getTopN(1);
        return (!top.empty() && top[0] == 2);
    }());

    // Test B: Tie-Breaking Visual Example (Crucial!)
    // PDF Visual Example: Player A (ID 10) 500pts, Player B (ID 20) 500pts.
    // Correct Order: ID 10 then ID 20.
    runner.runTest("Leaderboard: Tie-Break (ID 10 before ID 20)", [&]() {
        board->addScore(10, 500);
        board->addScore(20, 500);
        vector<int> top = board->getTopN(2);
        // We expect {10, 20} NOT {20, 10}
        if (top.size() < 2) return false;
        return (top[0] == 10 && top[1] == 20); 
    }());
    
    delete board;

    // 3. AuctionTree (Red-Black Tree)
    // Requirement: Insert items without crashing
    AuctionTree* tree = createAuctionTree();
    runner.runTest("AuctionTree: Insert Items", [&]() {
        tree->insertItem(1, 100);
        tree->insertItem(2, 50);
        return true; // Pass if no crash
    }());
    delete tree;
}

// ==========================================
// PART B: INVENTORY SYSTEM
// ==========================================

void test_PartB_Inventory() {
    cout << "\n--- Part B: Inventory System ---" << endl;

    // 1. Loot Splitting (Partition)
    // PDF Example: coins = {1, 2, 4} -> Best split {4} vs {1,2} -> Diff 1
    runner.runTest("LootSplit: {1, 2, 4} -> Diff 1", [&]() {
        vector<int> coins = {1, 2, 4};
        return InventorySystem::optimizeLootSplit(3, coins) == 1;
    }());

    // 2. Inventory Packer (Knapsack)
    // PDF Example: Cap=10, Items={{1,10}, {2,20}, {3,30}}. All fit. Value=60.
    runner.runTest("Knapsack: Cap 10, All Fit -> Value 60", [&]() {
        vector<pair<int, int>> items = {{1, 10}, {2, 20}, {3, 30}};
        return InventorySystem::maximizeCarryValue(10, items) == 60;
    }());

    // 3. Chat Autocorrect (String DP)
    // PDF Example: "uu" -> "uu" or "w" -> 2 possibilities
    runner.runTest("ChatDecorder: 'uu' -> 2 Possibilities", [&]() {
        return InventorySystem::countStringPossibilities("uu") == 2;
    }());
}

// ==========================================
// PART C: WORLD NAVIGATOR
// ==========================================

void test_PartC_Navigator() {
    cout << "\n--- Part C: World Navigator ---" << endl;

    // 1. Safe Passage (Path Exists)
    // PDF Example: 0-1, 1-2. Path 0->2 exists.
    runner.runTest("PathExists: 0->1->2 -> True", [&]() {
        vector<vector<int>> edges = {{0, 1}, {1, 2}};
        return WorldNavigator::pathExists(3, edges, 0, 2) == true;
    }());

    // 2. The Bribe (MST)
    // PDF Example: 3 Nodes. Roads: {0,1,10}, {1,2,5}, {0,2,20}. Rate=1.
    // MST should pick 10 and 5. Total 15.
    runner.runTest("MinBribeCost: Triangle Graph -> Cost 15", [&]() {
        vector<vector<int>> roads = {
            {0, 1, 10, 0}, 
            {1, 2, 5, 0}, 
            {0, 2, 20, 0}
        };
        // n=3, m=3, goldRate=1, silverRate=1
        return WorldNavigator::minBribeCost(3, 3, 1, 1, roads) == 15;
    }());

    // 3. Teleporter (Binary Sum APSP)
    // PDF Example: 0-1 (1), 1-2 (2). Distances: 1, 2, 3. Sum=6 -> "110"
    runner.runTest("BinarySum: Line Graph -> '110'", [&]() {
        vector<vector<int>> roads = {
            {0, 1, 1},
            {1, 2, 2}
        };
        return WorldNavigator::sumMinDistancesBinary(3, roads) == "110";
    }());
}

// ==========================================
// PART D: SERVER KERNEL
// ==========================================

void test_PartD_Kernel() {
    cout << "\n--- Part D: Server Kernel ---" << endl;

    // 1. Task Scheduler
    // PDF Example: Tasks={A, A, B}, n=2.
    // Order: A -> B -> idle -> A. Total intervals: 4.
    runner.runTest("Scheduler: {A, A, B}, n=2 -> 4 Intervals", [&]() {
        vector<char> tasks = {'A', 'A', 'B'};
        return ServerKernel::minIntervals(tasks, 2) == 4;
    }());
}

int main() {
    cout << "Arcadia Engine - Student Happy Path Tests" << endl;
    cout << "-----------------------------------------" << endl;

    test_PartA_DataStructures();
    test_PartB_Inventory();
    test_PartC_Navigator();
    test_PartD_Kernel();

    runner.printSummary();

    return 0;
}


