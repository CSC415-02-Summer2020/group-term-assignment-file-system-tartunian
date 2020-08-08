/**************************************************************
* Class:  CSC-415-02 Summer 2020
* Name: Team Penta - Duy Nguyen, Taylor Artunian, Wameedh Mohammed Ali
* Student ID: 917446249, 920351715, 
* Project: Basic File System - PentaFS
*
* File: bitMap.c
*
* Description: This file defines the methods that change our free-space bit vector.
*
**************************************************************/

#include "bitMap.h"

void setBit(int A[], int k)
{
    A[k / 32] |= 1 << (k % 32);
}

void clearBit(int A[], int k)
{
    A[k / 32] &= ~(1 << (k % 32));
}

int findBit(int A[], int k)
{
    return ((A[k / 32] & (1 << (k % 32))) != 0);
}


