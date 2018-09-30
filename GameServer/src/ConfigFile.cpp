#include "ConfigFile.h"

ConfigFile::ConfigFile(string fileName)
{
    this->fileName = fileName;

}

ConfigFile::~ConfigFile()
{
    
}

string toLower(string & str){
	for(unsigned int i=0; i<str.length(); ++i){
		str[i] = tolower(str[i]);
	}
	return str;
}

void removeSpaces(string & str)
{
	while(str.size() && (*str.begin() == ' ' || *str.begin() == '\t'))
		str.erase(str.begin());
}

void removeAllSpaces(string & str)
{
	string::size_type off = str.find(" ");
	while(off != string::npos)
	{
		str.erase(off, 1);
		off = str.find(" ");
	}

	off = str.find("\t");
	while(off != string::npos)
	{
		str.erase(off, 1);
		off = str.find("\t");
	}
}


bool isComment(string & str, bool* in_multiline_quote)
{
	string stemp = str;
	removeSpaces(stemp);
	if(stemp.length() == 0)
		return false;

	if(stemp[0] == '/')
	{
		if(stemp.length() < 2)
			return false;

		if(stemp[1] == '*')
		{
			*in_multiline_quote = true;
			return true;
		}
		else if(stemp[1] == '/')
		{
			return true;
		}
	}

	if(stemp[0] == '#')
		return true;

	return false;
}

uint32 ahash(const char* str)
{
	register size_t len = strlen(str);
	register uint32 ret = 0;
	register size_t i = 0;
	for(; i < len; ++i)
		ret += 5 * ret + (tolower(str[i]));
	return ret;
}
uint32 ahash(string & str)
{
	return ahash(str.c_str());
}

void applySetting(string & str, ConfigSetting & setting)
{
	setting.AsString = str;
	setting.AsInt = atoi(str.c_str());
	setting.AsBool = (setting.AsInt > 0);
	setting.AsFloat = (float)atof(str.c_str());

	/* check for verbal yes/no answers */
	if(str.length() > 1)
	{
		// this might be a yes/no?
		if((str.size() >= 3 && !strncmp("yes", toLower(str).c_str(), 3))||(str.size() >= 4 && !strncmp("true", toLower(str).c_str(), 4)))
		{
			setting.AsBool = true;
			setting.AsInt = 1;
		}
		else if((str.size() >= 2 && !strncmp("no", toLower(str).c_str(), 2))||(str.size() >= 5 && !strncmp("false", toLower(str).c_str(), 5)))
		{
			setting.AsBool = false;
			setting.AsInt = 0;
		}
	}
}

bool ConfigFile::Load()
{
    //obtenemos el path del binario porque el archivo config debe estar donde esta el binario
    // int size=200,idx=0,count=0;
    // char buffer[size];
    // count=readlink("/proc/self/exe",buffer,size);
    // string path(buffer,count);
    // idx = path.find_last_of("/");
    // string path2 = path.substr(0,idx);
    // string file= path2 +"/"+fileName;

    string file=ConfigFile::GetPathBIN()+"/"+fileName;


    if(!file.empty())
    {
        #ifdef WIN32
            FILE* f = fopen(file.c_str(), "rb");
        #else
            FILE* f = fopen(file.c_str(), "r");
        #endif
        char* buf;
        int length;
        if(!f)
        {
            //could not open file
            return false;
        }

        /* get the length of the file */
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        buf = new char[length + 1];
        fseek(f, 0, SEEK_SET);

        fread(buf, length, 1, f);
        buf[length] = '\0';
        string buffer = string(buf);
        delete [] buf;

        /* close the file, it is no longer needed */
        fclose(f);

        string line;
        size_t end, offset, comment[3];
        bool multilineComment = false;
        bool multilineQuote = false;
        string currSect = "";
        string currVar  = "";
        string currVal  = "";
        ConfigBlock currBlockMap;
        ConfigSetting currSettingStruct;

        try
        {
            for(;;)
            {
                if(buffer.empty())
                {
                    if(!currBlockMap.empty())
                        mSettings[ahash(currSect)] = currBlockMap;
                    break;
                }

                end = buffer.find(EOL);
                if(end == string::npos)
                {
                    if(buffer.size() == 0)
                        break;
                    line = buffer;
                    buffer.clear();
                }else{
                    line = buffer.substr(0, end);
                    buffer.erase(0, end + EOL_SIZE);
                }

                if(!line.size())
                    continue;

                if(!multilineComment && isComment(line, &multilineComment))
                {
                    if(!multilineComment){
                        continue;
                    }
                }

                if(multilineComment)
                {
                    offset = line.find("*/");

                    if(offset == string::npos)
                        continue;

                    line.erase(0, offset + 2);
                    multilineComment = false;
                    if(line.find("=")==string::npos && line.find("[") == string::npos && line.find("\"") == string::npos)
                        continue;
                }

                if(multilineQuote)
                {
                    end = line.find('\"');
                    if(end == string::npos)
                    {
                        currVal += line;
                        currVal += '\n';
                        continue;
                    }else{
                        currVal += line.substr(0, end);
                        multilineQuote = false;
                        applySetting(currVal, /*&*/currSettingStruct);
                        currBlockMap[ahash(currVar)] = currSettingStruct;
                        currVar = "";
                        currVal = "";
                        continue;
                    }
                }
                if(currSect == "" || line[0]=='[')
                {
                    offset = line.find("[");
                    end = line.find("]");
                    if(offset == string::npos || end == string::npos){
                        //end or begin of section expected
                        return false;
                    }else{
                        if(currSect != "")
                        {
                            if(!currBlockMap.empty())
                                mSettings[ahash(currSect)] = currBlockMap;
                        }
                        currSect = line.substr(offset+1, end-offset-1);
                        currVal = "";
                        currVar = "";
                        multilineComment = false;
                        multilineQuote = false;
                        currBlockMap.clear();
                        continue;
                    }
                }

                if(currVar == "" && currVal == "")
                {
                    offset = line.find('=');
                    if(offset == string::npos)
                    {
                        // variable without quote
                        return false;
                    }else{
                        comment[2] = line.find("*/");
                        if(comment[2]!=string::npos){
                            currVar = line.substr(comment[2]+2, offset-comment[2]-2);
                        }else
                            currVar = line.substr(0, offset);
                        removeAllSpaces(currVar);
                        line.erase(0, offset+1);
                        removeSpaces(line);
                        offset = line.find('\"');
                        if(offset != string::npos){
                            line.erase(0, offset+1);
                            end = line.find('\"');
                            if(end == string::npos)
                            {
                                multilineQuote = true;
                                currVal = line;
                            }
                            else
                                currVal = line.substr(0, end);
                            applySetting(currVal, currSettingStruct);
                            currBlockMap[ahash(currVar)] = currSettingStruct;
                            currVar = "";
                            currVal = "";
                            continue;
                        }
                        else if(offset == string::npos)
                        {
                            currVal = line;
                            comment[0] = currVal.find("#");
                            comment[1] = currVal.find("//");
                            comment[2] = currVal.find("/*");
                            if((int)comment[2] > (int)comment[1] && (int)comment[2] > (int)comment[0])
                            {
                                currVal.erase(comment[2]);
                                multilineComment = true;
                            }else if((int)comment[1] > (int)comment[2] && (int)comment[1] > (int)comment[0])
                                currVal.erase(comment[1]);
                            else if((int)comment[0] > (int)comment[2] && (int)comment[0] > (int)comment[1])
                                currVal.erase(comment[0]);
                            applySetting(currVal, currSettingStruct);
                            currBlockMap[ahash(currVar)] = currSettingStruct;
                            currVar = "";
                            currVal = "";
                            comment[0] = string::npos;
                            comment[1] = string::npos;
                            comment[2] = string::npos;
                            continue;
                        }else
                            continue;

                    }
                }
            }
        }catch(...){
            return false;
        }

        if(multilineComment)
        {
            // Unterminated comment.
            return false;
        }

        if(multilineQuote)
        {
            // Unterminated quote.
            return false;
        }
        return true;
    }
    return false;
}

ConfigSetting* ConfigFile::GetSetting(const char* Block, const char* Setting)
{
	uint32 block_hash = ahash(Block);
	uint32 setting_hash = ahash(Setting);

	/* find it in the big map */
	map<uint32, ConfigBlock>::iterator itr = mSettings.find(block_hash);
	if(itr != mSettings.end())
	{
		ConfigBlock::iterator it2 = itr->second.find(setting_hash);
		if(it2 != itr->second.end())
			return &(it2->second);

		return 0;
	}

	return 0;
}
bool ConfigFile::SetSetting(const string strSection, const string strKey, const string strNewValue)
{
    //PATH
    // int size=200,idx=0,count=0;
    // char buffer[size];
    // count=readlink("/proc/self/exe",buffer,size);
    // string path(buffer,count);
    // idx = path.find_last_of("/");
    // string path2 = path.substr(0,idx);
    // string file = path2 +"/"+fileName;

    string file = ConfigFile::GetPathBIN() +"/"+fileName;

    if(!file.empty())
    {
        #ifdef WIN32
            FILE* f = fopen(file.c_str(), "rb");
        #else
            FILE* f = fopen(file.c_str(), "r");
        #endif
        char* buf;
        int length;
        if(!f)
        {
            //could not open file
            return false;
        }

        /* get the length of the file */
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        buf = new char[length + 1];
        fseek(f, 0, SEEK_SET);

        fread(buf, length, 1, f);
        buf[length] = '\0';
        string buffer = string(buf);
        delete [] buf;

        /* close the file, it is no longer needed */
        fclose(f);

        int iniSection= buffer.find(strSection);
        
        if(iniSection!=-1)
        {
            int iniKey = buffer.find(strKey,iniSection+strSection.length());

            if(iniKey!=-1)
            {
                int endLine=buffer.find(EOL,iniKey);

                if(endLine!=-1)
                {
                    int iniValue = iniKey+strKey.length()+1;
                    string value = buffer.substr(iniValue,endLine-iniValue);
                    buffer.erase(iniValue,value.length());
                    buffer.insert(iniValue,strNewValue);

                    FILE *arch= fopen(file.c_str(),"w");

                    fputs(buffer.c_str(),arch);
                    fclose(arch);
                    return true;
                }
            }
        }
    }
    return false;
}

string ConfigFile::GetPathBIN()
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
