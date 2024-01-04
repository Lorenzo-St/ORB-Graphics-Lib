#pragma once
#include <vector>
#include <sstream>
#include "Stream.h"

enum class types : int
{
  Failed,
  Int,
  Float
};

/**
 * @brief Information about a single data value.
 * 
 * bool - state either valid read or invalid
 * char* - pointer to read in string
 */
typedef std::pair<bool, std::shared_ptr<char>> Data;
typedef std::pair<types, void*> parseResults;


/**
 * @brief Read the next attribute in the data stream

 * @param source - File stream source
 * @return pair stating whether the data was read, and a pointer to the data
 */
Data ReadNextAttribute(Stream& source);
parseResults ReadNextAttribute(Stream& source, bool);
parseResults ReadNextAttribute(Stream& source, int hint);

/**
 * @brief Forfully parse the data stream as a type t.
 * 
 * @param c - the data to be parsed
 * @return a shared pointer to the read in data.
 */
template<typename t>
std::shared_ptr<t> Parse(std::shared_ptr<char> c)
{
  std::stringstream s(c.get());
  std::vector<t> results;
  while (!s.eof()) 
  {
    t temp;
    s >> temp;
    results.push_back(temp);

  }
  std::shared_ptr<t> res(new t[results.size()]);
  memcpy(res.get(), results.data(), results.size() * sizeof(t));
  return res;
}

/**
 * @brief Attempt to parse and guess the type of data in the stream.
 * 
 * @param c - the data stream
 * @param hint - A hint to how many values are in the stream.
 */
parseResults TryParse(std::shared_ptr<char> c);
parseResults TryParse(std::shared_ptr<char> c, int hint);



