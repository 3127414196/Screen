
#include <stdio.h>

#pragma once

BOOL LogDebug(char *buffer, int length);
BOOL LogError(char *buffer, int length);
void charTowchar(const char *chr, wchar_t *wchar, int size);
void wcharTochar(const wchar_t *wchar, char *chr, int length);