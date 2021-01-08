# Blockchain
Simple blockchain with PoW proof written on C++

### Capabilities
This program provides only a Command Line interface (I'll think about adding Qt GUI). You can add tansactions into the network, add (mine) blocks and view specific blocks in blockchain. When block become mined program send current blockchain to all peers in network (localhost for now) and if sent blockchain passed their check it replace previous one. This tool use Proof of Work to mine block with given difficulty. Transactions are signed with RSA keys.

### Requirements
- C++14 (or higher)
- Boost
- OpenSSL
- Crypto++
- [JSON library](https://github.com/nlohmann/json)

**cryptopp folder must be in the same dirrectory with other files**. You can run these commands to do that:
```
git clone https://github.com/weidai11/cryptopp.git
cd cryptopp
cp TestScripts/cryptest-cmake.sh . ./cryptest-cmake.sh
```

#### Conflicts in different chains

There can only be one explicit set of blocks in the chain at a given time. If there are conflicts(e.g. when the chains at different nodes have the same size but have different blocks), the longest chain is chosen.

So if some other node sends in a new chain that's longer than yours, your chain is replaced.

## BlockChain Object

#### Private Variables:

- blockchain(vector<unique_ptr<Block>>): vector of smart pointers to Block objects
- blockchain(KeyPairHex): pair of public and private key

Genesis Block is created during intialization.

#### Validating the integrity of blocks

Every time you want to add a block to the blockchain, you will need to provide it:

`int index, string prevHash, string hash, string nonce, vector<string> &merkle`, vector<Transaction> tr, string miner, bool self

- index: index of the block
- prevHash: hash of the previous block
- nonce: self-explantory
- merkle: vector holding in the data of the block
- tr: vector with the data about the transactions
- miner: public key of user mined this block
- self: technical variable which equals "false" when addBlock function called from any json-handler

It will then check whether you have the correct hash(it rehashes the information given), if you have "00" in front, and whether your index is correct. It'll check transactions correctness as well.

## PoW Mining

first n characters of the hash must be 0

- e.g. `003d9dc40cad6b414d45555e4b83045cfde74bcee6b09fb42536ca2500087fd9` works with n = 2

Note: this is the only way to add to the blockchain.

## Block Object

Hash header: index + prevHash + merkleRoot(data) + nonce

#### Private Variables:

- index
- Data: vector of strings
- previousHash
- blockHash
- nonce

For a block to be immutable, its properties are private and there are only methods that return them but not update them.

## Transaction Object

#### Private Variables:

- sender: sender's public key
- receiver: receiver's public key
- signature: string - RSA sign of transaction signed by sender's private key
- amount: transaction's amount
- isReward: bool variable which is "true" if this transaction has no sender and it is an reward for miner

There is a mechanism to check if transaction is correct via checking signature.

## Common Functions

#### getMerkleRoot(const vector<string> &merkle)

- gets merkle root based on elements of a vector

#### findHash(int index, string prevHash, vector<string> &merkle)

- "Mining" part
- finds hash and returns a std::pair of the hash found and nonce used to find it

## Borrowings

In this project I took a lot of code from [ya332 repo](https://github.com/ya332/Simple-Blockchain-Implementation). Also I took several concepts from [this repo](https://github.com/ayushmishra2005/rust_blockchain). Very big thanks to their developers!
