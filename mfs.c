#include <mfs.h>

int mfs_mkdir(const char *pathname, mode_t mode) {
  return 0;
}

int mfs_rmdir(const char *pathname) {
  return 0;
}

mfs_DIR * opendir(const char *name) {
  return 0;
}

struct mfs_dirent *readdir(mfs_DIR *dirp) {
  return 0;
}

int closedir(mfs_DIR *dirp) {
  return 0;
}

char * mfs_getcwd(char *buf, size_t size) {
  return 0;
}

char * mfs_setcwd(char *buf) {
  return 0;
}   //linux chdir
