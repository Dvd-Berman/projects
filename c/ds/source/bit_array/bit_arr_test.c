/****************************************
* Author: David Berman			        *
* Last Update: 2/11/2019		        *
* Data structure: Bit array	Tester File	*
* **************************************/

#include <stdio.h>

#include "bit_arr.h"

/*------------------------- Tester Function ----------------------------------*/

void Tester(bray_t result, bray_t expected)
{
	if (result == expected)
	{
		printf("AWESOME\n");
	}
	else
	{
		printf("OOPS\n");
	}
}

/*------------------------- Main Function ------------------------------------*/

int main(void)
{
	size_t last_index = size_of_bray_t - 1;
	bray_t only_first_bit = 1;
	bray_t only_last_bit = only_first_bit << (last_index);
	bray_t all_on = ~0;
	char bray_t_string[200] = {0};
	size_t *count_arr = LutCountOnInit();
	bray_t *mirror_arr = LutMirrorInit();
	
/*------------------------- Set Functions ------------------------------------*/	
	
	printf("-----------------Testing Set Functions-------------------------\n");
	printf("-----------------Testing SetOn Function------------------------\n");
	printf("Test1 SetOn last bit bray_t 0:\n"); 
	Tester(SetOn(0, size_of_bray_t - 1), only_last_bit);
	printf("Test2 SetOn for index 4 bray_t 18:\n");
	Tester(SetOn(18, 4), 18);
	printf("Test3 SetOn for index 0 bray_t 4:\n");
	Tester(SetOn(4, 0), 5);
	printf("-----------------Testing SetOff Function-----------------------\n");
	printf("Test1 SetOff for last bit:\n");
	Tester(SetOff(only_last_bit, last_index), 0);
	printf("Test2 SetOff for index 4 bray_t 18:\n");
	Tester(SetOff(18, 4), 2);
	printf("Test3 SetOff for index 0 bray_t 4:\n");
	Tester(SetOff(4, 0), 4);
	printf("-----------------Testing SetBit Function-----------------------\n");
	printf("Test1 SetBit for last bit to 1:\n");
	Tester(SetBit(0, last_index, 1),only_last_bit);
	printf("Test2 SetBit for index 0 bray_t 4 value 1:\n");
	Tester(SetBit(4, 0, 1),5);
	printf("Test3 SetBit for index 0 bray_t 4 value 0:\n");
	Tester(SetBit(4, 0, 0),4);
	printf("-----------------Testing SetAll Function-----------------------\n");
	printf("Testing SetAll for bray_t 200:\n");
	Tester(SetAll(),all_on);
	
/*------------------------- IsOn/IsOff Functions -----------------------------*/
	
	printf("-----------------Testing IsOn/IsOff Functions------------------\n");
	printf("-----------------Testing IsOn Function-------------------------\n");
	printf("Test1 IsOn for index 0 bray_t only_first_bit:\n");
	Tester(IsOn(only_first_bit, 0), 1);
	printf("Test2 IsOn for last_index, bray_t:\n");
	Tester(IsOn(only_last_bit, last_index), 1);
	printf("Test3 IsOn for last_index, bray_t 58:\n");
	Tester(IsOn(58, last_index), 0);
	printf("-----------------Testing IsOff Function------------------------\n");
	printf("Test1 IsOff for index 35 bray_t 0:\n");
	Tester(IsOff(0, 35), 1);
	printf("Test2 IsOff for last_index, bray_t only_last_bit:\n");
	Tester(IsOff(only_last_bit, last_index), 0);
	
/*------------------------- CountOn/CountOff Functions -----------------------*/	
	
	printf("-----------------Testing CountOn/CountOff Functions------------\n");
	printf("-----------------Testing CountOn Function----------------------\n");
	printf("Test1 CountOn for bray_t only_first_bit:\n");
	Tester(CountOn(only_first_bit), 1);
	printf("Test2 CountOn for bray_t only_last_bit:\n");
	Tester(CountOn(only_last_bit), 1); 
	printf("Test3 CountOn for bray_t all_on:\n");
	Tester(CountOn(all_on), size_of_bray_t);
	printf("-----------------Testing CountOff Function---------------------\n");
	printf("Test1 CountOff for bray_t 0:\n");
	Tester(CountOff(0), size_of_bray_t);
	printf("Test2 CountOff for bray_t only_last_bit:\n");
	Tester(CountOff(only_last_bit), last_index);
	printf("Test3 CountOff for bray_t all_on:\n");
	Tester(CountOff(all_on), 0);
	

/*------------------------- Flip Function ------------------------------------*/
	
	printf("-----------------Testing Flip Function-------------------------\n");
	printf("Test1 Flip for index 3, bray_t 0:\n");
	Tester(Flip(0, 3), 8);
	printf("Test2 Flip for index last_index, bray_t only_last_bit:\n");
	Tester(Flip(only_last_bit, last_index), 0);
	printf("Test3 Flip for index 0, bray_t only_first_bit:\n");
	Tester(Flip(only_first_bit, 0), 0);
			
/*------------------------- RotL/RotR Functions ------------------------------*/

	printf("-----------------Testing RotL/RotR Functions------------\n");
	printf("-----------------Testing RotL Function----------------------\n");
	printf("Test1 RotL for bray_t 5, shift 3:\n");
	Tester(RotL(5, 3), 40);
	printf("Test2 RotL for bray_t all_on, shift 400:\n");
	Tester(RotL(all_on, 400), all_on);
	printf("Test3 RotL for bray_t only_first_bit, shift size_of_bray_t:\n");
	Tester(RotL(only_first_bit, size_of_bray_t), only_first_bit);
	printf("-----------------Testing RotR Function----------------------\n");
	printf("Test1 RotR for bray_t only_first_bit, shift 1:\n");
	Tester(RotR(only_first_bit, 1), only_last_bit);
	printf("Test2 RotR for bray_t only_first_bit, shift size_of_bray_t:\n");
	Tester(RotR(only_first_bit, size_of_bray_t), only_first_bit);
	printf("Test3 RotR for bray_t only_last_bit, shift size_of_bray_t * 60:\n");
	Tester(RotR(only_last_bit, size_of_bray_t * 60), only_last_bit);
	
/*------------------------- Mirror Function ----------------------------------*/

	printf("-----------------Testing Mirror Function----------------------\n");
	printf("Test1 Mirror for bray_t only_first_bit:\n");
	Tester(Mirror(only_first_bit), only_last_bit);
	printf("Test2 Mirror for bray_t all_on:\n");
	Tester(Mirror(all_on), all_on);
	printf("Test3 Mirror for bray_t only_last_bit:\n");
	Tester(Mirror(only_last_bit), only_first_bit);

/*------------------------- ToString Function --------------------------------*/	
	
	printf("-----------------Testing ToString Function---------------------\n");
	ToString(only_last_bit, bray_t_string);
	printf("Testing ToString for bray_t only_last_bit:\n%s\n", bray_t_string);
	ToString(only_first_bit, bray_t_string);
	printf("Testing ToString for bray_t only_first_bit:\n%s\n", bray_t_string);
	ToString(all_on, bray_t_string);
	printf("Testing ToString for bray_t all_on:\n%s\n", bray_t_string);
	
/*------------------------- Lut Functions ------------------------------------*/			
	
	printf("-----------------Testing Lut Functions-------------------------\n");
	printf("-----------------Testing LutCountOnInit Function---------------\n");
	printf("Test1 Lut for bray_t only_first_bit:\n");
	Tester(count_arr[only_first_bit], CountOn(only_first_bit));
	printf("Test2 Lut for bray_t 255:\n");
	Tester(count_arr[255], CountOn(255));
	printf("-----------------Testing CountOnLut Function-------------------\n");
	printf("Test1 CountOnLut for bray_t only_last_bit:\n");
	Tester(CountOnLut(only_last_bit), CountOn(only_last_bit));
	printf("Test2 CountOnLut for bray_t all_on:\n");
	Tester(CountOnLut(all_on), CountOn(all_on));
	printf("Test3 CountOnLut for bray_t 0:\n");
	Tester(CountOnLut(0), CountOn(0));
	printf("-----------------Testing LutMirrorInit Function----------------\n");
	printf("Test1 Lut for bray_t only_first_bit:\n");
	Tester(mirror_arr[only_first_bit], Mirror(only_first_bit));
	printf("Test2 Lut for bray_t 255:\n");
	Tester(mirror_arr[255], Mirror(255));
	printf("-----------------Testing MirrorLut Function--------------------\n");
	printf("Test1 MirrorLut for bray_t only_last_bit:\n");
	Tester(MirrorLut(only_last_bit), Mirror(only_last_bit));
	printf("Test2 MirrorLut for bray_t all_on:\n");
	Tester(MirrorLut(all_on), Mirror(all_on));
	printf("Test3 MirrorLut for bray_t only_first_bit:\n");
	Tester(MirrorLut(only_first_bit), Mirror(only_first_bit));
	printf("Test4 MirrorLut for bray_t 12458:\n");
	Tester(MirrorLut(12458), Mirror(12458));
	printf("--------------------End Of Tester------------------------------\n\n");
	
/*------------------------- EndOfTester --------------------------------------*/		
	
	return 0;
}

