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
    static const int TABLE_SIZE = 101;   
    struct PlayerData
    {
        /* data */
        int PlayerID;
        string name;
        bool occupied;
    };
    
    
    // Hint: You'll need a hash table with double hashing collision resolution
    vector<PlayerData> table;

public:
    ConcretePlayerTable(): table(TABLE_SIZE){
        // TODO: Initialize your hash table
        for(int i = 0; i<TABLE_SIZE; i++){
            table[i].PlayerID=-1;
            table[i].name="";
            table[i].occupied= false;

        }
        
    }

    int h1(int k) {
    return k % TABLE_SIZE;
    }

    int h2(int k) {
        return 1 + (k % (TABLE_SIZE - 1));   // must be non-zero
    }

    int h(int k, int i) {
        return (h1(k) + i * h2(k)) % TABLE_SIZE;
    }

    int hashInsert(int k, string name){
        int i=0;

        while(i<TABLE_SIZE){
            int j = h(k,i);
            
            if(!table[j].occupied){
                table[j].PlayerID = k;
                table[j].name = name;
                table[j].occupied=true;
                return j;
            }
            i = i + 1;
        }
        cout<<endl<<"Table is full";
    }

    void insert(int playerID, string name) override {
        // TODO: Implement double hashing insert
        hashInsert(playerID, name);

        // Remember to handle collisions using h1(PlayerID) + i * h2(PlayerID)
    }

    string search(int playerID) override {
        // TODO: Implement double hashing search
        int i = 0;
        while(i<TABLE_SIZE){
            int j=h(playerID,i);
            if(table[j].PlayerID==playerID){
                return table[j].name;
            }
            if(!table[j].occupied){
                return "";
            }
            i = i + 1;
        }
        // Return "" if player not found
        return "";
    }
};

// --- 2. Leaderboard (Skip List) ---

class ConcreteLeaderboard : public Leaderboard {
private:
    // TODO: Define your skip list node structure and necessary variables
    // Hint: You'll need nodes with multiple forward pointers
    struct SLNode {
        int playerID;
        int score;
        vector<SLNode*> forward;

        SLNode(int id, int sc, int level)
            : playerID(id), score(sc), forward(level, nullptr) {}
    };

    static const int maxNumberOfLevel  = 16;
    SLNode* head;
    int currentLevel;

public:
    ConcreteLeaderboard() {
        // TODO: Initialize your skip list
    }

    void addScore(int playerID, int score) override {
        // TODO: Implement skip list insertion
        // Remember to maintain descending order by score
    }

    void removePlayer(int playerID) override {
        // TODO: Implement skip list deletion
    }

    vector<int> getTopN(int n) override {
        // TODO: Return top N player IDs in descending score order
        return {};
    }
};

// --- 3. AuctionTree (Red-Black Tree) ---

class ConcreteAuctionTree : public AuctionTree {
private:
    enum Color {
        Red, Black
    };

    struct RBNode {
        int id, price;
        Color color;
        RBNode *left;
        RBNode *right;
        RBNode *parent;

        RBNode(int id, int price) : id(id), price(price), color(Red), left(nullptr), right(nullptr), parent(nullptr) {
        }
    };

    RBNode *root = nullptr;

    Color getColor(RBNode *node) {
        if (node) {
            return node->color;
        }
        return Black;
    }

    void setColor(RBNode *node, Color color) {
        if (node) {
            node->color = color;
        }
    }

    bool lessThan(RBNode *node1, RBNode *node2) {
        if (node1->price != node2->price) {
            return node1->price < node2->price;
        }
        return node1->id < node2->id;
    }

    RBNode *searchById(RBNode *node, int id) {
        if (!node) {
            return nullptr;
        }
        if (node->id == id) {
            return node;
        }
        RBNode* left = searchById(node->left, id);
        if (left) {
            return left;
        }
        return searchById(node->right, id);
    }

    void transplant(RBNode *node1, RBNode *node2) {
        if (!node1->parent) {
            root = node2;
        } else if (node1->parent->left == node1) {
            node1->parent->left = node2;
        } else {
            node1->parent->right = node2;
        }
        if (node2) {
            node2->parent = node1->parent;
        }
    }

    RBNode *subTreeMin(RBNode *node) {
        if (!node) return nullptr;
        while (node->left) {
            node = node->left;
        }
        return node;
    }

    void leftRotate(RBNode *unbalancedNode) {
        RBNode *rightChild = unbalancedNode->right;
        unbalancedNode->right = rightChild->left;
        if (rightChild->left) {
            rightChild->left->parent = unbalancedNode;
        }
        rightChild->parent = unbalancedNode->parent;
        if (!unbalancedNode->parent) {
            root = rightChild;
        } else if (unbalancedNode == unbalancedNode->parent->left) {
            unbalancedNode->parent->left = rightChild;
        } else {
            unbalancedNode->parent->right = rightChild;
        }
        rightChild->left = unbalancedNode;
        unbalancedNode->parent = rightChild;
    }

    void rightRotate(RBNode *unbalancedNode) {
        RBNode *leftChild = unbalancedNode->left;
        unbalancedNode->left = leftChild->right;
        if (leftChild->right) {
            leftChild->right->parent = unbalancedNode;
        }
        leftChild->parent = unbalancedNode->parent;
        if (!unbalancedNode->parent) {
            root = leftChild;
        } else if (unbalancedNode == unbalancedNode->parent->right) {
            unbalancedNode->parent->right = leftChild;
        } else {
            unbalancedNode->parent->left = leftChild;
        }
        leftChild->right = unbalancedNode;
        unbalancedNode->parent = leftChild;
    }

    void fixInsert(RBNode *current) {
        while (current->parent && current->parent->color == Red) {
            if (current->parent == current->parent->left) {
                RBNode *uncle = current->parent->parent->right;
                if (getColor(uncle) == Red) {
                    setColor(current->parent, Black);
                    setColor(uncle, Black);
                    setColor(current->parent->parent, Red);
                    current = current->parent->parent;
                } else {
                    if (current == current->parent->right) {
                        current = current->parent;
                        leftRotate(current);
                    }
                    setColor(current->parent, Black);
                    setColor(current->parent->parent, Red);
                    rightRotate(current->parent->parent);
                }
            } else {
                RBNode *uncle = current->parent->parent->left;
                if (getColor(uncle) == Red) {
                    setColor(current->parent, Black);
                    setColor(uncle, Black);
                    setColor(current->parent->parent, Red);
                    current = current->parent->parent;
                } else {
                    if (current == current->parent->left) {
                        current = current->parent;
                        rightRotate(current);
                    }
                    setColor(current->parent, Black);
                    setColor(current->parent->parent, Red);
                    leftRotate(current->parent->parent);
                }
            }
        }
        setColor(root, Black);
    }

    void fixDelete(RBNode *current, RBNode *parent) {
        while (current != root && getColor(current) == Black) {
            if (!parent) {
                break;
            }
            if (current == parent->left) {
                RBNode *sibling = parent->right;
                if (getColor(sibling) == Red) {
                    setColor(sibling, Black);
                    setColor(parent, Red);
                    leftRotate(parent);
                    sibling = parent->right;
                }
                if (getColor(sibling->left) == Black && getColor(sibling->right) == Black) {
                    setColor(sibling, Red);
                    current = parent;
                    parent = current->parent;
                } else {
                    if (getColor(sibling->right) == Black) {
                        setColor(sibling->left, Black);
                        setColor(sibling, Red);
                        rightRotate(sibling);
                        sibling = parent->right;
                    }
                    setColor(sibling, parent->color);
                    setColor(parent, Black);
                    setColor(sibling->right, Black);
                    leftRotate(parent);
                    current = root;
                }
            } else {
                RBNode *sibling = parent->left;
                if (getColor(sibling) == Red) {
                    setColor(sibling, Black);
                    setColor(parent, Red);
                    rightRotate(parent);
                    sibling = parent->left;
                }
                if (getColor(sibling->right) == Black && getColor(sibling->left) == Black) {
                    setColor(sibling, Red);
                    current = parent;
                    parent = current->parent;
                } else {
                    if (getColor(sibling->left) == Black) {
                        setColor(sibling->right, Black);
                        setColor(sibling, Red);
                        leftRotate(sibling);
                        current = parent->left;
                    }
                    setColor(sibling, parent->color);
                    setColor(parent, Black);
                    setColor(sibling->left, Black);
                    rightRotate(parent);
                    current = root;
                }
            }
        }
        if (current) {
            setColor(current, Black);
        }
    }

public:
    ConcreteAuctionTree() {
    }

    void insertItem(int itemID, int price) override {
        RBNode *newNode = new RBNode(itemID, price);
        RBNode *parent = nullptr;
        RBNode *current = root;

        while (current) {
            parent = current;
            if (lessThan(newNode, current)) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        newNode->parent = parent;
        if (!parent) root = newNode;
        else if (lessThan(newNode, parent)) {
            parent->left = newNode;
        } else {
            parent->right = newNode;
        }
        fixInsert(newNode);
    }

    void deleteItem(int itemID) override {
        cout << "start delete" << endl;
        RBNode *nodeToDelete = searchById(root, itemID);
        if (!nodeToDelete) return;
        RBNode *actuallyDeleted = nodeToDelete;
        Color originalColor = getColor(actuallyDeleted);
        RBNode *child = nullptr;
        RBNode *childParent = nullptr;
        if (!nodeToDelete->left) {
            child = nodeToDelete->right;
            childParent = nodeToDelete->parent;
            transplant(nodeToDelete, child);
        } else if (!nodeToDelete->right) {
            child = nodeToDelete->left;
            childParent = nodeToDelete->parent;
            transplant(nodeToDelete, child);
        } else {
            actuallyDeleted = subTreeMin(nodeToDelete->right);
            originalColor = getColor(actuallyDeleted);
            child = actuallyDeleted->right;
            if (actuallyDeleted->parent == nodeToDelete) {
                if (child) {
                    child->parent = actuallyDeleted;
                } else {
                    transplant(actuallyDeleted, actuallyDeleted->right);
                    actuallyDeleted->right = nodeToDelete->right;
                    actuallyDeleted->right->parent = actuallyDeleted;
                    childParent = actuallyDeleted->parent;
                }
                transplant(nodeToDelete, actuallyDeleted);
                actuallyDeleted->left = nodeToDelete->left;
                actuallyDeleted->left->parent = actuallyDeleted;
                actuallyDeleted->color = nodeToDelete->color;
            }
            cout << "beforee" << endl;
            delete nodeToDelete;
            if (originalColor == Black) {
                cout << "fixxx" << endl;
                fixDelete(child, childParent);
            }
        }
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================
int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {
    int total=0;
    for (int i=0; i<n; i++){
        total+=coins[i];
    }
    int goal = total/2;

    vector<bool> dp(goal+1,false);
    dp[0]=true;

    for (int i=0; i<n; i++) {
        int coin = coins[i];
        for (int j = goal; j >= coin; j--) {
            dp[j] = dp[j] || dp[j - coin];
        }
    }

    int best = 0;
    for (int j=goal; j>=0; j--) {
        if (dp[j]) {
            best=j;
            break;
        }
    }
    return (total - best)-best;
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>>& items) {
    int n = items.size();
    vector<vector<int>>V(n+1, vector<int>(capacity+1, 0));

    for (int i=1; i<=n; ++i) {
        int w=items[i-1].first;
        int v=items[i-1].second;
        for (int j=0; j<=capacity; ++j) {
            if (w <= j) {
                V[i][j] = max(V[i-1][j], v+V[i-1][j-w]);
            } else {
                V[i][j]=V[i-1][j];
            }
        }
    }
    return V[n][capacity];
}

long long InventorySystem::countStringPossibilities(string s) {
    const long long MOD =1e9+7;
    int n=s.size();
    vector<long long> dp(n+1,0);
    dp[0]=1;

    for (int i = 1; i <= n; i++) {
        dp[i] = dp[i-1];
        if (i >1) {
            string pair = s.substr(i - 2, 2);
            if (pair =="uu" || pair =="nn") {
                dp[i]=(dp[i] + dp[i - 2])%MOD;
            }
        }
    }
    return dp[n];
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool WorldNavigator::pathExists(int n, vector<vector<int> > &edges, int source, int dest) {
    // TODO: Implement path existence check using BFS or DFS
    // edges are bidirectional
    return false;
}

long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                                       vector<vector<int> > &roadData) {
    // TODO: Implement Minimum Spanning Tree (Kruskal's or Prim's)
    // roadData[i] = {u, v, goldCost, silverCost}
    // Total cost = goldCost * goldRate + silverCost * silverRate
    // Return -1 if graph cannot be fully connected
    return -1;
}

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int> > &roads) {
    // TODO: Implement All-Pairs Shortest Path (Floyd-Warshall)
    // Sum all shortest distances between unique pairs (i < j)
    // Return the sum as a binary string
    // Hint: Handle large numbers carefully
    return "0";
}

// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================

int ServerKernel::minIntervals(vector<char> &tasks, int n) {
    if (tasks.empty()) {
        return 0;
    }
    map<char, int> freq;
    for (char &c: tasks) {
        freq[c]++;
    }
    int maxFreq = 0;
    for (auto &c: freq) {
        maxFreq = max(maxFreq, c.second);
    }
    int maxFreqCount = 0;
    for (auto &c: freq) {
        if (c.second == maxFreq) {
            maxFreqCount++;
        }
    }
    int intervalsNum = max((int) tasks.size(), (maxFreq - 1) * (n + 1) + maxFreqCount);
    return intervalsNum;
}

// =========================================================
// FACTORY FUNCTIONS (Required for Testing)
// =========================================================

extern "C" {
PlayerTable *createPlayerTable() {
    return new ConcretePlayerTable();
}

Leaderboard *createLeaderboard() {
    return new ConcreteLeaderboard();
}

AuctionTree *createAuctionTree() {
    return new ConcreteAuctionTree();
}
}


