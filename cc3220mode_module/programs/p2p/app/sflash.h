/*
 * sflash.h
 *
 *  Created on: Sep 4, 2018
 *      Author: Admin
 */

#ifndef SFLASH_H_
#define SFLASH_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "main_thread.h"
#include <ti/drivers/net/wifi/fs.h>


int InitFile_Test();
int CheckFile_System();
int st_writeFile( _i32 fileHandle, _u32 length, _const char *buffer);
int st_readFile( _i32 fileHandle, _u32 length);

int InitFile(unsigned long *ulToken, _i32 lFileHandle);
_i32 st_ShowStorageInfo();
_i32 st_listFiles(_i32 numOfFiles, int bPrintDescription);
int InitFile1();
long WriteFileToDevice(unsigned long *ulToken, long lFileHandle, unsigned char* data, int len);
long ReadFileFromDevice(unsigned long ulToken, long lFileHandle, unsigned char *data, int len);

#endif /* SFLASH_H_ */
