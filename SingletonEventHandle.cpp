
#include "stdafx.h"
#include "SingletonEventHandle.h"
#include "TaskPool.h"

//��ʼ����̬����
EventPool*  CSingletonEventHandle<EventPool>::m_pInstance = new EventPool();