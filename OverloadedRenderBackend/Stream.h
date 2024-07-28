/*********************************************************************
 * @file   Stream.h
 * @brief  File streaming class
 * @author Lorenzo St. Luce(lorenzo.stluce)
 * @date   September 2023
 *
 * @copyright © 2023 DigiPen (USA) Corporation.
 *********************************************************************/
#pragma once

#include <fstream>
#include <glm.hpp>
#include <string>
enum class fileTypes : int
{
  invalid = -1,
  dat,
  xml,
  obj,

};
/**
 * @brief Make a string lowercase
 *
 * @param s the string to lower
 * @return the lowered string
 */
std::string makeLowerCase(std::string s);
fileTypes GetFileType(std::string s);

class Stream
{
public:
    /**
     * @brief Ctors
     *
     * @param fileName the file to open a stream to
     */
    Stream(const char* fileName);
    Stream(std::string const& fileName);

    /**
     * @brief Dtor
     *
     */
    ~Stream();

    /* You cannot create a copy of a filestream and so you cannot copy this class*/
    Stream(Stream const&) = delete;
    Stream operator=(Stream const&) = delete;

    /**
     * @brief Return the location in the file the stream is currently at
     *
     * @return _pos of stream in file
     */
    size_t location();

    /**
     * @brief read an int from the stream
     *
     * @return the int read in
     */
    int readInt(void);

    /**
     * @brief read a char from the stream.
     * 
     * @return the char
     */
    char readChar(void);
    /**
     * @brief read an float from the stream
     *
     * @return the float read in
     */
    float readFloat(void);
    /**
     * @brief read an double from the stream
     *
     * @return the double read in
     */
    double readDouble(void);
    /**
     * @brief read an string from the stream
     *
     * @return the string read in
     */
    std::string readString(void);
    /**
     * @brief read all lines from the stream
     *
     * @return the lines read in
     */
    std::string readAllLines(void);
    /**
     * @brief Read the whole next line.
     * 
     * @return the line read in
     */
    std::string readLine(void);
    /**
     * @brief read an Vector from the stream
     *
     * @return the vector read in
     */
    glm::vec2 readVector(void);
    /**
     * @brief read an color from the stream
     *
     * @return the color read in
     */
    glm::vec4 readColor(void);
    /**
     * @brief read an bool from the stream
     *
     * @return the bool read in
     */
    bool readBool(void);
    /**
     * @brief Checks if a stream is open
     *
     * @return whether the stream is open or not
     */
    bool Open();
    /**
     * @brief Return if the stream is at the end of the file.
     *
     * @return is the stream at the end of the file
     */
    bool isEOF();
    /**
     * @brief Get the file path associated with this Stream.
     * 
     * @return the path
     */
    std::string Path();

private:
    std::ifstream _s;
    std::string _path;
};
