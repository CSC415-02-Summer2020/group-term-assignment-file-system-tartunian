
#include "mfs.h"

mfs_DIR* inodes;

size_t NumberOfElementsInInodesArray = sizeof(inodes)/sizeof(inodes[0]); // calculate the number of elemnts in inodes array

void fsFileOrgInit() {
  printf("---------------------------fsFileOrgInit------------------------\n");
  uint64_t totalBytes = getVCB()->totalInodeBlocks * getVCB()->blockSize;
  printf("totalInodeBlocks %ld, blockSize %ld\n", getVCB()->totalInodeBlocks, getVCB()->blockSize);
  printf("Allocating %ld bytes for inodes.\n", totalBytes);
  inodes = calloc(getVCB()->totalInodeBlocks, getVCB()->blockSize);
  printf("Inodes allocated at %p.\n", inodes);
  uint64_t blocksRead = LBAread(inodes, getVCB()->totalInodeBlocks, getVCB()->inodeStartBlock);
  printf("Loaded %ld blocks of inodes into cache.\n", blocksRead);
}

void writeInodes() {
  LBAwrite(inodes, getVCB()->totalInodeBlocks, getVCB()->inodeStartBlock);
}

//8-1-20 Taylor: Initial implementation of parseFilePath.
char currentDirectoryPath[MAX_FILENAME_SIZE];
char currentDirectoryPathArray[MAX_DIRECTORY_DEPTH][MAX_FILENAME_SIZE] = { "/" };
int currentDirectoryPathArraySize = 1;

char requestedFilePathArray[MAX_DIRECTORY_DEPTH][MAX_FILENAME_SIZE];
int requestedFilePathArraySize;

void parseFilePath(const char *pathname) {

  /* Clear previous count. */
  requestedFilePathArraySize = 0;

  /* Make mutable copy of pathname. */
  char _pathname[MAX_FILENAME_SIZE];
  strcpy(_pathname, pathname);

  /* Setup tokenizer and check for '.' or '..' at beginning. */
  char* savePointer;
  char* token = strtok_r(_pathname, "/", &savePointer);
  int isRelative = !strcmp(token, "..") || !strcmp(token, ".");
  if(token) {
    if(isRelative) {
      for(int i=0; i<currentDirectoryPathArraySize-1; i++) {
        strcpy(requestedFilePathArray[i], currentDirectoryPathArray[i]);
        requestedFilePathArraySize++;
      }
    }
  }

  /* Discard '.' or '..'. */
  if(isRelative) {
    token = strtok_r(0, "/", &savePointer);
  }

  while(token && requestedFilePathArraySize < MAX_DIRECTORY_DEPTH) {

    strcpy(requestedFilePathArray[requestedFilePathArraySize], token);
    requestedFilePathArraySize++;
    token = strtok_r(0, "/", &savePointer);

  }

}

//8-1-20 Taylor: Added to test parseFileName
void printFilePath() {

  for(int i=0; i<requestedFilePathArraySize; i++) {

    if(i<requestedFilePathArraySize-1) {
      printf("Directory %d: %s\n", i, requestedFilePathArray[i]);
    } else {
      printf("Filename: %s\n", requestedFilePathArray[i]);
    }

  }
}

mfs_DIR* getInode(const char *pathname){
  // 1- Loop over inodes
  // 2- find requested node
  // 3- return that node
  // if does not exist return NULL
  mfs_DIR* returnediNode;

  for (size_t i = 0; i < NumberOfElementsInInodesArray; i++) {
    if (strcmp(inodes[i].path, pathname) == 0) {
      returnediNode = &inodes[i];
      return returnediNode;
      break;
    }
  }
  return NULL;

}

mfs_DIR* getFreeInode(){
  // Search through inodes
  // Return the first avalable inode
  // Update inUSe int of the returned node to 1 (Which means it's in use right now)
  // if there is no free inode return NULL
  mfs_DIR* returnediNode;
  for (size_t i = 0; i < NumberOfElementsInInodesArray; i++) {
    if (inodes[i].inUse == 0) { // if the inode inUse equales 0 that means it is free so we return it
      inodes[i].inUse = 1; // upsdate the node to be in use before returning it
       returnediNode = &inodes[i];
      return returnediNode;
      
    }
  }
  return NULL;

}

// int b_open (char * filename, int flags){

// // Check flags
// // If flags read only then it is md_cp2l()
// // If flags write only | Create then it is cmd_cp2fs()

//  // For cmd_cp2l 
//     // Check if file exist
//     // if exist just open
//     // if not exit

//   // For cmd_cp2fs()
//     // Check if file exist
//     // if exist just open
//        // Get inode
//     // if not create!
//       // Creat inode

// // Get code from preveus assginment 

// };

// int b_write (int fd, char * buffer, int count) {

// // Check if there is storage
//   // if there is storage then write
//   // else exit
// // get free block to write


// // Get code from preveus assginment 

// }


// int b_read (int fd, char * buffer, int count){

// // Get code from preveus assginment 
// };

// int b_seek (int fd, off_t offset, int whence){


// };

// void b_close (int fd){

// // Get code from preveus assginment 
// };

void fsFileOrgEnd() {
  printf("--------------------------fsFileOrgClose------------------------\n");
  free(inodes);
  }
 

int mfs_mkdir(const char *pathname, mode_t mode) {
   // Parse file name 
  // Add info an inode mfs_DIR


  return 0;
}

int mfs_rmdir(const char *pathname) {
  return 0;
}

mfs_DIR * opendir(const char *fileName) {
  printf("OpenDir: %s\n", fileName);
  InodeType type = fileName[strlen(fileName)-1] == '/' ? I_DIR : I_FILE;

  char _fileName[MAX_FILENAME_SIZE] = "";
  strcpy(_fileName, fileName);
  char dirPath[MAX_FILENAME_SIZE] = "/";

  char* savePointer;
  char* token = strtok_r(_fileName, "/", &savePointer);
  do {
    if(*savePointer == "") {
      // Last element
    }
    strcat(dirPath, token);

    token = strtok_r(0, "/", &savePointer);

  } while(token && *token);
  printf("\n");
  return NULL;
}

struct mfs_dirent *readdir(mfs_DIR *dirp) {
  return 0;
}

int closedir(mfs_DIR *dirp) {
  return 0;
}

//8-1-20 Taylor: Initial implementation.
//8-2-20 Taylor: Modified to copy currentDirectoryPath to buf or set errno per description of getcwd in unistd.h
char * mfs_getcwd(char *buf, size_t size) {
  if(strlen(currentDirectoryPath) > size) {
    errno = ERANGE;
    return NULL;
  }
  strcpy(buf, currentDirectoryPath);
  return buf;
}

//8-1-20 Taylor: Initial implementation.
//Note: This does not currently check validity of 
//      the path.
//Note: This may need to first check whether the provided path is within limit of MAX_FILENAME_SIZE
//      and set errno similar to mfs_getcwd.

char * mfs_setcwd(char *buf) {

  /* Keep copy of pathname (buf) as a string. */
  strcpy(currentDirectoryPath, buf);
  
  /* Clear previous cwd and parse new path. */
  currentDirectoryPathArraySize = 0;
  parseFilePath(buf);

  /* Copy parsed pathname to currentDirectoryPathArray. */
  for(int i=0; i<requestedFilePathArraySize; i++) {
    strcpy(currentDirectoryPathArray[i], requestedFilePathArray[i]);
    currentDirectoryPathArraySize++;
  }

  return currentDirectoryPath;

}

//8-1-20 Taylor: Added to test mfs_setcwd
void printCurrentDirectoryPath() {

  for(int i=0; i<currentDirectoryPathArraySize; i++) {

    if(i<currentDirectoryPathArraySize-1) {
      printf("Directory %d: %s\n", i, currentDirectoryPathArray[i]);
    } else {
      printf("Filename: %s\n", currentDirectoryPathArray[i]);
    }

  }
}