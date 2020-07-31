#include "sha-256.h"
#include "fsMakeVol.h"

#define MAX_FILENAME_SIZE 256

void fsFileOrgInit();

mfs_DIR* openInode(char*);

void fsFileOrgEnd();