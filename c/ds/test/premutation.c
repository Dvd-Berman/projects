#include <stdio.h>
#include <assert.h>


void Swap(char *char1, char *char2)
{
	char temp = '\0';
	
	assert(NULL != char1);
	assert(NULL != char2);
	
 	temp = *char1;
 	*char1 = *char2;
 	*char2 = temp;
}



void PremutationsIMP(char *str, size_t curr_index)
{
	int i = 0;
	
	assert(NULL != str);
	
	if (str[curr_index + 1] == '\0')
	{
		printf("%s\n", str);
		
		return;
	}
	
	for (i = curr_index; str[i] != '\0'; ++i)
	{
		Swap(str + i, str + curr_index);
		PremutationsIMP(str, curr_index + 1);
		Swap(str + i, str + curr_index);
	}
}

void Premutations(char *str)
{
	assert(NULL != str);
	
	PremutationsIMP(str, 0);
}

int main(void)
{
	char str[] = { 'a', 'b', 'c', '\0'};
		
	Premutations(str);
	
	return 0;
}
