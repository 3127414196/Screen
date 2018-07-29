#include <map>
using namespace std;

#pragma once

/**
* 存放除了id以外的配置信息
*/
class CMapInfo
{
public:
    CMapInfo()
        : name(_T(""))
        , guid(_T(""))
        , path(_T(""))
        , build_min(0)
        , build_max(0)
        , appid(0)
        , policy_id(0)
        , switch_id(0)
        , url_id(0)
        , md5_id(0)
    {

    }

    CMapInfo(CString name, CString guid, CString path, int build_min, int build_max, int appid, int policy_id, int switch_id, int url_id, int md5_id)
        : name(name)
        , guid(guid)
        , path(path)
        , build_min(build_min)
        , build_max(build_max)
        , appid(appid)
        , policy_id(policy_id)
        , switch_id(switch_id)
        , url_id(url_id)
        , md5_id(md5_id)
    {

    }

    virtual ~CMapInfo()
    {
        delete(this);
    }

    //获取每个关键字的值
    CString _Getname()
    {
        return name;
    }
    CString _Getguid()
    {
        return guid;
    }
    CString _Getpath()
    {
        return path;
    }
    int _Getbuild_min()
    {
        return build_min;
    }
    int _Getbuild_max()
    {
        return build_max;
    }
    int _Getappid()
    {
        return appid;
    }
    int _Getpolicy_id()
    {
        return policy_id;
    }
    int _Getswitch_id()
    {
        return switch_id;
    }
    int _Geturl_id()
    {
        return url_id;
    }
    int _Getmd5_id()
    {
        return md5_id;
    }

private:
    CString name;
    CString guid;
    CString path;
    int build_min;
    int build_max;
    int appid;
    int policy_id;
    int switch_id;
    int url_id;
    int md5_id;
};


/**
* 使用map容器进行存储对应的id和MapInfo
*/
class CLocalMapParse
{
public:
    CLocalMapParse();

    /**
    * @brief 从file中读取数据 并填充LocalMap,FileName=路径+文件名
    */
    CLocalMapParse(CString FileName);

    virtual ~CLocalMapParse();

    /**
    * @brief 获取整个配置的版本号
    */
    int GetConfigVersion()
    {
        return ConfigVersion;
    }

    // 以下是根据id来获取对应的关键字对应的值
    CString Getname(int id);
    CString Getguid(int id);
    CString Getpath(int id);
    int Getbuild_min(int id);
    int Getbuild_max(int id);
    int Getappid(int id);
    int Getpolicy_id(int id);
    int Getswitch_id(int id);
    int Geturl_id(int id);
    int Getmd5_id(int id);
    //返回整个CMapInfo结构 只读
    const CMapInfo* GetMapInfo(int id);

private:
    map<int, CMapInfo*> LocalMap;
    int ConfigVersion;
};

