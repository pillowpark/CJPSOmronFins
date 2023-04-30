


#include "CommonSupport.h"

VOID DebugPrint(const TCHAR* ptszFormat, ...)
{
    //#ifdef _DEBUG
    TCHAR tszBuffer[MAXBYTE];
    va_list Arg_List;

    va_start(Arg_List, ptszFormat);
    _vstprintf_s(tszBuffer, _countof(tszBuffer), ptszFormat, Arg_List);
    va_end(Arg_List);

    OutputDebugString(tszBuffer);
    //#endif
}