#include "Utility.h"
#include <iostream>
string Utility::GetPathBIN()
{
	//obtenemos el path del binario porque el archivo config debe estar donde esta el binario
	/*
    int size=200,idx=0,count=0;
    char buffer[size];
    count=readlink("/proc/self/exe",buffer,size);
    string path(buffer,count);
    idx = path.find_last_of("/");
    string path2 = path.substr(0,idx);
    string PATH= path2 +"/";//+fileName;
    return PATH;
    */

   char buff[1024];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
    if (len != -1) {
      buff[len] = '\0';
      string path = (buff);
      int idx = path.find_last_of("/");
      string path2 = path.substr(0,idx);
      string PATH= path2 +"/";//+fileName;
      return PATH;
    } else {
     /* handle error condition */
        return NULL;
    }
}

void Utility::ConvertSecondToTime(int value,int *days, int *hours,int *minutes,int *seconds)
{
   *days = value / (60 * 60 * 24);
   *hours = (value / (60 * 60))-((*days) * 24);
   *minutes = (value / 60) - ((*days)*60*24)-((*hours) * 60);
   *seconds = value - ((*days)*24*60*60)-((*hours)*60*60)-((*minutes)*60);
}

list<string> Utility::SplitJSON(string buff)
{
  list<string> retorno;
  int inicio = 0;
  unsigned int fin = buff.find("}",inicio);
  while(fin != string::npos)
  {
    fin ++;
    string elemento = buff.substr(inicio, fin - inicio);
    retorno.push_back(elemento);


    inicio = fin;
    fin = buff.find("}",inicio);

  }

  return retorno;
}

