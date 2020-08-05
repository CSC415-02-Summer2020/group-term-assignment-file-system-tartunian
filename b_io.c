/**************************************************************
* Class:  CSC-415-02 Summer 2020
* Name: Taylor Artunian
* Student ID: 920351715
* Project: Assignment 5 – Buffered I/O
*
* File: b_io.c
*
* Description: Buffered io module - Now with b_write
*
**************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>			// for malloc
#include <string.h>			// for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "b_io.h"
#include "mfs.h"

#define MAXFCBS 20

/* Added this so that it can be set at runtime in b_init from value of getVCB()->blockSize. */
uint64_t bufSize;

typedef enum  {NoWRITE,WRITE} fileMode;

typedef struct b_fcb {
	int linuxFd;	//holds the systems file descriptor
	char * buf;		//holds the open file buffer
	int index;		//holds the current position in the buffer
	int buflen;		//holds how many valid bytes are in the buffer
	fileMode mode; // sets when file if open for write 
	mfs_DIR* inode; //holdsa pointer to the inode associated with the file 

	} b_fcb;
	
b_fcb fcbArray[MAXFCBS];

int startup = 0;	//Indicates that this has not been initialized

//Method to initialize our file system
void b_init ()
	{

		/* Set buffer size equal to the block size of our file system. */
		bufSize = getVCB()->blockSize;

	//init fcbArray to all free
	for (int i = 0; i < MAXFCBS; i++)
		{
		fcbArray[i].linuxFd = -1; //indicates a free fcbArray
		fcbArray[i].mode = NoWRITE; //indicates a free fcbArray
		}
		
	startup = 1;
	}

//Method to get a free FCB element
int b_getFCB ()
	{
	for (int i = 0; i < MAXFCBS; i++)
		{
		if (fcbArray[i].linuxFd == -1)
			{
			fcbArray[i].linuxFd = -2; // used but not assigned
			return i;		//Not thread safe
			}
		}
	return (-1);  //all in use
	}
	
// Interface to open a buffered file
// Modification of interface for this assignment, flags match the Linux flags for open
// O_RDONLY, O_WRONLY, or O_RDWR


/* Todo: Make mode field on fcb correspond to flags for consistency. */
int b_open (char * filename, int flags)
	{
	int fd;
	int returnFd;
	
	/*** TODO ***:  Modify to save or set any information needed*/
	if (startup == 0) b_init();  //Initialize our system
	
	// lets try to open the file before I do too much other work
	
	//fd = open (filename, flags);
	//if (fd  == -1)
	//	return (-1);		//error opening filename

	//Should have a mutex here
	returnFd = b_getFCB();				// get our own file descriptor
																// check for error - all used FCB's
	

	/* Check if there was an available fcb returned to us. 
	 * Return -1 if there was not. */
	b_fcb* fcb;
	if(returnFd < 0) {
		return -1;
	}

	fcb = &fcbArray[returnFd];

	// For cmd_cp2l 
    // Check if file exist
    // if exist just open
    // if not exit

  // For cmd_cp2fs()
    // Check if file exist
    // if exists just open
       // Get inode
    // if not create!
      // Creat inode

	/* CMD_CP2L */
	/* Get inode and check if it exists. !inode = !NULL = exists */
	mfs_DIR* inode = getInode(filename);
	if(!inode) {
		
		/* CMD_CP2FS */
		/* Check for create mode. */
		if(flags & O_CREAT) {

			/* Create child, get parent and set parent. */
			inode = createInode(I_FILE, filename);
			char parentpath[MAX_FILENAME_SIZE];
			getParentPath(parentpath, filename);
			mfs_DIR* parent = getInode(parentpath);
			setParent(parent, inode);

		} else {

			/* File doesn't exist and we don't have create mode. */
			return -1;
		}
	}

	/* Save the inode in the FCB. */
	fcb->inode = inode;

	//	release mutex
	
	//allocate our buffer
	fcb->buf = malloc (bufSize);
	if (fcb->buf  == NULL)
		{
		// very bad, we can not allocate our buffer
		close (fd);							// close linux file
		fcb->linuxFd = -1; 	//Free FCB
		return -1;
		}
		
	fcb->buflen = 0; 			// have not read anything yet
	fcb->index = 0;			// have not read anything yet
	return (returnFd);						// all set
	}


// Interface to write a buffer	
int b_write (int fd, char * buffer, int count)
	{
	if (startup == 0)
		{
		b_init();  //Initialize our system
		}

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}

	if (fcbArray[fd].linuxFd == -1)		//File not open for this descriptor
		{
		return -1;
		}


	/* Calculate free space in buffer of FCB.
	 */

	b_fcb* fcb = &fcbArray[fd];
	int freeSpace = bufSize - fcb->index;

	printf("b_write: count=%d, fcb->index=%d, freeSpace=%d\n", count, fcb->index, freeSpace);

	fcb->mode = WRITE; // set the file to write the last bytes in b_close

	/* Calculate how many bytes can fit at the end of the buffer and if there is any overflow.
	 * Copy these chunks to the buffer and update index.
	 */

	int copyLength = freeSpace > count ? count : freeSpace;
	int secondCopyLength = count - copyLength;

	printf("Copying first segment to fcb->buf+%d: %d bytes\n", fcb->index, copyLength);
	memcpy(fcb->buf+fcb->index, buffer, copyLength);
	fcb->index += copyLength;
	fcb->index %= bufSize;

	/* If there is a second segment, write out the full buffer,
	 * reset buffer and copy second segment to buffer.
	 */

	if(secondCopyLength) {
		printf("Writing buffer to fd.\n"); 
		uint64_t indexOfBlock = getFreeBlock();

		//write(fcbArray[fd].linuxFd, fcb->buf, BUFSIZE);

		if (indexOfBlock == -1){
			printf("There is not enough free space!");
			return 0;
		} else {

			/* Write block of data to disk. */
			writeBufferToInode(fcb->inode, fcb->buf, copyLength + secondCopyLength, indexOfBlock);
		}
		fcb->index = 0;
		printf("Copying second segment to fcb->buf+%d: %d bytes\n", fcb->index, secondCopyLength);
		memcpy(fcb->buf+fcb->index, buffer+copyLength, secondCopyLength);
		fcb->index += secondCopyLength;
		fcb->index %= bufSize;
	}

// Copy above in b_close

	// Return total bytes copied to buffer.
	return copyLength + secondCopyLength;
	}



// Interface to read a buffer	
int b_read (int fd, char * buffer, int count)
	{
	int bytesRead;				// for our reads
	int bytesReturned;			// what we will return
	int part1, part2, part3;	// holds the three potential copy lengths
	
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
		
	if (fcbArray[fd].linuxFd == -1)		//File not open for this descriptor
		{
		return -1;
		}	
		
	
	// number of bytes available to copy from buffer
	int remain = fcbArray[fd].buflen - fcbArray[fd].index;	
	part3 = 0;				//only used if count > BUFSIZE
	if (remain >= count)  	//we have enough in buffer
		{
		part1 = count;		// completely buffered
		part2 = 0;
		}
	else
		{
		part1 = remain;				//spanning buffer (or first read)
		part2 = count - remain;
		}
				
	if (part1 > 0)	// memcpy part 1
		{
		memcpy (buffer, fcbArray[fd].buf + fcbArray[fd].index, part1);
		fcbArray[fd].index = fcbArray[fd].index + part1;
		}
		
	if (part2 > 0)		//We need to read to copy more bytes to user
		{
		// Handle special case where user is asking for more than a buffer worth
		if (part2 > bufSize)
			{
			int blocks = part2 / bufSize; // calculate number of blocks they want
			bytesRead = read (fcbArray[fd].linuxFd, buffer+part1, blocks*bufSize);
			part3 = bytesRead;
			part2 = part2 - part3;  //part 2 is now < BUFSIZE, or file is exusted
			}				
		
		//try to read BUFSIZE bytes into our buffer
		bytesRead = read (fcbArray[fd].linuxFd, fcbArray[fd].buf, bufSize);
		
		// error handling here...  if read fails
		
		fcbArray[fd].index = 0;
		fcbArray[fd].buflen = bytesRead; //how many bytes are actually in buffer
		
		if (bytesRead < part2) // not even enough left to satisfy read
			part2 = bytesRead;
			
		if (part2 > 0)	// memcpy bytesRead
			{
			memcpy (buffer+part1+part3, fcbArray[fd].buf + fcbArray[fd].index, part2);
			fcbArray[fd].index = fcbArray[fd].index + part2;
			}
			
		}
	bytesReturned = part1 + part2 + part3;
	return (bytesReturned);	
}

int b_seek(int fd, off_t offset, int whence) {
	if(fd >= MAXFCBS || fd < 0 || fcbArray[fd].linuxFd == -1) { //fd is invalid or not being used
		return -1;
	}
	
	if( whence != SEEK_SET || whence != SEEK_CUR || whence != SEEK_END ) { //whence is invalid
		return -1;
	}

	if(offset < 0) { //invalid offset, saying negative is not a proper offset
		return -1;
	}
	
	//supposed to be able to set locations past the end of file to null
	if( whence == SEEK_SET ) { //SEEK_SET sets to the next location adjusted at [current index + offset]
		fcbArray[fd].index += offset; //should be able to go past file size/end of file
		return fcbArray[fd].index;
	}
	
	else if( whence == SEEK_CUR ) { //SEEK_CUR sets index to be the given offset
		fcbArray[fd].index = offset;
		return fcbArray[fd].index;
	}
	
	else if( whence == SEEK_END ) { //SEEK_END sets index to be the [end of the file + offset]
		fcbArray[fd].index = bufSize + offset;
		return fcbArray[fd].index;
	}
}

// Interface to Close the file	
void b_close (int fd)
	{
	b_fcb* fcb = &fcbArray[fd];
	printf("Closing file %d. Writing %d remaining bytes to fd.\n", fd, fcb->index);

//	write(fcb->linuxFd, fcb->buf, fcb->index);	//Write any remaining bytes to fd.

	if (fcb->mode == WRITE && fcb->index > 0){ // write last bytes
			uint64_t indexOfBlock = getFreeBlock();
			if (indexOfBlock == -1){
				printf("There is no enough free space!");
			} else {

				/* Write any remaining bytes (index) to a new block. */
				writeBufferToInode(fcb->inode, fcb->buf, fcb->index, indexOfBlock);
			}
	}

	close (fcb->linuxFd);		// close the linux file handle
	free (fcb->buf);			// free the associated buffer
	fcb->buf = NULL;			// Safety First
	fcb->linuxFd = -1;			// return this FCB to list of available FCB's 
	}
