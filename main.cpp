/**
 *Dr's Testcase
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
PlayerTable *createPlayerTable();

Leaderboard *createLeaderboard();

AuctionTree *createAuctionTree();
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
    PlayerTable *table = createPlayerTable();
    runner.runTest("PlayerTable: Insert 'Alice' and Search", [&]() {
        table->insert(101, "Alice");
        return table->search(101) == "Alice";
    }());
    delete table;

    // 2. Leaderboard (Skip List)
    Leaderboard *board = createLeaderboard();

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
    AuctionTree *tree = createAuctionTree();
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
        vector<pair<int, int> > items = {{1, 10}, {2, 20}, {3, 30}};
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
        vector<vector<int> > edges = {{0, 1}, {1, 2}};
        return WorldNavigator::pathExists(3, edges, 0, 2) == true;
    }());

    // 2. The Bribe (MST)
    // PDF Example: 3 Nodes. Roads: {0,1,10}, {1,2,5}, {0,2,20}. Rate=1.
    // MST should pick 10 and 5. Total 15.
    runner.runTest("MinBribeCost: Triangle Graph -> Cost 15", [&]() {
        vector<vector<int> > roads = {
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
        vector<vector<int> > roads = {
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



//
// /* Our Testcase */
//
// #include <iostream>
// #include <vector>
// #include <string>
// #include <iomanip>
// #include <algorithm>   // for find
// #include "ArcadiaEngine.h"
//
// using namespace std;
//
// // ==========================================
// // FACTORY FUNCTIONS (LINKING)
// // ==========================================
// extern "C" {
// PlayerTable *createPlayerTable();
//
// Leaderboard *createLeaderboard();
//
// AuctionTree *createAuctionTree();
// }
//
// // ==========================================
// // TEST RUNNER
// // ==========================================
// class TestRunner {
//     int total = 0;
//     int passed = 0;
//
// public:
//     void run(const string &name, bool condition) {
//         total++;
//         cout << left << setw(65) << name;
//         if (condition) {
//             cout << "[ PASS ]";
//             passed++;
//         } else {
//             cout << "[ FAIL ]";
//         }
//         cout << endl;
//     }
//
//     void summary() {
//         cout << "\n==========================================" << endl;
//         cout << "PASSED: " << passed << " / " << total << endl;
//         cout << "==========================================" << endl;
//         if (passed == total)
//             cout << "Excellent! All tests passed." << endl;
//         else
//             cout << "Some tests failed. Check edge cases." << endl;
//     }
// };
//
// TestRunner runner;
//
// // ==========================================
// // PART A: DATA STRUCTURES
// // ==========================================
// void testPartA() {
//     cout << "\n--- PART A: DATA STRUCTURES ---" << endl;
//
//     // PlayerTable (Double Hashing)
//     PlayerTable *table = createPlayerTable();
//
//     runner.run("PlayerTable: Search Empty Table", table->search(100) == "");
//
//     table->insert(1, "Alice");
//     table->insert(102, "Bob"); // collision with 1
//     table->insert(203, "Charlie"); // collision with 1 & 102
//
//     runner.run("PlayerTable: Collision Alice", table->search(1) == "Alice");
//     runner.run("PlayerTable: Collision Bob", table->search(102) == "Bob");
//     runner.run("PlayerTable: Collision Charlie", table->search(203) == "Charlie");
//     runner.run("PlayerTable: Missing Key", table->search(999) == "");
//
//     delete table;
//
//     // Leaderboard (Skip List)
//     Leaderboard *board = createLeaderboard();
//
//     runner.run("Leaderboard: Empty TopN", board->getTopN(3).empty());
//
//     board->addScore(1, 100);
//     board->addScore(2, 300);
//     board->addScore(3, 200);
//
//     vector<int> top = board->getTopN(3);
//     runner.run("Leaderboard: Correct Order",
//                top.size() >= 3 && top[0] == 2 && top[1] == 3 && top[2] == 1);
//
//     board->addScore(10, 500);
//     board->addScore(5, 500);
//
//     vector<int> tie = board->getTopN(2);
//     runner.run("Leaderboard: Tie-Break by ID", tie[0] == 5 && tie[1] == 10);
//
//     board->removePlayer(2);
//     vector<int> afterRemove = board->getTopN(10);
//     runner.run("Leaderboard: Remove Player",
//                find(afterRemove.begin(), afterRemove.end(), 2) == afterRemove.end());
//
//     delete board;
//
//     // AuctionTree (Red-Black Tree)
//     AuctionTree *tree = createAuctionTree();
//
//     runner.run("AuctionTree: Insert Nodes", [&]() {
//         tree->insertItem(1, 100);
//         tree->insertItem(2, 50);
//         tree->insertItem(3, 150);
//         return true;
//     }());
//
//     runner.run("AuctionTree: Delete Existing", [&]() {
//         tree->deleteItem(2);
//         return true;
//     }());
//
//     runner.run("AuctionTree: Delete Missing", [&]() {
//         tree->deleteItem(999);
//         return true;
//     }());
//
//     delete tree;
// }
//
// // ==========================================
// // PART B: INVENTORY SYSTEM
// // ==========================================
// void testPartB() {
//     cout << "\n--- PART B: INVENTORY SYSTEM ---" << endl;
//
//     vector<int> emptyCoins;
//     runner.run("LootSplit: Empty", InventorySystem::optimizeLootSplit(0, emptyCoins) == 0);
//
//     vector<int> coins1 = {10};
//     runner.run("LootSplit: Single Coin", InventorySystem::optimizeLootSplit(1, coins1) == 10);
//
//     vector<int> coins2 = {3, 1, 4, 2, 2};
//     runner.run("LootSplit: Balanced", InventorySystem::optimizeLootSplit(5, coins2) == 0);
//
//     vector<pair<int, int> > items;
//     runner.run("Knapsack: Empty", InventorySystem::maximizeCarryValue(10, items) == 0);
//
//     items = {{5, 10}, {4, 40}, {6, 30}};
//     runner.run("Knapsack: Normal", InventorySystem::maximizeCarryValue(10, items) == 70);
//
//     runner.run("Decoder: Empty String", InventorySystem::countStringPossibilities("") == 1);
//     runner.run("Decoder: uu", InventorySystem::countStringPossibilities("uu") == 2);
// }
//
// // ==========================================
// // PART C: WORLD NAVIGATOR
// // ==========================================
// void testPartC() {
//     cout << "\n--- PART C: WORLD NAVIGATOR ---" << endl;
//
//     vector<vector<int> > edges;
//     runner.run("PathExists: No Path",
//                WorldNavigator::pathExists(3, edges, 0, 2) == false);
//
//     vector<vector<int> > roads = {{0, 1, 1}, {1, 2, 2}};
//     runner.run("BinarySum: Simple",
//                WorldNavigator::sumMinDistancesBinary(3, roads) == "110");
// }
//
// // ==========================================
// // PART D: SERVER KERNEL
// // ==========================================
// void testPartD() {
//     cout << "\n--- PART D: SERVER KERNEL ---" << endl;
//
//     vector<char> tasks;
//     runner.run("Scheduler: Empty", ServerKernel::minIntervals(tasks, 2) == 0);
//
//     tasks = {'A', 'A', 'A'};
//     runner.run("Scheduler: Cooldown", ServerKernel::minIntervals(tasks, 2) == 7);
// }
//
// // ==========================================
// // MAIN
// // ==========================================
// int main() {
//     cout << "Arcadia Engine - Extended Main Test" << endl;
//     cout << "---------------------------------" << endl;
//
//     testPartA();
//     testPartB();
//     testPartC();
//     testPartD();
//
//     runner.summary();
//     return 0;
// }
