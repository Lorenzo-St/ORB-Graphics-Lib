#include <iostream>
#include <fstream>
#include <stdexcept>

int main(int argc, char *argv[])
{
  if (argc < 2)
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
  for (int i = 1; i < argc; ++i)
  {
    std::fstream file(argv[i], std::ios::in);
    if (!file.is_open())
      throw std::runtime_error("Could not open file");
    std::string filename = std::string(argv[i]);
    std::fstream outFile(filename + ".inc", std::ios::out);
    outFile << "char const* " << filename.replace(filename.find('.'), 1, "_" ) << " = \"";
    while (!file.eof())
    {
      char c;
      file.get(c);
      if (file.eof())
        break;
      if (c == '\n')
      {
        outFile << "\\n" << (char)92 << (char)10;
        continue;
      }
      else if (c == '\r')
      {
        continue;
      }
      else
      {
        outFile << c;
      }
    }
    outFile << "\";" << std::endl;
    outFile.close();
    file.close();
  }
}