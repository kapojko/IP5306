#ifndef BITOPS_H
#define BITOPS_H


#ifdef  __cplusplus
extern "C" {
#endif


#include <stdint.h>


/** The value corresponding to the bit.
 * @param n : bit index (from 0) */
#define BITOPS_BIT(n) (1 << (n))

/** The value corresponding to the bit (unsigned int).
 * @param n : bit index (from 0) */
#define BITOPS_BIT_U(n) (1u << (n))

/** The value corresponding to the bit (unsigned long).
 * @param n : bit index (from 0) */
#define BITOPS_BIT_UL(n) (1ul << (n))

/** The value corresponding to the bit (unsigned long long).
 * @param n : bit index (from 0) */
#define BITOPS_BIT_ULL(n) (1ull << (n))

/** Extracting a bit from a number.
@param i : (unsigned) int number
@param b : int bit index starting from 0
@return int Read value. */
#define BITOPS_GET_BIT(i, b) ((i)>>(b)&1)

/** Extracting a bit from a number (64-bit version).
@param i : unsigned long long number
@param b : int bit index starting from 0
@return unsigned long long Read value. */
#define BITOPS_GET_BIT_64(i, b) (((i) >> (b)) & 1)

/** Selecting several bits from a number.
@param i : (unsigned) int number
@param b1 : int index of the first bit, starting from 0
@param bn : int number of bits
@return int Read value. */
#define BITOPS_GET_BITS(i, b1, bn) (((i)>>(b1))&((1<<(bn))-1))

/** Extracting several bits from a number (64-bit version).
@param i : unsigned long long number
@param b1 : int index of the first bit, starting from 0
@param bn : int number of bits
@@return unsinged long long Read value. */
#define BITOPS_GET_BITS_64(i, b1, bn) (((i) >> (b1)) & ((1ull << (bn)) - 1))

/** Setting a bit in a number.
@param i : (unsigned) int* variable for setting bits
@param b : (unsigned) int bit index starting from 0
@param v : (unsigned) int value to be set */
#define BITOPS_SET_BIT(i, b, v) (*(i)&= ~(1<<(b)), *(i)|= ((v)&1)<<(b))

/** Setting a bit in a number (64-bit version).
@param i : unsigned long long* variable for setting bits
@param b : int bit index starting from 0
@param v : unsigned long long value to be set */
#define BITOPS_SET_BIT_64(i, b, v) (*(i) &= ~(1ull << (b)), *(i) |= (unsigned long long)((v) & 1) << (b))

/** Setting multiple bits in a number.
@param i : (unsigned) int* variable for setting bits
@param b1 : (unsigned) int index of the first bit, starting from 0
@param bn : (unsigned) int number of bits
@param v : (unsigned) int value to be set */
#define BITOPS_SET_BITS(i, b1, bn, v) (*(i)&= ~(((1<<(bn))-1)<<(b1)), \
		*(i)|= ((v)&((1<<(bn))-1))<<(b1))

/** Setting multiple bits in a number (64-bit version).
@param i : unsigned long long* variable for setting bits
@param b1 : int index of the first bit, starting from 0
@param bn : int number of bits
@param v : unsigned long long value to be set */
#define BITOPS_SET_BITS_64(i, b1, bn, v) (*(i) &= ~(((1ull << (bn)) - 1) << (b1)), \
        *(i) |= (unsigned long long)((v) & ((1ull << (bn))-1)) << (b1))

/** Reading a signed value from an unsigned source.
 * @param i : unsigned value
 * @param b1 : index of the first bit, starting from 0
 * @param bn : value width in bits
 * @return The read sign value. */
#define BITOPS_GET_SIGNED_BITS(i, b1, bn) (BITOPS_GET_BIT((i), (b1) + (bn) - 1) ? \
		-(signed)(BITOPS_GET_BITS(~(i), (b1), (bn) - 1) - 1) : (signed)BITOPS_GET_BITS((i), (b1), (bn) - 1))
		
/** Reading a signed value from an unsigned source.
 * @param i : unsigned long long value
 * @param b1 : index of the first bit, starting from 0
 * @param bn : value width in bits
 * @* @return signed long long unread signed value. */
#define BITOPS_GET_SIGNED_BITS_64(i, b1, bn) (BITOPS_GET_BIT_64((i), (b1) + (bn) - 1) ? \
        -(signed)(BITOPS_GET_BITS_64(~(i), (b1), (bn) - 1) - 1) : (signed)BITOPS_GET_BITS_64((i), (b1), (bn) - 1))

/** Setting or resetting the flag.
 * @param flags : int* variable for setting or resetting the flag
 * @param flag : int flag (type 0b0001000)
* @param set : bool indicates whether the flag is set or reset */
#define BITOPS_SET_FLAG(flags, flag, set) ((set) ? (*(flags)|= (flag)) : \
		(*(flags)&= ~(flag)))  

/** Reading a 16-bit unsigned number with the highest byte forward.
@param c : const void* initial read address
@return uint16_t The read value. */
#define BITOPS_READ_U16B(c) ((*((const uint8_t*)(c))<<8)|(*((const uint8_t*)(c)+1)))

/** Reading a 16-bit unsigned number with the lowest byte forward.
@param c : const void* initial read address
@return uint16_t The read value. */
#define BITOPS_READ_U16L(c) ((*((const uint8_t*)(c)))|(*((const uint8_t*)(c)+1)<<8))

/** Reading a 24-bit unsigned number with the highest byte forward.
@param c : const void* initial read address
@return uint32_t Read value. */
#define BITOPS_READ_U24B(c) ((*((const uint8_t*)(c))<<16)| \
		(*((const uint8_t*)(c)+1)<<8)| \
		(*((const uint8_t*)(c)+2)))

/** Reading a 24-bit unsigned number with the lowest byte forward.
@param c : const void* initial read address
@return uint32_t Read value. */
#define BITOPS_READ_U24L(c) ((*((const uint8_t*)(c)))| \
		(*((const uint8_t*)(c)+1)<<8)| \
		(*((const uint8_t*)(c)+2)<<16))

/** Reading a 32-bit unsigned number with the highest byte forward.
@param c : const void* initial read address
@return uint32_t Read value. */
#define BITOPS_READ_U32B(c) ((*((const uint8_t*)(c))<<24)| \
		(*((const uint8_t*)(c)+1)<<16)| \
		(*((const uint8_t*)(c)+2)<<8)| \
		(*((const uint8_t*)(c)+3)))

/** Reading a 32-bit unsigned number with the lowest byte forward.
@param c : const void* initial read address
@return uint32_t Read value. */
#define BITOPS_READ_U32L(c) ((*((const uint8_t*)(c)))| \
		(*((const uint8_t*)(c)+1)<<8)| \
		(*((const uint8_t*)(c)+2)<<16)| \
		(*((const uint8_t*)(c)+3)<<24))

/** Reading a 64-bit unsigned number with the highest byte forward.
@param c : const void* initial read address
@return uint64_t The read value. */
#define BITOPS_READ_U64B(c) (((uint64_t)*((const uint8_t*)(c))<<56)| \
		((uint64_t)*((const uint8_t*)(c)+1)<<48)| \
		((uint64_t)*((const uint8_t*)(c)+2)<<40)| \
		((uint64_t)*((const uint8_t*)(c)+3)<<32)| \
		((uint64_t)*((const uint8_t*)(c)+4)<<24)| \
		((uint64_t)*((const uint8_t*)(c)+5)<<16)| \
		((uint64_t)*((const uint8_t*)(c)+6)<<8)| \
		((uint64_t)*((const uint8_t*)(c)+7)))

/** Reading a 64-bit unsigned number with the lowest byte forward.
@param c : const void* initial read address
@return uint64_t The read value. */
#define BITOPS_READ_U64L(c) (((uint64_t)*((const uint8_t*)(c)))| \
		((uint64_t)*((const uint8_t*)(c)+1)<<8)| \
		((uint64_t)*((const uint8_t*)(c)+2)<<16)| \
		((uint64_t)*((const uint8_t*)(c)+3)<<24)| \
		((uint64_t)*((const uint8_t*)(c)+4)<<32)| \
		((uint64_t)*((const uint8_t*)(c)+5)<<40)| \
		((uint64_t)*((const uint8_t*)(c)+6)<<48)| \
		((uint64_t)*((const uint8_t*)(c)+7)<<56))


/** Reading a 16-bit signed number with the highest byte forward.
@param c : const void* initial read address
@return : int16_t Read value. */
#define BITOPS_READ_S16B(c) ((int16_t)BITOPS_READ_U16B(c))

/** Reading a 16-bit signed number with the lowest byte forward.
@param c : const void* initial read address
@return : int16_t Read value. */
#define BITOPS_READ_S16L(c) ((int16_t)BITOPS_READ_U16L(c))

/** Reading a 32-bit signed number with the highest byte forward.
@param c : const void* initial read address
@return : int32_t is the read value. */
#define BITOPS_READ_S32B(c) ((int32_t)BITOPS_READ_U32B(c))

/** Reading a 32-bit signed number with the lowest byte forward.
@param c : const void* initial read address
@return : int32_t is the read value. */
#define BITOPS_READ_S32L(c) ((int32_t)BITOPS_READ_U32L(c))

/** Reading a 64-bit signed number from the highest byte forward.
@param c : const void* initial read address
@return : int64_t is the read value. */
#define BITOPS_READ_S64B(c) ((int64_t)BITOPS_READ_U64B(c))

/** Reading a 64-bit signed number with the lowest byte forward.
@param c : const void* initial read address
@return : int64_t is the read value. */
#define BITOPS_READ_S64L(c) ((int64_t)BITOPS_READ_U64L(c))


/** Writing a 16-bit unsigned number with the highest byte forward.
@param c : char* entry start address
@param v : uint16_t value for writing */
#define BITOPS_WRITE_U16B(c, v) (*((uint8_t*)(c))= (uint8_t)((unsigned short)(v)>>8), \
		*((uint8_t*)(c)+1)= (uint8_t)(v))

/** Writing a 16-bit unsigned number with the lowest byte forward.
@param c : char* entry start address
@param v : uint16_t value for writing */
#define BITOPS_WRITE_U16L(c, v) (*((uint8_t*)(c))= (uint8_t)(v), \
		*((uint8_t*)(c)+1)= (uint8_t)((unsigned short)(v)>>8))

/** Writing a 24-bit unsigned number with the highest byte forward.
@param c : char* entry start address
@param v : uint32_t value for writing */
#define BITOPS_WRITE_U24B(c, v) (*((uint8_t*)(c))= (uint8_t)((unsigned long)(v)>>16), \
		*((uint8_t*)(c)+1)= (uint8_t)((unsigned long)(v)>>8), \
		*((uint8_t*)(c)+2)= (uint8_t)(v))

/** Writing a 24-bit unsigned number with the lowest byte forward.
@param c : char* entry start address
@param v : uint32_t value for writing */
#define BITOPS_WRITE_U24L(c, v) (*((uint8_t*)(c))= (uint8_t)(v), \
		*((uint8_t*)(c)+1)= (uint8_t)((unsigned long)(v)>>8), \
		*((uint8_t*)(c)+2)= (uint8_t)((unsigned long)(v)>>16))

/** Writing a 32-bit unsigned number with the highest byte forward.
@param c : char* entry start address
@param v : uint32_t value for writing */
#define BITOPS_WRITE_U32B(c, v) (*((uint8_t*)(c))= (uint8_t)((unsigned long)(v)>>24), \
		*((uint8_t*)(c)+1)= (uint8_t)((unsigned long)(v)>>16), \
		*((uint8_t*)(c)+2)= (uint8_t)((unsigned long)(v)>>8), \
		*((uint8_t*)(c)+3)= (uint8_t)(v))

/** Writing a 32-bit unsigned number with the lowest byte forward.
@param c : char* entry start address
@param v : uint32_t value for writing */
#define BITOPS_WRITE_U32L(c, v) (*((uint8_t*)(c))= (uint8_t)(v), \
		*((uint8_t*)(c)+1)= (uint8_t)((unsigned long)(v)>>8), \
		*((uint8_t*)(c)+2)= (uint8_t)((unsigned long)(v)>>16), \
		*((uint8_t*)(c)+3)= (uint8_t)((unsigned long)(v)>>24))

/** Writing a 64-bit unsigned number with the highest byte forward.
@param c : char* entry start address
@param v : uint64_t value for writing */
#define BITOPS_WRITE_U64B(c, v) (*((uint8_t*)(c))= (uint8_t)((unsigned long long)(v)>>56), \
		*((uint8_t*)(c)+1)= (uint8_t)((unsigned long long)(v)>>48), \
		*((uint8_t*)(c)+2)= (uint8_t)((unsigned long long)(v)>>40), \
		*((uint8_t*)(c)+3)= (uint8_t)((unsigned long long)(v)>>32), \
		*((uint8_t*)(c)+4)= (uint8_t)((unsigned long long)(v)>>24), \
		*((uint8_t*)(c)+5)= (uint8_t)((unsigned long long)(v)>>16), \
		*((uint8_t*)(c)+6)= (uint8_t)((unsigned long long)(v)>>8), \
		*((uint8_t*)(c)+7)= (uint8_t)(v))

/** Writing a 64-bit unsigned number with the lowest byte forward.
@param c : char* entry start address
@param v : uint64_t value for writing */
#define BITOPS_WRITE_U64L(c, v) (*((uint8_t*)(c))= (uint8_t)(v), \
		*((uint8_t*)(c)+1)= (uint8_t)((unsigned long long)(v)>>8), \
		*((uint8_t*)(c)+2)= (uint8_t)((unsigned long long)(v)>>16), \
		*((uint8_t*)(c)+3)= (uint8_t)((unsigned long long)(v)>>24), \
		*((uint8_t*)(c)+4)= (uint8_t)((unsigned long long)(v)>>32), \
		*((uint8_t*)(c)+5)= (uint8_t)((unsigned long long)(v)>>40), \
		*((uint8_t*)(c)+6)= (uint8_t)((unsigned long long)(v)>>48), \
		*((uint8_t*)(c)+7)= (uint8_t)((unsigned long long)(v)>>56))


/** Writing a 16-bit signed value with the highest byte forward.
@param c : char* entry start address
@param v : int16_t value for writing */
#define BITOPS_WRITE_S16B(c, v) BITOPS_WRITE_U16B(c, (int16_t)v)

/** Writing a 16-bit signed value with the lowest byte forward.
@param c : char* the initial address of the record
@param v : int16_t value for writing */
#define BITOPS_WRITE_S16L(c, v) BITOPS_WRITE_U16L(c, (int16_t)v)

/** Writing a 32-bit signed value with the highest byte forward.
@param c : char* the initial address of the record
@param v : int32_t value for writing */
#define BITOPS_WRITE_S32B(c, v) BITOPS_WRITE_U32B(c, (int32_t)v)

/** Writing a 32-bit signed value with the lowest byte forward.
@param c : char* the initial address of the record
@param v : int32_t value for writing */
#define BITOPS_WRITE_S32L(c, v) BITOPS_WRITE_U32L(c, (int32_t)v)

/** Writing a 64-bit signed value with the highest byte forward.
@param c : char* entry start address
@param v : int64_t value for writing */
#define BITOPS_WRITE_S64B(c, v) BITOPS_WRITE_U64B(c, (int64_t)v)

/** Writing a 64-bit signed value with the lowest byte forward.
@param c : char* the initial address of the record
@param v : int64_t value for writing */
#define BITOPS_WRITE_S64L(c, v) BITOPS_WRITE_U64L(c, (int64_t)v)


/** Converting an integer or part of an 8-bit number to a signed number.
 * @param v uint8_t value
 * @* @param int bitn number of bits to use
 * @return int8_t read signed value */
#define BITOPS_UINT8_TO_INT8(v, bitn) (((bitn) == sizeof(v) * 8) ? (int8_t)(v) : AFC_GET_SIGNED_BITS((v), 0, (bitn)))

/** Converting an integer or part of a 16-bit number to a signed number.
 * @param v uint16_t value
 * @* @param int bitn number of bits to use
 * @return int16_t read signed value */
#define BITOPS_UINT16_TO_INT16(v, bitn) (((bitn) == sizeof(v) * 8) ? (int16_t)(v) : AFC_GET_SIGNED_BITS((v), 0, (bitn)))

/** Converting an integer or part of a 32-bit number to a signed number.
 * @param v uint32_t value
 * @* @param int bitn number of bits to use
 * @return int32_t read signed value */
#define BITOPS_UINT32_TO_INT32(v, bitn) (((bitn) == sizeof(v) * 8) ? (int32_t)(v) : AFC_GET_SIGNED_BITS((v), 0, (bitn)))

/** Converting an integer or part of a 64-bit number to a signed number.
 * @param v uint64_t value
 * @* @param int bitn number of bits to use
 * @return int64_t read signed value */
#define BITOPS_UINT64_TO_INT64(v, bitn) (((bitn) == sizeof(v) * 8) ? (int64_t)(v) : AFC_GET_SIGNED_BITS((v), 0, (bitn)))


/** Getting an element in a two-dimensional array in which the data is arranged in rows.
@param a type*: array
@param xc int: the number of items in one row
@param x int: index on the x axis
@param y int: y-axis index
@return type& An array element (lvalue). */
#define BITOPS_EL2(a, xc, x, y) ((a)[(y)*(xc)+(x)])


#ifdef  __cplusplus
}
#endif


#endif // BITOPS_H

