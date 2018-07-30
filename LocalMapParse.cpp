#include "stdafx.h"
#include "LocalMapParse.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include <memory>

using namespace rapidxml;

CLocalMapParse::CLocalMapParse()
{

}

CLocalMapParse::~CLocalMapParse()
{
    map<int, CMapInfo*>::iterator iter;

    for (iter = LocalMap.begin(); iter != LocalMap.end(); iter++)
        delete(iter->second);

    delete(this);
}


BOOL ReadXmlFile(CString FileName, std::shared_ptr<char*>& content)
{
    CFile mFile;

    if (!mFile.Open(FileName, CFile::modeRead | CFile::typeBinary))
    {
        //��ȡ�ļ�ʧ��
        return FALSE;
    }

    UINT FileSize = (UINT)mFile.GetLength();

    shared_ptr<char*> buf = make_shared<char*>(new char[FileSize]);
    memset(*buf, 0, FileSize);
    mFile.Read(*buf, FileSize);
    mFile.Close();

    //�Ƚ�UTF-8ת����wchar
    int wsize = MultiByteToWideChar(CP_UTF8, 0, *buf, FileSize, NULL, 0);
    shared_ptr<wchar_t*> pWideChar = make_shared<wchar_t*>(new wchar_t[wsize]);
    MultiByteToWideChar(CP_UTF8, 0, *buf, FileSize, *pWideChar, wsize);

    //�ٽ�wcharת����char
    int asize = WideCharToMultiByte(CP_ACP, 0, *pWideChar, wsize, NULL, 0, NULL, 0);
    content = make_shared<char *>(new char[asize + 1]);
    std::shared_ptr<char *>::element_type pChar = *content;
    WideCharToMultiByte(CP_ACP, 0, *pWideChar, wsize, pChar, asize, NULL, 0);
    
    LogDebug(pChar, strlen(pChar));

    return TRUE;
}

//char->int
int char2int(char *array)
{
    //ʹ����������ܲ�������������� ������������ת����û�������,���ܹ���������123abc
    return atoi(array);
}

CMapInfo * ParseDoc(xml_node<> *root, int *id)
{
    xml_attribute<> *attribute = NULL;
    CMapInfo *newItem = NULL;

    do
    {
        //������ʱ�洢�ı���
        int build_min;
        int build_max;
        int appid;
        int policy_id;
        int switch_id;
        int url_id;
        int md5_id;

        //CfgItem
        attribute = root->first_attribute();
        if (strcmp(attribute->name(), "id") != 0)
            break;
        *id = char2int(attribute->value());
        attribute = attribute->next_attribute();
        if (strcmp(attribute->name(), "name") != 0)
            break;
        CString name(attribute->value());
        attribute = attribute->next_attribute();
        if (strcmp(attribute->name(), "guid") != 0)
            break;
        CString guid(attribute->value());

        //QQPCMgrSupport
        xml_node<> *childroot = root->first_node("QQPCMgrSupport");
        if (childroot == NULL)
            break;
        attribute = childroot->first_attribute();
        if (strcmp(attribute->name(), "build_min") != 0)
            break;
        build_min = char2int(attribute->value());
        attribute = attribute->next_attribute();
        if (strcmp(attribute->name(), "build_max") != 0)
            break;
        build_max = char2int(attribute->value());

        //Download
        childroot = root->first_node("Download");
        if ((childroot == NULL))
            break;
        attribute = childroot->first_attribute();
        if (strcmp(attribute->name(), "appid") != 0)
            break;
        appid = char2int(attribute->value());
        attribute = attribute->next_attribute();
        if (strcmp(attribute->name(), "path") != 0)
            break;
        CString path(attribute->value());

        //Cloud
        childroot = root->first_node("Cloud");
        if ((childroot == NULL))
            break;
        attribute = childroot->first_attribute();
        if (strcmp(attribute->name(), "policy_id") != 0)
            break;
        policy_id = char2int(attribute->value());
        attribute = attribute->next_attribute();
        if (strcmp(attribute->name(), "switch_id") != 0)
            break;
        switch_id = char2int(attribute->value());
        attribute = attribute->next_attribute();
        if (strcmp(attribute->name(), "url_id") != 0)
            break;
        url_id = char2int(attribute->value());
        attribute = attribute->next_attribute();
        if (strcmp(attribute->name(), "md5_id") != 0)
            break;
        md5_id = char2int(attribute->value());

        //����Ҫȥ�ж��Ƿ����ɹ� ���ڵ��ú������ж�
        newItem = new CMapInfo(name, guid, path, build_min, build_max, appid, policy_id, switch_id, url_id, md5_id);
    } while (FALSE);

    return newItem;
}

CLocalMapParse::CLocalMapParse(CString FileName)
{
    std::shared_ptr<char *> content;
    if (!ReadXmlFile(FileName, content))
    {
        //��ȡ�ļ�ʧ��
        abort();
    }
    shared_ptr<char *>::element_type val = *content;

    if (content == NULL)
    {
        LogDebug("�ļ���ʧ��", 0);
        abort();
    }

    // �����ĵ�����map��
    xml_document<> doc;
    try
    {
        doc.parse<0>(val);
    }
    catch (const std::exception&)
    {
        abort();
    }

    //Config
    xml_node<> *root = doc.first_node();
    if (NULL == root)
    {
        LogDebug("����rootΪNULL", 0);
        abort();
    }
    xml_attribute<> *attribute = root->first_attribute();
    ConfigVersion = char2int(attribute->value());

    //CfgItem
    root = root->first_node("CfgItem");
    int id = 0;
    while (root)
    {
        CMapInfo *newItem = ParseDoc(root, &id);
       
        if (newItem == NULL)
            break;

        LocalMap.insert(map<int, CMapInfo*>::value_type(id, newItem));
        root = root->next_sibling();
    }

    CreatPolicyIDMap();

    return;
}

void CLocalMapParse::CreatPolicyIDMap()
{
    map<int, CMapInfo*>::iterator Mapiter;
    map<int, std::vector<int>>::iterator Policyiter;

    int policy_id = 0;
    int id = 0;
    for (Mapiter = LocalMap.begin(); Mapiter != LocalMap.end(); Mapiter++) {
        id = Mapiter->first;
        policy_id = Getpolicy_id(id);

        Policyiter = PolicyIDMap.find(policy_id);
        if (Policyiter != PolicyIDMap.end())
        {
            Policyiter->second.push_back(id);
        }
        else
        {
            vector<int> temp;
            temp.push_back(id);
            PolicyIDMap.insert(map<int, vector<int>>::value_type(policy_id, temp));
        }
    }
}

//��ȡÿ���ؼ��ֵ�ֵ
CString CLocalMapParse::Getname(int id)
{
    map<int, CMapInfo*>::iterator iter = LocalMap.find(id);

    if (iter != LocalMap.end())
    {
        return iter->second->_Getname();
    }
    return _T("");
}

CString CLocalMapParse::Getguid(int id)
{
    map<int, CMapInfo*>::iterator iter = LocalMap.find(id);

    if (iter != LocalMap.end())
    {
        return iter->second->_Getguid();
    }
    return _T("");
}

CString CLocalMapParse::Getpath(int id)
{
    map<int, CMapInfo*>::iterator iter = LocalMap.find(id);

    if (iter != LocalMap.end())
    {
        return iter->second->_Getpath();
    }
    return _T("");
}
int CLocalMapParse::Getbuild_min(int id)
{
    map<int, CMapInfo*>::iterator iter = LocalMap.find(id);

    if (iter != LocalMap.end())
    {
        return iter->second->_Getbuild_min();
    }
    return -1;
}
int CLocalMapParse::Getbuild_max(int id)
{
    map<int, CMapInfo*>::iterator iter = LocalMap.find(id);

    if (iter != LocalMap.end())
    {
        return iter->second->_Getbuild_max();
    }
    return -1;
}
int CLocalMapParse::Getappid(int id)
{
    map<int, CMapInfo*>::iterator iter = LocalMap.find(id);

    if (iter != LocalMap.end())
    {
        return iter->second->_Getappid();
    }
    return -1;
}
int CLocalMapParse::Getpolicy_id(int id)
{
    map<int, CMapInfo*>::iterator iter = LocalMap.find(id);

    if (iter != LocalMap.end())
    {
        return iter->second->_Getpolicy_id();
    }
    return -1;
}
int CLocalMapParse::Getswitch_id(int id)
{
    map<int, CMapInfo*>::iterator iter = LocalMap.find(id);

    if (iter != LocalMap.end())
    {
        return iter->second->_Getswitch_id();
    }
    return -1;
}
int CLocalMapParse::Geturl_id(int id)
{
    map<int, CMapInfo*>::iterator iter = LocalMap.find(id);

    if (iter != LocalMap.end())
    {
        return iter->second->_Geturl_id();
    }
    return -1;
}
int CLocalMapParse::Getmd5_id(int id)
{
    map<int, CMapInfo*>::iterator iter = LocalMap.find(id);

    if (iter != LocalMap.end())
    {
        return iter->second->_Getmd5_id();
    }
    return -1;
}

BOOL CLocalMapParse::GetPolicyIDMapInfo(int policy_id, std::vector<int> &id)
{
    map<int, std::vector<int>>::iterator iter = PolicyIDMap.find(policy_id);

    if (iter != PolicyIDMap.end())
    {
        int size = iter->second.size();

        for (int i = 0; i < size; i++)
            id.push_back((iter->second)[i]);
        return TRUE;
    }

    return FALSE;
}