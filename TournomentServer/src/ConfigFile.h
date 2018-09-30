#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <string> //all string functions
#include <cstdio>
#include <string.h>
#include <map>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#if defined(_WIN32) && !defined(WIN32)
# define WIN32
#endif

#ifndef EOL
# ifdef WIN32
#  define EOL "\r\n"
# else
#  define EOL "\n"
# endif
#endif

#ifndef EOL_SIZE
# ifdef WIN32
#  define EOL_SIZE 2
# else
#  define EOL_SIZE 1
# endif
#endif

using namespace std;
typedef uint32_t uint32;

struct ConfigSetting{
    string AsString;
    int AsInt;
    double AsFloat;
    bool AsBool;
};
typedef map < uint32, ConfigSetting> ConfigBlock;


class ConfigFile
{
private:
    string fileName;
    map < uint32, ConfigBlock > mSettings;
public:
    ConfigFile(string);
    ~ConfigFile();
    ConfigFile(const ConfigFile &o){}
    bool Load();    
    ConfigSetting* GetSetting(const char* Block, const char* Setting);
    bool SetSetting(const string strSection, const string strKey, const string strNewValue);
    static string GetPathBIN();
    
};

#endif // CONFIG_H
