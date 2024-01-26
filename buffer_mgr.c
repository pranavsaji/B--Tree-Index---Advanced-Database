#include <stdio.h>
#include <stdlib.h>
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include <math.h>


int fl = 0; //a flag which increments when page frame is added
int completeWrtCnt = 0; //counts the number of I/O write
int sizeOfBuffer = 0; //size of the buffer pool
int readCountOfNumPgs = 0; //stores count of number of pages
int clkPtr = 0; //tracks last addded page in buffer pool

// Page Frame 
typedef struct bufferPageFrame
{
	bool dirty;
	int flNumber;
	SM_PageHandle data;
	int clientCount;
	PageNumber numOfPgs;
	int refNumber;
} bufferPageFrame;


//  implementation of setNewPg
// This function sets a new page in a specified page frame by copying the content of the given page into it.
void setNewPg(bufferPageFrame *pageFrame, bufferPageFrame *page, int pageFrameIndex)
{
	// Assigning the page number and file number of the new page to the corresponding fields of the page frame.
	
	int pgNum;
	pgNum=page->numOfPgs;
	pageFrame[pageFrameIndex].numOfPgs = pgNum;
	int flNum;
	flNum=page->flNumber;
	pageFrame[pageFrameIndex].flNumber = flNum;
	//Copying the data from the new page to the page frame's data field.
	pageFrame[pageFrameIndex].data = page->data;
	bool isItDirty;
	// Copying the dirty flag, client count, and reference number from the new page to the page frame.
	isItDirty=page->dirty;
	pageFrame[pageFrameIndex].dirty = isItDirty;
	int countOfClient;
	countOfClient=page->clientCount;
	// Note: the reference number field is not copied because it is set to 0 by default for new pages.
	pageFrame[pageFrameIndex].clientCount = countOfClient;

}

// writeToDisk method implemented
//This function is used to persist the data from a page frame to the page file on disk. 
void writeToDisk(BM_BufferPool *const bm, bufferPageFrame *pageFrame, int pageFrameIndex)
{
	bool writblk;
	//first sets the writblk flag to true and opens the page file specified in the buffer pool. 
	writblk=true;
	SM_FileHandle fh;
	openPageFile(bm->pageFile, &fh);
	// Persisting the data from the page frame to the page file on disk.
	
	if(writblk!=false)
	{
	writeBlock(pageFrame[pageFrameIndex].numOfPgs, &fh, pageFrame[pageFrameIndex].data);
	}
	// The buffer manager's total number of writes should be incremented to reflect an increase in completeWrtCnt.
	int dskwrtcnt;
	dskwrtcnt=completeWrtCnt;
	dskwrtcnt=dskwrtcnt+1;
	//it increments the completeWrtCnt variable, which keeps track of the total number of writes performed by the buffer pool
	completeWrtCnt=dskwrtcnt;
	
}

// STRATEGIES //

extern void CLOCK_Strategy(BM_BufferPool *const bm, bufferPageFrame *page) {
    bufferPageFrame *pageFrame = (bufferPageFrame *)bm->mgmtData;
    bool pageFound = false;

    while (!pageFound) {
        // Resetting CLOCK_Strategy pointer
        if (!(clkPtr % sizeOfBuffer))
            clkPtr = 0;
            int a = 1;
         if(a == 1){
        if (pageFrame[clkPtr].clientCount == NULL) {
            // If page in memory has been modified then write the page to the disk
            int B = 1;
            if(B == 1){
            if (pageFrame[clkPtr].dirty == 1) {
                writeToDisk(bm, pageFrame, clkPtr);
            }
            }
            // Setting page frame's content to new page's content
            setNewPg(pageFrame, page, clkPtr);
            clkPtr = clkPtr + 1;
            pageFound = true;
        } }else {
            // If page is already in the buffer pool, update its flNumber
            if ((pageFrame[clkPtr]).numOfPgs == (*page).numOfPgs) {
                pageFound = true;
            }
            pageFrame[clkPtr].flNumber = 1;
            clkPtr = clkPtr + 1;
        }
    }

    // Reset all flNumber flags to 0
    for (int i = 0; i < sizeOfBuffer; i++) {
        pageFrame[i].flNumber = 0;
    }
}

// Implementing LRU_Strategy function
//LRU_Strategy function which is responsible for implementing the Least Recently Used page replacement algorithm. 
void LRU_Strategy(BM_BufferPool *const bm, bufferPageFrame *page)
{  
	 // Getting the pointer to the buffer page frame data structure
	bufferPageFrame *pageframeptr = (bufferPageFrame *)bm->mgmtData;
	//the function declares a pointer to the buffer pool's memory data, and initializes two variables, i and lstflInd
	//Initializing variables
	int i=0;
	int lstflInd;
	int lstflNumber;

	// Assign the first page frame as the least frequently accessed index.
	lstflInd = 0;
	lstflNumber = pageframeptr[0].flNumber;

	// To find the least recently used page frame, identify the page frame with the lowest flNumber.
	i = lstflInd + 1;
	while (i < sizeOfBuffer)
	{
		int flnum;
		flnum=pageframeptr[i].flNumber;
		if (flnum < lstflNumber)
		{
			lstflNumber = flnum;
			lstflInd = i;
			
		}
		i=i+1;
	}
bool isItDirty;
isItDirty=pageframeptr[lstflInd].dirty;
	// If a page in memory has been changed, save the page to disk.
	if (isItDirty != true)
	{
		isItDirty=isItDirty;
	}
	else
	{
		writeToDisk(bm, pageframeptr, lstflInd);
	}

	// Copying the contents of a new page to the contents of the corresponding page frame.
	if (isItDirty==isItDirty)
	{
	setNewPg(pageframeptr, page, lstflInd);
	}
}

//This function implements the FIFO page replacement strategy for a buffer pool
void FIFO_Strategy(BM_BufferPool *const bm, bufferPageFrame *page)
{   
	//Casting the management data to the type of buffer page frame structure.
	bufferPageFrame *pageframeptr = (bufferPageFrame *)bm->mgmtData;
    // Initializing the variables used in this function.
	int i = 0;

	int currInd;
	int numberofpages;
	numberofpages=sizeOfBuffer;
	int pgreadcnt;
	pgreadcnt=readCountOfNumPgs;
    // Using modulo operator to get the index of the page frame that is to be replaced.
	currInd = pgreadcnt % numberofpages;

	
	
	while(i < numberofpages)
	
	{   // Get the client count of the current page frame to determine if it is being accessed by any client.
		int clntcnt;
		// If the current page frame is not currently being accessed by any clients, replace it with the new page.
		clntcnt=pageframeptr[currInd].clientCount;
		// Determine if the current page frame is not currently being accessed by any clients.
		if (clntcnt == 0)
		{
			

		// If a page in memory has been modified, write it to the disk before replacing it.
		bool isItDirty;
		isItDirty=pageframeptr[currInd].dirty; 

			if (isItDirty == true)
			{
				writeToDisk(bm, pageframeptr, currInd);
			}
			int k=0;
			if(k==0)
			{
			// Set the new page in the page frame.
			setNewPg(pageframeptr, page, currInd);

			break;
			}
		}
		else
		{
		    // If the current page frame is being accessed by any clients, move to the next page frame index.
			currInd=currInd+1;
			
			if (currInd % numberofpages != 0)
			{
		
				currInd = currInd;
				
			}
			else
			// If the final page has already been read, the index is reset to 0 to loop over the buffer pool again.
				currInd = 0;
		}
		i++;
	}


}

// BUFFER POOL FUNCTIONS//

RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
						 const int numberOfPages, ReplacementStrategy strategy,
						 void *stratData)
{

	int i=0;
	// pageFrame memory allocation
	//The buffer pool to be initialized.

	bufferPageFrame *page = calloc(numberOfPages, sizeof(bufferPageFrame));

	sizeOfBuffer = numberOfPages;

	


	for (i=0;i < numberOfPages;i++)
	{
		page[i].flNumber = 0;
		page[i].refNumber = 0;	
		page[i].dirty = false;
		
	}

	for (i=0;i < numberOfPages;i++)
	{
		page[i].clientCount = 0;
		page[i].data = NULL;
		page[i].numOfPgs = -1;	
	}


	int j=0;

	if(j==0)
	{
	bm->numPages = numberOfPages;
	bm->mgmtData = page;
	bm->pageFile = (char *)pageFileName;
	completeWrtCnt = clkPtr = 0;
	bm->strategy = strategy;	
	
	}
 return RC_OK;
 
}
// The function shutdownBufferPool is responsible for closing the buffer pool by removing all pages from memory and releasing any unused memory space.
RC shutdownBufferPool(BM_BufferPool *const bm)
{
	bufferPageFrame *page_frameptr = (bufferPageFrame *)bm->mgmtData;
	
	

	int numberOfPages=sizeOfBuffer;

	int i = 0;


	// Verify if any page in the buffer pool is currently being accessed by a user.
	for (i=0;i < numberOfPages;i++)
	{
		// Active users are still accessing the iterated page.
		if (page_frameptr[i].clientCount == 0)
		{
			continue;
		}
		else
		{
			return RC_BUFFER_PIN_PG;
		}
	
	}
// Save any modified or dirty pages to disk.
	forceFlushPool(bm);
	// Freeing up the memory used by the page frame.
	
	bm->mgmtData = NULL;
	free(page_frameptr);
	return RC_OK;
}

// The forceFlushPool function saves all modified pages to disk.
 RC forceFlushPool(BM_BufferPool *const bm)
{
	bufferPageFrame *page_frameptr = (bufferPageFrame *)bm->mgmtData;
	SM_FileHandle fh;	
	int i = 0;
	int numberOfPages=sizeOfBuffer;
	bool isItDirty;
	int clientcnt;

	// Save any modified or dirty pages to disk.
	for (i=0;i < numberOfPages;i++)
	{

		isItDirty=page_frameptr[i].dirty;
		
		clientcnt=page_frameptr[i].clientCount;
		//Verify if a page in the buffer pool is both unused and modified.
		if (clientcnt != 0 )
		{
			continue;
		}
		if (isItDirty == false )
		{
			continue;
		}
		if (clientcnt == 0 )
		{
		if(isItDirty == true)
		{
			int k=0;
			if(k==0)
			{
			openPageFile(bm->pageFile, &fh);
			}
			int l=0;
			if(l==0)
			{
			writeBlock(page_frameptr[i].numOfPgs, &fh, page_frameptr[i].data);
			}
			if(l==0)
			{
			completeWrtCnt++;
			}
			isItDirty=false;
			
			page_frameptr[i].dirty = isItDirty;
			
		}
		}
		
	}
	return RC_OK;
}

// PAGE MANAGEMENT FUNCTIONS

//This function marks the page dirty.

RC markDirty(BM_BufferPool *const bm, BM_PageHandle *const page)
{
	bufferPageFrame *pgFrame = (bufferPageFrame *)bm->mgmtData;
	int iterator = 0;
	//It retrieves the buffer pool's metadata and iterates through the page frames.
	for (iterator; iterator < sizeOfBuffer; iterator = iterator + 1)
	{   // If the page number of the current page frame matches the page number in the page handle,
    	if (page->pageNum == pgFrame[iterator].numOfPgs)
   		{  // then the dirty bit of that page frame is set to true.
        	pgFrame[iterator].dirty = true;
        	return RC_OK;
    	}
	}
	// If no matching page frame is found, the function returns an error code.
	return RC_BUFFER_ERROR;
}

RC unpinPage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
	// Iterate through the buffer pool to find the buffer page corresponding to the given page handle
	int iterator = 0;
	bufferPageFrame *pgFrame = (bufferPageFrame *)bm->mgmtData;
	while (iterator < sizeOfBuffer)
	{
		if (page->pageNum != pgFrame[iterator].numOfPgs)
		{
			iterator = iterator + 1;
		}
		else
		{   // Decrease the clientCount of the buffer page by 1
			pgFrame[iterator].clientCount = pgFrame[iterator].clientCount - 1;
			break;
		}
	}
	// If the buffer page is found, return RC_OK
	return RC_OK;
}

RC forcePage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
	// Get buffer pool page frames
	int iterator = 0;
	bufferPageFrame *pageFrame = (bufferPageFrame *)bm->mgmtData;
	bool isItDirty = pageFrame[iterator].dirty;
	for (iterator; iterator < sizeOfBuffer; iterator = iterator+1)
	{
   		if (pageFrame[iterator].numOfPgs == page->pageNum)
    	{
       		writeToDisk(bm, pageFrame, iterator);
			//Check if the page is dirty
        	isItDirty = false;
    	}
	}
	return RC_OK;
}

bool noPg(BM_BufferPool *const bm, BM_PageHandle *const page,const PageNumber numOfPgs)
{
	    // Access the buffer page frames array
		bufferPageFrame *pgFrame = (bufferPageFrame *)bm->mgmtData;
		// Access the first page frame in the buffer
		const int firstPgPos = 0;
		// Increase the client count of the first page frame
		readCountOfNumPgs = fl;
		// Open the page file
		pgFrame[firstPgPos].clientCount++;
		//Read the total number of pages in the file and set it to the readCountOfNumPgs variable
		SM_FileHandle filehandlepointer;
		openPageFile(bm->pageFile, &filehandlepointer);
		readCountOfNumPgs = 0;
		// Set the page number of the requested page in the page handle
		page->pageNum = numOfPgs;
		// Allocate memory for the page data in the first page frame
		pgFrame[firstPgPos].data = (SM_PageHandle)malloc(PAGE_SIZE);
		// Read the page data from disk and store it in the first page frame
		readBlock(numOfPgs, &filehandlepointer, pgFrame[firstPgPos].data);
		// Update the page frame information
		pgFrame[firstPgPos].numOfPgs = numOfPgs;
		ensureCapacity(numOfPgs, &filehandlepointer);
		pgFrame[firstPgPos].flNumber = fl;
		// Set the data pointer in the page handle to the data in the first page frame
		pgFrame[firstPgPos].refNumber = 0;
		page->data = pgFrame[0].data;
}

RC pinPage(BM_BufferPool *const bm, BM_PageHandle *const page,const PageNumber numOfPgs)
{   
	// Allocate memory for a new page frame and set isTheBufferFull flag to true
	bufferPageFrame *newPg = (bufferPageFrame *)malloc(sizeof(bufferPageFrame));
	bool isTheBufferFull = true;
	// Get the array of page frames from the buffer pool
	bufferPageFrame *pgFrame = (bufferPageFrame *)bm->mgmtData;
	// Initialize the iterator variable to traverse through the array of page frames and the firstPgPos variable to 0
	int iterator = 0;
	const int firstPgPos = 0;
	// Check if the buffer pool is empty, in which case call the noPg function
	if (pgFrame[firstPgPos].numOfPgs == -1)
	{
		noPg(bm, page, numOfPgs);
		return RC_OK;
	}
    // Get the page number of the first page frame
	int Pg = pgFrame[iterator].numOfPgs;
	 
	// Traverse through the array of page frames
	while (iterator < sizeOfBuffer)
	{   
		// Check if the page is already in the buffer pool
		if (pgFrame[iterator].numOfPgs == numOfPgs)
		{
			isTheBufferFull = false;
			fl++;
			// Increment the clientCount for that page
			pgFrame[iterator].clientCount++;
             // If the replacement strategy is LRU, set the flNumber for that page to the current value of fl
			if (bm->strategy == RS_LRU)
				pgFrame[iterator].flNumber = fl;
            // Set the page number and data of the page handle to the current page frame
			page->pageNum = numOfPgs;
			page->data = pgFrame[iterator].data;
             // If the replacement strategy is CLOCK, set the flNumber for that page to 1
			if (bm->strategy == RS_CLOCK)
			pgFrame[iterator].flNumber = 1;
            // Increment the clock pointer
			clkPtr = clkPtr +1;
			break;
		}

		// Check if the page frame is empty
		int iter=1;
		if (Pg == -1) {
			if (iter == 1)
			{
				// Read the block from the page file into the page frame data
			SM_FileHandle filehandlepointer;
			openPageFile(bm->pageFile, &filehandlepointer);
			pgFrame[iterator].data = (SM_PageHandle)malloc(PAGE_SIZE);
			readBlock(numOfPgs, &filehandlepointer, pgFrame[iterator].data);
			readCountOfNumPgs = readCountOfNumPgs+1;
			pgFrame[iterator].numOfPgs = numOfPgs;
			// Initialize the reference number and client count for that page frame and increment the fl value
			pgFrame[iterator].refNumber = 0;
			
			fl = fl +1;
			pgFrame[iterator].clientCount = 1;

			if (bm->strategy == RS_LRU)
				pgFrame[iterator].flNumber = fl;

			page->pageNum = numOfPgs;
			page->data = pgFrame[iterator].data;

			if (bm->strategy == RS_CLOCK)
				pgFrame[iterator].flNumber = 1;

			isTheBufferFull = false;
			break;
			}
		}

		
		
		iterator = iterator+1;
	}

	// If the buffer is full and the requested page is not in the buffer,
    // then we need to replace an existing page with the requested one.

	if (isTheBufferFull == true)
	{

		// Open the page file to read the requested page
		SM_FileHandle filehandlepointer;
		openPageFile(bm->pageFile, &filehandlepointer);
		// Create a new page frame for the requested page
		bufferPageFrame *newPg = (bufferPageFrame *)malloc(sizeof(bufferPageFrame));
		// Increment the frame number for the new page
		fl = fl + 1;
		// Increment the frame number for the new page
		newPg->refNumber = 0;
		// Allocate memory for the new page data
		newPg->data = (SM_PageHandle)malloc(PAGE_SIZE);
		// Set the page number of the new page to the requested page number
		newPg->numOfPgs = numOfPgs;
		// Read the requested page from the file into the new page's data
		readBlock(numOfPgs, &filehandlepointer, newPg->data);
		// Increment the read count for the requested page
		readCountOfNumPgs = readCountOfNumPgs +1;
		// Set the new page's dirty bit to false
		newPg->dirty = !true;
		// Set the client count of the new page to 1, since it's the first client accessing it
		newPg->clientCount = 1;
	
	    // Set the frame number of the new page based on the replacement strategy
		switch (bm->strategy) {
    		case RS_LRU:
        		newPg->flNumber = fl;
        		break;
    		case RS_CLOCK:
        		newPg->flNumber = 1;
        		break;
    		default:
        		// Do nothing
        		break;
		}

        // Set the page handle's page number and data to point to the new page
		page->pageNum = numOfPgs;
		page->data = newPg->data;
        // Call the appropriate replacement strategy function based on the buffer manager's strategy
		ReplacementStrategy strategy = bm->strategy;

		switch (strategy) {
    		case RS_FIFO:
        		FIFO_Strategy(bm, newPg);
        		break;
    		case RS_LRU:
        		LRU_Strategy(bm, newPg);
        		break;
    		case RS_CLOCK:
        		CLOCK_Strategy(bm, newPg);
        		break;
    		default:
        		printf("\n No Strategy Implemented\n");
			}

	}
	// Return OK status code to indicate successful completion
	return RC_OK;
}

//STATISTICS FUNCTIONS//

PageNumber *getFrameContents(BM_BufferPool *const bm)
{   
	// Allocate memory for the frame contents array
    PageNumber *frameContents = malloc(sizeof(PageNumber) * bm->numPages);
	// Get the pointer to the buffer pool's management data
    bufferPageFrame *pageFrame = (bufferPageFrame *) bm->mgmtData;
	// Iterate over all the pages in the buffer pool and store their page numbers in the frameContents array
	int iterator = 0;
    for (iterator; iterator < bm->numPages; iterator = iterator+1)
    {
        if (pageFrame[iterator].numOfPgs == -1)
        {
			// If the page number of the page in this frame is -1, it means that the frame is empty
             // So, we store NO_PAGE in the frameContents array for this frame
			frameContents[iterator] = NO_PAGE;
        }
        else
        {
			// If the page number of the page in this frame is not -1, it means that the frame is not empty
           // So, we store the page number of the page in this frame in the frameContents array for this frame
			 frameContents[iterator] = pageFrame[iterator].numOfPgs;
        }
    }
	// Return the frame contents array
    return frameContents;
}

bool *getDirtyFlags(BM_BufferPool *const bm)
{   
     int iter;
    bufferPageFrame *pageFrame = (bufferPageFrame *)bm->mgmtData;
    bool *isPageDirtyFlags = malloc(sizeof(bool) * sizeOfBuffer);
    // Iterate through each page in the buffer pool
    for (iter = 0; iter < sizeOfBuffer; iter++)
    {
		// Store the dirty flag for this page in the isPageDirtyFlags array
        isPageDirtyFlags[iter] = pageFrame[iter].dirty;
    }
    // Return the array of dirty flags
    return isPageDirtyFlags;
}

int getNumWriteIO(BM_BufferPool *const bm)
{ 
	// Cast the management data to buffer page frame.
   bufferPageFrame *pageFrame = (bufferPageFrame *)bm->mgmtData;
   // Get the number of completed write operations from the global variable.
    int numWrites = completeWrtCnt;
	// Return the number of completed write operations.
    return numWrites;
}

int *getFixCounts(BM_BufferPool* const bm)
{    

	// Get the buffer pool's page frames
   bufferPageFrame* pageFrames = (bufferPageFrame*)bm->mgmtData;
   // Allocate memory for the fix counts array
   int* fixCounts = malloc(sizeof(int) * bm->numPages);
    // Loop through all the pages in the buffer pool
    int iter;
    for (iter = 0; iter < bm->numPages; iter++)
    {
		// If the client count for a page is not -1, set its fix count to the client count
        if (!(pageFrames[iter].clientCount == -1))
        {
            fixCounts[iter] = (pageFrames[iter].clientCount);
        }
		// Otherwise, set the fix count for the page to false
        else
        {
            fixCounts[iter] = false;
        }
    }

    return fixCounts;
}

int getNumReadIO(BM_BufferPool *const bm)
{
    // Cast the management data to the buffer pool information struct
   bufferPageFrame* pgrRame = ( bufferPageFrame*)bm->mgmtData;
    // Return the number of pages that have been read from disk
    return ++readCountOfNumPgs;
}

