#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// var to store the current test's name
extern char *testName;

// short cut for test information
#define TEST_INFO  __FILE__, testName, __LINE__, __TIME__

// check the return code and exit if it's and error
#define TEST_CHECK(_code)												\
	do {																\
		int _rc_internal = (_code);										\
		if (_rc_internal != RC_OK)										\
		{																\
			char *_message = errorMessage(_rc_internal);				\
			printf("[%s-%s-L%i-%s] FAILED: Operation returned error: %s\n",TEST_INFO, _message); \
			free(_message);												\
			exit(1);													\
		}																\
	} while(0);

// check whether two strings are equal
#define ASSERT_EQUALS_STRING(_expected,_real,_message)					\
	do {																\
		if (strcmp((_expected),(_real)) != 0)							\
		{																\
			printf("[%s-%s-L%i-%s] FAILED: expected <%s> but was <%s>: %s\n",TEST_INFO, _expected, _real, _message); \
			exit(1);													\
		}																\
		printf("[%s-%s-L%i-%s] OK: expected <%s> and was <%s>: %s\n",TEST_INFO, _expected, _real, _message); \
	} while(0)

// check whether two ints are equals
#define ASSERT_EQUALS_INT(_expected,_real,_message)						\
	do {																\
		if ((_expected) != (_real))										\
		{																\
			printf("[%s-%s-L%i-%s] FAILED: expected <%i> but was <%i>: %s\n",TEST_INFO, _expected, _real, _message); \
			exit(1);													\
		}																\
		printf("[%s-%s-L%i-%s] OK: expected <%i> and was <%i>: %s\n",TEST_INFO, _expected, _real, _message); \
	} while(0)

// check whether two ints are equals
#define ASSERT_TRUE(_real,_message)										\
	do {																\
		if (!(_real))													\
		{																\
			printf("[%s-%s-L%i-%s] FAILED: expected true: %s\n",TEST_INFO, _message); \
			exit(1);													\
		}																\
		printf("[%s-%s-L%i-%s] OK: expected true: %s\n",TEST_INFO, _message); \
	} while(0)


// check that a method returns an error code
#define ASSERT_ERROR(_expected,_message)								\
	do {																\
		int result = (_expected);										\
		if (result == (RC_OK))											\
		{																\
			printf("[%s-%s-L%i-%s] FAILED: expected an error: %s\n",TEST_INFO, _message); \
			exit(1);													\
		}																\
		printf("[%s-%s-L%i-%s] OK: expected an error and was RC <%i>: %s\n",TEST_INFO,  result , _message); \
	} while(0)

// test worked
#define TEST_DONE()													\
	do {															\
		printf("[%s-%s-L%i-%s] OK: finished test\n\n",TEST_INFO);	\
	} while (0)

#endif // TEST_HELPER_H
