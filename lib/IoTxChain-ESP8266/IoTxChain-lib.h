#ifndef IoTxChain_LIB_H
#define IoTxChain_LIB_H

#include <Arduino.h>
#include <vector>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>  // Changed for ESP8266
#include <ArduinoJson.h>
#include <vector>

// For ESP8266, we'll use software-based crypto implementations
// Since some libraries might not be available, we'll create simplified versions

// Custom Base58 encode/decode functions
// #include "base58.h"  // We'll implement basic versions inline

std::vector<uint8_t> base58ToPubkey(const String& base58Str);

struct EpochInfo {
    uint64_t absoluteSlot;
    uint64_t blockHeight;
    uint64_t epoch;
    uint64_t slotIndex;
    uint64_t slotsInEpoch;
};

// Simplified Solana structures for ESP8266
struct Pubkey {
    std::vector<uint8_t> data;

    static Pubkey fromBase58(const String& str) {
        Pubkey pk;
        pk.data = base58ToPubkey(str);
        return pk;
    }
};

struct Keypair {
    std::vector<uint8_t> publicKey;
    std::vector<uint8_t> privateKey;

    static Keypair fromPrivateKey(const uint8_t* privKey) {
        Keypair kp;
        kp.privateKey.assign(privKey, privKey + 64);
        // In a real implementation, derive public key from private key
        return kp;
    }
};

struct AccountMeta {
    Pubkey pubkey;
    bool isSigner;
    bool isWritable;

    AccountMeta(const Pubkey& pk, bool signer, bool writable) 
        : pubkey(pk), isSigner(signer), isWritable(writable) {}

    static AccountMeta writable(const Pubkey& pk, bool signer) {
        return AccountMeta(pk, signer, true);
    }

    static AccountMeta signer(const Pubkey& pk) {
        return AccountMeta(pk, true, false);
    }
};

struct Instruction {
    Pubkey programId;
    std::vector<AccountMeta> accounts;
    std::vector<uint8_t> data;

    Instruction(const Pubkey& progId, const std::vector<AccountMeta>& accts, const std::vector<uint8_t>& instructionData)
        : programId(progId), accounts(accts), data(instructionData) {}
};

struct Transaction {
    Pubkey fee_payer;
    String recent_blockhash;
    std::vector<Instruction> instructions;
    std::vector<uint8_t> signature;

    void add(const Instruction& ix) {
        instructions.push_back(ix);
    }

    void sign(const std::vector<Keypair>& signers) {
        // Simplified signing - in real implementation this would be complex
        signature.resize(64, 0x00);
    }

    String serializeBase64() const {
        // Simplified serialization
        return ""; // Would implement actual serialization here
    }

private:
    std::vector<uint8_t> serializeMessage() const {
        // Simplified message serialization
        return std::vector<uint8_t>();
    }
};

class IoTxChain {
public:
    explicit IoTxChain(const String& rpcUrl) : _rpcUrl(rpcUrl) {}

    // Core blockchain functions
    bool getSolBalance(const String& walletPubkeyBase58, uint64_t& outLamports);
    String getLatestBlockhash();
    bool confirmTransaction(const String& signature, uint32_t maxWaitMs = 5000);
    
    // Transaction functions
    bool sendRawTransaction(const String &txBase64, String &outSignature);
    
    // Token functions
    bool findAssociatedTokenAccount(const String& ownerPubkeyBase58, const String& mintPubkeyBase58, String& outATA);
    
    // PDA functions
    bool findProgramAddress(const std::vector<std::vector<uint8_t>>& seeds, const std::vector<uint8_t>& programId, std::vector<uint8_t>& outPDA, uint8_t& outBump);
    
    // Utility functions
    String base64Encode(const uint8_t* data, size_t len);
    std::vector<uint8_t> calculateDiscriminator(const std::string& functionName);

private:
    String _rpcUrl;
    HTTPClient http;  // Using ESP8266HTTPClient
};

// Utility functions
std::vector<uint8_t> encodeU64LE(uint64_t value);
String base58Encode(const uint8_t* data, size_t len);

#endif // IoTxChain_LIB_H