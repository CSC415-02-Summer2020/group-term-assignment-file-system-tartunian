/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: fsLow.h
*
* Description: 
*	This is the file system interface.
*	This is the interface needed by the driver to interact with
*	your filesystem.
*
**************************************************************/
#ifndef _MFS_H
#define _MFS_H
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "fsMakeVol.h"
#define	MAX_FILENAME_SIZE						256
#define MAX_DIRECTORY_DEPTH					10				//8-1-20 Taylor: Added to limit directory depths
#define MAX_DATABLOCK_POINTERS			64
#define INVALID_DATABLOCK_POINTER		-1
#define INVALID_INODE_NAME					"unused_inode"

// The following should be in b_io.h but included for for completness
#ifndef _B_IO_H
#define _B_IO_H
#include <fcntl.h>

int b_open (char * filename, int flags);
int b_read (int fd, char * buffer, int count);
int b_write (int fd, char * buffer, int count);
int b_seek (int fd, off_t offset, int whence);
void b_close (int fd);

#endif

/* In memory structure defined by linux. */

struct mfs_dirent
{
    ino_t          d_ino;       /* inode number  - not used by driver*/
    off_t          d_off;       /* offset to the next dirent  - not used by driver*/
    unsigned short d_reclen;    /* length of this record */
    unsigned char  d_type;      /* type of file; not supported
                                   by all file system types  - not used by driver*/
    char           d_name[256]; /* filename */
};

/* This is the equivalent to an inode in the Unix file system. */

typedef enum { I_FILE, I_DIR } InodeType;

//8-1-20 Taylor: Changed type from int to InodeType
typedef struct
{
		int inUse; // in b_open for create
		InodeType type; // in b_open for create
		char parent[MAX_FILENAME_SIZE];  // in b_open for create
		char children[64][MAX_FILENAME_SIZE]; // in updateInodewhenCeated()
		int numChildren; // in updateInodewhenCeated()
		char name[MAX_FILENAME_SIZE]; // in b_open for create
		// pathe will include the name, ex. 0/1/2/3/4/x, in tis ex. x is the name of the file
		char path[256]; // in b_open for create
		time_t lastAccessTime; // Always whenver opened 
		time_t lastModificationTime; // in b_write 
		blkcnt_t sizeInBlocks; // depends
		off_t sizeInBytes; // depends
		int directBlockPointers[MAX_DATABLOCK_POINTERS]; // in b_write
		int numDirectBlockPointers; // in b_write

} mfs_DIR;

//8-2-20 Taylor: added mfs prefix to readdir, opendir, closedir per Professor's changes
// Added mfs_isFile, mfs_isFile and mfs_delete
// Changed mfs_setcwd to return an int instead of char*
int mfs_mkdir(const char *pathname, mode_t mode);
int mfs_rmdir(const char *pathname);
mfs_DIR * mfs_opendir(const char *fileName);
struct mfs_dirent *mfs_readdir(mfs_DIR *dirp);
int mfs_closedir(mfs_DIR *dirp);

char * mfs_getcwd(char *buf, size_t size);
int mfs_setcwd(char *buf);   //linux chdir

int mfs_isFile(char * path);    //return 1 if file, 0 otherwise
int mfs_isDir(char * path);        //return 1 if directory, 0 otherwise
int mfs_delete(char* filename);    //removes a file

//*******************************//
// Added Functions by Team Penta //
//*******************************//

void mfs_init();																	//8-3-20 Taylor: Changed from fsFileOrgInit
void writeInodes();
void mfs_close();																		//8-3-20 Taylor: Changed from fsFileOrgEnd

void parseFilePath(const char *pathname);
void printFilePath();															//8-1-20 Taylor: Added to test parseFilePath
mfs_DIR* getInode(const char *pathname);
mfs_DIR* getFreeInode();
void printCurrentDirectoryPath();									//8-1-20 Taylor: Added to test mfs_setcwd

/* ADDED ON 8-3-2020 */
// void updateInode(mfs_DIR* inode);
mfs_DIR* createInode(InodeType type,const char* path); // Wameedh!
int checkValidityOfPath();							   // Duy
int setParent(mfs_DIR* parent, mfs_DIR* child);			// Duy
char* getParentPath(char* buf ,const char* path);		// Duy

/* ADDED ON 8-4-20 */

/* Writes a buffer to a provided data block, adds blockNumber to inode, updates size and timestamps
 * of inode, writes inodes to disk. */
int writeBufferToInode(mfs_DIR* inode, char* buffer, size_t bufSizeBytes, uint64_t blockNumber);

//************************************//
// End of our Functions by Team Penta //
//***********************************//


struct mfs_stat {
	dev_t     st_dev;     /* ID of device containing file - not needed by driver*/
	ino_t     st_ino;     /* inode number - not needed by driver program*/
	mode_t    st_mode;    /* protection  - not needed by driver program*/
	nlink_t   st_nlink;   /* number of hard links - not needed by driver program*/
	uid_t     st_uid;     /* user ID of owner - not needed by driver program*/
	gid_t     st_gid;     /* group ID of owner - not needed by driver program*/
	dev_t     st_rdev;    /* device ID (if special file) - not needed by driver program*/
	off_t     st_size;    /* total size, in bytes */
	blksize_t st_blksize; /* blocksize for file system I/O */
	blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
	time_t    st_accesstime;   /* time of last access */
	time_t    st_modtime;   /* time of last modification */
	time_t    st_createtime;   /* time of last status change */
	
	/* add additional attributes here for your file system */
};

int mfs_stat(const char *path, struct mfs_stat *buf);

#endif

