
#include <windows.h>
#pragma comment( lib, "Oleacc.lib" )

#pragma once
/**
* @brief ��ʼ��COM������event hook
*/
void InitializeMSAA(HWND hwnd);

/**
* @brief Unhook event���ҹر�COM
*/
void ShutdownMSAA();