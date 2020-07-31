#include "sha-256.h"
#include "fsMakeVol.h"

#define MAX_FILENAME_SIZE 256

void fsFileOrgInit();

mfs_DIR* openDIR(char*);
mfs_DIR* openInode(char*,InodeType);

void fsFileOrgEnd();