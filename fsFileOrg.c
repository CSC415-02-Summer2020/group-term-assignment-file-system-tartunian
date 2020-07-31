#include "fsFileOrg.h"

mfs_DIR* inodes;

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

mfs_DIR* openInode(char* fileName) {
  printf("---------------------------Opening File-------------------------\n");
  if(strlen(fileName) > MAX_FILENAME_SIZE) {
    printf("openInode: fileName too long.\n");
    return NULL;
  }

  int firstFreeInodeIndex = -1;

  int totalInodes = getVCB()->totalInodes;
  mfs_DIR* inode;
  for(int i=0; i<totalInodes; i++) {
    inode = &inodes[i];
    if(inode->inUse == 0 && firstFreeInodeIndex == -1) {
      firstFreeInodeIndex = i;
    }
    if(!strcmp(inode->name, fileName)) {
      printf("Opening existing file '%s'.\n", inode->name);
      return inode;
    }
  }

  if(firstFreeInodeIndex != -1) {
    inode = &inodes[firstFreeInodeIndex];
    inode->inUse = 1;

    strcpy(inode->name, fileName);
    printf("Created new file '%s'.\n", fileName);
    writeInodes();
    return inode;
  }

  return NULL;
}

void fsFileOrgEnd() {
  printf("--------------------------fsFileOrgClose------------------------\n");
  free(inodes);
}
