#ifndef TRANSACTION_H

#include <hex.h>
#include <osrng.h>
#include <pssr.h>
#include <rsa.h>
#include <whrlpool.h>
#include <base64.h>

#include <utility>

using Verifier = CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::Whirlpool>::Verifier;
using Signer = CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::Whirlpool>::Signer;

class Transaction {
public:
    Transaction(string, string, int64_t);
    Transaction(string, string, string, int64_t);
    Transaction(string, int64_t);
    void signTransaction(const string&);
    bool isValidTransaction();
    string asString();
    string getReceiver();
    string getSender();
    string getSign();
    int64_t getAmount();

    bool isMinerReward();

private:
    string sender;
    string receiver;
    string signature;
    int64_t amount;
    string getHash();

    bool isReward = false;
};

string Transaction::asString() {
    return "\tSender: " + this->sender + "; Receiver: " + this->receiver + "; Amount: " + to_string(this->amount);
}

string Transaction::getHash() {
    std::string digest;
    CryptoPP::SHA256 hash;

    CryptoPP::StringSource foo(this->asString(), true,
                               new CryptoPP::HashFilter(hash,
                                                        new CryptoPP::Base64Encoder (
                                                                new CryptoPP::StringSink(digest))));

    return digest;
}

Transaction::Transaction(string sender, string receiver, int64_t amount) {
    this->sender = std::move(sender);
    this->receiver = std::move(receiver);
    this->amount = amount;
}

Transaction::Transaction(string sender, string receiver, string signature, int64_t amount) {
    this->sender = std::move(sender);
    this->receiver = std::move(receiver);
    this->signature = std::move(signature);
    this->amount = amount;
}

void Transaction::signTransaction(const string& pr) {
    CryptoPP::RSA::PrivateKey privateKey;
    privateKey.Load(CryptoPP::StringSource(pr, true,
                                           new CryptoPP::HexDecoder()).Ref());

    // sign message
    std::string sig;
    Signer signer(privateKey);
    CryptoPP::AutoSeededRandomPool rng;

    CryptoPP::StringSource ss(this->getHash(), true,
                              new CryptoPP::SignerFilter(rng, signer,
                                                         new CryptoPP::HexEncoder(
                                                                 new CryptoPP::StringSink(sig))));

    this->signature = sig;
}

bool Transaction::isValidTransaction() {
    if (isReward) {
        return true;  // checking for reward correctness?
    }

    string message = this->getHash();

    CryptoPP::RSA::PublicKey publicKey;
    publicKey.Load(CryptoPP::StringSource(this->sender, true,
                                          new CryptoPP::HexDecoder()).Ref());

    // decode signature
    std::string decodedSignature;
    CryptoPP::StringSource ss(this->signature, true,
                              new CryptoPP::HexDecoder(
                                      new CryptoPP::StringSink(decodedSignature)));

    // verify message
    bool result = false;
    Verifier verifier(publicKey);
    CryptoPP::StringSource ss2(decodedSignature + message, true,
                               new CryptoPP::SignatureVerificationFilter(verifier,
                                                                         new CryptoPP::ArraySink(
                                                                                 (CryptoPP::byte *) &result,
                                                                                 sizeof(result))));

    return result;
}

string Transaction::getReceiver() {
    return receiver;
}

string Transaction::getSender() {
    return sender;
}

string Transaction::getSign() {
    return signature;
}

int64_t Transaction::getAmount() {
    return amount;
}

bool Transaction::isMinerReward() {
    return this->isReward;
}

Transaction::Transaction(string rec, int64_t amount) {
    this->amount = amount;
    this->receiver = std::move(rec);
    this->isReward = true;
    this->sender = "";
}

#endif