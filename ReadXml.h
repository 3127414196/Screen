
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"

using namespace rapidxml;

#pragma once

class ReadXml
{
public:
    ReadXml();
    ReadXml(char *filename);
    ~ReadXml();

public:
    xml_document<> *doc;
    BOOL Isignore(char * classname);
};
