/*********************************************************************
 * @file   ConsoleMacros.hpp
 * @brief  Macros for use with the console cause I'm lazy
 * with formatting and some formatting isnt c++ standardized yet
 *
 * @author Lorenzo St. Luce(lorenzo.stluce)
 * @date   November 2023
 *********************************************************************/
#include <iostream>
#include <fstream>
#include <stacktrace>
#define RESET "\033[0m"
#define BLACK "\033[30m"          /* Black */
#define RED "\033[31m"            /* Red */
#define GREEN "\033[32m"          /* Green */
#define YELLOW "\033[33m"         /* Yellow */
#define BLUE "\033[34m"           /* Blue */
#define MAGENTA "\033[35m"        /* Magenta */
#define CYAN "\033[36m"           /* Cyan */
#define BRIGHT_BLACK "\033[90m"   /* Bright Black */
#define BRIGHT_RED "\033[91m"     /* Bright Red */
#define BRIGHT_GREEN "\033[92m"   /* Bright Green */
#define BRIGHT_YELLOW "\033[93m"  /* Bright Yellow */
#define BRIGHT_BLUE "\033[94m"    /* Bright Blue */
#define BRIGHT_MAGENTA "\033[95m" /* Bright Magenta */
#define BRIGHT_CYAN "\033[96m"    /* Bright Cyan */
#define WHITE "\033[97m"          /* White */

#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

enum class TraceLevels : int
{
  CONSOLESPAMMMM = -1, // Lowest severity, spammy messages
  Verbose,             // Lowest severity messages
  Extra,               // Low Severity, messages
  High,                // Meh severity, Warnings, messages etc
  Normal,              // Normal Severity, errors and warnings
  Low,                 // Highest Severity ERRORS
  Off = 255
};

/**
 * @brief Convert from trace level to string
 */
std::string toString(TraceLevels t);

constexpr TraceLevels Error = TraceLevels::Low;
constexpr TraceLevels Warning = TraceLevels::Normal;
constexpr TraceLevels Message = TraceLevels::Extra;

/**
 * @brief Log to the console and trace.
 * 
 * @param l trace level to log at
 * @param arg1 the first argument
 * @param variadic the variadic arguments
 */
template <typename Arg, typename... vArgs>
static void Log(TraceLevels l, Arg&& arg1, vArgs&&... variadic)
{
    std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm ltime;
    localtime_s(&ltime, &t);
    std::stringstream tm;
    tm << std::put_time(&ltime, "%H:%M:%S");
    std::cout << tm.str() << "\t" << (l == Error ? RED : (l == Warning ? YELLOW : RESET))
              << toString(l) << "\t" <<  RESET << std::forward<Arg>(arg1);
    ((std::cout << " " << std::forward<vArgs>(variadic)), ...);
    std::cout << std::endl;

}