
#include "stdafx.h"
#include "ReadXml.h"


ReadXml::ReadXml()
{
    doc = NULL;
}

ReadXml::ReadXml(char *filename)
{
    FILE *pfile = NULL;
    int fileSize = 0;
    char *buf = NULL;
    
    if ((pfile = fopen(filename, "r")) == NULL)
    {
        return;
    }

    //获取文件的大小
    fseek(pfile, 0L, SEEK_END);
    fileSize = ftell(pfile);

    
    buf = (char *)malloc(sizeof(char) * (fileSize + 1));
    if (buf == NULL)
    {
        fclose(pfile);
        return;
    }

    doc = new xml_document<>();
    if (doc == NULL)
    {
        delete(buf);
        fclose(pfile);
        return;
    }

    //读文件
    fseek(pfile, (-1)*fileSize, SEEK_CUR);
    fread(buf, sizeof(char), fileSize, pfile);
    buf[fileSize] = '\0';

    try
    {
        doc->parse<0>(buf);
    }
    catch (const parse_error&)
    {
        
        return ;
    }
}

ReadXml::~ReadXml()
{
    delete(doc);
    doc = NULL;
}

BOOL ReadXml::Isignore(char * classname)
{
    xml_node<> *root = doc->first_node();
    if (NULL == root)
    {
        LogDebug("xml文件为空", 0);
        return -1;
    }

    root = root->first_node("ScreenshotRule");
    if (NULL == root)
    {
        LogDebug("xml中没有Screenshot Rule", 0);
        return -1;
    }

    xml_node<> *rulegroup = root->first_node("Rule");

    while (rulegroup)
    {
        xml_attribute<> *attribute = rulegroup->first_attribute();

        if ((strcmp(attribute->name(), "type") == 0) && (strcmp(attribute->value(), classname) == 0))
        {
            attribute = attribute->next_attribute();
            if ((strcmp(attribute->name(), "action") == 0) && (strcmp(attribute->value(), "ignore") == 0))
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
        rulegroup = rulegroup->next_sibling("Rule");
    }

    return FALSE;
}