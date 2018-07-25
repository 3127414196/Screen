//������־��д��

#include "stdafx.h"

extern "C" {
#include "log.h"
}


const char* logfile = "log.txt";

BOOL LogDebug(char *buffer, int length)
{
    FILE *pfile = NULL;

    if ((pfile = fopen(logfile, "a+")) == NULL)
    {
        return FALSE;
    }

    fprintf(pfile, "[D]  %s\n", buffer);

    fclose(pfile);

    return TRUE;
}

BOOL LogError(char *buffer, int length)
{
    FILE *pfile = NULL;

    if ((pfile = fopen(logfile, "a+")) == NULL)
    {
        return FALSE;
    }

    fprintf(pfile, "[E]  %s\n", buffer);

    fclose(pfile);

    return TRUE;
}

BOOL LogRestore()
{
    return TRUE;
}

void charTowchar(const char *chr, wchar_t *wchar, int size)
{
    MultiByteToWideChar(CP_ACP, 0, chr, strlen(chr) + 1, wchar, size / sizeof(wchar[0]));
}

//function: wcharTochar 
//purpose:WCHAR ��wchar_t��LPWSTR to char 
void wcharTochar(const wchar_t *wchar, char *chr, int length)
{
    WideCharToMultiByte(CP_ACP, 0, wchar, -1, chr, length, NULL, NULL);
}