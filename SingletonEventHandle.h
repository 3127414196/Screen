#pragma once

#ifndef _H_SINGLETONEVENTHANDLES_H
#define _H_SINGLETONEVENTHANDLES_H

//只会有一个Tobject变量
template<typename Tobject>
class CSingletonEventHandle
{
public:
    static Tobject& getInstance();

private:
    
    CSingletonEventHandle()
    {}
    
    //把复制构造函数和=操作符也设为私有，防止被复制
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