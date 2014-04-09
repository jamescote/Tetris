/*
	Defines different data types to be used 
	throughout the project.
*/
#ifndef _TYPES_H
#define _TYPES_H

/* Unsigned Data definitions */
typedef unsigned char UINT8;
typedef unsigned int  UINT16;
typedef unsigned long UINT32;

/* Boolean Defines */
typedef UINT8 bool;
#define true 1
#define false 0

/* ISR Defines */
typedef void (*Vector)();
#define USER_MODE 	0
#define SUPER_MODE 	-1

#endif
