#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<math.h>
#include <stdbool.h> 
#include "storage_mgr.h"

//creating a file pointer 
FILE *Fpointer;

void initStorageManager (void){
    printf("..........Initializing Storage Manager..........\n");
}

//in this function new page file is created here 
RC createPageFile (char *Fname){    
    //here the new page file is created 
    Fpointer = fopen(Fname, "w+");
    if (Fname != NULL){
        //allocating a memory for the page size 
    SM_PageHandle newBlankPg = (SM_PageHandle)calloc(PAGE_SIZE, sizeof(char)); 
    fwrite(newBlankPg, 1, PAGE_SIZE, Fpointer);
    //closing the file pointer 
    fclose(Fpointer);
    printf("File is closed successfully\n");

    //Memory Management using free() function
    free(newBlankPg);
    printf("Memory allocated is freed successfully\n");
    return RC_OK;
    }
    else
    {
        return RC_FILE_NOT_FOUND;
    }
}

RC openPageFile (char *fileName, SM_FileHandle *filehandlepointer) {
	Fpointer = fopen(fileName, "r+");
	if (filehandlepointer == NULL)
	{
		return RC_FILE_HANDLE_NOT_INIT;
	}
	else
	{
		if(Fpointer != NULL) {
		filehandlepointer->fileName = fileName;
		filehandlepointer->curPagePos = 0;
		struct stat fileInfo;
		if(fstat(fileno(Fpointer), &fileInfo) < 0)    
		return RC_BUFFER_ERROR;
		filehandlepointer->totalNumPages = fileInfo.st_size/ PAGE_SIZE;
		fclose(Fpointer);
		return RC_OK;
	} else { 
		return RC_FILE_NOT_FOUND;
	}
	}
}

extern RC closePageFile (SM_FileHandle *fHandle) {
	// Checking if file pointer or the storage manager is intialised. If initialised, then close.
	 if (Fpointer == NULL) 
    {
        return RC_FILE_NOT_FOUND;
    }
	if(Fpointer == NULL)
		Fpointer = NULL;	
	printf("Succesfully the file is closed \n ");
	return RC_OK; 
}


RC destroyPageFile (char *Fname)
{	// here we are trying to remove if there is any file existing 
	Fpointer = fopen(Fname, "r");
    if(Fpointer != NULL){
        //closing file before terminating
        fclose(Fpointer);
        
        unlink(Fname);
        Fpointer=NULL;
        
        return RC_OK;
    
        }
    else
    {
         return RC_FILE_NOT_FOUND;
    }
}

//the page in the memory block is readed 

RC readBlock (int pagenumber, SM_FileHandle *filehandleptr, SM_PageHandle pagehandleptr)
{
     /*
This method opens the file and reads the pagenumber value or block which is passed as an argument and then it stores its value or content to memory with the help of Pagehandleptr, which is the page handle.
*/
    Fpointer = fopen(filehandleptr->fileName, "r");
    if (Fpointer != NULL)
    {
        printf("The file is opened\n");
        //Page number is passed as an argument to this function and if this value is greater than the total number of pages, then it should throw an error code

        if ((filehandleptr->totalNumPages)> pagenumber && pagenumber>=0)
        {
            printf("\nYaay! The page you are looking exists!");
        }
        else
        {
             int totalPagenum;
             totalPagenum=filehandleptr->totalNumPages;
    
            return RC_READ_NON_EXISTING_PAGE;
        }
        int pagestart = (pagenumber *PAGE_SIZE);

       //using SEEK_SET to point to the start of the page
        if(fseek(Fpointer, pagestart, SEEK_SET)!=0)
        {
            printf("\nFailed to put pointer to start of the page");
            exit(1);
        }
        else
        {
            printf("\nPointer is now pointed to the start of the page");
        }
	    printf("\nBefore reading the file, the pointer is positioned at  %d \n \n", pagestart);
        int ptrpos;
        fread(pagehandleptr, sizeof(char), PAGE_SIZE, Fpointer);
	    ptrpos=ftell(Fpointer);

        printf("After reading the file, the pointer is positioned at  %d \n \n", ptrpos);
           
        int currentpos;
      
        if((ptrpos % PAGE_SIZE)==0){
        currentpos=((ptrpos/PAGE_SIZE)-1);
        }
        else
        {
          currentpos=(ptrpos/PAGE_SIZE);
        }
       
        // Updating the current position of pointer in the struct SM_FileHandle
        filehandleptr -> curPagePos = currentpos;

     int a=filehandleptr -> curPagePos;
     
     int b=filehandleptr->totalNumPages;
    

            if(fclose(Fpointer) == 0)
            {
                printf("The file is closed succesfully\n");
                return RC_OK;
            }
            
            else  
            {
                printf("\nOops! The file is not closed \n");
                exit(1);
            }
    }
    else 
    {
        printf("\nThe file does not exist\n");
        return RC_FILE_NOT_FOUND;
        
    }
}

// getting the position of the current block 
int getBlockPos (SM_FileHandle *filehandleptr) 
{   
    if (filehandleptr != NULL) 
    {
        int bposition = filehandleptr->curPagePos;
       
        printf("the position of the Block is %d\n", bposition);
        
    //returning the currentpage position
        return bposition;
    }
    if(filehandleptr == NULL)
    printf("the given File handler is not initiated.");
    return 0;
}

// reading the page from first block 
RC readFirstBlock (SM_FileHandle *filehandleptr, SM_PageHandle pagehandleptr)
{
    if (filehandleptr == NULL) {
        return RC_FILE_NOT_FOUND;
    }
    //calling the readBlock() fuction 
   int bposition = filehandleptr->curPagePos;
        printf("the position of the Block before reading first block is %d\n", bposition);

    return readBlock(0, filehandleptr, pagehandleptr);
}

// reading the page from Previous block 

RC readPreviousBlock (SM_FileHandle *filehandleptr, SM_PageHandle pagehandleptr)
{
    int currentPgPos;
    int prevPgNum;
    currentPgPos = getBlockPos(filehandleptr);
    prevPgNum = currentPgPos - 1;
    filehandleptr->curPagePos=prevPgNum;
     int bposition = filehandleptr->curPagePos;
        printf("the position of the Block after reading prev block is %d\n", bposition);

    //calling the readBlock() fuction 
    return readBlock(prevPgNum, filehandleptr, pagehandleptr);

}

// reading the page from current block 
RC readCurrentBlock (SM_FileHandle *filehandleptr, SM_PageHandle pagehandleptr)
{
	int currentPgPos;
	currentPgPos = getBlockPos(filehandleptr);
     int bposition = filehandleptr->curPagePos;
        printf("the position of the Block after reading current block is %d\n", bposition);
    //calling the readBlock() fuction
    return readBlock(currentPgPos, filehandleptr, pagehandleptr);
}

// reading the page from next block 
RC readNextBlock (SM_FileHandle *filehandleptr, SM_PageHandle pagehandleptr)
{
    int currentPgPos;
    int nextPgPos;
	currentPgPos = getBlockPos(filehandleptr);
    nextPgPos = currentPgPos + 1;
    filehandleptr -> curPagePos=nextPgPos;
       int currentpos=filehandleptr -> curPagePos;
     printf("current position of block after readnextblock is %d \n",currentpos);
	fclose(Fpointer); 
    //calling the readBlock() fuction
    return readBlock(nextPgPos, filehandleptr, pagehandleptr);
}

// reading the page from Last block 
RC readLastBlock (SM_FileHandle *filehandleptr, SM_PageHandle pagehandleptr)
{
    int lastPgNum = filehandleptr->totalNumPages - 1;
    //calling the readBlock() fuction
    return readBlock(lastPgNum, filehandleptr, pagehandleptr);
}

RC writeBlock (int pageNum, SM_FileHandle *filehandlepointer, SM_PageHandle pagehandleptr) 
{
	int i=0;
	// If the pageNumber parameter is either less than 0 or greater than or equal to the total number of pages, return the appropriate error code.
	if (pageNum > filehandlepointer->totalNumPages)
	{
		return RC_WRITE_FAILED;
	}
	int m=0;
	if (pageNum < 0)
	{
        return RC_WRITE_FAILED;
	}
	// The file stream is opened in both read and write mode using the 'r+' mode.
	
	if(m==0)
	{
	Fpointer = fopen(filehandlepointer->fileName, "r+");
	}
	// Verifying whether the file was opened successfully.
	
	if(Fpointer != NULL)
	{
	int startPos;
	int num_pg;
	num_pg=pageNum;
	int pg_sz;
	pg_sz=PAGE_SIZE;
	startPos = num_pg * pg_sz;
	if (num_pg != 0) 
	{	
		// Recording information onto the initial page.
		filehandlepointer->curPagePos = startPos;
		fclose(Fpointer);
		writeCurrentBlock(filehandlepointer, pagehandleptr);
	}
	else
	{ 
		int seek;
		seek = 1;
		
		//Writing data to non-first page
		
		if(seek!=0)
		fseek(Fpointer, startPos, SEEK_SET);	
		
		while(i < pg_sz)
		{
		
			// Verifying whether it is the end of the file. If so, add an empty block to the end.

			// Check if the file has ended while writing.
			int pgend;
			pgend=feof(Fpointer);

			if(pgend != 0)
			{ 
				 appendEmptyBlock(filehandlepointer);
			}
			int pgout=0;
			if (pgout!=1)
			{
			// Copying a character from pagehandleptr to the page file.		
			fputc(pagehandleptr[i], Fpointer);
			}
			i=i+1;
		
		}
		int tell=0;
		if(tell!=1)
		{
		// Adjusting the current page position to match the file stream's cursor or pointer position.
		filehandlepointer->curPagePos = ftell(Fpointer); 
		}
		int close=0;
		if (close!=1)
		{
		// Terminating the file stream to ensure all buffers are flushed.
		fclose(Fpointer);	
		}
	} 
	return RC_OK;
	}
	else
	{
		return RC_FILE_NOT_FOUND;
	}
}

RC writeCurrentBlock (SM_FileHandle *filehandlepointer, SM_PageHandle pagehandleptr) {
	Fpointer = fopen(filehandlepointer->fileName, "r+");
	if (filehandlepointer != 0)
	{
		if(Fpointer != NULL)
		{
			appendEmptyBlock(filehandlepointer);
			int curPos = filehandlepointer->curPagePos;
			fseek(Fpointer, curPos, SEEK_SET);
			fwrite(pagehandleptr, 1, strlen(pagehandleptr), Fpointer);
			curPos = ftell(Fpointer);     	
			fclose(Fpointer);
			return RC_OK;
	}
	else
	{
		return RC_FILE_NOT_FOUND;
	}
	}
	else
	{
		return RC_FILE_HANDLE_NOT_INIT;
	}
	
}


RC appendEmptyBlock (SM_FileHandle *filehandlepointer) {
	if (filehandlepointer != 0)
	{
		SM_PageHandle emptyBlock = (SM_PageHandle)calloc(PAGE_SIZE, sizeof(char));
	int j = 0;
	if( fseek(Fpointer, j, SEEK_END) != 0 ) {
		free(emptyBlock);
		return RC_WRITE_FAILED;
	} else {
		fwrite(emptyBlock, sizeof(char), PAGE_SIZE, Fpointer);
	}
	free(emptyBlock);
	filehandlepointer->totalNumPages = filehandlepointer->totalNumPages +1;
	return RC_OK;
	}
	else
	{
		return RC_FILE_HANDLE_NOT_INIT;
	}
	

}

/*This function compares the passed number of page and the total number of page in the file and 
then adds that many number of empty blocks or pages which is further needed using appendEmptyBlock(...).*/
RC ensureCapacity (int totalNumofPages, SM_FileHandle *filehandleptr)
{
	Fpointer = fopen(filehandleptr->fileName, "a");

	if(Fpointer != NULL)
	{    //checking the total number of pages in our file with the number of pages passed as the argument 
		if(totalNumofPages <= filehandleptr->totalNumPages) 
	    {
            
		    return RC_OK;
	    }
	    else
        {      
            //call the appendEmptyBlock() block till it satifies the condition 
		    while((filehandleptr->totalNumPages) < totalNumofPages)
		    {

			    appendEmptyBlock(filehandleptr);
		    }

	    } 
	}
    else
    {
         printf("\nThe file does not exist\n");
        return RC_FILE_NOT_FOUND;
    }
         int currentpos=filehandleptr -> curPagePos;
     printf("current position of block after appendemptyblock or ensure capacity is %d \n",currentpos);
      int totalpage=filehandleptr->totalNumPages;
     printf("total pagenumber after appendemptyblock or ensure capacity is %d \n",totalpage);
	fclose(Fpointer); 
    return RC_OK;
}