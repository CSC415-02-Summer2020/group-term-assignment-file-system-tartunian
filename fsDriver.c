

#include "mfs.h"

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

  mfs_init();

  char directory[64] = "/usr/user001/home/Documents/";
  char fileName0[64] = "../Desktop/file0.txt";
  char fileName1[64] = "/usr/user002/Documents/folder0/file0.txt";
  char fileName2[64] = "/usr/user002/Documents/folder0/file1.txt";
  char fileName3[64] = "/usr/user002/Documents/folder0/";
  
  mfs_setcwd(directory);
  printCurrentDirectoryPath();

  char cwd[MAX_FILENAME_SIZE];
  printf("cwd: %s\n", mfs_getcwd(cwd, MAX_FILENAME_SIZE));

  parseFilePath(fileName0);
  printFilePath();
  
  mfs_DIR* inode;

  inode = getInode(fileName0);
  inode = getFreeInode();

  mfs_close();

  closeVolume();

  return 0;
}
