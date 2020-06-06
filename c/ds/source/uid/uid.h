/****************************************
* Author: OL78							*
* Last Update: 17/11/2019				*
* Data structure: Uid			        *
* ***************************************/

#ifndef __UID_H__
#define __UID_H__

#include <sys/types.h> /* for system types */

typedef struct ilrd_uid
{
	time_t timestamp;
	pid_t pid;
	unsigned long serial;
}ilrd_uid_t;

/*---------------------------------------------------------------------------*/

/* Creates and returns new UID. if fails
 * returns BAD_UID												
 * Algorithmic complexity: O(?). */ 
ilrd_uid_t UIDCreate(void);

/*---------------------------------------------------------------------------*/

/* Returns 1 if the uids are same
 *  Otherwise - 0. */ 
int UIDIsSame(ilrd_uid_t uid1, ilrd_uid_t uid2);

/*---------------------------------------------------------------------------*/

/* Returns 1 if the uid equals to BAD_UID
 *  Otherwise - 0. */ 
int UIDIsBad(ilrd_uid_t uid_check);

/*---------------------------------------------------------------------------*/
/* Returns BAD_UID */
ilrd_uid_t UIDGetBad(void);

/*---------------------------------------------------------------------------*/

#endif /* __UID_H__ */
