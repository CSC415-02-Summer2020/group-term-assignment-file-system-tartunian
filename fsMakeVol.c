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

/* Store this as global var since inodeStartBlock is unknown
 * at compile time and is computed based on VCV_START_BLOCK
 *  and the number of blocks required for the VCB.
 */

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
int checkIfStorageIsAvalibale(int numberOfRequestedBlocks){
  int count;
  // loop over freeMap
  // increment count when there is a free block
      // (if freemap[i] == 0)
      // else if freemap[i] != 0
      // count = 0;

  // if count == numberOfRequestedBlocks return 1

return 0;
}

uint64_t getFreeBlock(){

  // loop over freeMap
  // return posion of first free block

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

  /* Initialize properties of the root directory entry. */
  // openVCB_p->rootDir.d_ino = 1;
  // openVCB_p->rootDir.d_off = 1;
  // openVCB_p->rootDir.d_reclen = 16;
  // openVCB_p->rootDir.d_type = '1';
  // sprintf(openVCB_p->rootDir.d_name, "root");

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
  //inodes[0].parent = "root";
  sprintf(inodes[0].name, "root_inode");
  inodes[0].numDirectBlockPointers = 0;
  for(int i = 1; i<totalInodes; i++) {
    inodes[i].inUse = 0;
    inodes[i].type = 0;
    sprintf(inodes[i].parent, "");
    sprintf(inodes[i].name, "unused_inode");
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
  printf("Size of mfs_dirent: %ld bytes\n", sizeof(struct mfs_dirent));
}

void init(uint64_t _volumeSize, uint64_t _blockSize) {
  printf("------------------------------Init------------------------------\n");
  printf("volumeSize: %ld\n", volumeSize = _volumeSize);
  printf("blockSize: %ld\n", blockSize = _blockSize);
  printf("diskSizeBlocks: %ld\n", diskSizeBlocks = ceilDiv(volumeSize, blockSize));
  printf("freeMapSize: %d\n", freeMapSize = diskSizeBlocks <= sizeof(uint32_t) * 8 ? 1 : diskSizeBlocks / sizeof(uint32_t) / 8);
  printf("totalVCBBlocks: %d\n", totalVCBBlocks = ceilDiv(sizeof(mfs_VCB) + sizeof(uint32_t[freeMapSize]), blockSize));
  printf("inodeStartBlock: %d\n", inodeStartBlock = VCB_START_BLOCK + totalVCBBlocks);
  printf("totalInodes: %d\n", totalInodes = ceilDiv(diskSizeBlocks, BLOCKS_PER_INODE));
  printf("totalInodeBlocks: %d\n", totalInodeBlocks = totalInodes * ceilDiv(sizeof(mfs_DIR), blockSize));

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
