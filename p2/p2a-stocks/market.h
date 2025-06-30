// Project Identifier: 0E04A31E0D60C01986ACB20081C9D8722A1899B6
#include <vector>
#include <iostream>
#include <algorithm>
#include <getopt.h>
#include <string>
#include <deque>
#include <queue>
#include <cassert>
#include <sstream>
#include <limits>
#include "P2random.h"

using namespace std;
struct Order {
    //int timestamp = 0;
    int traderID = 0;
    //int stockID = 0;
    //bool isBuy = false;
    int priceLimit = 0;
    int quantity = 0;
    int orderID = 0;
};

struct BuyOrderComparator{
    bool operator()(const Order& a, const Order& b);
};

struct SellOrderComparator{
    bool operator()(const Order& a, const Order& b);
};


class StockMarket{
//private:
public:
    struct TraderStats{
        int numBought = 0;
        int numSold = 0;
        //int totalSpent = 0;
        //int totalGained = 0;
        int net = 0;
    };
    vector<priority_queue<int>> maxHeaps; // Vector of max-heaps (one for each stock)
    vector<priority_queue<int, vector<int>, greater<int>>> minHeaps; // Vector of min-heaps (one for each stock)
    vector<priority_queue<Order, vector<Order>, BuyOrderComparator>> buyQueues;  // Vector of buy order queues (one for each stock)
    vector<priority_queue<Order, vector<Order>, SellOrderComparator>> sellQueues; // Vector of sell order queues (one for each stock)
    vector<TraderStats> traderInfo;
    int currentTimestamp = 0;
    int numStocks = 0;
    int numTraders = 0;
    bool verbose = 0;
    bool medianFlag = 0;
    bool traderInfoFlag = 0;
    bool timeTravelersFlag = 0;
    void balanceHeaps(int stockID);
    int orderCnt = 0;
    mutable int tradesCompleted = 0;
    enum class TimeStatus { Notrades, Canbuy, Complete, Potential };
struct TimeTravelerRecord {
    TimeStatus status = TimeStatus::Notrades;
    int minBuyPrice = numeric_limits<int>::max();
    int potentialBuyPrice = numeric_limits<int>::max();
    int maxProfit = 0;
    int minBuyTimestamp = -1;
    int potentialBuyTimestamp = -1;
    int bestBuyTimestamp = -1;
    int bestSellTimestamp = -1;
};
    vector<TimeTravelerRecord> timeTravelerRecords;
public:
    void processOrder(Order& order, int stockID, bool isBuy, int timestamp);//process one order
    void matchOrder(Order& order, int stockID, bool isBuy);
    void printMedian();
    void addPriceToHeap(int stockID, int price);
    int getMedian(int stockID);
    void printTraderInfo();
    void updateTimeTravelerInfo(Order& order, int stockID, bool isBuy, int timestamp);
    void printTimeTravelers();
    void readOrders();
    void processOrders(istream &inputStream);//process all orders one by one
    void readOptions(int argc, char* argv[]);
};
