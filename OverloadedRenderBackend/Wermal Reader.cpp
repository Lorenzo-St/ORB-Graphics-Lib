#include "pch.h"
#include "Wermal Reader.h"

Data ReadNextAttribute(Stream& source)
{

  std::string tokens;
  size_t lastpos = source.location();
  tokens = source.readLine();
  if (source.isEOF())
    return { false, nullptr };
  if (tokens.find('<') != std::string::npos) 
  {
    tokens = source.readLine();
    // I hate this so much 
    std::shared_ptr<char> data(new char[tokens.size() + 1]);
    std::memcpy(data.get(), tokens.c_str(), tokens.size());
    data.get()[tokens.size()] = '\0';
    return { true, data};
  }
  return { false, nullptr };
}

parseResults ReadNextAttribute(Stream& source, bool)
{
  return { types::Failed, nullptr };
}

parseResults ReadNextAttribute(Stream& source, int hint)
{
  return { types::Failed, nullptr };

}

parseResults TryParse(std::shared_ptr<char> c)
{
  return { types::Failed, nullptr };
}

parseResults TryParse(std::shared_ptr<char> c, int hint)
{
  return { types::Failed, nullptr };
}
