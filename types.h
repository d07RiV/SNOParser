#pragma once

typedef unsigned char uint8;
typedef signed char sint8;
typedef char int8;
typedef unsigned short uint16;
typedef signed short sint16;
typedef short int16;
typedef unsigned long uint32;
typedef signed long sint32;
typedef long int32;
typedef unsigned __int64 uint64;
typedef signed __int64 sint64;
typedef __int64 int64;

typedef unsigned char* uint8_ptr;
typedef signed char* sint8_ptr;
typedef char* int8_ptr;
typedef unsigned short* uint16_ptr;
typedef signed short* sint16_ptr;
typedef short* int16_ptr;
typedef unsigned long* uint32_ptr;
typedef signed long* sint32_ptr;
typedef long* int32_ptr;
typedef unsigned __int64* uint64_ptr;
typedef signed __int64* sint64_ptr;
typedef __int64* int64_ptr;

typedef unsigned char const* uint8_const_ptr;
typedef signed char const* sint8_const_ptr;
typedef char const* int8_const_ptr;
typedef unsigned short const* uint16_const_ptr;
typedef signed short const* sint16_const_ptr;
typedef short const* int16_const_ptr;
typedef unsigned long const* uint32_const_ptr;
typedef signed long const* sint32_const_ptr;
typedef long const* int32_const_ptr;
typedef unsigned __int64 const* uint64_const_ptr;
typedef signed __int64 const* sint64_const_ptr;
typedef __int64 const* int64_const_ptr;

const uint8 max_uint8 = 0xFFU;
const sint8 min_int8 = sint8(0x80);
const sint8 max_int8 = 0x7F;
const uint16 max_uint16 = 0xFFFFU;
const sint16 min_int16 = sint16(0x8000);
const sint16 max_int16 = 0x7FFF;
const uint32 max_uint32 = 0xFFFFFFFFU;
const sint32 min_int32 = 0x80000000;
const sint32 max_int32 = 0x7FFFFFFF;
const uint64 max_uint64 = 0xFFFFFFFFFFFFFFFFULL;
const sint64 min_int64 = 0x8000000000000000LL;
const sint64 max_int64 = 0x7FFFFFFFFFFFFFFFLL;

template<int N> struct UIntTraits {};
template<> struct UIntTraits<8> { typedef uint8 Type; };
template<> struct UIntTraits<16> { typedef uint16 Type; };
template<> struct UIntTraits<32> { typedef uint32 Type; };
template<> struct UIntTraits<64> { typedef uint64 Type; };
template<int N> using UInt = typename UIntTraits<N>::Type;
