/****************************************
* Author: David Berman					*
* Last Update: 18/11/2019				*
* Data structure: Uid                   *
* Code File                             *
* ***************************************/

#include <unistd.h> /* for getpid */
#include <time.h> /* for time func */

#include "uid.h"

static unsigned long GetSerial(void);

/******************************************************************************/
/*------------------------- UIDCreate function -------------------------------*/

ilrd_uid_t UIDCreate(void)
{
	ilrd_uid_t uid = {0};
	
	uid.timestamp = time(NULL);
	uid.pid = getpid();
	uid.serial = GetSerial();
	
	return uid; 
}

/******************************************************************************/
/*------------------------- UIDCreate function -------------------------------*/

int UIDIsSame(ilrd_uid_t uid1, ilrd_uid_t uid2)
{
	return (uid1.timestamp == uid2.timestamp &&
		 uid1.pid == uid2.pid && uid1.serial == uid2.serial); 	
}

/******************************************************************************/
/*------------------------- UIDIsBad function --------------------------------*/

int UIDIsBad(ilrd_uid_t uid_check)
{
	return UIDIsSame(uid_check, UIDGetBad());
}

/******************************************************************************/
/*------------------------- GetBad function ----------------------------------*/

ilrd_uid_t UIDGetBad(void)
{
	ilrd_uid_t bad_uid = {0};
		
	return bad_uid;
}

/******************************************************************************/
/*------------------------- GetSerial function -------------------------------*/

static unsigned long GetSerial(void)
{
	static unsigned long serial_number = 0;
	
	return ++serial_number;
}
