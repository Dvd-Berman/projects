
#include <stdlib.h> /* for free */
#include <unistd.h> /* for sleep */

#include "wd.h"

int main(int argc, char *argv[])
{
	wd_t *wd = NULL;
	size_t sleep_time = 40;
	
	
	(void)argc;
	
	wd = MMI(argv);
	
	while(0 != sleep_time)
	{
		sleep_time = sleep(sleep_time);
	}
	
	DNR(wd);
		
	return 0;
}	
