#include "mfs.h"

int main (int argc, char* argv[]) {

  if(argc<4) {
    printf("Missing arguments. Try fsFormat volumeName volumeSize blockSize\n");
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

  closeVolume();

  return 0;
}
