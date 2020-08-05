

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

  char rootDirectory[64] = "/root";
  char directory[64] = "/root/folder0";
  
  mfs_setcwd(rootDirectory);

  mfs_mkdir(directory, 0);

  mfs_close();

  closeVolume();

  return 0;
}
