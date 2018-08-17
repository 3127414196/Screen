#pragma once

#ifndef _H_SINGLETONEVENTHANDLES_H
#define _H_SINGLETONEVENTHANDLES_H

//ֻ����һ��Tobject����
template<typename Tobject>
class CSingletonEventHandle
{
public:
    static Tobject& getInstance();

private:
    
    CSingletonEventHandle()
    {}
    
    //�Ѹ��ƹ��캯����=������Ҳ��Ϊ˽�У���ֹ������
    CSingletonEventHandle(const Tobject&)
    {}
    
    Tobject& operator=(const Tobject&)
    {}

    ~CSingletonEventHandle()
    {
        if (m_pInstance)
        {
            delete m_pInstance;
            m_pInstance = NULL;
        }
    }

    static Tobject* m_pInstance;
};

template<typename Tobject>
inline Tobject & CSingletonEventHandle<Tobject>::getInstance()
{
    return *m_pInstance;
}


#endif // !_H_SINGLETONEVENTHANDLES_H