#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <unistd.h>
#include <list>

using namespace std;

class Utility
{
	public:
		static string GetPathBIN();
		static void ConvertSecondToTime(int ,int *,int *,int *,int *);
		static list<string> SplitJSON(string);
};
#endif