/*********************************************************************
 * @file   Stream.cpp
 * @brief  The implementation of the Stream class
 * @author Lorenzo St. Luce(lorenzo.stluce)
 * @date   September 2023
 *
 * @copyright © 2023 DigiPen (USA) Corporation.
 *********************************************************************/
#include "pch.h"

#include "Stream.h"
#include <algorithm>
std::string makeLowerCase(std::string s)
{
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
  return s;
}
fileTypes GetFileType(std::string s)
{
  if (s == ".wrx")
    return fileTypes::dat;
  if (s == ".dat")
    return fileTypes::dat;
  if (s == ".xml")
    return fileTypes::xml;
  if (s == ".obj")
    return fileTypes::obj;
  return fileTypes::invalid;
}


Stream::Stream(const char* fileName)
{
    _s.open(fileName);
    _path = fileName;
}

Stream::Stream(std::string const& fileName) : Stream(fileName.c_str())
{
}

Stream::~Stream()
{
    if (_s.is_open())
        _s.close();
}

size_t Stream::location()
{
    return _s.tellg();
}

int Stream::readInt(void)
{
    int i;
    _s >> i;
    return i;
}

char Stream::readChar(void)
{
  char c;
  return _s >> c, c;
}

float Stream::readFloat(void)
{
    float i;
    _s >> i;
    return i;
}

double Stream::readDouble(void)
{
    double i;
    _s >> i;
    return i;
}

std::string Stream::readString(void)
{
    std::string i;
    _s >> i;
    return i;
}

std::string Stream::readAllLines(void)
{
    std::string h((std::istreambuf_iterator<char>(_s)), (std::istreambuf_iterator<char>()));
    return h;
}

std::string Stream::readLine(void)
{
  std::string line;
  return std::getline(_s, line), line == "" ? std::getline(_s, line) : _s , line;
}



glm::vec2 Stream::readVector(void)
{
    glm::vec2 i;
    _s >> i.x;
    _s >> i.y;
    return i;
}

glm::vec4 Stream::readColor(void)
{
    glm::vec4 i;
    _s >> i.r;
    _s >> i.g;
    _s >> i.b;
    _s >> i.a;
    return i;
}

bool Stream::readBool(void)
{
    std::string st;
    _s >> st;
    if (st == "TRUE" || st == "1" || st == "true" || st == "True")
        return true;
    return false;
}

bool Stream::Open()
{
    return _s.is_open();
}

bool Stream::isEOF()
{
    return _s.eof();
}

std::string Stream::Path()
{
  return _path;
}
