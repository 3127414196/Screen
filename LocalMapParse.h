#include <map>
using namespace std;

#pragma once

/**
* ��ų���id�����������Ϣ
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

    //��ȡÿ���ؼ��ֵ�ֵ
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
* ʹ��map�������д洢��Ӧ��id��MapInfo
*/
class CLocalMapParse
{
public:
    CLocalMapParse();

    /**
    * @brief ��file�ж�ȡ���� �����LocalMap,FileName=·��+�ļ���
    */
    CLocalMapParse(CString FileName);

    virtual ~CLocalMapParse();

    /**
    * @brief ��ȡ�������õİ汾��
    */
    int GetConfigVersion()
    {
        return ConfigVersion;
    }

    // �����Ǹ���id����ȡ��Ӧ�Ĺؼ��ֶ�Ӧ��ֵ
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
    //��������CMapInfo�ṹ ֻ��
    const CMapInfo* GetMapInfo(int id);

private:
    map<int, CMapInfo*> LocalMap;
    int ConfigVersion;
};

