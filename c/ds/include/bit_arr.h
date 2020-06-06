/********************************
* Author: OL78					*
* Last Update: 2/11/2019		*
* Data structure: Bit array		*
* ******************************/

#ifndef __BIT_ARRAY_H__
#define __BIT_ARRAY_H__

typedef size_t bray_t;

static size_t size_of_bray_t = sizeof(bray_t) * 8;

/* Sets the bit to the given value. Returns the new bray_t.		*
 * The index must be between 0 to (WORDSIZE-1). 				*
 * The value must be 0 or 1.									*/
bray_t SetBit(bray_t bit_arr, size_t index, int value);

/* Sets the bit to 1. Returns the new bray_t.					*
 * The index must be between 0 to (WORDSIZE-1).					*/
bray_t SetOn(bray_t bit_arr, size_t index);

/* Sets the bit to 0. Returns the new bray_t.					*
 * The index must be between 0 to (WORDSIZE-1).					*/
bray_t SetOff(bray_t bit_arr, size_t index);

/* Sets all the bits to 1. Returns the new bray_t.				*/
bray_t SetAll(void);

/* Returns 1 if the bit is set to 1. Otherwise - 0.				*
 * The index must be between 0 to (WORDSIZE-1).					*/
int IsOn(bray_t bit_arr, size_t index);

/* Returns 1 if the bit is set to 0. Otherwise - 0. 			*
 * The index must be between 0 to (WORDSIZE-1).					*/
int IsOff(bray_t bit_arr, size_t index);

/* Returns the number of bits set to 1.							*/
size_t CountOn(bray_t bit_arr);

/* Returns the number of bits set to 0. 						*/
size_t CountOff(bray_t bit_arr);

/* FLip the value of the bit.									*
 * The index must be between 0 to (WORDSIZE-1).           		*/
bray_t Flip(bray_t bit_arr, size_t index);

/* Rotates all the bits to the left "shift" places.				*/
bray_t RotL(bray_t bit_arr, size_t shift);

/* Rotates all the bits to the right "shift" places. 			*/
bray_t RotR(bray_t bit_arr, size_t shift);

/* Returns the new bray_t which is a mirror of bit_arr.			*/
bray_t Mirror(bray_t bit_arr);

/* Writes all the bits to str and returns a pointer to str.		*
 * str length must be at least (WORDSIZE+1) characters.			*/
char *ToString(bray_t bit_arr, char *str);

/* Creates an array of 256 values, representing the number      *
of bits That are turned on, in all possible values of one byte. *
If Array already exists, it isn't created again                 */
size_t *LutCountOnInit(void);

/* Returns the number of bits set to 1, Using Lut array.		*/
size_t CountOnLut(bray_t bit_arr);

/* Creates an array of 256 values, representing the mirror      *
of each value. If Array already exists, it isn't created again  */
bray_t *LutMirrorInit(void);

/* Returns the mirror of bray_t using Lut array.		            */
bray_t MirrorLut(bray_t bit_arr);


#endif /* __BIT_ARRAY_H__ */
