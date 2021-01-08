//author: tko
#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include "json.hh"
#include "Transaction.hpp"

using json = nlohmann::json;

class Block {
    public:
        Block(int index, string prevHas, string hash, string nonce, vector<string> data,
              vector<Transaction> t);
        string getPreviousHash();
        string getHash();
        string getNonce();
        int getIndex() const;
        vector<string> getData();
        bool isValidBlock();
        void addTransaction(Transaction);
        string miner();

        void toString();
        json toJSON();

        vector<Transaction> transactions;
    private:
        int index;
        string previousHash;
        string blockHash;
        string nonce;
        vector<string> data;
        // string getMerkleRoot(const vector<string> &merkle);
};
// Constructor
Block::Block(int index, string prevHash, string hash, string nonce, vector<string> data,
             vector<Transaction> t) {
    printf("\nInitializing Block: %d ---- Hash: %s \n", index,hash.c_str());
    this -> previousHash = prevHash;
    this -> data = data;
    this -> index = index;
    this -> nonce = nonce;
    this -> blockHash = hash;
    this -> transactions = t;

}

int Block::getIndex() const {
    return this -> index;
}

string Block::getPreviousHash() {
    return this -> previousHash;
}

string Block::getHash() {
    return this -> blockHash;
}

string Block::getNonce() {
    return this->nonce;
}

vector<string> Block::getData(){
    return this -> data;
}

// Prints Block data
void Block::toString() {
    string tr = "[\n";
    for (auto& q: this->transactions) {
        tr += q.asString() + "\n";
    }
    tr += "];\n";

    string dataString;
    for (auto & i : data)
        dataString += i + ", ";
    printf("\n-------------------------------\n");
    printf("Block %d\nHash: %s\nPrevious Hash: %s\nContents: %s\nTransactions: %s",
        index,this->blockHash.c_str(),this->previousHash.c_str(),dataString.c_str(),tr.c_str());
    printf("\n-------------------------------\n");
}
json Block::toJSON() {
    json j;
    j["index"] = this->index;
    j["hash"] = this->blockHash;
    j["previousHash"] = this->previousHash;
    j["nonce"] = this->nonce;
    j["data"] = this->data;

    string tr;
    for (auto& q: this->transactions) {
        string sen = q.getSender();
        string rec = q.getReceiver();
        string sig = q.getSign();
        string amount = to_string(q.getAmount());
        string isrew = to_string(q.isMinerReward());
        tr += sen + ";" + rec + ";" + sig + ";" + amount + ";" + isrew + "|";
    }

    j["transactions"] = tr;

    j["miner"] = miner();

    return j;
}

bool Block::isValidBlock() {
    for (auto& tran: this->transactions) {
        if (!tran.isValidTransaction()) {
            return false;
        }
    }
    return true;
}

void Block::addTransaction(Transaction t) {
    if (t.isValidTransaction()) {
        transactions.push_back(t);
    }
}

string Block::miner() {
    return this->transactions[this->transactions.size() - 1].getReceiver();
}

#endif