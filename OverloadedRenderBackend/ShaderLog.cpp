#include "pch.h"
#include "ShaderLog.hpp"

std::string toString(TraceLevels t)
{
  switch (t)
  {
  case TraceLevels::CONSOLESPAMMMM:
    return "Message";
  case TraceLevels::Verbose:
    return "Message";
  case TraceLevels::Extra:
    return "Message";
  case TraceLevels::High:
    return "Message";
  case TraceLevels::Normal:
    return "Warning";
  case TraceLevels::Low:
    return "Error";
  case TraceLevels::Off:
    return "OFF";
  default:
    throw std::invalid_argument("BWAHAHAH This is not a valid tracelevel");
  }

}
std::ofstream traceLog;
