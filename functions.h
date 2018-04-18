#ifndef FUNC_H__
#define FUNC_H__
#include <windows.h>
//#include <process.h>
//#include <iostream>
//#include <sstream>
//#include <string>
//#include <io.h>
//#include <cctype>
#include <ctime>
#include <cmath>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include "CppSQLite3.h"

#define BUFSIZE MAX_PATH

#define inifile "sborka\\config.ini"
#define database "sborka\\base.db"
#define spisok_pokaz "sborka\\words.txt"
#define spisok "sborka\\variants.txt"
#define rangs "sborka\\rangs.txt"


using namespace std;

//-----------------
void ConnectUser(wstring Name, wstring ip, wstring password, int icon);
void InviteUser(wstring Name, wstring Chanel);
void MessagetoChannel (wstring Name, wstring Chanel, wstring text);
void PrivatMessage (wstring Name, wstring Poluchatel, wstring text);
void ChangeTheme (wstring Name, wstring Chanel, wstring text);
void ChangePrivet (wstring Name, wstring Chanel, wstring text);
void ChangeStatus (wstring Name, wstring text);


string AskPluginTempPath();
DWORD AskProgramType();
//-----------------


//-----------------------------------------------------
int fReadInteger(BYTE * bInBuffer, int * iOffset);
wstring fReadText(BYTE * bInBuffer, int * iOffset);
void fWriteInteger(BYTE * bOutBuffer, int * iOffset, int iValue);
void fWriteText(BYTE * bOutBuffer, int * iOffset, wstring uValue);
string fTextTostring(wstring uText);
string fIntegerTostring(int iValue);
//-----------------------------------------------------
wstring strtows(const string str, UINT codePage);
string wstostr(const wstring ws, UINT codePage);
string chcp(const string str, UINT codePageSrc, UINT codePageDst);
string utf2acp(string src);
string acp2utf(string src);

string trim(string str);
string toLowerCase(string str);
string toUpperCase(string str);

string inttostr(int a);
int strtoint(string s);
string getParam(wstring txt);
int countchar(string st, char ch);
bool fExists(const string&filename);
void replace_with(string & src, const string & what, const string & with);
//-------------------------------------------------------
unsigned int getrandom(unsigned int min, unsigned int max);
double Round (double x);

string current_dir();
string getConfigVal(const string& key);
wstring getConfigValW(const string& key);

void  SetIgraut(bool igr);
bool  GetIgraut();

void loadWords();
void dllCreate();
void dllDestroy();

string getRandomWord();
void show_next_slovo();
int getCount(string tab, string keyfield, string value);
string getFraza(string key);
int getRangId(string);
int getRangId(int);
bool isadmin(string user);
void startgame(string user);
void stopgame();
void checkquestion(string mess, string user);
void next_command(string user);
void showtop (int n, string to, bool privat);
void personalStatistic(string user, string to);

void adsuper();
void adrangs();
void clearStat();

#endif
