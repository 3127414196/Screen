#pragma once

#ifndef _H_TASKPOOL_H
#define _H_TASKPOOL_H

template<typename Tobject>
class CTaskPool
{
public:
    CTaskPool()
    {
    }

    ~CTaskPool()
    {
        if (m_pObj)
        {
            delete[] m_pObj;
            m_pObj = NULL;
        }
    }

    CTaskPool(DWORD size)
        : m_maxsize(size)
        ,m_nowsize(0)
        , m_front(0)
        , m_rear(0)
    {
        m_pObj = new Tobject[size];
    }

    void AddTask(Tobject &obj)
    {
            //如果队列已满，m_rear==m_front直接进行覆盖
        if (m_nowsize >= m_maxsize)
        {
            m_front++;
            m_pObj[(m_rear + 1) % m_maxsize] = obj;
            m_rear++;
        }
        else
        {
            m_pObj[(m_rear + 1) % m_maxsize] = obj;
            m_rear++;
            m_nowsize++;
        }
    }

    BOOL GetTask(Tobject* outObject)
    {
        BOOL bOK = FALSE;
        do
        {
            if (m_nowsize <= 0)
                break;

            *outObject = m_pObj[(m_front + 1) % m_maxsize];
            m_front++;
            m_nowsize--;
            bOK = TRUE;
        } while (FALSE);

        return bOK;
    }

    BOOL IsRepeat(Tobject &obj)
    {
#define REPEATSIZE 100

        //在测试过程中发现，一些黑屏的情况处理的特别快，通常队列中元素都不多，导致不能够满足过滤的效果
        //int begin = (nowSize < REPEATSIZE) ? m_front : (m_rear - REPEATSIZE + m_maxsize) % m_maxsize;
        int begin = (m_rear < REPEATSIZE) ? m_front : (m_rear - REPEATSIZE + m_maxsize) % m_maxsize;
        while (begin != m_rear)
        {
            if (obj.IsEqual(m_pObj[begin]))
                return TRUE;
            begin = (begin + 1) % m_maxsize;
        }
        return FALSE;
    }

    DWORD m_maxsize;
    DWORD m_nowsize;
    DWORD m_front;
    DWORD m_rear;
private:
    Tobject* m_pObj;
};



class EVENTINFO
{
public:
    EVENTINFO()
    {
    }

    EVENTINFO(DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
        : m_event(event)
        , m_hwnd(hwnd)
        , m_idObject(idObject)
        , m_idChild(idChild)
        , m_dwEventThread(dwEventThread)
        , m_dwmsEventTime(dwmsEventTime)
    {
    }
    ~EVENTINFO()
    {
    }

    EVENTINFO& operator=(const EVENTINFO& right)
    {
        m_event = right.m_event;
        m_hwnd = right.m_hwnd;
        m_idObject = right.m_idObject;
        m_idChild = right.m_idChild;
        m_dwEventThread = right.m_dwEventThread;
        m_dwmsEventTime = right.m_dwmsEventTime;
        return *this;
    }

    BOOL IsEqual(const EVENTINFO& oInfo) const
    {
        if (m_event == oInfo.m_event &&
            m_hwnd == oInfo.m_hwnd &&
            m_idObject == oInfo.m_idObject)
        {
            return TRUE;
        }

        return FALSE;
    }

    BOOL IsInValid()
    {
        //表示没有窗口与事件相连
        if (m_hwnd == NULL)
            return TRUE;

        //判断是否是有效窗口
        if (!IsWindow(m_hwnd))
            return TRUE;

        //判断窗口是否可见
        if (!IsWindowVisible(m_hwnd))
            return TRUE;

        CRect rc;
        ::GetWindowRect(m_hwnd, &rc);
        //如果整个窗口都为负数,则表示窗口无效
        if (rc.left <= 0 && rc.right <= 0 && rc.top <= 0 && rc.bottom <= 0)
            return TRUE;
        //如果窗口的矩形区域长或者宽小于等于0,表示窗口无效
        if (rc.Width() <= 0 || rc.Height() <= 0)
            return TRUE;

        return FALSE;
    }

    DWORD m_event;
    HWND m_hwnd;
    LONG m_idObject;
    LONG m_idChild;
    DWORD m_dwEventThread;
    DWORD m_dwmsEventTime;
};


//单例全局变量
class EventPool
{
#define TASKPOOLSIZE 10000

public:
    EventPool()
    {
        m_taskpool = new CTaskPool<EVENTINFO>(TASKPOOLSIZE);
        if (m_taskpool != NULL)
        {
            m_Continue = TRUE;
        }
        else
        {
            m_Continue = FALSE;
        }
    }

    ~EventPool()
    {
        if (m_Continue)
        {
            delete m_taskpool;
            m_taskpool = NULL;
        }
    }

    BOOL m_Continue;
    CTaskPool<EVENTINFO>* m_taskpool;
    ATL::CComAutoCriticalSection m_cs;
};

#endif // !_H_TASKPOOL_H