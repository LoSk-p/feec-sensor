#pragma once

#define KEYS_SIZE 32
#define SS58_ADDRESS_SIZE 48

#define POLKADOT_PARACHAIN
// #define KUSAMA_PARACHAIN

// Kusama Parachain
#ifdef KUSAMA_PARACHAIN

#define WS_SERVER_HOST "kusama.rpc.robonomics.network"
#define GENESIS_BLOCK_HASH "631ccc82a078481584041656af292834e1ae6daab61d2875b4dd0c14bb9b17bc"

#endif

// Polkadot Parachain

#ifdef POLKADOT_PARACHAIN

#define WS_SERVER_HOST "polkadot.rpc.robonomics.network"
#define GENESIS_BLOCK_HASH "29f4371dcc41045f5041489dfcd51389bf8ccd2161332e0de1ca803bcc3ee872"

#endif

#define WS_SERVER_PORT 443

#define SIGNATURE_SIZE 64
