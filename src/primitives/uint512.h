// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_UINT512_H
#define BITCOIN_UINT512_H

#include <assert.h>
#include <cstring>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <vector>
#include "crypto/common.h"


typedef long long  int64;
typedef unsigned long long  uint64;

/** Base class without constructors for uint256 and uint160.
 * This makes the compiler let you use it in a union.
 */
template<unsigned int BITS>
class base_uint
{
protected:
    enum { WIDTH=BITS/32 };
    uint32_t data[WIDTH];
public:

    bool operator!() const
    {
        for (int i = 0; i < WIDTH; i++)
            if (data[i] != 0)
                return false;
        return true;
    }

    const base_uint operator~() const
    {
        base_uint ret;
        for (int i = 0; i < WIDTH; i++)
            ret.data[i] = ~data[i];
        return ret;
    }

    const base_uint operator-() const
    {
        base_uint ret;
        for (int i = 0; i < WIDTH; i++)
            ret.data[i] = ~data[i];
        ret++;
        return ret;
    }

    double getdouble() const
    {
        double ret = 0.0;
        double fact = 1.0;
        for (int i = 0; i < WIDTH; i++) {
            ret += fact * data[i];
            fact *= 4294967296.0;
        }
        return ret;
    }

    base_uint& operator=(uint64 b)
    {
        data[0] = (unsigned int)b;
        data[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            data[i] = 0;
        return *this;
    }

    base_uint& operator^=(const base_uint& b)
    {
        for (int i = 0; i < WIDTH; i++)
            data[i] ^= b.data[i];
        return *this;
    }

    base_uint& operator&=(const base_uint& b)
    {
        for (int i = 0; i < WIDTH; i++)
            data[i] &= b.data[i];
        return *this;
    }

    base_uint& operator|=(const base_uint& b)
    {
        for (int i = 0; i < WIDTH; i++)
            data[i] |= b.data[i];
        return *this;
    }

    base_uint& operator^=(uint64 b)
    {
        data[0] ^= (unsigned int)b;
        data[1] ^= (unsigned int)(b >> 32);
        return *this;
    }

    base_uint& operator|=(uint64 b)
    {
        data[0] |= (unsigned int)b;
        data[1] |= (unsigned int)(b >> 32);
        return *this;
    }

    base_uint& operator<<=(unsigned int shift)
    {
        base_uint a(*this);
        for (int i = 0; i < WIDTH; i++)
            data[i] = 0;
        int k = shift / 32;
        shift = shift % 32;
        for (int i = 0; i < WIDTH; i++)
        {
            if (i+k+1 < WIDTH && shift != 0)
                data[i+k+1] |= (a.data[i] >> (32-shift));
            if (i+k < WIDTH)
                data[i+k] |= (a.data[i] << shift);
        }
        return *this;
    }

    base_uint& operator>>=(unsigned int shift)
    {
        base_uint a(*this);
        for (int i = 0; i < WIDTH; i++)
            data[i] = 0;
        int k = shift / 32;
        shift = shift % 32;
        for (int i = 0; i < WIDTH; i++)
        {
            if (i-k-1 >= 0 && shift != 0)
                data[i-k-1] |= (a.data[i] << (32-shift));
            if (i-k >= 0)
                data[i-k] |= (a.data[i] >> shift);
        }
        return *this;
    }

    base_uint& operator+=(const base_uint& b)
    {
        uint64 carry = 0;
        for (int i = 0; i < WIDTH; i++)
        {
            uint64 n = carry + data[i] + b.data[i];
            data[i] = n & 0xffffffff;
            carry = n >> 32;
        }
        return *this;
    }

    base_uint& operator-=(const base_uint& b)
    {
        *this += -b;
        return *this;
    }

    base_uint& operator+=(uint64 b64)
    {
        base_uint b;
        b = b64;
        *this += b;
        return *this;
    }

    base_uint& operator-=(uint64 b64)
    {
        base_uint b;
        b = b64;
        *this += -b;
        return *this;
    }


    base_uint& operator++()
    {
        // prefix operator
        int i = 0;
        while (++data[i] == 0 && i < WIDTH-1)
            i++;
        return *this;
    }

    const base_uint operator++(int)
    {
        // postfix operator
        const base_uint ret = *this;
        ++(*this);
        return ret;
    }

    base_uint& operator--()
    {
        // prefix operator
        int i = 0;
        while (--data[i] == -1 && i < WIDTH-1)
            i++;
        return *this;
    }

    const base_uint operator--(int)
    {
        // postfix operator
        const base_uint ret = *this;
        --(*this);
        return ret;
    }


    friend inline bool operator<(const base_uint& a, const base_uint& b)
    {
        for (int i = base_uint::WIDTH-1; i >= 0; i--)
        {
            if (a.data[i] < b.data[i])
                return true;
            else if (a.data[i] > b.data[i])
                return false;
        }
        return false;
    }

    friend inline bool operator<=(const base_uint& a, const base_uint& b)
    {
        for (int i = base_uint::WIDTH-1; i >= 0; i--)
        {
            if (a.data[i] < b.data[i])
                return true;
            else if (a.data[i] > b.data[i])
                return false;
        }
        return true;
    }

    friend inline bool operator>(const base_uint& a, const base_uint& b)
    {
        for (int i = base_uint::WIDTH-1; i >= 0; i--)
        {
            if (a.data[i] > b.data[i])
                return true;
            else if (a.data[i] < b.data[i])
                return false;
        }
        return false;
    }

    friend inline bool operator>=(const base_uint& a, const base_uint& b)
    {
        for (int i = base_uint::WIDTH-1; i >= 0; i--)
        {
            if (a.data[i] > b.data[i])
                return true;
            else if (a.data[i] < b.data[i])
                return false;
        }
        return true;
    }

    friend inline bool operator==(const base_uint& a, const base_uint& b)
    {
        for (int i = 0; i < base_uint::WIDTH; i++)
            if (a.data[i] != b.data[i])
                return false;
        return true;
    }

    friend inline bool operator==(const base_uint& a, uint64 b)
    {
        if (a.data[0] != (unsigned int)b)
            return false;
        if (a.data[1] != (unsigned int)(b >> 32))
            return false;
        for (int i = 2; i < base_uint::WIDTH; i++)
            if (a.data[i] != 0)
                return false;
        return true;
    }

    friend inline bool operator!=(const base_uint& a, const base_uint& b)
    {
        return (!(a == b));
    }

    friend inline bool operator!=(const base_uint& a, uint64 b)
    {
        return (!(a == b));
    }



    std::string GetHex() const
    {
        char psz[sizeof(data)*2 + 1];
        for (unsigned int i = 0; i < sizeof(data); i++)
            sprintf(psz + i*2, "%02x", ((unsigned char*)data)[sizeof(data) - i - 1]);
        return std::string(psz, psz + sizeof(data)*2);
    }

    void SetHex(const char* psz)
    {
        for (int i = 0; i < WIDTH; i++)
            data[i] = 0;

        // skip leading spaces
        while (isspace(*psz))
            psz++;

        // skip 0x
        if (psz[0] == '0' && tolower(psz[1]) == 'x')
            psz += 2;

        // hex string to uint
        static const unsigned char phexdigit[256] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0, 0,0xa,0xb,0xc,0xd,0xe,0xf,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0xa,0xb,0xc,0xd,0xe,0xf,0,0,0,0,0,0,0,0,0 };
        const char* pbegin = psz;
        while (phexdigit[(unsigned char)*psz] || *psz == '0')
            psz++;
        psz--;
        unsigned char* p1 = (unsigned char*)data;
        unsigned char* pend = p1 + WIDTH * 4;
        while (psz >= pbegin && p1 < pend)
        {
            *p1 = phexdigit[(unsigned char)*psz--];
            if (psz >= pbegin)
            {
                *p1 |= (phexdigit[(unsigned char)*psz--] << 4);
                p1++;
            }
        }
    }

    void SetHex(const std::string& str)
    {
        SetHex(str.c_str());
    }

    std::string ToString() const
    {
        return (GetHex());
    }

    unsigned char* begin()
    {
        return (unsigned char*)&data[0];
    }

    unsigned char* end()
    {
        return (unsigned char*)&data[WIDTH];
    }

    const unsigned char* begin() const
    {
        return (unsigned char*)&data[0];
    }

    const unsigned char* end() const
    {
        return (unsigned char*)&data[WIDTH];
    }

    unsigned int size() const
    {
        return sizeof(data);
    }

    uint64 Get64(int n=0) const
    {
        return data[2*n] | (uint64)data[2*n+1] << 32;
    }

//    unsigned int GetSerializeSize(int nType=0, int nVersion=PROTOCOL_VERSION) const
    unsigned int GetSerializeSize(int nType, int nVersion) const
    {
        return sizeof(data);
    }

    template<typename Stream>
//    void Serialize(Stream& s, int nType=0, int nVersion=PROTOCOL_VERSION) const
    void Serialize(Stream& s, int nType, int nVersion) const
    {
        s.write((char*)data, sizeof(data));
    }

    template<typename Stream>
//    void Unserialize(Stream& s, int nType=0, int nVersion=PROTOCOL_VERSION)
    void Unserialize(Stream& s, int nType, int nVersion)
    {
        s.read((char*)data, sizeof(data));
    }


    friend class uint160;
    friend class uint256;
    friend class uint512;
    friend inline int Testuint256AdHoc(std::vector<std::string> vArg);
};

typedef base_uint<512> base_uint512;

//////////////////////////////////////////////////////////////////////////////
//
// uint512
//

/** 512-bit unsigned integer */
class uint512 : public base_uint512 {

public:
    typedef base_uint512 basetype;

    uint512()
    {
        for (int i = 0; i < WIDTH; i++)
            data[i] = 0;
    }

    uint512(const basetype& b)
    {
        for (int i = 0; i < WIDTH; i++)
            data[i] = b.data[i];
    }

    uint512& operator=(const basetype& b)
    {
        for (int i = 0; i < WIDTH; i++)
            data[i] = b.data[i];
        return *this;
    }

    uint512(uint64 b)
    {
        data[0] = (unsigned int)b;
        data[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            data[i] = 0;
    }

    uint512& operator=(uint64 b)
    {
        data[0] = (unsigned int)b;
        data[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            data[i] = 0;
        return *this;
    }

    explicit uint512(const std::string& str)
    {
        SetHex(str);
    }

    explicit uint512(const std::vector<unsigned char>& vch)
    {
        if (vch.size() == sizeof(data))
            memcpy(data, &vch[0], sizeof(data));
        else
            *this = 0;
    }

    uint256 trim256() const
    {
        uint256 ret;
        for (unsigned int i = 0; i < uint256::WIDTH; i++){
            ret.data[i] = data[i];
        }
        return ret;
    }
};

inline bool operator==(const uint512& a, uint64 b)                           { return (base_uint512)a == b; }
inline bool operator!=(const uint512& a, uint64 b)                           { return (base_uint512)a != b; }
inline const uint512 operator<<(const base_uint512& a, unsigned int shift)   { return uint512(a) <<= shift; }
inline const uint512 operator>>(const base_uint512& a, unsigned int shift)   { return uint512(a) >>= shift; }
inline const uint512 operator<<(const uint512& a, unsigned int shift)        { return uint512(a) <<= shift; }
inline const uint512 operator>>(const uint512& a, unsigned int shift)        { return uint512(a) >>= shift; }

inline const uint512 operator^(const base_uint512& a, const base_uint512& b) { return uint512(a) ^= b; }
inline const uint512 operator&(const base_uint512& a, const base_uint512& b) { return uint512(a) &= b; }
inline const uint512 operator|(const base_uint512& a, const base_uint512& b) { return uint512(a) |= b; }
inline const uint512 operator+(const base_uint512& a, const base_uint512& b) { return uint512(a) += b; }
inline const uint512 operator-(const base_uint512& a, const base_uint512& b) { return uint512(a) -= b; }

inline bool operator<(const base_uint512& a, const uint512& b)          { return (base_uint512)a <  (base_uint512)b; }
inline bool operator<=(const base_uint512& a, const uint512& b)         { return (base_uint512)a <= (base_uint512)b; }
inline bool operator>(const base_uint512& a, const uint512& b)          { return (base_uint512)a >  (base_uint512)b; }
inline bool operator>=(const base_uint512& a, const uint512& b)         { return (base_uint512)a >= (base_uint512)b; }
inline bool operator==(const base_uint512& a, const uint512& b)         { return (base_uint512)a == (base_uint512)b; }
inline bool operator!=(const base_uint512& a, const uint512& b)         { return (base_uint512)a != (base_uint512)b; }
inline const uint512 operator^(const base_uint512& a, const uint512& b) { return (base_uint512)a ^  (base_uint512)b; }
inline const uint512 operator&(const base_uint512& a, const uint512& b) { return (base_uint512)a &  (base_uint512)b; }
inline const uint512 operator|(const base_uint512& a, const uint512& b) { return (base_uint512)a |  (base_uint512)b; }
inline const uint512 operator+(const base_uint512& a, const uint512& b) { return (base_uint512)a +  (base_uint512)b; }
inline const uint512 operator-(const base_uint512& a, const uint512& b) { return (base_uint512)a -  (base_uint512)b; }

inline bool operator<(const uint512& a, const base_uint512& b)          { return (base_uint512)a <  (base_uint512)b; }
inline bool operator<=(const uint512& a, const base_uint512& b)         { return (base_uint512)a <= (base_uint512)b; }
inline bool operator>(const uint512& a, const base_uint512& b)          { return (base_uint512)a >  (base_uint512)b; }
inline bool operator>=(const uint512& a, const base_uint512& b)         { return (base_uint512)a >= (base_uint512)b; }
inline bool operator==(const uint512& a, const base_uint512& b)         { return (base_uint512)a == (base_uint512)b; }
inline bool operator!=(const uint512& a, const base_uint512& b)         { return (base_uint512)a != (base_uint512)b; }
inline const uint512 operator^(const uint512& a, const base_uint512& b) { return (base_uint512)a ^  (base_uint512)b; }
inline const uint512 operator&(const uint512& a, const base_uint512& b) { return (base_uint512)a &  (base_uint512)b; }
inline const uint512 operator|(const uint512& a, const base_uint512& b) { return (base_uint512)a |  (base_uint512)b; }
inline const uint512 operator+(const uint512& a, const base_uint512& b) { return (base_uint512)a +  (base_uint512)b; }
inline const uint512 operator-(const uint512& a, const base_uint512& b) { return (base_uint512)a -  (base_uint512)b; }

inline bool operator<(const uint512& a, const uint512& b)               { return (base_uint512)a <  (base_uint512)b; }
inline bool operator<=(const uint512& a, const uint512& b)              { return (base_uint512)a <= (base_uint512)b; }
inline bool operator>(const uint512& a, const uint512& b)               { return (base_uint512)a >  (base_uint512)b; }
inline bool operator>=(const uint512& a, const uint512& b)              { return (base_uint512)a >= (base_uint512)b; }
inline bool operator==(const uint512& a, const uint512& b)              { return (base_uint512)a == (base_uint512)b; }
inline bool operator!=(const uint512& a, const uint512& b)              { return (base_uint512)a != (base_uint512)b; }
inline const uint512 operator^(const uint512& a, const uint512& b)      { return (base_uint512)a ^  (base_uint512)b; }
inline const uint512 operator&(const uint512& a, const uint512& b)      { return (base_uint512)a &  (base_uint512)b; }
inline const uint512 operator|(const uint512& a, const uint512& b)      { return (base_uint512)a |  (base_uint512)b; }
inline const uint512 operator+(const uint512& a, const uint512& b)      { return (base_uint512)a +  (base_uint512)b; }
inline const uint512 operator-(const uint512& a, const uint512& b)      { return (base_uint512)a -  (base_uint512)b; }




#endif // BITCOIN_UINT512_H
