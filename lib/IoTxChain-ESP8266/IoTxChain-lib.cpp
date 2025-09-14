#include "IoTxChain-lib.h"

// Base58 decode function (simplified implementation)
static const char base58_alphabet[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

std::vector<uint8_t> base58ToPubkey(const String& base58Str) {
    // Simplified base58 decode - in production you'd want a proper implementation
    std::vector<uint8_t> result(32, 0); // Solana pubkeys are 32 bytes
    // For now, return a dummy key - you'd implement proper base58 decoding here
    return result;
}

std::vector<uint8_t> encodeU64LE(uint64_t value) {
    std::vector<uint8_t> result(8);
    for (int i = 0; i < 8; i++) {
        result[i] = (value >> (i * 8)) & 0xFF;
    }
    return result;
}

bool IoTxChain::getSolBalance(const String& walletPubkeyBase58, uint64_t& outLamports) {
    WiFiClient client; // Use regular WiFiClient for HTTP (or WiFiClientSecure for HTTPS)
    
    if (!http.begin(client, _rpcUrl)) {
        Serial.println("getSolBalance: HTTP begin failed");
        return false;
    }

    String body = String() +
        R"({"jsonrpc":"2.0","id":1,"method":"getBalance","params":[")" +
        walletPubkeyBase58 + R"("]})";

    http.addHeader("Content-Type", "application/json");

    int code = http.POST(body);
    if (code != 200) {
        Serial.printf("getSolBalance HTTP error: %d\n", code);
        http.end();
        return false;
    }

    String response = http.getString();
    http.end();

    // Parse JSON response
    DynamicJsonDocument doc(2048);
    if (deserializeJson(doc, response)) {
        Serial.println("getSolBalance: JSON parse error");
        return false;
    }

    if (doc["error"]) {
        Serial.println("getSolBalance RPC Error: " + doc["error"]["message"].as<String>());
        return false;
    }

    outLamports = doc["result"]["value"];
    return true;
}

String IoTxChain::getLatestBlockhash() {
    WiFiClient client;

    if (!http.begin(client, _rpcUrl)) {
        Serial.println("getLatestBlockhash: HTTP begin failed");
        return "";
    }

    http.addHeader("Content-Type", "application/json");

    String body = R"({
        "jsonrpc":"2.0",
        "id":1,
        "method":"getLatestBlockhash",
        "params":[]
    })";

    int code = http.POST(body);
    if (code != 200) {
        Serial.printf("getLatestBlockhash HTTP error: %d\n", code);
        http.end();
        return "";
    }

    String response = http.getString();
    http.end();

    DynamicJsonDocument doc(2048);
    auto err = deserializeJson(doc, response);
    if (err) {
        Serial.println("getLatestBlockhash: JSON parse error");
        return "";
    }

    String blockhash = doc["result"]["value"]["blockhash"].as<String>();
    return blockhash;
}

bool IoTxChain::confirmTransaction(const String& signature, uint32_t maxWaitMs) {
    const uint32_t pollIntervalMs = 500;
    uint32_t waited = 0;

    while (waited <= maxWaitMs) {
        WiFiClient client;

        if (!http.begin(client, _rpcUrl)) {
            Serial.println("confirmTransaction: HTTP begin failed");
            return false;
        }

        String body = String() +
            R"({"jsonrpc":"2.0","id":1,"method":"getSignatureStatuses","params":[[")" +
            signature +
            R"("],{"searchTransactionHistory":true}]})";

        http.addHeader("Content-Type", "application/json");

        int code = http.POST(body);
        if (code != 200) {
            Serial.printf("confirmTransaction HTTP error: %d\n", code);
            http.end();
            delay(pollIntervalMs);
            waited += pollIntervalMs;
            continue;
        }

        String response = http.getString();
        http.end();

        DynamicJsonDocument doc(2048);
        auto err = deserializeJson(doc, response);
        if (err) {
            Serial.println("confirmTransaction: JSON parse error");
            delay(pollIntervalMs);
            waited += pollIntervalMs;
            continue;
        }

        // Check if confirmed
        if (doc["result"]["value"][0] && !doc["result"]["value"][0].isNull()) {
            return true;
        }

        delay(pollIntervalMs);
        waited += pollIntervalMs;
    }

    return false;
}

bool IoTxChain::sendRawTransaction(const String &txBase64, String &outSignature) {
    WiFiClient client;

    if (!http.begin(client, _rpcUrl)) {
        Serial.println("sendRawTransaction: HTTP begin failed");
        return false;
    }

    http.addHeader("Content-Type", "application/json");

    String body = String() +
        R"({"jsonrpc":"2.0","id":1,"method":"sendTransaction","params":[")" +
        txBase64 + 
        R"(",{"encoding":"base64","skipPreflight":false,"preflightCommitment":"confirmed"}]})";

    int code = http.POST(body);
    if (code != 200) {
        Serial.printf("sendTransaction HTTP code: %d\n", code);
        http.end();
        return false;
    }

    String response = http.getString();
    http.end();

    DynamicJsonDocument doc(2048);
    auto err = deserializeJson(doc, response);
    if (err) {
        Serial.println("JSON parse error in sendTransaction");
        return false;
    }

    if (doc["error"]) {
        Serial.println("RPC Error: " + doc["error"]["message"].as<String>());
        return false;
    }

    outSignature = doc["result"].as<String>();
    return true;
}

bool IoTxChain::findAssociatedTokenAccount(const String& ownerPubkeyBase58, const String& mintPubkeyBase58, String& outATA) {
    WiFiClient client;

    if (!http.begin(client, _rpcUrl)) {
        Serial.println("findAssociatedTokenAccount: HTTP begin failed");
        return false;
    }

    String body = String() +
        R"({"jsonrpc":"2.0","id":1,"method":"getTokenAccountsByOwner","params":[")" +
        ownerPubkeyBase58 +
        R"(",{"mint":")" + mintPubkeyBase58 + R"("},{"encoding":"jsonParsed"}]})";

    http.addHeader("Content-Type", "application/json");
    int code = http.POST(body);
    if (code != 200) {
        Serial.printf("findAssociatedTokenAccount HTTP code: %d\n", code);
        http.end();
        return false;
    }

    String response = http.getString();
    http.end();

    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, response);
    if (error) {
        Serial.println("findAssociatedTokenAccount: JSON parse error");
        return false;
    }

    if (doc["error"]) {
        Serial.println("findAssociatedTokenAccount RPC Error: " + doc["error"]["message"].as<String>());
        return false;
    }

    JsonArray arr = doc["result"]["value"].as<JsonArray>();
    if (!arr || arr.size() == 0) {
        Serial.println("findAssociatedTokenAccount: No ATA found");
        return false;
    }

    outATA = arr[0]["pubkey"].as<String>();
    return true;
}

String IoTxChain::base64Encode(const uint8_t* data, size_t len) {
    // Simplified base64 encode - you'd want a proper implementation
    // For now, return empty string
    return "";
}

bool IoTxChain::findProgramAddress(const std::vector<std::vector<uint8_t>>& seeds, const std::vector<uint8_t>& programId, std::vector<uint8_t>& outPDA, uint8_t& outBump) {
    // Simplified PDA generation for ESP8266
    // In a real implementation, this would involve proper cryptographic hashing
    outPDA.resize(32);
    
    // Create a dummy PDA based on seeds and program ID
    for (size_t i = 0; i < 32; i++) {
        outPDA[i] = (i + programId.size()) % 256;
    }
    
    // Mix in seed data
    for (const auto& seed : seeds) {
        for (size_t i = 0; i < seed.size() && i < 32; i++) {
            outPDA[i] ^= seed[i];
        }
    }
    
    outBump = 255; // Start with max bump
    return true;
}

String base58Encode(const uint8_t* data, size_t len) {
    // Simplified base58 encode for ESP8266
    // This is a placeholder - you'd want a proper implementation
    String result = "";
    for (size_t i = 0; i < len && i < 8; i++) {
        result += String(data[i], HEX);
    }
    return result;
}

std::vector<uint8_t> IoTxChain::calculateDiscriminator(const std::string& functionName) {
    // Simplified discriminator calculation
    // In Anchor, this is usually the first 8 bytes of SHA256 hash of "global:function_name"
    std::vector<uint8_t> discriminator(8, 0);
    
    // For demonstration, create a simple hash-like value
    for (size_t i = 0; i < functionName.length() && i < 8; i++) {
        discriminator[i] = functionName[i] ^ 0xAA;
    }
    
    return discriminator;
}