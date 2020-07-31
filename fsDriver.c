#include "fsFileOrg.h"

int main (int argc, char* argv[]) {

  if(argc<4) {
    printf("Missing arguments. Try fsDriver volumeName volumeSize blockSize\n");
    return 0;
  }

  char volumeName[MAX_FILENAME_SIZE];

  uint64_t volumeSize;
  uint64_t blockSize;

  strcpy(volumeName, argv[1]);
  volumeSize = atoll(argv[2]);
  blockSize = atoll(argv[3]);

  createVolume(volumeName, volumeSize, blockSize);

  openVolume(volumeName);

  char fileName0[64] = "/usr/user001/Desktop/file0.txt";
  char fileName1[64] = "/usr/user002/Documents/folder0/file0.txt";
  char fileName2[64] = "/usr/user002/Documents/folder0/file1.txt";
  uint8_t hashValue[4];

  fsFileOrgInit();
  
  mfs_DIR* inode;

  inode = openInode(fileName0);
  inode = openInode(fileName1);
  inode = openInode(fileName2);
  inode = openInode(fileName0);

  fsFileOrgEnd();

  closeVolume();

  return 0;
}
