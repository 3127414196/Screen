
#include <windows.h>
#pragma comment( lib, "Oleacc.lib" )

#pragma once
/**
* @brief 初始化COM并建立event hook
*/
void InitializeMSAA(HWND hwnd);

/**
* @brief Unhook event并且关闭COM
*/
void ShutdownMSAA();