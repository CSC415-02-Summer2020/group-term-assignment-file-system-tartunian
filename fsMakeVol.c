/**************************************************************
* Class:  CSC-415
* Team: Team Penta
* Student ID: N/A
* Project: Basic File System
*
* File: fsMakeVol.c
*
* Description: This program is used to create a volume for the
*              Penta File System.
*
**************************************************************/

#include "fsMakeVol.h"

int initialized = 0;

char header[16] = "*****PentaFS****";
uint64_t volumeSize;
uint64_t blockSize;
uint64_t diskSizeBlocks;
uint32_t vcbStartBlock;
uint32_t totalVCBBlocks;
uint32_t inodeStartBlock;
uint32_t totalInodes;
uint32_t totalInodeBlocks;
uint32_t freeMapSize;

mfs_VCB* openVCB_p;

uint64_t ceilDiv(uint64_t a, uint64_t b) {
  /* Rounds up integer division. */
  return (a + b - 1) / b;
}

int allocateVCB(mfs_VCB** vcb_p) {
  *vcb_p = calloc(totalVCBBlocks, blockSize);
  return totalVCBBlocks;
}

uint64_t fsRead(void* buf, uint64_t blockCount, uint64_t blockPosition) {
  if(!initialized) {
    printf("fsRead: System not initialized.\n");
    return 0;
  }
  if(blockPosition + blockCount > openVCB_p->diskSizeBlocks) {
    printf("fsRead: Invalid block range.\n");
    return 0;
  }
  LBAread(buf, blockCount, blockPosition);
  return blockCount;
}

uint64_t fsWrite(void* buf, uint64_t blockCount, uint64_t blockPosition) {
  if(!initialized) {
    printf("fsWrite: System not initialized.\n");
    return 0;
  }
  if(blockPosition + blockCount > openVCB_p->diskSizeBlocks) {
    printf("fsWrite: Invalid block range.\n");
    return 0;
  }
  LBAwrite(buf, blockCount, blockPosition);
  for(int i=0; i<blockCount; i++) {
    setBit(openVCB_p->freeMap, blockPosition + i);
  }
  writeVCB();
  return blockCount;
}

void fsFree(void* buf, uint64_t blockCount, uint64_t blockPosition) {
  if(!initialized) {
    printf("fsFree: System not initialized.\n");
    return;
  }
  if(blockPosition + blockCount > openVCB_p->diskSizeBlocks) {
    printf("fsFree: Invalid block range.\n");
    return;
  }
  for(int i=0; i<blockCount; i++) {
    clearBit(openVCB_p->freeMap, blockPosition + i);
  }
  writeVCB();
}


/************************************/
//  DONE AARON!! MAYBE!
/************************************/

/* Checks if there is enough contiguous blocks for a requested number of blocks
return 0 for free 1 for full */
// loop over freeMap
  // increment count when there is a free block
      // (if freemap[i] == 0)
      // else if freemap[i] != 0
      // count = 0;



// merge checkIfStorageIsAvailable() and getFreeBlock() toghather then return the first free block in that contiguous span of blocks
// return -1 if dose not exists!


  // if count == numberOfRequestedBlocks return 1
// int checkIfStorageIsAvailable(int numberOfRequestedBlocks){ //roughly scratched up, feel free to change or edit
//   int count = 0;
//   for( int i = 0; i < freeMapSize; i++ ) {
	
// 	if( count == numberOfRequestedBlocks ) { //returns 1 when count does = the requested amount
// 	  return 1;
// 	}
	
// 	if( openVCB_p->freeMap[i] == 0 ) { //increment count for each sequential 0
// 	  count++;
// 	}
	
// 	if( count > 0 && openVCB_p->freeMap[i] == 1 ) { //if bit = 1, reset count, only resets when count > 0
// 	  count = 0; //could reset count only when openVCB_p->freeMap[i] == 1
// 	}
	
//   }
//   return 0;
// }
//Loop over freeMap. Return position of first free block
uint64_t getFreeBlock(){
  
    /* 
  PROBLEM: only if their requesting one block. In the event need more than one, their should
  be an argument to accept number of blocks like the example in line 154->162
  */
  for (int index = 0; index < freeMapSize; index++)
  {
    if (openVCB_p->freeMap[index] == 0)
    {
      return index; //The position in the VolumeSpaceArray
    }
    
  }
  /*
  for (int index = 0; index < freeMapSize; index++)
  {
    if (openVCB_p->freeMap[index] == 0 && openVCB_p->freeMap[index + 1])
    {
      return index;
    }
  }
*/
return -1;
}

/************************************/
//  AARON JOBS ENDS!
/************************************/


uint64_t readVCB() {
  if(!initialized) {
    printf("readVCB: System not initialized.\n");
    return 0;
  }

  /* Read VCB from disk to openVCB_p */
  uint64_t blocksRead = LBAread(openVCB_p, totalVCBBlocks, VCB_START_BLOCK);
  printf("Read VCB in %d blocks starting at block %d.\n", totalVCBBlocks, VCB_START_BLOCK);
  return blocksRead;
}

uint64_t writeVCB() {
  if(!initialized) {
    printf("writeVCB: System not initialized.\n");
    return 0;
  }

  /* Write openVCB_p to disk. */
  uint64_t blocksWritten = LBAwrite(openVCB_p, totalVCBBlocks, VCB_START_BLOCK);
  printf("Wrote VCB in %d blocks starting at block %d.\n", totalVCBBlocks, VCB_START_BLOCK);
  return blocksWritten;
}

mfs_VCB* getVCB() {
  return openVCB_p;
}

void initializeVCB() {
  if(!initialized) {
    printf("initializeVCB: System not initialized.\n");
    return;
  }
  printf("------------------------Initializing VCB------------------------\n");
 

  sprintf(openVCB_p->header, "%s", header); 

  /* Set information on volume sizes and block locations. */
  openVCB_p->volumeSize = volumeSize;
  openVCB_p->blockSize = blockSize;
  openVCB_p->diskSizeBlocks = diskSizeBlocks;
  openVCB_p->vcbStartBlock = VCB_START_BLOCK;
  openVCB_p->totalVCBBlocks = totalVCBBlocks;
  openVCB_p->inodeStartBlock = inodeStartBlock;
  openVCB_p->totalInodes = totalInodes;
  openVCB_p->totalInodeBlocks = totalInodeBlocks;
  printf("initVCB: totalInodeBlocks %ld", openVCB_p->totalInodeBlocks);

  openVCB_p->freeMapSize = freeMapSize;

  /* Initialize freeBlockMap to all 0's. */
  for(int i=0; i<freeMapSize; i++) {
    openVCB_p->freeMap[i] = 0;//4294967295;
  }

  printVCB();
  writeVCB();
}

void initializeInodes() {
  if(!initialized) {
    printf("initializeInodes: System not initialized.\n");
    return;
  }

  printf("-----------------------Initializing inodes----------------------\n");

  printf("Total disk blocks: %ld, total inodes: %d, total inode blocks: %d\n", diskSizeBlocks, totalInodes, totalInodeBlocks);

  /* Allocate and initialize inodes. First inode is root directory and has id=1. */
  mfs_DIR* inodes = calloc(totalInodeBlocks, blockSize);
  inodes[0].inUse = 1;
  inodes[0].type = 1;
  strcpy(inodes[0].name, "root");
  strcpy(inodes[0].path, "/root");
  printf("Root inode name: %s\n", inodes[0].name);
  inodes[0].numDirectBlockPointers = 0;
  for(int i = 1; i<totalInodes; i++) {
    inodes[i].inUse = 0;
    inodes[i].type = 0;
    strcpy(inodes[i].parent, "");
    strcpy(inodes[i].name, INVALID_INODE_NAME);
    
    /* Set all direct block pointers to -1 (invalid). */
    for(int j=0; j<MAX_DATABLOCK_POINTERS; j++) {
      inodes[i].directBlockPointers[j] = INVALID_DATABLOCK_POINTER;
    }
    inodes[i].numDirectBlockPointers = 0;
    
  }

  /* Write inodes to disk. */
  char* char_p = (char*) inodes;
  LBAwrite(char_p, totalInodeBlocks, inodeStartBlock);
  printf("Wrote %d inodes of size %ld bytes each starting at block %d.\n", totalInodes, sizeof(mfs_DIR), inodeStartBlock);
  free(inodes);
}

void printVCB() {
  int size = openVCB_p->totalVCBBlocks*(openVCB_p->blockSize);
  int width = 16;
  char* char_p = (char*)openVCB_p;
  char ascii[width+1];
  sprintf(ascii, "%s", "................");
  printf("--------------------------Printing VCB--------------------------\n");
  for(int i = 0; i<size; i++) {
    printf("%02x ", char_p[i] & 0xff);
    if(char_p[i]) {
      ascii[i%width] = char_p[i];
    }
    if((i+1)%width==0&&i>0) {
      ascii[i%width+1] = '\0';
      printf("%s\n", ascii);
      sprintf(ascii, "%s", "................");
    } else if (i==size-1) {
      for(int j=0; j<width-(i%(width-1)); j++) {
        printf("   ");
      }
      ascii[i%width+1] = '\0';
      printf("%s\n", ascii);
      sprintf(ascii, "%s", "................");
    }
  }
  printf("VCB Size: %d bytes\n", size);
}

//8-3-20 Taylor: Modified totalInodes calculated to account for the size of the mfs_DIR + DATA_BLOCKS_PER_INODE
void init(uint64_t _volumeSize, uint64_t _blockSize) {
  printf("------------------------------Init------------------------------\n");
  printf("volumeSize: %ld\n", volumeSize = _volumeSize);
  printf("blockSize: %ld\n", blockSize = _blockSize);
  printf("diskSizeBlocks: %ld\n", diskSizeBlocks = ceilDiv(volumeSize, blockSize));
  printf("freeMapSize: %d\n", freeMapSize = diskSizeBlocks <= sizeof(uint32_t) * 8 ? 1 : diskSizeBlocks / sizeof(uint32_t) / 8);
  printf("totalVCBBlocks: %d\n", totalVCBBlocks = ceilDiv(sizeof(mfs_VCB) + sizeof(uint32_t[freeMapSize]), blockSize));
  printf("inodeStartBlock: %d\n", inodeStartBlock = VCB_START_BLOCK + totalVCBBlocks);
  printf("totalInodes: %d\n", totalInodes = (diskSizeBlocks - inodeStartBlock) / (DATA_BLOCKS_PER_INODE + ceilDiv(sizeof(mfs_DIR), blockSize)));
  printf("totalInodeBlocks: %d\n", totalInodeBlocks = ceilDiv(totalInodes * sizeof(mfs_DIR), blockSize));
  printf("inodeSizeBytes: %ld\n", sizeof(mfs_DIR));
  printf("inodeSizeBlocks: %ld\n", ceilDiv(sizeof(mfs_DIR), blockSize));

  /* Allocate the VCB in memory. */
  int vcbSize = allocateVCB(&openVCB_p);
  printf("VCB allocated in %d blocks.\n", vcbSize);

  initialized = 1;
  printf("----------------------------End Init----------------------------\n");
}

int createVolume(char* volumeName, uint64_t _volumeSize, uint64_t _blockSize) {
  printf("------------------------Creating Volume-------------------------\n");
  /* Check whether volume exists already. */
  if(access(volumeName, F_OK) != -1) {
    printf("Cannot create volume '%s'. Volume already exists.\n", volumeName);
    return -3;
  }

  uint64_t existingVolumeSize = _volumeSize;
  uint64_t existingBlockSize = _blockSize;

  /* Initialize the volume with the fsLow library. */
  int retVal = startPartitionSystem (volumeName, &existingVolumeSize, &existingBlockSize);

  printf("Opened %s, Volume Size: %llu;  BlockSize: %llu; Return %d\n", volumeName, (ull_t)existingVolumeSize, (ull_t)existingBlockSize, retVal);

  /* Format the disk if the volume was opened properly. */
  if(!retVal) {
    init(_volumeSize, _blockSize);
    initializeVCB();
    initializeInodes();
  }

  closeVolume();
  return retVal;
}

void openVolume(char* volumeName) {
  printf("--------------------------Opening Volume------------------------\n");
  if(!initialized) {
    uint64_t existingVolumeSize;
    uint64_t existingBlockSize;

    int retVal =  startPartitionSystem(volumeName, &existingVolumeSize, &existingBlockSize);
    if(!retVal) {
      init(existingVolumeSize, existingBlockSize);
      readVCB();

      printVCB();
    }
  } else {
    printf("Can't open volume '%s'. Another volume is already open.\n", volumeName);
  }
}

void closeVolume() {
  printf("--------------------------Closing Volume------------------------\n");
  if(initialized) {
    closePartitionSystem();
    free(openVCB_p);
    initialized = 0;
  } else {
    printf("Can't close volume. Volume not open.\n");
  }
}
