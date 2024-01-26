#ifndef DBERROR_H
#define DBERROR_H

#include "stdio.h"

/* module wide constants */
#define PAGE_SIZE 4096

/* return code definitions */
typedef int RC;

#define RC_OK 0
#define RC_FILE_NOT_FOUND 1
#define RC_FILE_HANDLE_NOT_INIT 2
#define RC_WRITE_FAILED 3
#define RC_READ_NON_EXISTING_PAGE 4

#define RC_PINNED_PAGES_IN_BUFFER 500 

#define RC_RM_COMPARE_VALUE_OF_DIFFERENT_DATATYPE 200
#define RC_RM_EXPR_RESULT_IS_NOT_BOOLEAN 201
#define RC_RM_BOOLEAN_EXPR_ARG_IS_NOT_BOOLEAN 202
#define RC_RM_NO_MORE_TUPLES 203
#define RC_RM_NO_PRINT_FOR_DATATYPE 204
#define RC_RM_UNKOWN_DATATYPE 205

#define RC_IM_KEY_NOT_FOUND 300
#define RC_IM_KEY_ALREADY_EXISTS 301
#define RC_IM_N_TO_LAGE 302
#define RC_IM_NO_MORE_ENTRIES 303
#define RC_BUFFER_ERROR 375

#define RC_ERROR 402
#define RC_PIN_PAGE_FAILED 403
#define RC_UNPIN_PAGE_FAILED 404
#define RC_MARK_DIRTY_FAILED 405
#define RC_BUFFER_SHUTDOWN_FAILED 406
#define RC_NULL_IP_PARAM 407
#define RC_FILE_DESTROY_FAILED 408
#define RC_IVALID_PAGE_SLOT_NUM 409
#define RC_SCHEMA_NOT_INIT 410
#define RC_MELLOC_MEM_ALLOC_FAILED 411
#define RC_BUFFER_PIN_PG 450

// Added new definitions for Record Manager
#define RC_RM_NO_TUPLE_WITH_GIVEN_RID 600
#define RC_SCAN_CONDITION_NOT_FOUND 601

// Added new definition for B-Tree
#define RC_ORDER_TOO_HIGH_FOR_PAGE 701
#define RC_INSERT_ERROR 702
#define RC_NO_RECORDS_TO_SCAN 703

#define RC_RM_NO_TUPLE_WITH_GIVEN_RID 600
#define RC_SCAN_CONDITION_NOT_FOUND 601

/* holder for error messages */
extern char *RC_message;

/* print a message to standard out describing the error */
extern void printError (RC error);
extern char *errorMessage (RC error);

#define THROW(_rc,_message)						\
	do {										\
		RC_message=_message;					\
		return _rc;								\
	} while (0)									\

// check the return code and exit if it is an error
#define CHECK(_code)													\
	do {																\
		int _rc_internal = (_code);										\
		if (_rc_internal != RC_OK)										\
		{																\
			char *_message = errorMessage(_rc_internal);				\
			printf("[%s-L%i-%s] ERROR: Operation returned error: %s\n",__FILE__, __LINE__, __TIME__, _message); \
			free(_message);												\
			exit(1);													\
		}																\
	} while(0);


#endif



