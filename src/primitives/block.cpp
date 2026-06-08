// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "primitives/block.h"
#include "hash.h"
#include "hashblock.h"
#include "tinyformat.h"
#include "utilstrencodings.h"
#include "crypto/common.h"
#include "uint512.h"
#include <string.h>
#include <stdint.h>

#ifdef GLOBALDEFINED
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL sph_skein512_context     z_skein;
uint256 CBlockHeader::GetHash() const
{
    return SerializeHash(*this);
}

uint256 CBlockHeader::GetPoWHash() const
{
        //printf("Our block: %s \n", ToString().c_str());
        //printf("Inside block header get PoWHash \n");
        //printf(" hi there %s \n", this->ToString().c_str());
        //printf("Performing pow hash on block of time %i \n", nTime);
        uint256 thash, thash2;
        //scrypt_1024_1_1_256(BEGIN(nVersion), BEGIN(thash));
        skein2hash(&thash2, BEGIN(nVersion));

        //thash = Hash2(BEGIN(nVersion), END(nNonce));
        //thash = Hash2(BEGIN(nVersion), END(nNonce));
        //printf("Thash Out Is: %s \n ",thash.ToString().c_str());
        //printf("Thash2 Out Is: %s \n ",thash2.ToString().c_str());
        return thash2;
}


std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=0x%08x, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%u)\n",
        GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce,
        vtx.size());
    for (unsigned int i = 0; i < vtx.size(); i++)
    {
        s << "  " << vtx[i].ToString() << "\n";
    }
    return s.str();
}
/*
    // header
    int32_t nVersion;
    uint256 hashPrevBlock;
    uint256 hashMerkleRoot;
    uint32_t nTime;
    uint32_t nBits;
    uint32_t nNonce;
*/
std::string CBlockHeader::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlockHeader(ver=0x%08x, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u)\n",
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce);
    return s.str();
}

int64_t GetBlockWeight(const CBlock& block)
{
    // This implements the weight = (stripped_size * 4) + witness_size formula,
    // using only serialization with and without witness data. As witness_size
    // is equal to total_size - stripped_size, this formula is identical to:
    // weight = (stripped_size * 3) + total_size.
    return ::GetSerializeSize(block, SER_NETWORK, PROTOCOL_VERSION | SERIALIZE_TRANSACTION_NO_WITNESS) * (WITNESS_SCALE_FACTOR - 1) + ::GetSerializeSize(block, SER_NETWORK, PROTOCOL_VERSION);
}
