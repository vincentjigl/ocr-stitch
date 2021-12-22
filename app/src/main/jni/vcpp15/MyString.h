// 在此类中操作文件IO

// 单独使用时需要以下头文件
 #include <time.h>
 #include <string>
#include <map>
 #include <vector>
 #include <fstream>
 #include <iostream>
 #include <sstream>
 using namespace std;

// 字符串转换
// 字符集-使用 Unicode 字符集 ok
// CString cStr=lpszPathName;
// string  sstr=(CW2A)cStr;		// ok
// char *lppathname=(CW2A)lpszPathName;			// 字符集-使用 Unicode 字符集 ok
// LPCTSTR lpszPathName;
// matsrc = cv::imread((LPCSTR)lpszPathName);	// 字符集-未设置 ok

// 此宏定义是为了 能添加变量名称 有回车
#define STRVAR_APPEND(s1,x1) s1.append(#x1);s1.append(" = ");s1.appendln(x1)//printf("%s = %d\n", #x1, x1)
#define	MAX_STRING_LEN 8192
#define MAX_LINE 1024

class MyString
{
public:
	string	str;

	MyString(void){
		str="";
	};

	MyString(const MyString &mm){
		str = mm.str;
	}

	// 本类不可直接用char*赋值, char*用于文件名， char*可直接对string赋值
	// gcc不支持
	//MyString(const char* iFileName)
	//{
	//	string s;
	//	ifstream ifs;
	//	ifs.open(iFileName);
	//	while( getline(ifs, s) )
	//		str.append(s + "\n");
	//	ifs.close();
	//}

	MyString(const char* iFileName)
	{
		char buf[MAX_LINE];
		FILE *fp;
		if((fp = fopen(iFileName,"r"))==NULL)
		{
			printf("Failed to read file.\n");
			exit(1);
		}
		while(fgets(buf,MAX_LINE,fp)!=NULL)
		{
			//len = strlen(buf);
			//buf[len-1] = '\0';	// 去掉换行符
			str.append(buf);
		}
		fclose(fp);
	}

	~MyString(void){
	};

	void readFile(const char* iFileName)
	{
		string s;
		ifstream ifs;
		ifs.open(iFileName);
		while( getline(ifs, s) )
			str.append(s + "\n");
	}

	const char* ptr()
	{
		return str.c_str();
	}
	
#ifdef _AFXDLL		// 定义这个符号表示动态连接MFC
	// char转TCHAR
	// 返回TCHAR*字符串指针,使用后要delete
	static wchar_t *Convert_LPSTR_LPWSTR(const char *sBuf)
	{
		int sBufSize=strlen(sBuf);//获取输入缓存大小

		//获取输出缓存大小	//VC++ 默认使用ANSI，故取第一个参数为CP_ACP
		DWORD dBufSize=MultiByteToWideChar(CP_ACP, 0, sBuf, sBufSize, NULL, 0);
		//printf("需要wchar_t%u个\n", dBufSize);

		// 多申请一个用于结束符mbh
		wchar_t * dBuf=new wchar_t[dBufSize+1];
		wmemset(dBuf, 0, dBufSize+1);

		//进行转换
		int nRet=MultiByteToWideChar(CP_ACP, 0, sBuf, sBufSize, dBuf, dBufSize);

		if(nRet<=0)
		{
			cout<<"转换失败"<<endl;
			DWORD dwErr=GetLastError();
			switch(dwErr)
			{
			case ERROR_INSUFFICIENT_BUFFER:
				printf("ERROR_INSUFFICIENT_BUFFER\n");
				break;
			case ERROR_INVALID_FLAGS:
				printf("ERROR_INVALID_FLAGS\n");
				break;
			case ERROR_INVALID_PARAMETER:
				printf("ERROR_INVALID_PARAMETER\n");
				break;
			case ERROR_NO_UNICODE_TRANSLATION:
				printf("ERROR_NO_UNICODE_TRANSLATION\n");
				break;
			}
		}
		// 	else
		// 	{
		// 		cout<<"转换成功"<<endl;
		// 		cout<<dBuf; 
		// 	}

		return dBuf;
	}
	// 如果不是Unicode字符集，就不需要转换，直接复制即可
	// char* Convert_LPSTR_LPWSTR(char* pszIn, LPWSTR lpwszStrOut)
	// {
	// #ifdef UNICODE
	// 	MultiByteToWideChar(CP_ACP, 0, pszIn, -1, lpwszStrOut, 100);
	// #else
	// 	strcpy(Name, strUsr);
	// #endif
	// 	return NULL;
	// }

	wchar_t *Convert_LPSTR_LPWSTR()
	{
		return Convert_LPSTR_LPWSTR(str.c_str());
		// delete(strText);使用后要delete
	}

	// TCHAR转char
	// 返回char*字符串指针,使用后要delete
	static char* Convert_LPWSTR_LPSTR(LPWSTR lpwszStrIn)
	{
		LPSTR pszOut = NULL;
		if (lpwszStrIn != NULL)
		{
			int nInputStrLen = wcslen(lpwszStrIn);

			// Double NULL Termination
			int nOutputStrLen = WideCharToMultiByte (CP_ACP, 0, lpwszStrIn, nInputStrLen, NULL, 0, 0, 0) + 2;
			pszOut = new char[nOutputStrLen];

			if (pszOut)
			{
				memset (pszOut, 0x00, nOutputStrLen);
				WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, pszOut, nOutputStrLen, 0, 0);
			}
		}
		return pszOut;
	}
#endif

	void appendln(unsigned short* pus, int _nLen)	// ushort数组 + 回车
	{
		for (int i=0; i < _nLen; i++)
			str.append(" " + cvt(pus[i]));
		str.append("\n");
	}
	
	void append(const char* astr)	// 字符串
	{
		str.append(astr);
	}
	void appendln(const char* astr)	// 字符串 + 回车
	{
		str.append(astr);
		str.append("\n");
	}
	void append(string astr)
	{
		str.append(astr);
	}
	void appendln(string astr)
	{
		str.append(astr + "\n");
	}
	void append(MyString mstr)
	{
		str.append(mstr.str);
	}

	void append(int i1)
	{
		str.append(cvt(i1));
	}
	void append(int i1, int i2)
	{
		str.append(cvt(i1) + " " + cvt(i2));
	}
	void append(int i1, int i2, int i3)
	{
		str.append(cvt(i1) + " " + cvt(i2) + " " + cvt(i3));
	}
	void append(int i1, int i2, int i3, int i4)
	{
		str.append(cvt(i1) + " " + cvt(i2) + " " + cvt(i3) + " " + cvt(i4));
	}

// 	void appendln(int i1)
// 	{
// 		str.append(cvt(i1) + "\n");
// 	}
// 	void appendln(int i1, int i2)
// 	{
// 		str.append(cvt(i1) + " " + cvt(i2) + "\n");
// 	}
// 	void appendln(int i1, int i2, int i3)
// 	{
// 		str.append(cvt(i1) + " " + cvt(i2) + " " + cvt(i3) + "\n");
// 	}
// 	void appendln(int i1, int i2, int i3, int i4)
// 	{
// 		str.append(cvt(i1) + " " + cvt(i2) + " " + cvt(i3) + " " + cvt(i4) + "\n");
// 	}

	void append(double d1, int nprecision=2)
	{
		str.append(cvt(d1, nprecision));
	}
	void append(double d1, double d2)
	{
		str.append(cvt(d1) + " " + cvt(d2));
	}
	void append(double d1, double d2, double d3)
	{
		str.append(cvt(d1) + " " + cvt(d2) + " " + cvt(d3));
	}
	void append(double d1, double d2, double d3, double d4)
	{
		str.append(cvt(d1) + " " + cvt(d2) + " " + cvt(d3) + " " + cvt(d4));
	}

	void appendln(double d1)
	{
		str.append(cvt(d1) + "\r\n");
	}
	void appendln(double d1, double d2)
	{
		str.append(cvt(d1) + " " + cvt(d2) + "\n");
	}
	void appendln(double d1, double d2, double d3)
	{
		str.append(cvt(d1) + " " + cvt(d2) + " " + cvt(d3) + "\n");
	}
	void appendln(double d1, double d2, double d3, double d4)
	{
		str.append(cvt(d1) + " " + cvt(d2) + " " + cvt(d3) + " " + cvt(d4) + "\n");
	}

	void appendln(string astr, int i1)
	{
		str.append(astr);
		str.append(cvt(i1) + "\r\n");
	}
	void appendln(string astr, double d1)
	{
		str.append(astr);
		str.append(cvt(d1) + "\r\n");
	}

	void dellastchar()
	{
		str = str.substr(0, str.length()-1);
	}

	// 与另一个字符串比较
	bool compare(const MyString &mm)
	{
		bool ret=true;
		vector<string> vs0, vs1;

		SplitString(str, vs0, "\n");
		SplitString(mm.str, vs1, "\n");
		int len0=vs0.size();
		int len1=vs1.size();

		for (int i=0; i < len0 && i < len1; i++)
		{
			if(vs0[i].compare(vs1[i]))
			{
				cout << "line " << i << ": " << endl;
				cout << vs0[i] << endl;
				cout << vs1[i] << endl;
				ret=false;
			}
		}

		// 当长度不同时，输出不同的位置
		if(ret)
		{
			if(len0 < len1)
			{
				cout << "Output is too short!" << endl;
				cout << "\t" << vs1[len0] << endl;
			}
			if(len1 < len0)
			{
				cout << "Output is too long!" << endl;
				cout << "\t" << vs0[len1] << endl;
			}
		}

// 		int lineNum=0;
// 		const char* p = str.c_str();
// 		const char* q = mm.str.c_str();
// 
// 		printf("%d: ", lineNum++);
// 		for (int i=0; i < str.length(); i++)
// 		{
// 			Sleep(50);
// 			if(p[i]=='\n')
// 				printf("%c", '\r');
// 			else
// 				printf("%c", p[i]);	// 竟然也能输出汉字, 汉字用2个字符显示,遇到汉字时循环2次才显示。
// 
// 			if(p[i]=='\n')
// 			{
// 				printf("                                                      --\r");
// 				printf("%d: ", lineNum++);
// 			}
// 		}

// 		if(str.compare(mm.str))
// 		{
// 			printf("Compare Failed!\n");
// 			return false;
// 		}

		if(ret)
			printf("Compare Success!\n\n");
		else
			printf("Compare Failed!\n\n");
		return ret;
	}

	void print()
	{
		cout << str;
		cout << endl;// << "-- end ok --" << endl;
	}

	// 十六进制输出
	static void print(unsigned char *msg, int msg_length)
	{
		for (int i = 0; i < msg_length; i++)
			printf("[%.2X]", msg[i]);
		printf("\n");
	}

	/////////////////// 静态函数 ///////////////////////
	static string	itoa(int i){ stringstream s;s << i;return s.str(); }
	static string	ftoa(double i){ std::ostringstream s;s << i;return s.str(); }

	// 整数转字符串
	static string cvt(int i)
	{
		ostringstream s1;
		s1 << i;
		return s1.str();
	}
	static string cvt(double i, int nprecision=2)
	{
		ostringstream s1;
		s1.precision(nprecision);
		s1.setf(std::ios::fixed);
		s1 << i;
		return s1.str();
	}

	// 字符串加前缀 字符串连接 在字符串头部连接另一个字符串
	// 返回连接后的字符串 要保证目的字符串长度足够
	static char* myStrcatHead(char* strDest, const char* strHead)
	{
		if (strDest == NULL || strHead == NULL) return strDest;
		int dn = strlen(strDest);
		int hn = strlen(strHead);
		char* p = strDest + dn;
		char* q = strDest + hn + dn;
		while(p >= strDest) *q--=*p--;
		p++;
		while(*strHead!='\0') *p++=*strHead++;
		return strDest;
	}

	// 将utf8字符串切分为单个字符
	static int SplitCharUtf8(const std::string &input, vector<string> &output)
	{
		std::string ch; 
		for (size_t i = 0, len = 0; i != input.length(); i += len) {
			unsigned char byte = (unsigned)input[i];
			if (byte >= 0xFC) // lenght 6
				len = 6;
			else if (byte >= 0xF8)
				len = 5;
			else if (byte >= 0xF0)
				len = 4;
			else if (byte >= 0xE0)
				len = 3;
			else if (byte >= 0xC0)
				len = 2;
			else
				len = 1;
			ch = input.substr(i, len);
			output.push_back(ch);
		}   
		return output.size();
	}

	// 为utf8字符串每个单字符加空格
	std::string insertSpaceUtf8()
	{
		vector<string> output;

		SplitCharUtf8(str, output);

		// 	for (int i=0; i < output.size(); i++)
		// 	{
		// 		string ss = CChineseCode::UTF8ToGB2312(output[i].c_str());
		// 		cout << ss;
		// 	}
		//	cout << endl;

		string input = output[0];
		for (int i=1; i < output.size(); i++)
			input += " " + output[i];

		return input;
	}

	// 切分字符串中的数字 int
	void SplitStringInt(vector<int>& v, const string& c)
	{
		vector<string> vstr;
		SplitString(vstr, c);
		for (int i=0; i < vstr.size(); i++)
			v.push_back(atoi(vstr[i].c_str()));
	}
	static void SplitStringInt(const string& s, vector<int>& v, const string& c)
	{
		vector<string> vstr;
		SplitString(s, vstr, c);
		for (int i=0; i < vstr.size(); i++)
			v.push_back(atoi(vstr[i].c_str()));
	}

	// 切分字符串中的数字 double
	void SplitStringDouble(vector<double>& v, const string& c)
	{
		vector<string> vstr;
		SplitString(vstr, c);
		for (int i=0; i < vstr.size(); i++)
			v.push_back(atof(vstr[i].c_str()));
	}

	// 切分字符串
	void SplitString(vector<string>& v, const string& c)
	{
		string::size_type pos1, pos2;
		pos2 = str.find(c);
		pos1 = 0;
		while(string::npos != pos2)
		{
			v.push_back(str.substr(pos1, pos2-pos1));

			pos1 = pos2 + c.size();
			pos2 = str.find(c, pos1);
		}
		if(pos1 != str.length())
			v.push_back(str.substr(pos1));
	}

	// 切分字符串
	// s	输入字符串, v 切分后的字符串数组, c 切分标记字符串
	static void SplitString(const string& s, vector<string>& v, const string& c)
	{
		string::size_type pos1, pos2;
		pos2 = s.find(c);
		pos1 = 0;
		v.clear();
		while(string::npos != pos2)
		{
			v.push_back(s.substr(pos1, pos2-pos1));

			pos1 = pos2 + c.size();
			pos2 = s.find(c, pos1);
		}
		if(pos1 != s.length())
			v.push_back(s.substr(pos1));
	}

	// 以空格切分字符串
	// s 输入字符串, v 切分后的字符串数组
	static void SplitSpace(const string& s, vector<string>& v)
	{
		string result;
		stringstream input(s);
		//依次输出到result中，并存入res中 
		while(input>>result)
			v.push_back(result);
	}

	void split_ini(map<string, string> &mss)
	{
		const char* delim="\r\n";
		char  buff[2048];
		vector<string> v;

		strcpy(buff, str.c_str());
		char* token = strtok(buff, delim);

		while(token != NULL)
		{
			//printf("%s\n", token);
			v.push_back(token);
			token = strtok(NULL, delim);
		}

		for (int i=0; i < v.size(); i++)
		{
			int p = v[i].find(";");	// 去掉注释
			string ss = v[i].substr(0, p);
			p = ss.find("=");
			if(p <= 0) continue;
			string s1 = ss.substr(0, p);
			string s2 = ss.substr(p+1);
			mss.insert(make_pair(s1, s2));
			//printf("%s : %s\n", s1.c_str(), s2.c_str());
		}

		//map<string, string>::iterator it;
		//for (it=mss.begin(); it!=mss.end(); it++)
		//	printf("[%s, %s]\n", it->first.c_str(), it->second.c_str());
	}

	// 获取c1和c2之间的子串
	string getSubString(const string c1, const string c2)
	{
		string::size_type p = str.find(c1)+c1.length();
		string ss = str.substr(p);
		p = ss.find(c2);
		return ss.substr(0, p);
	}

	// 获取c1和c2之间的两个数字
	void getInt2(const string c1, const string sp, const string c2, int& a, int& b)
	{
		vector<int> vi;
		string s = getSubString(c1, c2);
		SplitStringInt(s, vi, sp);
		a = vi[0];
		b = vi[1];
	}

	static bool isDigit(char c)
	{
		if(47 < c && c < 58) return true;
		return false;
	}

	// 获取字符串中的表示数字的字符		注意：返回字符串数组长度要足够
	void getNumOfString(char * pStr, int startPos=0)
	{
		char k=0;
		//memset(pStr,0,nLen);

		for(int i=startPos; i < str.size() && str[i]!='\n'; i++){
			if(isDigit(str[i]))	// isalpha isdigit
			{
				pStr[k++]=str[i];
				if(k > 100) break;
			}
		}

		pStr[k] = '\0';
	}

	// 获取时间字符串
	static void getTimeStr(char *buf)
	{
		time_t t;  //秒时间  
		tm* local; //本地时间   tm* gmt;   //格林威治时间  

		t = time(NULL); //获取目前秒时间  
		local = localtime(&t); //转为本地时间  
		strftime(buf, 20, "%d-%m-%Y %H:%M:%S", local);  
	}

#ifdef _AFXDLL		// 定义这个符号表示动态连接MFC
	// 获取剪贴板字符串
	void getClipboard()
	{
		if (::OpenClipboard(NULL))//打开剪贴板 或 ::OpenClipboard(AfxGetMainWnd()->GetSafeHwnd()); 在MFC窗口程序中只需OpenClipboard()
		{
			if (IsClipboardFormatAvailable(CF_TEXT))//判断格式是否是我们所需要
			{
				HANDLE hClip=GetClipboardData(CF_TEXT);
				str = (char*)GlobalLock(hClip);
				GlobalUnlock(hClip);
				CloseClipboard();
			}
		}
	}
#endif

	// 添加时间字符串
	void appendTimeStr()
	{
		time_t t;  //秒时间  
		tm* local; //本地时间   tm* gmt;   //格林威治时间  
		char buf[128]= {0};  

		t = time(NULL); //获取目前秒时间  
		local = localtime(&t); //转为本地时间  
		strftime(buf, 64, "%Y-%m-%d %H:%M:%S", local);  
		str += buf;
	}
	
	//http://www.cnblogs.com/catgatp/p/6407783.html string替换所有指定字符串（C++）
	//cout   <<   replace_all(string("12212"),"12","21")   <<   endl;     
	//cout   <<   replace_all_distinct(string("12212"),"12","21")   <<   endl;     
	// 对替换后又有匹配的再替换 循环替换,替换指针每次回退
	// 如果旧的字符串新字符串的前缀,则会死循环
	inline int   replace_all(string&   str,const   string&   old_value,const   string&   new_value)     
	{
		int n=0;
		while(true)   {     
			string::size_type   pos(0);     
			if(   (pos=str.find(old_value))!=string::npos   )     
			{
				str.replace(pos,old_value.length(),new_value);
				n++;
			}
			else   break;     
		}     
		return   n;     
	}

	// 替换所有指定字符串
	// 对替换后又有匹配的不再替换 只替换一次,替换指针不回退 (指因替换而产生的新匹配,前缀的因素)
	inline int   replace_all_distinct(string&   str,const   string&   old_value,const string& new_value, int nCount=0)
	{
		int n=0;
		for(string::size_type   pos(0);   pos!=string::npos;   pos+=new_value.length())   {     
			if((pos=str.find(old_value,pos))!=string::npos)     
			{
				str.replace(pos,old_value.length(),new_value);
				n++;
				if(nCount > 0 && n >= nCount) break;
			}
			else break;
		}
		return   n;     
	}

	// 替换所有指定字符串
	// bDistinct=true  对替换后又有匹配的不再替换 只替换一次
	// bDistinct=false 对替换后又有匹配的再替换 循环替换
	int replace_all(const string &oldstr, const string &newstr, int nCount=0, bool bDistinct=true)
	{
		if(oldstr.empty()) return 0;
		if(bDistinct)
			return replace_all_distinct(str, oldstr, newstr, nCount);
		return replace_all(str, oldstr, newstr);
	}

	/*截取字符串 主要是处理不要把汉字切乱
	 name :要截取的字符串
	 store:要存储的字符串
	 len:要截取的长度 */
	inline int subString(const char * name , char * store , int len )
	 {
		 int i= 0 ;
		 //char strTemp[64]={0};
		 if ( strlen(name) < len)
		 {
			 strcpy( store, name );  //*name=0;
			 return strlen(name);
		 }

		 //从第１个字节开始判断
		 while( i < len )
		 {
			 if ( name[i]>>7&1 && name[i+1]>>7&1 )       //if ( name[i] < 0 && name[i+1] < 0 )
				 i = i + 2 ;
			 else
				 i = i + 1 ;
		 }
		 i = i > len ? i-3 :i-1;
		 strncpy( store , name , i+1 ); //截取前i+1位
		 *(store+i+1)=0;

		 //strcpy( strTemp , name + i + 1 );
		 //strcpy( name , strTemp );
		 return i+1;
	 } 

	// 每50个字符插入回车 要判断汉字 大约为60个字符切分
	inline string insertln(string &str)
	{
		char strTemp[54]={0};
		int nLen=str.length(),i=0;
		string str2="";

		for (int s=0; s < nLen; )
		{
			int n = subString(str.c_str()+s, strTemp, 50);
			str2.append(cvt(i++) + " " + cvt(s)+": ");
			str2.append(strTemp);
			str2.append("\n");
			s += n;
		}
		return str2;
	}

	// 取得文件名, 去掉路径
	void getFileName(bool bHasExtName=false)
	{
		int p = str.rfind('\\');
		str = str.substr(p+1, str.length()-1);
		if(!bHasExtName)
		{
			p = str.rfind('.');
			str = str.substr(0, p);
		}
	}

	// 取得扩展名 后缀名,	从'.'后替换扩展名
	void getExtName()
	{
		int p = str.rfind('.');
		str = str.substr(p+1);
	}

	// 判断是否扩展名 后缀名,	从'.'后扩展名比较
	bool issuffix(const char* extname)
	{
		int p = str.rfind('.');
		string suff = str.substr(p+1, str.length()-1);
		return !suff.compare(extname);
	}

	// 设置扩展名 后缀名,	从'.'后替换扩展名
	void modifyExtName(const char* extname)
	{
		int p = str.rfind('.');
		str = str.substr(0, p+1);
		str.append(extname);
	}

	// 文件名添加后缀,	从'.'前添加
	void addFileNameSuffix(const char* suffixstr)
	{
		int p = str.rfind('.');
		string str2 = str.substr(p, str.length());
		str = str.substr(0, p);
		str.append(suffixstr+str2);
	}

	// 保留若干行数据
	// 删除n行数据
	void deletePreLines(int nReservedLines=8)
	{
		int p=str.length();
		for (int i=0; i < nReservedLines; i++)
		{
			p = str.rfind('\n', p-1);
			if(p < 0) break;
		}

		if(p >= 0 && p < str.length()-1)
			str = str.substr(p+1, str.length());
	}

	// 将文件拷贝到当前目录 扩展名"_bak"
	static void copyFile(string fileName)
	{
		MyString newfilename;
		newfilename.str = fileName;
		newfilename.getFileName();
		newfilename.addFileNameSuffix("_bak");
	 	string strcmd = "copy " + fileName + " " + newfilename.str;
		system(strcmd.c_str());
	}

	// 控制台utf-8字符显示的设置
	// 需要添加 #include <Windows.h>
	static void setprintutf8()
	{
		system("chcp 65001"); //设置字符集 （使用SetConsoleCP(65001)设置无效，原因未知）
		printf("设置字体需要添加 #include <Windows.h>");
// 		CONSOLE_FONT_INFOEX info = { 0 }; // 以下设置字体
// 		info.cbSize = sizeof(info);
// 		info.dwFontSize.Y = 18; // leave X as zero
// 		info.FontWeight = FW_NORMAL;
// 		wcscpy(info.FaceName, L"Consolas");
// 		SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &info);
	}

	// 例子程序： 修改替换文件中的字符串
	static void replaceInFile(string srcfile)
	{
		string tmpfile = "tmp.vcxproj";
		string cmdstr = "copy " + srcfile + " " + tmpfile;

		// 复制到临时文件
		system(cmdstr.c_str());

		ifstream ifs;
		ifs.open(tmpfile.c_str());
		ofstream ofs;
		ofs.open(srcfile.c_str());	// ,ios::app 追加 默认是清空

		// 从临时文件向源文件复制, 在复制时查找并修改
		string s;
		while( getline(ifs, s) )
		{
			int npos = s.find("opencv_features2d249.lib");
			if(npos > 0)
			{
				cout << s << "\n";
				cout << "替换为：\n";

				s.insert(npos+20,"d");
				npos = s.find("opencv_imgproc249.lib");
				s.insert(npos+17,"d");
				npos = s.find("opencv_highgui249.lib");
				s.insert(npos+17,"d");
				npos = s.find("opencv_legacy249.lib");
				s.insert(npos+16,"d");
				npos = s.find("opencv_flann249.lib");
				s.insert(npos+15,"d");
				npos = s.find("opencv_core249.lib");
				s.insert(npos+14,"d");
				cout << s << "\n";

				ofs << s << "\n";
				break;		// 相同字符串,只查找一次
			}
			ofs << s << "\n";
		}

		// 已找到 直接复制剩余的部分
		while( getline(ifs, s) )
			ofs << s << "\n";

		ifs.close();
		ofs.close();

		// 删除临时文件
		cmdstr = "del " + tmpfile;
		system(cmdstr.c_str());
	}

//        static void writefile(const char * pstr, const char *filename, bool bAddData=true)
//        {
//                FILE *fp;

//                if(bAddData)
//                        fp=fopen(filename, "a+");
//                else
//                        fp=fopen(filename, "w+");

//                fprintf(fp,"%s\r\n",pstr);
//                fclose(fp);
//        }

	// 将本对象字符串保存到文件 每次加回车 savefile
	// bAddData=false 清空原文件数据 bAddData=true 添加到原文件
	void writefile(const char *filename, bool bTime=false, bool bAddData=false)
	{
		if(!filename) return;

		FILE *fp;

		if(bAddData)
			fp=fopen(filename, "a+");
		else
			fp=fopen(filename, "w+");

		fprintf(fp,"%s",str.c_str());
		if(bTime)	// 加入时间戳
		{
			char bufftime[64];
			getTimeStr(bufftime);
			fprintf(fp,"%s\r\n",bufftime);
			//printf("%s\n",bufftime);
		}
		fclose(fp);
	}

	static void printusage()
	{
		printf("使用说明:\n");
		printf("1 功能: 在txt文件中替换字符串\n");
		printf("2 请先安装vcredist2010_x86.exe, 若提示已安装或已安装有较新的版本,则不必安装\n");
		printf("3 参数格式如下, 参数不分先后顺序, 中括号内为可选参数:\n");
		printf("      lpReplaceString [-h] \n");
		printf("      	  [-i 输入文件名(*.txt)]\n");
		printf("          [-o 输出文件名(*.txt)]\n");
		printf("          [-s0 字符串0] [-s1 字符串1] [-d 调试号]\n");
		printf("4 参数选项: \n");
		printf("  -h 帮助 \n");
		//printf("  -v 视频模式 \n");
		//printf("  -w 显示时间 \n");
		//printf("  -r 缩放倍数(浮点),缺省为1 \n");
		printf("  -i 输入文件名 \n");
		printf("  -o 输出文件名, 有输出文件名才保存 \n");
		printf("  -rc x y w h 输入矩形位置大小 \n");
		//printf("  -[-s0 str0] [-s1 str1] 其他参数  \n");
		//printf("  -d 调试模式,&0x01-显示,&0x02-写入日志文件,&0x03-日志写入剪贴板,&0x04-源图写入剪贴板 \n");
		//printf("5 对码图要求: \n");	// 用E模板找铁路线计算边缘要求:
		//printf("	(1) 铁路线黑码点不可少于9个. \n");
		//printf("	(2) 留白必须大于1.5倍码点宽度. \n");
		//printf("	(3) 2.5像素 < 码点宽度 < 6像素.  \n");
		//printf("	(4) 码点越小,相对误差越大,此时可将图像放大检测. \n");
		//printf("6 运行示例程序, 双击run.bat\n");
		printf("                                             20181225 mbh\n");
		system("pause");
		exit(0);
	}

	static void parsecmdline(int argc, char **argv, bool &bVideo, int &nTotal, int &waittime,
		double &fScale, double &fScale1, double &fScale2,
		double &fDegree, double &fBright, int &addBright,
		int &px, int &py, int &x, int &y, int &w, int &h,
		string &file1, string &file2,
		string &str0, string &str1, int &nDebug, const char* strHelp)
	{
		if(argc < 3 || !strcmp(argv[1],"-h") || !strcmp(argv[1],"--h") || !strcmp(argv[1],"-help") || !strcmp(argv[1],"--help"))
		{
			printf("%s\n", strHelp);
			system("pause");
			exit(0);
		}

		for (int i=1; i < argc; i++)
		{
			if(!strcmp(argv[i], "-v"))			// 视频模式
				bVideo=true;
			else if(!strcmp(argv[i], "-n"))		// 计数参数
				nTotal = atoi(argv[++i]);
			else if(!strcmp(argv[i], "-w"))		// 显示图像等待时间
				waittime = atoi(argv[++i]);
			else if(!strcmp(argv[i], "-r"))		// 图片缩放倍数
				fScale = atof(argv[++i]);
			else if(!strcmp(argv[i], "-r"))		// 图片缩放倍数1
				fScale1 = atof(argv[++i]);
			else if(!strcmp(argv[i], "-r2"))	// 图片缩放倍数2
				fScale2 = atof(argv[++i]);
			else if(!strcmp(argv[i], "-a"))		// 图片旋转角度
				fDegree = atof(argv[++i]);
			else if(!strcmp(argv[i], "-b"))		// 图片亮度提高百分比 增加量
			{
				fBright = atof(argv[++i]);	// 系数
				addBright = atoi(argv[++i]);// 常量
			}
			else if(!strcmp(argv[i], "-i"))		// 输入图片文件名
				file1 = argv[++i];
			else if(!strcmp(argv[i], "-o"))		// 输出图片文件名
				file2 = argv[++i];
			else if(!strcmp(argv[i], "-pt"))	// 输入点的位置
			{
				px = atoi(argv[++i]); py = atoi(argv[++i]);
			}
			else if(!strcmp(argv[i], "-rc"))	// 输入矩形位置大小
			{
				x = atoi(argv[++i]); y = atoi(argv[++i]); w = atoi(argv[++i]); h = atoi(argv[++i]);
			}
			else if(!strcmp(argv[i], "-ss0"))	// 其他参数0 字符串
				str0 = argv[++i];
			else if(!strcmp(argv[i], "-ss1"))	// 其他参数1 字符串
				str1 = argv[++i];
			else if(!strcmp(argv[i], "-d"))		// 调试号码
				nDebug = atoi(argv[++i]);
			else
				printusage();
		}
	}

#ifdef _AFXDLL
	// 将本对象字符串 拷贝数据到剪切板	不支持汉字 // 有汉字会乱码,所以自己写了一个保存剪贴板的程序
	void SetDataToClip()
	{
		const char *strText=str.c_str();

		// 分配全局内存 不能使用2次
		HGLOBAL hMen = GlobalAlloc(GMEM_MOVEABLE, ((strlen(strText)+1)*sizeof(char)));    
		if(!hMen)
		{
			printf("剪切板分配全局内存出错！/n");
			return;
		}

		// 打开剪贴板   
		if (!OpenClipboard(NULL)|| !EmptyClipboard())    
		{
			printf("打开或清空剪切板出错！\n");   
			return;
		}

		char* lpStr = (char*)GlobalLock(hMen);       // 锁住内存区
		memcpy(lpStr, strText, ((strlen(strText))*sizeof(char)));    // 把数据拷贝到全局内存中
		lpStr[strlen(strText)] = (char)0;   // 字符结束符
		GlobalUnlock(hMen);   // 释放锁

		SetClipboardData(CF_TEXT, hMen);   // 把内存中的数据放到剪切板上
		CloseClipboard();   // 关闭剪切板

		return;
	}

	// 从剪切板获取数据  保存为本对象字符串 不支持汉字
	void GetDataFromClip()
	{
		// 判断数据格式    
		if (!IsClipboardFormatAvailable(CF_TEXT) )   
			return;	// printf("剪切板数据格式不是CF_TEXT\n");
		if (!OpenClipboard(NULL))    
			return;	// printf("打开剪切板出错！\n");

		// 从剪切板获取数据    
		HGLOBAL hMem = GetClipboardData(CF_TEXT);
		if(hMem)   
		{
			char* lpStr = (char*)GlobalLock(hMem);	// 获取字符串
			if(lpStr)							// 释放内存锁    
			{
				GlobalUnlock(hMem);				// printf(lpStr);
				str = lpStr;
			}
		}
		//else printf("读取剪切板失败!\n");

		CloseClipboard();						// 关闭剪切板
		//return lpStr;		// 可以返回指针,数据不丢失
	}

	// 将本对象字符串 拷贝数据到剪切板  支持汉字
	// 必须使用配置属性-常规,使用Unicode字符集
	void SetDataToClip2()
	{
		wchar_t * strText=Convert_LPSTR_LPWSTR();
		//TCHAR strText[256] = L"我被拷贝到剪切板！\r\n到剪切板a"; // 可以直接赋值加"L"符号

		// 分配全局内存 不能使用2次
		HGLOBAL hMen = GlobalAlloc(GMEM_MOVEABLE, ((wcslen(strText))*sizeof(wchar_t))*2+20);	// 必须使用配置属性-常规,使用Unicode字符集
		if(!hMen)
		{
			printf("剪切板分配全局内存出错！\n");
			return;
		}

		// 打开剪贴板
		if (!OpenClipboard(NULL)|| !EmptyClipboard())    
		{
			printf("打开或清空剪切板出错！\n");   
			return;
		}

		// 把数据拷贝考全局内存中
		wchar_t* lpStr = (wchar_t*)GlobalLock(hMen);						// 锁住内存区
		//outInt2(wcslen(strText),lpStr);
		//outLinePos(lpStr);
		wmemcpy(lpStr, strText, ((wcslen(strText))*sizeof(wchar_t)));		// 内存复制
		//outLinePos(lpStr);
		lpStr[wcslen(strText)] = (wchar_t)0;								// 字符结束符
		GlobalUnlock(hMen);												// 释放锁

		SetClipboardData(CF_UNICODETEXT, hMen);   // 把内存中的数据放到剪切板上
		CloseClipboard();   // 关闭剪切板

		delete(strText);

		return;
	}

	// 从剪切板获取数据  保存为本对象字符串 支持汉字
	void GetDataFromClip2()
	{
		// 判断数据格式    
		if (!IsClipboardFormatAvailable(CF_UNICODETEXT) )   
			return;	// printf("剪切板数据格式不是CF_TEXT\n");
		if (!OpenClipboard(NULL))    
			return;	// printf("打开剪切板出错！\n");

		// 从剪切板获取数据    
		HGLOBAL hMem = GetClipboardData(CF_UNICODETEXT);
		if(hMem)
		{
			wchar_t *lpStr = (wchar_t *)GlobalLock(hMem);	// 获取字符串
			if(lpStr)							// 释放内存锁    
			{
				GlobalUnlock(hMem);				// printf(lpStr);
				char* ss = Convert_LPWSTR_LPSTR(lpStr);
				str = ss;
				delete(ss);
			}
		}
		//else printf("读取剪切板失败!\n");

		CloseClipboard();						// 关闭剪切板
		//return lpStr;		// 可以返回指针,数据不丢失
	}

	// 添加字符串到剪贴板 同时加在本对象 注意最大长度
	void addDataToClip(HGLOBAL hMen, int maxLen=MAX_STRING_LEN)
	{

	}
#endif

	// 显示_写剪贴板_写日志
        // 如果想要将最后粘贴板中的日志写入文件,请在程序最后添加:ms.GetDataFromClip();	ms.writefile(logFilename);如果想要将最后粘贴板中的日志写入文件
	// 1 先写入剪贴板,并存入本对象 2 满足长度后写入日志,清空本对象 3 显示当前字符串
        void printandlog(string pstr, const char *logFilename=NULL, bool bPrint=true, bool bClipboard=false)
	{
		//pstr += "\r\n";	// 每个字符串加回车 对应剪贴板要加'\r'否则不能回车

		// 写入剪贴板
		if(bClipboard)
		{
			if(str.length() > MAX_STRING_LEN)// 满足长度后写入日志
			{
				if(logFilename)
					writefile(logFilename, true);
				str=pstr;
			}
			else str+=pstr;
 			//SetDataToClip();	// 使用支持汉字的函数会报错mbh
		}
		// 仅写入日志
		else if(logFilename)
		{
			FILE *fp=fopen(logFilename, "a+");
			fprintf(fp,"%s",pstr.c_str());
			fclose(fp);
		}

		// 显示
		if(bPrint)
			printf("%s",pstr.c_str());
	}

	void printandlog(char *logFilename=NULL, bool bPrint=true, bool bClipboard=false)
	{
		printandlog(str, logFilename, bPrint, bClipboard);
	}

};

/*
#ifdef _MYLOG
#define LOG_LINE2(str,x,y) fprintf(logFile,"\n%d:[%s]_%s_%d_%d",__LINE__,__FUNCTION__,str,x,y)//需预先定义和打开：FILE * f; f=fopen(...); fclose(f);
FILE * logFile=NULL;
#endif
例如:
	char strtime[32];
	time_t tt = time(0);//产生“YYYY-MM-DD hh:mm:ss”格式的字符串。
	strftime(strtime, sizeof(strtime), "%Y-%m-%d %H:%M:%S", localtime(&tt));
	strtime[31] = '\n';

	logFile=fopen("log0802.txt", "a+");
	LOG_LINE2(strtime,printi++,0);
	int ret=PrintGeneralTag(istr, codeContent);
	fclose(logFile);
*/
