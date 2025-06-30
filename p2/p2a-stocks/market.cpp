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
#include "P2random.h"
#include "market.h"
using namespace std;

bool BuyOrderComparator::operator()(const Order& a, const Order& b){
    if (a.priceLimit == b.priceLimit){
        return b.orderID < a.orderID;
    }

    return a.priceLimit < b.priceLimit;
};

bool SellOrderComparator::operator()(const Order& a, const Order& b){
    if (a.priceLimit == b.priceLimit){
            return b.orderID < a.orderID;
    }

    return a.priceLimit > b.priceLimit;
};

void StockMarket::balanceHeaps(int stockID) {
    if (maxHeaps[stockID].size() > minHeaps[stockID].size() + 1) {
        minHeaps[stockID].push(maxHeaps[stockID].top());
        maxHeaps[stockID].pop();
    } else if (minHeaps[stockID].size() > maxHeaps[stockID].size()) {
        maxHeaps[stockID].push(minHeaps[stockID].top());
        minHeaps[stockID].pop();
    }
}


void StockMarket::addPriceToHeap(int stockID, int price) {
    if (maxHeaps[stockID].empty() || price <= maxHeaps[stockID].top()) {
        maxHeaps[stockID].push(price);
    } else {
        minHeaps[stockID].push(price);
    }

    balanceHeaps(stockID);
}
int StockMarket::getMedian(int stockID){
    if (maxHeaps[stockID].size() > minHeaps[stockID].size()){
        return maxHeaps[stockID].top();
    } else if (maxHeaps[stockID].size() < minHeaps[stockID].size()){
        return minHeaps[stockID].top();
    } else {
        return (minHeaps[stockID].top() + maxHeaps[stockID].top()) / 2;
    }
}

void StockMarket::matchOrder(Order& order, int stockID, bool isBuy){
    if (isBuy){
        while (!sellQueues[stockID].empty() && order.quantity > 0){
            Order sellOrder = sellQueues[stockID].top();
            sellQueues[stockID].pop();
            if (sellOrder.priceLimit <= order.priceLimit){
                int tradedQuantity = min(order.quantity, sellOrder.quantity);
                order.quantity -= tradedQuantity;
                sellOrder.quantity -= tradedQuantity;
                tradesCompleted++;
                if (verbose){
                    cout << "Trader " << order.traderID << " purchased " << tradedQuantity
                    << " shares of Stock " << stockID << " from Trader " 
                    << sellOrder.traderID << " for $" << sellOrder.priceLimit << "/share\n";
                }

                if (medianFlag){
                    addPriceToHeap(stockID, sellOrder.priceLimit);
                }

                if (traderInfoFlag){
                    traderInfo[order.traderID].numBought += tradedQuantity;
                    //traderInfo[order.traderID].totalSpent += tradedQuantity * sellOrder.priceLimit;
                    traderInfo[order.traderID].net -= tradedQuantity * sellOrder.priceLimit;
                    traderInfo[sellOrder.traderID].numSold += tradedQuantity;
                    //traderInfo[sellOrder.traderID].totalGained += tradedQuantity * sellOrder.priceLimit;//may decrease speed
                    traderInfo[sellOrder.traderID].net += tradedQuantity * sellOrder.priceLimit;//may decrease speed
                }

                if (sellOrder.quantity > 0){
                    sellQueues[stockID].push(sellOrder);
                }
            } else {
                sellQueues[stockID].push(sellOrder);
                break;
            }
        }

        if (order.quantity > 0){
            buyQueues[stockID].push(order);
        }
    } else {
        while (!buyQueues[stockID].empty() && order.quantity > 0){
            Order buyOrder = buyQueues[stockID].top();
            buyQueues[stockID].pop();
            if (buyOrder.priceLimit >= order.priceLimit){
                int tradedQuantity = min(order.quantity, buyOrder.quantity);
                order.quantity -= tradedQuantity;
                buyOrder.quantity -= tradedQuantity;
                tradesCompleted++;
                if (verbose) {
                    cout << "Trader " << buyOrder.traderID << " purchased " << tradedQuantity 
                        << " shares of Stock " << stockID << " from Trader " 
                        << order.traderID << " for $" << buyOrder.priceLimit << "/share\n";
                }

                if (medianFlag){
                    addPriceToHeap(stockID, buyOrder.priceLimit);
                }

                if (traderInfoFlag){
                    traderInfo[order.traderID].numSold += tradedQuantity;
                    //traderInfo[order.traderID].totalGained += tradedQuantity * buyOrder.priceLimit;
                    traderInfo[order.traderID].net += tradedQuantity * buyOrder.priceLimit;
                    traderInfo[buyOrder.traderID].numBought += tradedQuantity;
                    //traderInfo[buyOrder.traderID].totalSpent += tradedQuantity * buyOrder.priceLimit;
                    traderInfo[buyOrder.traderID].net -= tradedQuantity * buyOrder.priceLimit;
                }

                if (buyOrder.quantity > 0){
                    buyQueues[stockID].push(buyOrder);
                }
            } else {
                buyQueues[stockID].push(buyOrder);
                break;
            }
        }

        if (order.quantity > 0){
            sellQueues[stockID].push(order);
        }
    }
}

void StockMarket::processOrder(Order& order, int stockID, bool isBuy, int timestamp){
    if (timestamp != currentTimestamp) {
        if (medianFlag) {
            printMedian();
        }

        currentTimestamp = timestamp;
    }

    matchOrder(order, stockID, isBuy);
    if (timeTravelersFlag){
        updateTimeTravelerInfo(order, stockID, isBuy, timestamp);
    }
}
    
void StockMarket::printMedian(){
    for (int i = 0; i < numStocks; i++){
        if (!(maxHeaps[i].empty() && minHeaps[i].empty())){
            cout << "Median match price of Stock " << i << " at time " 
                << currentTimestamp << " is $" << getMedian(i) << "\n";
        }
    }
}

void StockMarket::printTraderInfo(){
    cout << "---Trader Info---\n";
    for (int i = 0; i < numTraders; i++){
        int net = traderInfo[i].net;
        cout << "Trader " << i << " bought " << traderInfo[i].numBought
                << " and sold " << traderInfo[i].numSold << " for a net transfer of $"
                << net << "\n";
    }
}

void StockMarket::readOptions(int argc, char* argv[]){
    opterr = false;
    int choice = 0;
    int index = 0;
    struct option long_options[] = {
        {"verbose", no_argument, nullptr, 'v'},
        {"median", no_argument, nullptr, 'm'},
        {"trader_info", no_argument, nullptr, 'i'},
        {"time_travelers", no_argument, nullptr, 't'},
        { nullptr, 0, nullptr, '\0' }
    };
    while ((choice = getopt_long(argc, argv, "vmit", long_options, &index)) != -1) {
			switch (choice) {
			case 'v':
				verbose = true;
				break;
			case 'm':
				medianFlag = true;
				break;
			case 'i':
				traderInfoFlag = true;
				break;
			case 't':
				timeTravelersFlag = true;
				break;
			}
		}
    
}
    
void StockMarket::readOrders(){
    string comment;
    string mode;
    getline(cin, comment);
    cin >> comment;
    cin >> mode;
    cin >> comment;
    cin >> numTraders;
    cin >> comment;
    cin >> numStocks;
    cout << "Processing orders...\n";
    if (mode == "PR") {
        unsigned int randomSeed, numOrders, arrivalRate;
        string seedStr, orderStr, rateStr;
        cin >> comment;
        cin >> randomSeed;
        cin >> comment;
        cin >> numOrders;
        cin >> comment;
        cin >> arrivalRate;
        stringstream ss;
        P2random::PR_init(ss, randomSeed, numTraders, numStocks, numOrders, arrivalRate);
        processOrders(ss);
    } else if (mode == "TL") {
        processOrders(cin);
    }
}

void StockMarket::processOrders(istream &inputStream){
    //buyQueues.reserve(numStocks);
    //sellQueues.reserve(numStocks);
    /*priority_queue<Order, vector<Order>, BuyOrderComparator> buyPQ;
    priority_queue<Order, vector<Order>, SellOrderComparator> sellPQ;
    for (int i = 0; i < numStocks; i++){
        buyQueues.push_back(buyPQ);
        sellQueues.push_back(sellPQ);
    }*/
    buyQueues.resize(numStocks);
    sellQueues.resize(numStocks);
    if (traderInfoFlag){
        traderInfo.resize(numTraders);//may be reserve
    }

    if (medianFlag){
        maxHeaps.resize(numStocks);
        minHeaps.resize(numStocks);
    }

    if (timeTravelersFlag){
        timeTravelerRecords.resize(numStocks);
    }

    string action;
    int timestamp, traderID, stockID, price, quantity;
    char t, s, dollar, hash;
    int lastTimestamp = -1;
    while (inputStream >> timestamp >> action >> t >> traderID >> s >> stockID >> dollar >> price >> hash >> quantity) {
        // Validate input
        if (timestamp < 0) {
            cerr << "Error: Negative timestamp detected.\n";
            exit(1);
        }
        if (timestamp < lastTimestamp) {
            cerr << "Error: Timestamps must be non-decreasing.\n";
            exit(1);
        }
        if (traderID < 0 || traderID >= numTraders) {
            cerr << "Error: Invalid trader ID.\n";
            exit(1);
        }
        if (stockID < 0 || stockID >= numStocks) {
            cerr << "Error: Invalid stock ID.\n";
            exit(1);
        }
        if (price <= 0 || quantity <= 0) {
            cerr << "Error: Price and quantity must be positive.\n";
            exit(1);
        }
        bool isBuy = (action == "BUY");
        Order order = {traderID, price, quantity, orderCnt};//woshishabi
        orderCnt++;
        processOrder(order, stockID, isBuy, timestamp);
        lastTimestamp = timestamp;
    }
}

void StockMarket::updateTimeTravelerInfo(Order& order, int stockID, bool isBuy, int timestamp) {
    TimeTravelerRecord& record = timeTravelerRecords[stockID];

        switch (record.status) {
            case TimeStatus::Notrades:
                if (!isBuy) {
                    record.status = TimeStatus::Canbuy;
                    record.minBuyPrice = order.priceLimit;
                    record.minBuyTimestamp = timestamp;
                }
                break;
            case TimeStatus::Canbuy:
                if (!isBuy) {
                    if (order.priceLimit < record.minBuyPrice) {
                        record.minBuyPrice = order.priceLimit;
                        record.minBuyTimestamp = timestamp;
                    }
                } else {
                    int profit = order.priceLimit - record.minBuyPrice;
                    if (profit > 0) {
                        record.maxProfit = profit;
                        record.bestBuyTimestamp = record.minBuyTimestamp;
                        record.bestSellTimestamp = timestamp;
                        record.status = TimeStatus::Complete;
                    }
                }
                break;
            case TimeStatus::Complete:
                if (isBuy) {
                    if (order.priceLimit > record.minBuyPrice) {
                        int profit = order.priceLimit - record.minBuyPrice;
                        if (profit > record.maxProfit) {
                            record.maxProfit = profit;
                            record.bestSellTimestamp = timestamp;
                        }
                    }
                } else {
                    if (order.priceLimit < record.minBuyPrice) {
                        record.status = TimeStatus::Potential;
                        record.potentialBuyPrice = order.priceLimit;
                        record.potentialBuyTimestamp = timestamp;
                    }
                }
                break;
            case TimeStatus::Potential:
                if (isBuy) {
                    int potentialProfit = order.priceLimit - record.potentialBuyPrice;
                    if (potentialProfit > 0 && potentialProfit > record.maxProfit) {
                        record.maxProfit = potentialProfit;
                        record.minBuyPrice = record.potentialBuyPrice;
                        record.bestBuyTimestamp = record.potentialBuyTimestamp;
                        record.bestSellTimestamp = timestamp;
                        record.status = TimeStatus::Complete;
                    }
                } else {
                    if (order.priceLimit < record.potentialBuyPrice) {
                        record.potentialBuyPrice = order.priceLimit;
                        record.potentialBuyTimestamp = timestamp;
                    }
                }
                break;
        }
    }

void StockMarket::printTimeTravelers() {
        cout << "---Time Travelers---\n";
        for (int stockID = 0; stockID < numStocks; ++stockID) {
            TimeTravelerRecord& record = timeTravelerRecords[stockID];
            if (record.maxProfit > 0) {
                cout << "A time traveler would buy Stock " << stockID << " at time " << record.bestBuyTimestamp
                    << " for $" << record.minBuyPrice << " and sell it at time " << record.bestSellTimestamp
                    << " for $" << (record.minBuyPrice + record.maxProfit) << "\n";
            } else {
                cout << "A time traveler could not make a profit on Stock " << stockID << "\n";
            }
        }
    }





int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false);
    StockMarket market;
    market.readOptions(argc, argv);
    market.readOrders();

    // first version,may be changed
    if (market.medianFlag) {
        market.printMedian();
    }

    cout << "---End of Day---\n";
    cout << "Trades Completed: " << market.tradesCompleted << "\n";
    
    if (market.traderInfoFlag) {
        market.printTraderInfo();
    }
    
    if (market.timeTravelersFlag) {
        market.printTimeTravelers();
    } 
    return 0;
}
