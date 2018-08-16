
#include "stdafx.h"
#include "SingletonEventHandle.h"
#include "TaskPool.h"

//初始化静态变量
EventPool*  CSingletonEventHandle<EventPool>::m_pInstance = new EventPool();