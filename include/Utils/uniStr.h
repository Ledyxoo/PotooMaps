#pragma once

#include "settings.h"

typedef unsigned int char32;

typedef struct UniStr_s
{
    char32 *data;
    int length;
} UniStr;

void UniStr_destroy(UniStr *string);
void UniStr_destroyArray(UniStr **strings, int count);

UniStr *UniStr_decodeU8(char *bufferU8, int bufferSize);
UniStr *UniStr_decodeAscii(char *bufferU8, int bufferSize);

char *UniStr_encodeU8(UniStr *string);
char *UniStr_encodeAscii(UniStr *string);

void UniStr_print(UniStr *string);

UniStr *UniStr_slice(UniStr *string, int start, int end);

int UniStr_count(UniStr *string, UniStr *sub);

int UniStr_find(UniStr *string, UniStr *sub);
int UniStr_rfind(UniStr *string, UniStr *sub);
UniStr **UniStr_split(UniStr *string, UniStr *sep, int *subCount);

UniStr *UniStr_join(UniStr *string1, UniStr *string2);
UniStr *UniStr_copy(UniStr *string);

int UniStr_getInt(UniStr *string, int start, int *end);
double UniStr_getFloat(UniStr *string, int start, int *end);
double UniStr_getDouble(UniStr *string, int start, int *end);
unsigned long long UniStr_getULL(UniStr *string, int start, int *end);

bool UniStr_isEmpty(UniStr *string);

/// @brief Extrait une sous-cha�ne d�limit�e par (), {} ou [].
/// @param string 
/// @param index 
/// @return 
UniStr *UniStr_getBlock(UniStr *string, int index);

void UnitTest_UniStr_decodeAscii(void);
void UnitTest_UniStr_encodeU8(void);
void UnitTest_UniStr_split(void);
void UnitTest_UniStr_getBlock(void);
