#include "Robonomics.h"
// #include "esp_log.h"
#include <inttypes.h>
#include "address.h"

static const char *TAG = "ROBONOMICS";

void Robonomics::setup() {
    blockchainUtils.setup();
}

void Robonomics::disconnectWebsocket() {
    blockchainUtils.disconnectWebsocket();
}

void Robonomics::setPrivateKey(uint8_t *privateKey) {
    memcpy(privateKey_, privateKey, KEYS_SIZE);
    char* tempAddress = getAddrFromPrivateKey(privateKey_);
    if (tempAddress != nullptr) {
        strncpy(ss58Address, tempAddress, SS58_ADDRESS_SIZE); // Leave space for null terminator
        ss58Address[SS58_ADDRESS_SIZE] = '\0'; // Ensure null termination
        delete[] tempAddress;
        Serial.print("Robonomics Address: ");
        Serial.println(ss58Address);
    } else {
        Serial.println("Failed to get address from public key.");
    }
}

const char* Robonomics::getSs58Address() const {
        return ss58Address;
}

void Robonomics::sendDatalogRecord(std::string data) {
    Data head_dr_  = Data{0x33,0};
    Data call = callDatalogRecord(head_dr_, data);

    createAndSendExtrinsic(call);
}

void Robonomics::sendRWSDatalogRecord(std::string data, const char *owner_address) {
    Data head_dr_ = Data{0x33,0};
    Data head_rws_ = Data{0x37,0};
    Data call_nested = callDatalogRecord(head_dr_, data);
    RobonomicsPublicKey ownerKey = getPublicKeyFromAddr(owner_address);
    Data call = callRws(head_rws_, ownerKey, call_nested);

    createAndSendExtrinsic(call);
}

void Robonomics::createAndSendExtrinsic(Data call) {
    Ed25519::derivePublicKey(publicKey_, privateKey_);

    uint64_t payloadNonce = getNonce(& blockchainUtils, ss58Address);
    std::string payloadBlockHash = getGenesisBlockHash();
    uint32_t payloadEra = getEra();
    uint64_t payloadTip = getTip();
    JSONVar runtimeInfo = getRuntimeInfo(& blockchainUtils);
    uint32_t payloadSpecVersion = getSpecVersion(runtimeInfo);
    uint32_t payloadTransactionVersion = getTransactionVersion(runtimeInfo);
    Serial.printf("Spec version: %u, tx version: %u, nonce: %u, era: %u, tip: %u\n", payloadSpecVersion, payloadTransactionVersion, payloadNonce, payloadEra, payloadTip);
    Data data_ = createPayload(call, payloadEra, payloadNonce, payloadTip, payloadSpecVersion, payloadTransactionVersion, payloadBlockHash, payloadBlockHash);
    Data signature_ = createSignature(data_, privateKey_, publicKey_);
    std::vector<std::uint8_t> pubKey( reinterpret_cast<std::uint8_t*>(std::begin(publicKey_)), reinterpret_cast<std::uint8_t*>(std::end(publicKey_)));
    Data edata_ = createSignedExtrinsic(signature_, pubKey, payloadEra, payloadNonce, payloadTip, call);
    int requestId = blockchainUtils.getRequestId();
    sendExtrinsic(edata_, requestId);
}

Data Robonomics::createPayload(Data call, uint32_t era, uint64_t nonce, uint64_t tip, uint32_t sv, uint32_t tv, std::string gen, std::string block) {
    Data data_ = doPayload (call, era, nonce, tip, sv, tv, gen, block);
    Serial.printf("\nPayload size %d : \n", data_.size());
    // for (int k = 0; k < data_.size(); k++) 
    //     Serial.printf("%02x", data_[k]);
    // Serial.println("");
    return data_;
}

Data Robonomics::createSignature(Data data, uint8_t privateKey[32], uint8_t publicKey[32]) {
    Data signature_ = doSign (data, privateKey, publicKey);
    Serial.printf("\nSignatured size %d : \n", signature_.size());
    // for (int k = 0; k < signature_.size(); k++) 
    //     Serial.printf("%02x", signature_[k]);
    // Serial.println("");
    return signature_;
}

Data Robonomics::createSignedExtrinsic(Data signature, Data pubKey, uint32_t era, uint64_t nonce, uint64_t tip, Data call) {
    Data edata_ = doEncode (signature, pubKey, era, nonce, tip, call);
    Serial.printf("Extrinsic %s: size %d\n", "Datalog", edata_.size());
    // for (int k = 0; k < edata_.size(); k++) 
    //     Serial.printf("%02x", edata_[k]);
    // Serial.println("");
    return edata_;
}

void Robonomics::sendExtrinsic(Data extrinsicData, int requestId) {
    String extrinsicMessage = fillParamsJs(extrinsicData, requestId);
    // Serial.println("After to string: ");
    // Serial.print(extrinsicMessage);
    blockchainUtils.rpcRequest(extrinsicMessage);
    getExstrinsicResult();
}

void Robonomics::getExstrinsicResult() {
    blockchainUtils.setOnTextCallback([this](uint8_t *payload) {getExstrinsicResultCallback(payload);});
    while (!got_extrinsic_result) {
        blockchainUtils.websocketLoop();
    }
    got_extrinsic_result = false;
    // Serial.print("Extrinsic result: ");
    // Serial.println(extrinsicResult);
}

void Robonomics::getExstrinsicResultCallback(uint8_t *payload) {
    Serial.printf("Extrinsic result: %s", (char *)payload);
    JSONVar received_message = JSON.parse((char *)payload);
    extrinsicResult = JSON.stringify(received_message["result"]);
    got_extrinsic_result = true;
}