
#include <windows.h>

#pragma once

/**
* @brief 初始化COM并建立event hook
*/
void InitializeMSAA();

/**
* @brief Unhook event并且关闭COM
*/
void ShutdownMSAA();