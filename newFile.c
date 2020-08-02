// #include <stdio.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <fcntl.h>
// #include "strrep.h"

// #define BUF_SIZE 1024

// char FILE_HEADER[BUF_SIZE] =	"/**************************************************************\n"
// 				"* Class:  CSC-415\n"
// 				"* Team: Team Penta\n"
// 				"* Student ID: N/A\n"
// 				"* Project: Basic File System\n"
// 				"*\n"
// 				"* File: {fileName}\n"
// 				"*\n"
// 				"* Description: \n"
// 				"*\n"
// 				"**************************************************************/\n";

// int main(int argc, char** argv) {
//   if(argc<2) {
//     printf("No filename specified.\n");
//     return 0;
//   }
//   char* fileName = argv[1];

//   /* Check if file exists. */
//   if(access(fileName, F_OK) != -1) {
//     printf("File %s already exists.\n", fileName);
//     return 0;
//   }

//   char* newHeader = strrep(FILE_HEADER, "{fileName}", fileName);

//   /* Create file and open it. */
//   int fd = open(fileName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
//   if(fd == -1) {
//     printf("Error opening file '%s'.\n", fileName);
//     return 0;
//   }

//   write(fd, newHeader, strlen(newHeader));
//   free(newHeader);
//   close(fd);
// }
