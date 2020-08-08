/**************************************************************
* Class:  CSC-415-02 Summer 2020
* Name: Team Penta - Duy Nguyen, Taylor Artunian, Wameedh Mohammed Ali
* Student ID: 917446249, 920351715, 
* Project: Basic File System - PentaFS
*
* File: b_io.h
*
* Description: The header file of the buffered I/O module that contains the prototypes of the methods.
*
**************************************************************/

#ifndef _B_IO_H
#define _B_IO_H
#include <fcntl.h>

int b_open(char *filename, int flags);
int b_read(int fd, char *buffer, int count);
int b_write(int fd, char *buffer, int count);
void b_close(int fd);

#endif
