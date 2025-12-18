// ArcadiaEngine.cpp - STUDENT TEMPLATE

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

    struct PlayerData {
        /* data */
        int PlayerID;
        string name;
        bool occupied;
    };

    vector<PlayerData> table;

public:
    ConcretePlayerTable() : table(TABLE_SIZE) {
        // TODO: Initialize your hash table
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i].PlayerID = -1;
            table[i].name = "";
            table[i].occupied = false;
        }
    }

    int h1(int k) {
        return k % TABLE_SIZE;
    }

    int h2(int k) {
        return 1 + (k % (TABLE_SIZE - 1)); // must be non-zero
    }

    int h(int k, int i) {
        return (h1(k) + i * h2(k)) % TABLE_SIZE;
    }

    int hashInsert(int k, string name) {
        int i = 0;

        while (i < TABLE_SIZE) {
            int j = h(k, i);

            if (!table[j].occupied) {
                table[j].PlayerID = k;
                table[j].name = name;
                table[j].occupied = true;
                return j;
            }
            i = i + 1;
        }
        cout << endl << "Table is full";
    }

    void insert(int playerID, string name) override {
        hashInsert(playerID, name);
    }

    string search(int playerID) override {
        int i = 0;
        while (i < TABLE_SIZE) {
            int j = h(playerID, i);
            if (table[j].PlayerID == playerID) {
                return table[j].name;
            }
            if (!table[j].occupied) {
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
    struct SLNode {
        int playerID;
        int score;
        vector<SLNode *> forward;

        SLNode(int id, int sc, int level)
            : playerID(id), score(sc), forward(level, nullptr) {
        }
    };

    static const int maxNumberOfLevel = 16;
    SLNode *head;
    int currentLevel;

public:
    ConcreteLeaderboard() {
        currentLevel = 1;
        head = new SLNode(-1, -1, maxNumberOfLevel);
    }

    int randomLevel() {
        int lvl = 1;
        while ((rand() % 2) && lvl < maxNumberOfLevel) {
            lvl++;
        }
        return lvl;
    }

    void addScore(int playerID, int score) override {
        SLNode *update[maxNumberOfLevel];
        SLNode *current = head;
        for (int i = currentLevel - 1; i >= 0; i--) {
            while (current->forward[i] != nullptr &&
                   (current->forward[i]->score > score ||
                    (current->forward[i]->score == score && current->forward[i]->playerID < playerID))) {
                current = current->forward[i];
            }
            update[i] = current;
        }


        int newLevel = randomLevel();

        if (newLevel > currentLevel) {
            for (int i = currentLevel; i < newLevel; i++) {
                update[i] = head;
            }
            currentLevel = newLevel;
        }

        SLNode *newNode = new SLNode(playerID, score, newLevel);

        // Re-link pointers
        for (int i = 0; i < newLevel; i++) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
    }

    void removePlayer(int playerID) override {
        SLNode *update[maxNumberOfLevel];
        SLNode *current = head;
        SLNode *target = nullptr;

        SLNode *temp = head->forward[0];
        while (temp != nullptr) {
            if (temp->playerID == playerID) {
                target = temp;
                break;
            }
            temp = temp->forward[0];
        }

        if (!target) return;

        int targetScore = target->score;
        current = head;
        for (int i = currentLevel - 1; i >= 0; i--) {
            while (current->forward[i] != nullptr &&
                   (current->forward[i]->score > targetScore ||
                    (current->forward[i]->score == targetScore && current->forward[i]->playerID < playerID))) {
                current = current->forward[i];
            }
            update[i] = current;
        }


        if (current->forward[0] == target) {
            for (int i = 0; i < currentLevel; i++) {
                if (update[i]->forward[i] != target) break;
                update[i]->forward[i] = target->forward[i];
            }
            delete target;
        }

        while (currentLevel > 1 &&
               head->forward[currentLevel - 1] == nullptr
        ) {
            currentLevel--;
        }
    }

    vector<int> getTopN(int n) override {
        vector<int> topPlayers;


        SLNode *current = head->forward[0];

        // Traverse the base level (level 0) until we:
        // 1. Collect 'n' players
        // 2. Or reach the end of the list (nullptr)
        while (current != nullptr && (int) topPlayers.size() < n) {
            topPlayers.push_back(current->playerID);
            current = current->forward[0];
        }

        return topPlayers;
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

    RBNode *getParent(RBNode *node, RBNode *fallback) {
        return node ? node->parent : fallback;
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
        RBNode *left = searchById(node->left, id);
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
            if (!parent) break;

            if (current == parent->left) {
                RBNode *sibling = parent->right;

                if (getColor(sibling) == Red) {
                    setColor(sibling, Black);
                    setColor(parent, Red);
                    leftRotate(parent);
                    sibling = parent->right;
                }

                if (getColor(sibling ? sibling->left : nullptr) == Black &&
                    getColor(sibling ? sibling->right : nullptr) == Black) {
                    setColor(sibling, Red);
                    current = parent;
                    parent = current->parent;
                } else {
                    if (getColor(sibling ? sibling->right : nullptr) == Black) {
                        setColor(sibling ? sibling->left : nullptr, Black);
                        setColor(sibling, Red);
                        rightRotate(sibling);
                        sibling = parent->right;
                    }
                    setColor(sibling, parent->color);
                    setColor(parent, Black);
                    setColor(sibling ? sibling->right : nullptr, Black);
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

                if (getColor(sibling ? sibling->right : nullptr) == Black &&
                    getColor(sibling ? sibling->left : nullptr) == Black) {
                    setColor(sibling, Red);
                    current = parent;
                    parent = current->parent;
                } else {
                    if (getColor(sibling ? sibling->left : nullptr) == Black) {
                        setColor(sibling ? sibling->right : nullptr, Black);
                        setColor(sibling, Red);
                        leftRotate(sibling);
                        sibling = parent->left;
                    }
                    setColor(sibling, parent->color);
                    setColor(parent, Black);
                    setColor(sibling ? sibling->left : nullptr, Black);
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

            childParent = actuallyDeleted->parent;

            if (actuallyDeleted->parent != nodeToDelete) {
                transplant(actuallyDeleted, child);
                actuallyDeleted->right = nodeToDelete->right;
                actuallyDeleted->right->parent = actuallyDeleted;
            } else {
                childParent = actuallyDeleted;
            }

            transplant(nodeToDelete, actuallyDeleted);
            actuallyDeleted->left = nodeToDelete->left;
            actuallyDeleted->left->parent = actuallyDeleted;
            actuallyDeleted->color = nodeToDelete->color;
        }
        delete nodeToDelete;

        if (originalColor == Black) {
            fixDelete(child, childParent);
        }
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================
int InventorySystem::optimizeLootSplit(int n, vector<int> &coins) {
    int total = 0;
    for (int i = 0; i < n; i++) {
        total += coins[i];
    }
    int goal = total / 2;

    vector<bool> dp(goal + 1, false);
    dp[0] = true;

    for (int i = 0; i < n; i++) {
        int coin = coins[i];
        for (int j = goal; j >= coin; j--) {
            dp[j] = dp[j] || dp[j - coin];
        }
    }

    int best = 0;
    for (int j = goal; j >= 0; j--) {
        if (dp[j]) {
            best = j;
            break;
        }
    }
    return (total - best) - best;
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int> > &items) {
    int n = items.size();
    vector<vector<int> > V(n + 1, vector<int>(capacity + 1, 0));

    for (int i = 1; i <= n; ++i) {
        int w = items[i - 1].first;
        int v = items[i - 1].second;
        for (int j = 0; j <= capacity; ++j) {
            if (w <= j) {
                V[i][j] = max(V[i - 1][j], v + V[i - 1][j - w]);
            } else {
                V[i][j] = V[i - 1][j];
            }
        }
    }
    return V[n][capacity];
}

long long InventorySystem::countStringPossibilities(string s) {
    const long long MOD = 1e9 + 7;
    int n = s.size();
    vector<long long> dp(n + 1, 0);
    dp[0] = 1;

    for (int i = 1; i <= n; i++) {
        dp[i] = dp[i - 1];
        if (i > 1) {
            string pair = s.substr(i - 2, 2);
            if (pair == "uu" || pair == "nn") {
                dp[i] = (dp[i] + dp[i - 2]) % MOD;
            }
        }
    }
    return dp[n];
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool WorldNavigator::pathExists(int n, vector<vector<int>> &edges, int source, int dest) {

    vector<vector<int>> adj(n);

    for (int i = 0; i < edges.size(); i++) {
        int u = edges[i][0];
        int v = edges[i][1];
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    vector<bool> visited(n, false);
    queue<int> q;

    q.push(source);
    visited[source] = true;

    while (!q.empty()) {
        int curr = q.front();
        q.pop();

        for (int i = 0; i < adj[curr].size(); i++) {
            int next = adj[curr][i];

            if (!visited[next]) {
                if (next == dest)
                    return true;

                visited[next] = true;
                q.push(next);
            }
        }
    }

    return false;
}

long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                                       vector<vector<int>> &roadData) {

    // 1️⃣ بناء adjacency list مع cost لكل edge
    vector<vector<pair<int,long long>>> adj(n);
    for (int i = 0; i < m; i++) {
        int u = roadData[i][0];
        int v = roadData[i][1];
        long long cost = roadData[i][2]*goldRate + roadData[i][3]*silverRate;
        adj[u].push_back({v,cost});
        adj[v].push_back({u,cost});
    }

    // 2️⃣ تهيئة visited array
    vector<bool> visited(n,false);

    // 3️⃣ تهيئة الـ priority queue (min-heap)
    priority_queue<pair<long long,int>, vector<pair<long long,int>>, greater<pair<long long,int>>> pq;

    pq.push({0,0}); 
    long long total = 0;

    while (!pq.empty()) {
        pair<long long,int> top = pq.top();
        pq.pop();

        long long cost = top.first;
        int u = top.second;

        if (visited[u]) continue; 
        visited[u] = true;
        total += cost; 
        for (int i = 0; i < adj[u].size(); i++) {
            int v = adj[u][i].first;
            long long c = adj[u][i].second;

            if (!visited[v]) {
                pq.push({c, v});
            }
        }
    }

    for (int i = 0; i < n; i++) {
        if (!visited[i]) return -1;
    }

    return total;
}

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int> > &roads) {
    long long inf = INT_MAX;
    vector<vector<long long> > dist(n, vector<long long>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dist[i][j] = inf;
            if (i == j) {
                dist[i][j] = 0;
            }
        }
    }

    for (int i = 0; i < roads.size(); i++) {
        int u = roads[i][0];
        int v = roads[i][1];
        long long cost = roads[i][2];
        dist[u][v] = cost;
        dist[v][u] = cost;
    }
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (dist[i][k] != inf && dist[k][j] != inf) {
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
                }
            }
        }
    }
    long long sum = 0;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            sum += dist[i][j];
        }
    }
    if (sum == 0)
        return "0";

    string binary = "";
    while (sum > 0) {
        binary = char('0' + (sum % 2)) + binary;
        sum /= 2;
    }
    return binary;
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
