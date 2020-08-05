/**************************************************************
* Class:  CSC-415-0# Summer 2020
* Name: Taylor Artunian
* Student ID: 920351715
* Project: Assignment 2 – Buffered I/O
*
* File: b_io.h
*
* Description: Header for library which buffers reads and writes
* to secondary storage in order to reduce overall I/O operations.
*
**************************************************************/

#ifndef _B_IO_H
#define _B_IO_H
#include <fcntl.h>

int b_open(char *filename, int flags);
int b_read(int fd, char *buffer, int count);
int b_write(int fd, char *buffer, int count);
int b_seek(int fd, off_t offset, int whence);
void b_close(int fd);

#endif
