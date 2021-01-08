//author: tko
#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <iostream>
#include <string>
#include "hash.hpp"
#include <utility>
#include <vector>
#include <memory>
#include <stdexcept>

#include "json.hh"
using json = nlohmann::json;


struct KeyPairHex {
    std::string publicKey;
    std::string privateKey;
};


class BlockChain {
public:
    explicit BlockChain(KeyPairHex, int genesis = 1);
    Block getBlock(int index);
    // getBlock(string hash); //not implemented
    int getNumOfBlocks();
    int addBlock(int index, const string& prevHash, const string& hash, const string& nonce,
                 vector<string> merkle, vector<Transaction>, string, bool self = true);
    string getLatestBlockHash();
    // void toString(void);
    string toJSON();
    int replaceChain(json chain);
    bool isValidChain();
    vector<Transaction> transactions;
    void addTransaction(Transaction);
private:
    KeyPairHex mn;  // TODO FIIIIIIIX  line 117 & 155 :(
    vector<unique_ptr<Block> > blockchain; //vector that is the blockchain
};

// If integer passed into constructor is 0, it the first node and creates the genesis block
BlockChain::BlockChain(KeyPairHex miner, int genesis){
    this->mn = miner;

    if (genesis == 0) {
        vector<string> v;
        v.emplace_back("Genesis Block!");
        // string header = to_string(0) + string("00000000000000") + getMerkleRoot(v);
        auto hash_nonce_pair = findHash(0,string("00000000000000"), v);

        Transaction tr(miner.publicKey, MINING_REWARD);

        this -> blockchain.push_back(std::make_unique<Block>(0, string("00000000000000"),
                                                             hash_nonce_pair.first,
                                                             hash_nonce_pair.second,v,
                                                             vector<Transaction>{tr}));
        printf("Created blockchain!\n");
    }
}
// Gets block based on the index
Block BlockChain::getBlock(int index) {
    for (auto & i : blockchain){
        if (i->getIndex() == index) {
            return *i;
        }
    }
    throw invalid_argument("Index does not exist.");
}

// returns number of blocks
int BlockChain::getNumOfBlocks() {
    return this -> blockchain.size();
}

// checks whether data fits with the right hash -> add block
int BlockChain::addBlock(int index, const string& prevHash, const string& hash, const string& nonce,
                         vector<string> merkle, vector<Transaction> tr, string miner, bool self) {
    string header = to_string(index) + prevHash + getMerkleRoot(merkle) + nonce;
    if ((sha256(header) == hash) &&
    (hash.substr(0, MINING_DIFFICULTY) == std::string(MINING_DIFFICULTY, '0') ) &&
    (index == blockchain.size())) {
        printf("Block hashes match --- Adding Block %s \n",hash.c_str());

        if (self) {
            Transaction t(miner, MINING_REWARD);

            tr.push_back(t);
        }

        this->blockchain.push_back(std::make_unique<Block>(index, prevHash, hash, nonce, merkle,
                                                           tr));
        if (self) {
            this->transactions.clear();
        }
        return 1;
    }
    cout << "Hash doesn't match criteria\n";
    return 0;
}

// returns hash of the latest block, used for finding the previousHash when mining new block
string BlockChain::getLatestBlockHash() {
    return this->blockchain[blockchain.size()-1]->getHash();
}

// returns JSON string of JSON - used to send to network
string BlockChain::toJSON() {
    json j;
    j["length"] = this->blockchain.size();
    for (auto & i : this->blockchain){
        j["data"][i->getIndex()] = i->toJSON();
    }
    return j.dump(3);
}

// replaces Chain with new chain represented by a JSON, used when node sends new blockchain
int BlockChain::replaceChain(json chain) {
    BlockChain copy(this->mn);
    copy.addBlock(0, this->blockchain[0]->getPreviousHash(),
                  this->blockchain[0]->getHash(),this->blockchain[0]->getNonce(),
                  this->blockchain[0]->getData(),this->blockchain[0]->transactions,
                  this->blockchain[0]->miner(), false);

    //remove all blocks except for the first block

    for (int a = 1; a <chain["length"].get<int>(); a++ ){
        auto block = chain["data"][a];
        vector<string> data = block["data"].get<vector<string> >();

        vector<Transaction> v;
        string trans = block["transactions"];
        auto o = split(trans, '|');
        for (auto x: o) {
            auto o_ = split(x, ';');
            if (o_[4] == "1") {
                v.emplace_back(o_[1], std::stoi(o_[3]));
            } else {
                v.emplace_back(o_[0], o_[1], o_[2], std::stoi(o_[3]));
            }
        }

        copy.addBlock(block["index"],block["previousHash"],block["hash"],
                       block["nonce"],data, v, block["miner"], false);
    }

    cout << "Validating blockchian... ";
    if (copy.isValidChain()) {
        cout << "Done!\n";
        while (this->blockchain.size() > 1){
            this->blockchain.pop_back();
        }

        for (int index = 1; index < copy.blockchain.size(); ++index) {
            this->addBlock(copy.blockchain[index]->getIndex(),
                           copy.blockchain[index]->getPreviousHash(),
                           copy.blockchain[index]->getHash(),
                           copy.blockchain[index]->getNonce(),
                           copy.blockchain[index]->getData(),
                           copy.blockchain[index]->transactions,
                           copy.blockchain[index]->miner(), false);
        }
        cout << "----Replaced current chain with new one" << endl;
    } else {
        cout << "Checking failed!" << endl;
    }
    return 1;
}

bool BlockChain::isValidChain() {
    for (auto& block: this->blockchain) {
        if (!block->isValidBlock()) {
            return false;
        }
    }
    return true;
}

void BlockChain::addTransaction(Transaction t) {
    cout << "Verifying transaction... ";
    if (t.isValidTransaction()) {
        cout << "Done." << std::endl;
        transactions.push_back(t);
        cout << "Transaction added!" << std::endl;
    } else {
        cout << "Invalid transaction. Abort" << std::endl;
    }
}

#endif