#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "record_mgr.h"
#include "buffer_mgr.h"
#include "storage_mgr.h"

BM_PageHandle *pgHandle;
RecordManager *recMgr;
BM_BufferPool *bufPool;


int getFreeSlotIndex(char* data, int recordSize)
{
   // int totalSlots = PAGE_SIZE / recordSize;
       int totalSlots = (recordSize != 0) ? (PAGE_SIZE / recordSize) : 0;

    for (int index = 0; totalSlots > index; index++)
    {
        int offset = index * recordSize;
        if (*(data + offset) != '#')
            return index;
    }

    // if all the slots are processed and no free slot is available, code reaches here.
    return -1;
}

char *incrementPointer(char *pointer, int offset)
{
    // Incrementing the pointer to the next position
    pointer += offset;
    return pointer;
}

//// TABLE AND RECORD MANAGER FUNCTIONS 

// Function used for initializing the record manager
RC initRecordManager(void *mgmtData)
{
	return RC_OK;
}

// Function used for shutting down of the record manager
RC shutdownRecordManager()
{
	realloc(recMgr, 0);
	return RC_OK;
}

// creating a table
RC createTable(char *name, Schema *schema)
{
	char data[PAGE_SIZE];
	char *pgHandle = data;

	int const NUMBER_OF_TUPLES = 0;
	SM_FileHandle fileHandle;

	recMgr = (RecordManager *)malloc(sizeof(RecordManager));

	// defsault page replacement stragey used for initialzaing buffer pool
	initBufferPool( &recMgr->bufferPool, name, 100, RS_LRU, NULL);

	int attributesOfTableCreate[] = {NUMBER_OF_TUPLES, 1, schema->numAttr, schema->keySize};
	
	for (int iterator = 0; iterator < sizeof(attributesOfTableCreate) / sizeof(attributesOfTableCreate[0]); iterator++)
	{
		*(int*)pgHandle = attributesOfTableCreate[iterator];
		pgHandle = pgHandle + sizeof(int);
	}

	for (int iterator = 0; iterator < schema->numAttr; iterator++)
	{
	strncpy(pgHandle, schema->attrNames[iterator], 20);
	pgHandle = pgHandle + 20;
	*(int*)pgHandle = (int)schema->dataTypes[iterator];
	pgHandle = pgHandle + sizeof(int);
	*(int*)pgHandle = (int)schema->typeLength[iterator];
	pgHandle = pgHandle + sizeof(int);
	}

	bool exceptionThrown = false;

// Check page file creation with page name as table name
int result = createPageFile(name);
if (result != RC_OK)
{
exceptionThrown = true;
}

// Check the success of opening the newly created page
if (!exceptionThrown)
{
result = openPageFile(name, &fileHandle);
if (result != RC_OK)
{
exceptionThrown = true;
}
}

// Check the success of writing the schema to first location of the page file
if (!exceptionThrown)
{
result = writeBlock(0, &fileHandle, data);
if (result != RC_OK)
{
exceptionThrown = true;
}
}

// Check the success of closing the file after writing
if (!exceptionThrown)
{
result = closePageFile(&fileHandle);
if (result != RC_OK)
{
exceptionThrown = true;
}
}

// return the exception if there exists one
if (exceptionThrown)
{
return result;
}

	return RC_OK;
}

// Opening of the table
RC openTable(RM_TableData *rel, char *name)
{
	rel->mgmtData = recMgr;
	int numOfAttr;
	rel->name = name;
	pinPage(&recMgr->bufferPool, &recMgr->pageHandle, 0);
	pgHandle = (char *)recMgr->pageHandle.data;
	recMgr->totalRecordsInTable = *(int *)(char *)recMgr->pageHandle.data;
	pgHandle = incrementPointer(pgHandle, 4);
	int zero = 0;
	recMgr->firstFreePage.slot = zero;
	int one  = 1;
	recMgr->firstFreePage.page = one;
	pgHandle = incrementPointer(pgHandle, 4);
	numOfAttr = *(int *)pgHandle;
	pgHandle = incrementPointer(pgHandle,4);

	
	Schema *schema = (Schema *)malloc(sizeof(Schema));
	// memory allocation for schema parameters
	schema->numAttr = numOfAttr;
	// Allocate memory for attribute names
	schema->attrNames = malloc(numOfAttr * sizeof(char *));
	// Allocate memory for attribute data types
	schema->dataTypes = malloc(numOfAttr * sizeof(DataType));
	// Allocate memory for attribute data type lengths
	schema->typeLength = malloc(numOfAttr * sizeof(int));

	// Allocate memory space for storing attribute name for each attribute
	for (int i = 0; i < numOfAttr; i++) {
    schema->attrNames[i] = malloc(20);
	}

	for (int i = 0; i < schema->numAttr; i++) {
    // Allocate memory for attribute name
    schema->attrNames[i] = (char*)malloc(20);
    if (schema->attrNames[i] == NULL) {
        // Handle allocation failure
        return RC_ERROR;
    }
    
    // Copy attribute name from pageHandle
    strncat(schema->attrNames[i], pgHandle, 20);
    pgHandle = incrementPointer(pgHandle, 20);
    // Set data type of attribute
    schema->dataTypes[i] = *(int *)pgHandle;
	int intSiz = 4;
    pgHandle = incrementPointer(pgHandle, intSiz);
    // Set length of datatype (length of STRING) of the attribute
    schema->typeLength[i] = *(int *)pgHandle;
	int intSize = 4;
    pgHandle = incrementPointer(pgHandle, intSize);

}

	rel->schema = schema;


	unpinPage(&recMgr->bufferPool, pgHandle);

	forcePage(&recMgr->bufferPool, pgHandle);

	return RC_OK;
}

// Table referenced by "rel" is closed using this function
RC closeTable(RM_TableData *rel)
{
	RecordManager *recMgr = rel->mgmtData;
	shutdownBufferPool(&recMgr->bufferPool);
	return RC_OK;
}

// Deletion of the table
RC deleteTable(char *name)
{
	destroyPageFile(name);
	return RC_OK;
}

// This function returns the number of records in the table
int getNumTuples(RM_TableData *rel)
{
	RecordManager *recMgr = rel->mgmtData;
	int totRecords = recMgr->totalRecordsInTable;
	return totRecords;
}

// ******** RECORD FUNCTIONS ******** //

// A new record is added to the table by this function.
RC insertRecord(RM_TableData *rel, Record *record)
{
	bool checkrc;
	bool copybit;
	bool validslotsearch = true;
	bool assign=true;
	bool markdirtycheck;
	//Assigning the Record ID to this particular record.
	RID *recordID = &record->id;

	char *dataptr;
	char *recordptr;

	recordID->slot = -1;
	bool ptrvalue;
	int Sizeofrecord;
	Sizeofrecord = getRecordSize(rel->schema);
	int Offsetofrecord;
	markdirtycheck=true;
	ptrvalue=true;
	checkrc=true;
    if (assign)
	{
	recMgr = rel->mgmtData;
	pgHandle = &recMgr->pageHandle;
	
	bufPool = &recMgr->bufferPool;
	
	}
	int i=0;
	// We continue looping through until we come across a valid slot.
	for (i=0;recordID->slot == -1;i++)
	{
		if (!validslotsearch)
		{
			// In case the pinned page does not contain any available slot, then unpin that page.
			unpinPage(bufPool, pgHandle);

			// Increasing the page.
			recordID->page=recordID->page+1;
		}

		else
		{

			recordID->page = recMgr->firstFreePage.page;
			validslotsearch = false;
		}

		if(i>=0)
		{
		// Add the new page to the Buffer Pool.
		pinPage(bufPool, pgHandle, recordID->page);

		//Assigning the data to the initial position of the record's data.
		dataptr = recMgr->pageHandle.data;
		ptrvalue=true;
		// Perform another search for an available slot.
		recordID->slot = getFreeSlotIndex(dataptr, Sizeofrecord);
		markdirtycheck=true;
		}
	}
    copybit=true;
	recordptr = dataptr;

	if (markdirtycheck==true)
	{
	// Set the page's dirty bit to indicate that the page has been modified.
	if (markDirty(bufPool, pgHandle))
	{
		
		return RC_MARK_DIRTY_FAILED;
	}
	}

	Offsetofrecord = recordID->slot * Sizeofrecord;

	recordptr = incrementPointer(recordptr, Offsetofrecord);

	
	if (ptrvalue==true)
	{
	// A '#' character is added at the end to signify that this is a new record.
	*recordptr = '#';
	recordptr++;
	}
	
	

	if(copybit==true)
	{
	// Copy the data of the record to the memory location indicated by recordPointer.
	memcpy(recordptr, record->data + 1, Sizeofrecord - 1);

	}
	if(checkrc==true)
	{
	// Increasing the count of tuples.
	recMgr->totalRecordsInTable=recMgr->totalRecordsInTable+1;

	// Pin the page back into the Buffer Pool.
	pinPage(bufPool, pgHandle, 0);
	}

	// Removing a page from the Buffer Pool's pinned pages.
	if (unpinPage(bufPool, pgHandle))
	{	
		return RC_UNPIN_PAGE_FAILED;
	}

	return RC_OK;
}

RC deleteRecord(RM_TableData *rel, RID id)
{
	int Sizeofrecord;
	int Offsetofrecord;
	bool assign;
	Sizeofrecord = getRecordSize(rel->schema);
	assign=true;
	int ispinpage;
	Offsetofrecord = id.slot * Sizeofrecord;
	char *Datapageptr;
	ispinpage=1;
	bool ismodify;
// Fetching the metadata that is stored in the table.
if (assign==true)
{
	RecordManager *recordManager = rel->mgmtData;
    pgHandle = &recordManager->pageHandle;
	ismodify=true;
	bufPool = &recordManager->bufferPool;
}
	// Pinning the page that contains the record to be updated.
	if(ispinpage==1)
	{
	pinPage(bufPool, pgHandle, id.page);

	}


	if (ismodify==true)
	{
	// Modify the free page since this page is now in use.
	recMgr->firstFreePage.page = id.page;

	Datapageptr = recMgr->pageHandle.data;

	Datapageptr = incrementPointer(Datapageptr, Offsetofrecord);
	}
	// Unpin the page after retrieving the record.
	if (unpinPage(bufPool, pgHandle))
	{
		return RC_UNPIN_PAGE_FAILED;
	}

	// Set the page's dirty bit to indicate that the page has been modified.
	if (markDirty(bufPool, pgHandle))
	{
		return RC_MARK_DIRTY_FAILED;
	}
	return RC_OK;
}

// The function modifies a record indicated by "record" in the table referenced by "rel".
RC updateRecord(RM_TableData *rel, Record *record)
{
	char *Datapageptr;
	int assign=0;
	int Sizeofrecord;
	int Offsetofrecord;
	bool inc;
	bool copybit;
	if (assign==0)
	{
	recMgr = rel->mgmtData;
	
	bufPool = &recMgr->bufferPool;
	pgHandle = &recMgr->pageHandle;
	inc=true;
	copybit=true;
	}
	Sizeofrecord = getRecordSize(rel->schema);
	RID id = record->id;
	
	Offsetofrecord = id.slot * Sizeofrecord;
	// Pin the page containing the record to be updated.
	pinPage(bufPool, pgHandle, id.page);

	Datapageptr = recMgr->pageHandle.data;

	if (inc==true)
	{
	Datapageptr = incrementPointer(Datapageptr, Offsetofrecord);
	Datapageptr=Datapageptr+1;
	}

	if(copybit==true)
	{
	// Replace the existing record's pageData with that of the new record.
	
	memcpy(Datapageptr, record->data + 1, Sizeofrecord);

	}

	// Unpin the page after accessing the record.
	if (unpinPage(bufPool, pgHandle))
	{
		return RC_UNPIN_PAGE_FAILED;
	}
	
	//Set the page's dirty bit to indicate that the page has been modified.
	if (markDirty(bufPool, pgHandle))
	{	
		return RC_MARK_DIRTY_FAILED;
	}

	return RC_OK;
}

// The function fetches a record from the table.
RC getRecord(RM_TableData *rel, RID id, Record *record)
{
	int assign=0;
	int pagenum;
	int Sizeofrecord;
	int Offsetrecord;
	char *dataptr;
	bool inc;
	bool copybit;
	if (assign==0)
	{
	pgHandle = &recMgr->pageHandle;
	recMgr = rel->mgmtData;
	bufPool = &recMgr->bufferPool;
	}
	record->id = id;
	pagenum = id.page;
	copybit=true;
	inc=true;
	char *newdataptr;

	if (pinPage(bufPool, pgHandle, pagenum) == RC_OK)
	{
	// Determine the size of the record.
	Sizeofrecord = getRecordSize(rel->schema);
	Offsetrecord = id.slot * Sizeofrecord;
	if (inc==true)
	{
	dataptr = recMgr->pageHandle.data;
	dataptr = incrementPointer(dataptr, Offsetrecord);

	newdataptr = record->data;
	newdataptr=newdataptr+1;
	}
	if(copybit==true)
	{
	
	memcpy(newdataptr, dataptr + 1, Sizeofrecord);

	}
	

	// Unpin the page after accessing the record.
	if (unpinPage(bufPool, pgHandle) != RC_OK)
	{
		return RC_UNPIN_PAGE_FAILED;
	}
	}
	// Pin the page containing the desired record.
	else
	{
		return RC_PIN_PAGE_FAILED;
	}

	return RC_OK;
}


//// SCAN FUNCTIONS 

RC startScan(RM_TableData* rel, RM_ScanHandle* scan, Expr* cond)
{
    
     
    // Checking if there is no scan condition
     if (cond == 0)
    {
        return RC_SCAN_CONDITION_NOT_FOUND;
    }

    openTable(rel, "ScanTable");

     RecordManager *tableManager;
	tableManager = rel->mgmtData;
	tableManager->totalRecordsInTable = 20;

    // Allocate memory for the scan manager
    RecordManager *scanManager= malloc(sizeof(RecordManager));
    if (!scanManager)
    {
        return RC_MELLOC_MEM_ALLOC_FAILED;
    }

    // Initialize scan manager with default values
    (*scanManager).recordID.page = 1;
    (*scanManager).recordID.slot = 0;
    (*scanManager).scanCount = 0;
    (*scanManager).condition = cond;

    // Set the scan manager as the metadata for the scan handle
    (*scan).mgmtData = scanManager;

    // Set the table to be scanned
    (*scan).rel = rel;

    return RC_OK;
}

extern RC next(RM_ScanHandle *scan, Record *record)
{
	RecordManager *scanManager = ((RM_ScanHandle *)scan)->mgmtData;
	RecordManager *scanTableManager = scan->rel->mgmtData;
	Schema *schema = scan->rel->schema;

	// check if there is no scan condition
	if (!scanManager->condition)
	{
		return RC_SCAN_CONDITION_NOT_FOUND;
	}

	Value *result = (Value *)malloc(sizeof(Value));

	

	// Getting the schema's record size
	int sizeOfRecord = getRecordSize(schema);
	//  Total number of slots to be calculated
	int totalSlotsCount = (PAGE_SIZE + sizeOfRecord - 1) / sizeOfRecord;

	// Scan count retrieval
	int scanCount = (*scanManager).scanCount;

	//  tuples count of the table to be retrieved
	int totalRecordsInTable = getNumTuples(scan->rel);

	// determining if tuples are present in the table.If there are no tuples in the tables, then give the appropriate message code.
	if (!totalRecordsInTable)
		return RC_RM_NO_MORE_TUPLES;

	RID scanManagerRecordID = {scanManager->recordID.page, scanManager->recordID.slot};

	// iterating through the records
	while (totalRecordsInTable >= scanCount)
	{
		if (scanCount != 0)
		{
			scanManager->recordID.slot = scanManager->recordID.slot + 1;

			// If all the slots have been scanned execute this block
			if ((scanManager->recordID.page + 1) * totalSlotsCount <= scanTableManager->totalRecordsInTable)
			{
				scanManager->recordID.slot = 0;
				scanManager->recordID.page = scanManager->recordID.page + 1;
			}
		}
		else
		{
			// If no record has been scanned before, control comes here
			scanManager->recordID.page = 1;
			scanManager->recordID.slot = 0;
		}

		// Pinning the page i.e. putting the page in buffer pool
		pinPage(&scanTableManager->bufferPool, &scanManager->pageHandle, scanManager->recordID.page);

		char *data = scanManager->pageHandle.data;

		int recordOffset = scanManager->recordID.slot * sizeOfRecord;
		data = incrementPointer(data, recordOffset);

		// Setting record Id from Manager
		memcpy(&(record->id), &(scanManager->recordID), sizeof(RID));

		char *dataPointer = record->data + 1;
          memcpy(dataPointer, data + 1, sizeOfRecord);

		// Increment scan count by 1
		evalExpr(record, schema, scanManager->condition, &result), scanManager->scanCount++, scanCount++;

		// check if result of the scan record has satisfied the condition
		if (result->v.boolV == TRUE)
		{
           BM_PageHandle *pageHandle = &scanManager->pageHandle;
           BM_BufferPool *bufferPool = &scanTableManager->bufferPool;
           unpinPage(bufferPool, pageHandle);
			// Return Seccess Status
			return RC_OK;
		}
	}
    BM_PageHandle *pageHandle = &scanManager->pageHandle;
    BM_BufferPool *bufferPool = &scanTableManager->bufferPool;
    

	// Unpin the page from the buffer pool.
	unpinPage(bufferPool, pageHandle);

	// Reset the Scan Manager's values
	(*scanManager).recordID.page = 1;
	(*scanManager).recordID.slot = 0;
    (*scanManager).scanCount = 0;

	// None of the tuples satisfied the condition
	return RC_RM_NO_MORE_TUPLES;
}

extern RC closeScan(RM_ScanHandle *scan)
{
    // Get the scan manager and table manager
    RecordManager *scanManager = (RecordManager *) scan->mgmtData;
    RecordManager *tableManager = (RecordManager *) scan->rel->mgmtData;
    RecordManager *scanTableManager = scan->rel->mgmtData;
     int i = 0;
     if(i==0){
    // Check if the scan has been completed
    if (scanManager->scanCount >= 1)
    {
		BM_PageHandle *pageHandle = &scanManager->pageHandle;
   		BM_BufferPool *bufferPool = &scanTableManager->bufferPool;
        unpinPage(bufferPool, pageHandle);
        // Reset the Scan Manager's values
	(*scanManager).recordID.page = 1;
	(*scanManager).recordID.slot = 0;
    (*scanManager).scanCount = 0;
    }
    }
    // Deallocate the scan manager's memory
    free(scanManager);
    scan->mgmtData = NULL;

    return RC_OK;
}

//// SCHEMA FUNCTIONS 

// It returns the schema's record size.
int getRecordSize(Schema *schema)
{
	int sizeOfRecord = 0;
	int attributeSize = 0;

	for (int i = 0; i < schema->numAttr; i++) {
    switch (schema->dataTypes[i]) {
        case DT_STRING:
            attributeSize = schema->typeLength[i];
            break;
        case DT_INT:
            attributeSize = sizeof(int);
            break;
        case DT_FLOAT:
            attributeSize = sizeof(float);
            break;
        case DT_BOOL:
            attributeSize = sizeof(bool);
            break;
        default:
            break;
    }
    sizeOfRecord = sizeOfRecord + attributeSize;
}

	return ++sizeOfRecord;
}

Schema *createSchema(int numberofAttribute, char **attributeNames, DataType *dataTypes, int *typeLength, int sizeOfKey, int *keys)
{
	int schemaSize = sizeof(Schema);
	Schema *sch = (Schema *)malloc(schemaSize);
	if (attributeNames != NULL)
	{
		sch->attrNames = attributeNames;
	}
	else
	{
		sch->attrNames = NULL;
	}
	if (dataTypes != NULL)
	{
		sch->dataTypes = dataTypes;
	}
	else
	{
		sch->dataTypes = NULL;
	}
	if (numberofAttribute > 0)
	{
		sch->numAttr = numberofAttribute;
	}
	sch->keyAttrs = keys;
	if (typeLength > 0)
	{
		sch->typeLength = typeLength;
	}
	else
	{
		sch->typeLength = 0;
	}
	
	sch->keySize = sizeOfKey;
	return sch;
}

// deletion of schema
RC freeSchema(Schema *schema)
{
	realloc(schema, 0);
	return RC_OK;
}


//// DEALING WITH RECORDS AND ATTRIBUTE VALUES 

// adding a new record to the schema.
RC createRecord(Record **record, Schema *schema)
{
	Record *newRec = (Record *)calloc(1,sizeof(Record));
	newRec->data = (char *)calloc(1,getRecordSize(schema));
	newRec->id.page = newRec->id.slot = -1;
	char *dataPointer = newRec->data;
	*dataPointer++ = '-';
	*dataPointer = '\0';
	*record = newRec;

	return RC_OK;
}

// setting the attributtes offset to the record
RC attrOffset(Schema *schema, int attributeNumber, int *result)
{
	*result = 1;
for (int iterator = 0; iterator < attributeNumber; iterator++)
{
    switch (schema->dataTypes[iterator])
    {
        case DT_STRING:
            *result += schema->typeLength[iterator];
            break;
        case DT_INT:
            *result += sizeof(int);
            break;
        case DT_FLOAT:
            *result += sizeof(float);
            break;
        case DT_BOOL:
            *result += sizeof(bool);
            break;
        default:
            break;
    }
}

	return RC_OK;
}

// Discarding the record from memory
RC freeRecord(Record *record)
{
	realloc(record, 0);
	return RC_OK;
}

RC getAttr(Record *record, Schema *schema, int attributeNumber, Value **value)
{   
	
	int i = 0;
	int offset = i;
	int j = 0;
	int attributeSize = j;
	// Obtaining the attribute offset value based on the attribute number
	attrOffset(schema, attributeNumber, &offset);

	// Allocating memory for the attributes
	Value *attribute = (Value *)calloc(1, sizeof(Value));

	// Reference beginning address of record's data
	char *dataPtr = incrementPointer(record->data, offset);

	schema->dataTypes[attributeNumber] = (attributeNumber == 1) ? 1 : schema->dataTypes[attributeNumber];


	if (DT_STRING == schema->dataTypes[attributeNumber])
{
    attribute->v.stringV = (char *)calloc(schema->typeLength[attributeNumber] + 1, sizeof(char));

    // Copying string to position which is pointed by dataPtr
    attribute->v.stringV = strndup(dataPtr, schema->typeLength[attributeNumber]);

    attribute->dt = DT_STRING;
}

	else if (schema->dataTypes[attributeNumber] == DT_INT)
	{
		int value = *((int *)dataPtr);

    // Set the attribute value and data type
    attribute->v.intV = value;
    attribute->dt = DT_INT;

    // Update the attribute size
    attributeSize = sizeof(int);
	}
	else if (schema->dataTypes[attributeNumber] == DT_FLOAT)
{
    // Extract the float value from the data buffer
    float value = *((float *)dataPtr);

    // Set the attribute value and data type
    attribute->v.floatV = value;
    attribute->dt = DT_FLOAT;

    // Update the attribute size
    attributeSize = sizeof(float);
}
	else if (schema->dataTypes[attributeNumber] == DT_BOOL)
{
    // Extract the bool value from the data buffer
    bool value = *((bool *)dataPtr);

    // Set the attribute value and data type
    attribute->v.boolV = value;
    attribute->dt = DT_BOOL;

    // Update the attribute size
    attributeSize = sizeof(bool);
}
	else
	{
		printf("Serializer not defined for the given datatype. \n");
	}

	*value = attribute;
	return RC_OK;
}

// The function updates the attribute value of the record based on the provided schema.
RC setAttr(Record *record, Schema *schema, int attributeNumber, Value *value)
{
	bool callattroffset=true;
	char *dataptr;
	int offset_value;
	int incbit;
	int offset_new;
	offset_value=0;
	int len;
	int assignbit;
	int floatvalue;
	int stringvalue;
	int intvalue;
	int boolvalue;
	
	

	
	if(callattroffset!=false)
	{
	incbit=1;
	assignbit=1;
	// Retrieve the attribute offset value based on the attribute number.
	attrOffset(schema, attributeNumber, &offset_value);
	
	}

	if (incbit!=0)
	{
	//Retrieve the starting memory address of the record's data.
	dataptr = record->data;

	dataptr = incrementPointer(dataptr, offset_value);
	}
	if (assignbit!=0)
	{

	floatvalue=DT_FLOAT;
	stringvalue=DT_STRING;
	intvalue=DT_INT;
	boolvalue=DT_BOOL;
	if (schema->dataTypes[attributeNumber] == floatvalue)
	{
		// Assigning a value to an attribute of type FLOAT
		if (incbit!=0)
		{
			dataptr = incrementPointer(record->data, sizeof(float));
		}
		
	}
	
	
	else if (schema->dataTypes[attributeNumber] == boolvalue)
	{
		// Assigning a value to an attribute of type BOOL.
		if (incbit!=0)
		{
			dataptr = incrementPointer(value->v.boolV, sizeof(bool));
		}
	}
	
	
	else if (schema->dataTypes[attributeNumber] == stringvalue)
	{
		// Assigning a value to an attribute of type STRING.
		// Fetching the length of the string as defined in the schema during creation.
		len = schema->typeLength[attributeNumber];

		// Copying the attribute's value to the record pointer.
		strncpy(dataptr, value->v.stringV, len);
		offset_new = schema->typeLength[attributeNumber];
		
		if (incbit!=0)
		{
			dataptr = incrementPointer(dataptr, offset_new);
		}

		
	}

	else if (schema->dataTypes[attributeNumber] == intvalue)
	{
		// Assigning a value to an attribute of type INTEGER.
		*(int *)dataptr = value->v.intV;

		if (incbit!=0)
		{
			dataptr = incrementPointer(dataptr, sizeof(int));
		}

		
	}

	else
	{
		printf("The data type provided does not have a Serializer. \n");
	}
	}

	return RC_OK;
}
