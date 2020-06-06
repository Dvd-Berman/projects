/****************************************
* Author: David Berman			        *
* Last Update: 2/11/2019		        *
* Data structure: Bit array	Code File	*
* **************************************/

#include <stdio.h>
#include <assert.h>

#include "bit_arr.h"

#define VALUES_IN_BYTE 256
#define MAX_VALUE_OF_BYTE 255
#define BITS_IN_BYTE 8

/*------------------------- Set Functions ------------------------------------*/

bray_t SetBit(bray_t bit_arr, size_t index, int value)
{
	assert(0 == value || 1 == value);
	assert(size_of_bray_t >= index);
	
	if (1 == value)
	{
		return SetOn(bit_arr, index);
	}
	else
	{
		return SetOff(bit_arr, index);
	}
}

/******************************************************************************/

bray_t SetOn(bray_t bit_arr, size_t index)
{
	bray_t mask = 1;
	
	assert(size_of_bray_t > index);
	
	mask <<= index;
	
	return (bit_arr | mask);
}

/******************************************************************************/

bray_t SetOff(bray_t bit_arr, size_t index)
{
	bray_t mask = 1;
	
	assert(size_of_bray_t > index);
	
	mask <<= index;
	
	return ( bit_arr & (~mask) );
}

/******************************************************************************/

bray_t SetAll(void)
{
	bray_t mask = 0;
	
	return (~mask);	
}

/******************************************************************************/
/*------------------------- IsOn/IsOffFunctions ------------------------------*/

int IsOn(bray_t bit_arr, size_t index)
{
	bray_t mask = 1;
	
	assert(size_of_bray_t > index);
	
	mask <<= index;
	
	return ( (bit_arr & mask) == mask );
}

/******************************************************************************/

int IsOff(bray_t bit_arr, size_t index)
{
	assert(size_of_bray_t > index);
	
	return ( !IsOn(bit_arr, index) );
}

/******************************************************************************/
/*------------------------- CountOn/CountOff Functions -----------------------*/

size_t CountOn(bray_t bit_arr)
{
	size_t count = 0;
	
	for(; 0 != bit_arr; bit_arr = bit_arr & (bit_arr - 1))
	{
		++count;
	}
	
	return count;
}

/******************************************************************************/

size_t CountOff(bray_t bit_arr)
{
	return ( size_of_bray_t - CountOn(bit_arr) );
}

/******************************************************************************/
/*------------------------- Flip Function ------------------------------------*/

bray_t Flip(bray_t bit_arr, size_t index)
{
	bray_t mask = 1;
	
	assert(size_of_bray_t > index);
	
	mask <<= index;
	
	return (bit_arr ^ mask); 
}

/******************************************************************************/
/*------------------------- Rot Functions ------------------------------------*/

bray_t RotL(bray_t bit_arr, size_t shift)
{
	size_t remainder = (shift % size_of_bray_t);
	bray_t mask  = bit_arr;

    mask >>= ( size_of_bray_t - (remainder) );
	bit_arr <<= remainder;
	
	return (bit_arr | mask);
}

/******************************************************************************/

bray_t RotR(bray_t bit_arr, size_t shift)
{
	size_t shift_left = size_of_bray_t - (shift % size_of_bray_t);
	
	return ( RotL(bit_arr, shift_left) );
}

/******************************************************************************/
/*------------------------- Mirror Function ----------------------------------*/

bray_t Mirror(bray_t bit_arr)
{
	size_t last_index = size_of_bray_t - 1;
	bray_t mirror = 0;
	size_t index = 0;
	
	for (; index < size_of_bray_t; ++index)
	{
		if (1 == IsOn(bit_arr, index))
		{
			mirror = SetOn(mirror, last_index - index);
		}
	}
	
	return mirror;
}

/******************************************************************************/
/*------------------------- ToString Function --------------------------------*/

char *ToString(bray_t bit_arr, char *str)
{
	unsigned int str_index = 0;
	unsigned int is_on_index = size_of_bray_t - 1;  
	
	assert(NULL != str);
	
	for (; size_of_bray_t > str_index; ++str_index, --is_on_index)
	{
		if ( 1 == IsOn(bit_arr, is_on_index) )
		{
			str[str_index] = '1';
		}
		else
		{
			str[str_index] = '0';	
		}		
	}
	str[str_index] = '\0';
	
	return str;
}

/******************************************************************************/
/*------------------------- Lut Functions ------------------------------------*/

size_t *LutCountOnInit(void)
{
	static int been_here = 0;
	static size_t lut_arr[VALUES_IN_BYTE] = { 0 };
	bray_t index = 0;
	
	if (0 == been_here)
	{
		for (; index < VALUES_IN_BYTE; ++index)
		{
			lut_arr[index] = CountOn(index);
		}
		been_here = 1;	
	}
	
	return lut_arr;
}

/******************************************************************************/

size_t CountOnLut(bray_t bit_arr)
{
	size_t *lut_arr = LutCountOnInit();
	size_t count = 0;
	size_t shift = 0;
	size_t size_in_bytes = size_of_bray_t / BITS_IN_BYTE;	
	bray_t mask = MAX_VALUE_OF_BYTE;
	
	while (shift < size_of_bray_t)
	{
		count += lut_arr[ ( (bit_arr) & (mask) ) >> (shift)];
		shift += size_in_bytes;
		mask <<= size_in_bytes;
	}
	
	return count;
}

/******************************************************************************/

bray_t *LutMirrorInit(void)
{
	static int been_here = 0;
	static bray_t lut_arr[VALUES_IN_BYTE] = { 0 };
	bray_t index = 0;
	
	if (0 == been_here)
	{
		for (; index < VALUES_IN_BYTE; ++index)
		{
			lut_arr[index] = Mirror(index);
		}
		been_here = 1;	
	}
	
	return lut_arr;
}

/******************************************************************************/

bray_t MirrorLut(bray_t bit_arr)
{
	bray_t *lut_arr = LutMirrorInit();
	size_t shift = 0;
	size_t size_in_bytes = size_of_bray_t / BITS_IN_BYTE;	
	bray_t mask = MAX_VALUE_OF_BYTE;
	bray_t mirror = 0;
	
	while (shift < size_of_bray_t)
	{
		mirror = mirror | (lut_arr[ ( (bit_arr) & (mask) ) >> (shift)]) >> shift;
		shift += size_in_bytes;
		mask <<= size_in_bytes;
	}
	
	return mirror;
}



/******************************************************************************/

