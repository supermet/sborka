#include "functions.h"
#include "timer.h"

vector <string> words, words_variants;
vector <string> current_words;
map<string, string> configs;

CppSQLite3DB * sqlite;
string basa;

bool igraut;

int word_number;
string word;
int quests_no_anwer;
string lastAnswer;


unsigned int rsd; // random seed

Timer* timer_start, *timer_check;
string the_best;

//struct USER
//{
//    string rang;
//    bool in_game;
//    int replies;
//};
//map <string, USER> Users;



int fReadInteger(BYTE * bInBuffer, int * iOffset) //вспомогательная функция для упрощения работы с чтением данных
{
    int iLength;
    memcpy(&iLength, bInBuffer + (*iOffset),4);
    (*iOffset)+=4;
    return iLength;
}
//---------------------------------------------------------------------------
wstring fReadText(BYTE * bInBuffer, int * iOffset) //вспомогательная функция для упрощения работы с чтением данных
{
    int iLength;
    memcpy(&iLength, bInBuffer + (*iOffset),4);
    (*iOffset)+=4;
    wstring uRet;
    uRet.resize(iLength);
    memcpy((void*)uRet.c_str(), bInBuffer + (*iOffset),iLength*2);
    (*iOffset)+=iLength*2;
    return uRet;
}
//---------------------------------------------------------------------------
void fWriteInteger(BYTE * bOutBuffer, int * iOffset, int iValue) //вспомогательная функция для упрощения работы с записью данных
{
    memcpy(bOutBuffer + (*iOffset), &iValue, 4);
    (*iOffset)+=4;
}
//---------------------------------------------------------------------------
void fWriteText(BYTE * bOutBuffer, int * iOffset, wstring uValue) //вспомогательная функция для упрощения работы с записью данных
{
    int iLength = uValue.length();

    memcpy(bOutBuffer + (*iOffset),&iLength,4);
    (*iOffset)+=4;

    memcpy(bOutBuffer + (*iOffset),uValue.c_str(),iLength*2);
    (*iOffset)+=iLength*2;
}
//---------------------------------------------------------------------------
string fTextTostring(wstring uText) //вспомогательная функция для упрощения работы с данными
{
    //функция предназначена для ознакомительных целей,
    //не рекомендуется для реального применения,
    //так как при ее использовании проявляется избыточное копирование данных

    int iLength = uText.length();

    string aReturn;

    aReturn.resize(4+iLength*2);

    memcpy((void*)aReturn.c_str(),&iLength,4);
    memcpy((void*)(aReturn.c_str()+4),uText.c_str(),iLength*2);

    return aReturn;
}
//---------------------------------------------------------------------------
string fIntegerTostring(int iValue) //вспомогательная функция для упрощения работы с данными
{
    //функция предназначена для ознакомительных целей,
    //не рекомендуется для реального применения,
    //так как при ее использовании проявляется избыточное копирование данных

    string aReturn;
    aReturn.resize(4);
    memcpy((void*)aReturn.c_str(),&iValue,4);

    return aReturn;
}

std::wstring strtows(const std::string str, UINT codePage)
{
    std::wstring ws;
    int n = MultiByteToWideChar(codePage, 0, str.c_str(), str.size()+1, /*dst*/NULL, 0);
    if(n)
    {
        ws.resize(n-1);
        if(MultiByteToWideChar(codePage, 0, str.c_str(), str.size()+1, /*dst*/&ws[0], n) == 0)
            ws.clear();
    }
    return ws;
}
std::string wstostr(const std::wstring ws, UINT codePage)
{
    std::string str;
    int n = WideCharToMultiByte(codePage, 0, ws.c_str(), ws.size()+1, /*dst*/NULL, 0, /*defchr*/0, NULL);
    if(n)
    {
        str.resize(n-1);
        if(WideCharToMultiByte(codePage, 0, ws.c_str(), ws.size()+1, /*dst*/&str[0], n, /*defchr*/0, NULL) == 0)
            str.clear();
    }
    return str;
}

std::string chcp(const std::string str, UINT codePageSrc, UINT codePageDst)
{
    return wstostr(strtows(str, codePageSrc), codePageDst);
}

string utf2acp(string src)
{
    return chcp(src, CP_UTF8, CP_ACP);
}

string acp2utf(string src)
{
    return chcp(src, CP_ACP, CP_UTF8);
}

string trim(string str)
{
    string::size_type pos = str.find_last_not_of(' ');
    if(pos != string::npos)
    {
        str.erase(pos + 1);
        pos = str.find_first_not_of(' ');
        if(pos != string::npos) str.erase(0, pos);
    }
    else str.erase(str.begin(), str.end());
    return str;
}

string toLowerCase(string str)
{
    transform(str.begin(), str.end(), str.begin(), (int(*)(int))tolower);
    return str;
}

string toUpperCase(string str)
{
    transform(str.begin(), str.end(), str.begin(), (int(*)(int))toupper);
    return str;
}

string formatString (const string& format, ...)
{
    char buffer[512];
    va_list args;
    va_start (args, format);
    vsprintf (buffer,format.c_str(), args);
    va_end (args);
    return string(buffer);
}

string inttostr(int num)
{
    char result[20];
    sprintf(result, "%d", num);
    return result;
}

string doubletostr(double num)
{
    char result[20];
    sprintf(result, "%.0f", num);
    return result;
}

int strtoint(string s)
{
    return atoi(s.c_str());
}


string getParam(wstring txt)
{
    string parametr="";
    string zaprostrim = trim(wstostr(txt, CP_ACP));
    string::size_type pos = (zaprostrim.find_first_of(' '));
    if (pos!=string::npos)
        parametr = trim(zaprostrim.substr(pos+1));
    return parametr;
}


int countchar(string st, char ch)
{
    return count(st.begin(), st.end(), ch);
}

unsigned int getrandom(unsigned int min, unsigned int max)
{
    rsd = rsd * 0x8088405 + 1;
    unsigned int random = (unsigned __int64)(rsd) * (max-min+1) >> 32;
    random += min;
    return random;
}


//int random(int lim)
//{
//    g_RandSeed = g_RandSeed * 0x8088405 + 1;
//    return __int64(g_RandSeed) * lim >> 32;
////134775813
//}


string current_dir()
{
    CHAR Buffer[BUFSIZE];
    DWORD dwRet;
    //dwRet = GetCurrentDirectoryA(BUFSIZE, Buffer);
    dwRet = GetModuleFileNameA(GetModuleHandleA(""), Buffer, BUFSIZE);
    if( dwRet == 0 )
        return ("");
    if(dwRet > BUFSIZE)
        return ("");
    string s(Buffer);
    return s.substr(0,s.find_last_of("\\/"));
}

bool fExists(const string&filename)
{
	ifstream ifs(filename.c_str());
	bool result = (ifs != NULL);
	ifs.close();
	return result;
}

string getConfigVal(const string& key)
{
    if (configs.size()==0) return "";
    map<string, string>::iterator it;
    it = configs.find(key);
    if (it!=configs.end())
        return it->second;
    else return "";
}


wstring getConfigValW(const string& key)
{
    return strtows(getConfigVal(key), CP_ACP);
}


void  SetIgraut(bool igr)
{
    igraut=igr;
}
bool  GetIgraut()
{
    return igraut;
}


void __stdcall start_on_timer(HWND hwnd, UINT msg, UINT idtimer, DWORD dwTime)
{
    show_next_slovo();

    timer_check->start();
    timer_start->stop();
}

void __stdcall check_on_timer(HWND hwnd, UINT msg, UINT idtimer, DWORD dwTime)
{

    //int count= getCount("users", "inGame", "1");
    if (lastAnswer.length()==0)
        quests_no_anwer++;
    else
        quests_no_anwer=0;
    if (quests_no_anwer>=strtoint(getConfigVal("words_stop")))
    {
        stopgame();
        MessagetoChannel(getConfigValW("botname"), getConfigValW("kanal"), strtows(getFraza("15"), CP_ACP));
        return;
    }

    show_next_slovo();

}

void loadConfig()
{
    string line;
    ifstream myfile ((AskPluginTempPath()+inifile).c_str());
    if (!myfile.is_open()) return;
    while (!myfile.eof() )
    {
        getline (myfile,line);
        line = trim(line);
        if (line.substr(0, 1)=="#" || line.substr(0, 1)==";") continue;
        string::size_type pos=line.find("=",0);
        if (pos==string::npos) continue;
        string key = trim(line.substr(0, pos));
        string value = trim(line.substr(pos+1));

        configs[key] = value;
    }
    myfile.close();
}


void loadWords()
{
    string line;
    ifstream myfile ((AskPluginTempPath()+spisok_pokaz).c_str());
    if (myfile.is_open())
    {
        while (!myfile.eof() )
        {
            getline (myfile,line);
            line = toLowerCase(trim(line));
            if (line.length()>0)
                words.push_back(line);
        }
        myfile.close();
    }
    myfile.open((AskPluginTempPath()+spisok).c_str());
    if (myfile.is_open())
    {
        while (!myfile.eof() )
        {
            getline (myfile,line);
            line = toLowerCase(trim(line));
            if (line.length()>0)
                words_variants.push_back(line);
        }
        myfile.close();
    }
}


void dllCreate()
{
    rsd = GetTickCount();
    //  setlocale(LC_ALL,"Russian");
    setlocale(LC_ALL,"");

    igraut=false;

//MessageBoxA(0, basa.c_str(), getConfigVal("gamers_count").c_str(), 0);
    basa=acp2utf(AskPluginTempPath()+database);
    sqlite = new CppSQLite3DB();
    sqlite->open(basa.c_str());
    sqlite->execDML("Vacuum");
    sqlite->execDML("update users set inGame=0");

    loadWords();
    loadConfig();
    // MessageBoxA(0, getConfigVal("botname").c_str(), "", 0);
    //loadUsers();
    //  MessageBoxA(0, inttostr(AskProgramType()).c_str(), "", 0);

    timer_start = new Timer(&start_on_timer);
    timer_check = new Timer(&check_on_timer);


    adsuper();
    adrangs();
}

void dllDestroy()
{
    words.clear();
    words_variants.clear();
    configs.clear();

    timer_start->stop();
    timer_check->stop();
    delete timer_start;
    delete timer_check;

    sqlite->close();
    delete sqlite;
}

string getRandomWord()
{
    string s="";
    if (words.size()>0)
    {
        s=words.at(getrandom(0, words.size()-1));
    }
    return s;
}

int getCount(string tab, string keyfield, string value)
{
    int n=0;
    //replace_with(value, "'", "''");
    string s="select count(*) from "+tab+" where "+keyfield+" = '"+acp2utf(value)+"' limit 1";
    CppSQLite3Query stmt = sqlite->execQuery(s);
    if (!(stmt.eof()))
    {
        n=stmt.getIntField(0);
    }

    return n;
}

string getFieldVal(string tab, string fieldname, string keyfield, string value)
{
    string st="";
    //replace_with(value, "'", "''");
    string s = "select "+fieldname+" from "+tab+" where "+keyfield+" = '"+acp2utf(value)+"' limit 1";
    CppSQLite3Query stmt = sqlite->execQuery(s);
    if (!(stmt.eof()))
    {
        st=utf2acp(stmt.getStringField(0));
    }
    return st;
}


int getIntFieldVal(string tab, string fieldname, string keyfield, string value)
{
    int n=0;
    //replace_with(value, "'", "''");
    string s = "select "+fieldname+" from "+tab+" where "+keyfield+" = '"+acp2utf(value)+"' limit 1";
    CppSQLite3Query stmt = sqlite->execQuery(s);
    if (!(stmt.eof()))
    {
        n=stmt.getIntField(0);
    }
    return n;
}


string getStatistic(string fieldname)
{
    string result="";
    string s="select "+fieldname+" from statistic limit 1";
    CppSQLite3Query stmt = sqlite->execQuery(s);
    if (!(stmt.eof()))
    {
        result=stmt.getStringField(0);
    }
    return result;
}


string getFraza(string key)
{
    return getFieldVal ("frazy", "name", "id", key);
}


bool slovopodhodit(string varian, string ish)
{
    varian=toLowerCase(trim(varian));
    ish= toLowerCase(trim(ish));
    if ((ish.find(varian,0)!=string::npos) || (varian.length()>ish.length()))
        return false;

    for (unsigned int i=0; i<varian.length(); i++)
    {
        string::size_type  pos = ish.find(varian.at(i),0);
        if (pos!=string::npos)
            ish.erase(pos,1);
        else
            return false;
    }
    return true;
}

bool stringExists(string s)
{
    if (words_variants.size()==0) return true;
    vector<string>::iterator result;
    result = find( words_variants.begin(), words_variants.end(), toLowerCase(trim(s)));
    return (result != words_variants.end());
}


string getmesto(string user)
{
    int n=0;
    string s = "select name from users order by ochki desc, name";
    CppSQLite3Query stmt = sqlite->execQuery(s);
    while (!(stmt.eof()))
    {
        n++;
        string dbuser=utf2acp(stmt.getStringField(0));
        replace_with(dbuser, "'", "''");
        if (dbuser==user)
            return inttostr(n);
        stmt.nextRow();
    }
    return "0";
}


string getbest()
{
    CppSQLite3Query stmt = sqlite->execQuery("select name, ochki from users order by ochki desc, name limit 1");
    string n="";
    if (!(stmt.eof()))
        n=utf2acp(stmt.getStringField(0));

    return n;
}


int getRangId(string user) //старый ранг
{
    string sql = "select rang from users where rang is not null and name='"+acp2utf(user)+"'";
    CppSQLite3Query stmt = sqlite->execQuery(sql);
    int rang = 0;
    if (!(stmt.eof()))
        rang=stmt.getIntField(0);
    return rang;
}

int getRangId(int ochki) // новый ранг
{
    CppSQLite3Query stmt = sqlite->execQuery("select id from rangs where ochki<="+inttostr(ochki)+" order by ochki desc limit 1");
    int rang=0;
    if (!(stmt.eof()))
        rang=stmt.getIntField(0);
    return rang;
}

string getRang(string user)
{
    string sql = "select b.name from users a, rangs b where a.rang=b.id and a.rang is not null and a.name='"+acp2utf(user)+"'";
    CppSQLite3Query stmt = sqlite->execQuery(sql);
    string rang = "";
    if (!(stmt.eof()))
        rang=utf2acp(stmt.getStringField(0));
    return rang;
}

void setRang(string user, int ochki)
{
    int rangid = getRangId(ochki);
    if (rangid!=getRangId(user))
    {
        sqlite->execDML("update users set rang = "+inttostr(rangid)+" where name = '"+acp2utf(user)+"'");
        string s = formatString(getFraza("19"), user.c_str(),  getRang(user).c_str());
        MessagetoChannel(getConfigValW("botname"), getConfigValW("kanal"), strtows(s, CP_ACP));
    }
}

bool isadmin(string user)
{
    return (getFieldVal("users", "isadmin", "name", user)=="1");
}

void showtop (int n, string to, bool privat)
{

    int i=1;
    string s;
    CppSQLite3Query stmt = sqlite->execQuery("select ochki, name from users order by ochki desc, name limit "+ inttostr(n));

    s=formatString(getFraza("4"), inttostr(n).c_str());

    while (!(stmt.eof()))
    {
        s+=formatString(getFraza("22"), inttostr(i).c_str(), utf2acp(stmt.getStringField(1)).c_str(),stmt.getStringField(0));
        i++;
        stmt.nextRow();
    }

    s+=formatString(getFraza("34"), inttostr(words.size()).c_str(), getStatistic("zadano").c_str());


    if (privat)
        PrivatMessage(getConfigValW("botname"), strtows(to, CP_ACP), strtows(s, CP_ACP));
    else
        MessagetoChannel(getConfigValW("botname"), getConfigValW("kanal"), strtows(s, CP_ACP));
}



string DMYhms ()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [25];

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    strftime (buffer,25,"%d/%m/%Y - %H:%M:%S",timeinfo);
    return buffer;
}


void showstat_priv()
{
    int i;
    double vsego, otgadal;
    char procn[20];
    string s;
    CppSQLite3Query stmt = sqlite->execQuery("select ochki, name, vsego, otgadal from users order by ochki desc, name limit 10");
    i=1;
    s=getFraza("42");
    while (!(stmt.eof()))
    {
        vsego = stmt.getFloatField(2);
        otgadal = stmt.getFloatField(3);
        if (vsego>0)
            sprintf(procn,"%.1f",floor(otgadal/vsego*1000 + 0.5)/10);
        else
            strcpy(procn,"0,0");

        s+=formatString(getFraza("44"), inttostr(i).c_str(), utf2acp(stmt.getStringField(1)).c_str(),
                        stmt.getStringField(0), procn);
        i++;
        stmt.nextRow();
    }

    s+= formatString(getFraza("43"), inttostr(words.size()).c_str(), getStatistic("zadano").c_str(), DMYhms().c_str())+getFraza("50");
    ChangePrivet(getConfigValW("botname"), getConfigValW("kanal") , strtows(s, CP_ACP));
}



void personalStatistic(string user, string to)
{
    int count= getCount("users", "name", user);
    if (count==0) return;

    string s, adminis;
    CppSQLite3Query stmt = sqlite->execQuery("select vsego, otgadal, ochki, name from users where name='"+acp2utf(user)+"'");
    if (isadmin(user))
        adminis=utf2acp(" Да");
    else adminis=utf2acp(" Нет");
    s="";

    double vsego, otgadal;
    char procn[20];
    string sr_vr;

    if (!(stmt.eof()))
    {
        vsego = stmt.getFloatField(0);
        otgadal = stmt.getFloatField(1);
        if (vsego>0)
            sprintf(procn,"%.1f",floor(otgadal/vsego*1000 + 0.5)/10);
        else
            strcpy(procn,"0,0");

        s=formatString(getFraza("32"), utf2acp(stmt.getStringField(3)).c_str(), getmesto(user).c_str(), stmt.getStringField(2),
                       getRang(user).c_str(), stmt.getStringField(0), stmt.getStringField(1), procn, adminis.c_str());
    }
    PrivatMessage(getConfigValW("botname"), strtows(to, CP_ACP), strtows(s, CP_ACP));
}


void show_next_slovo()
{
    string s;
    //int zadano;

    word = getRandomWord();
    if (word_number==1)
        s=formatString(getFraza("3"),inttostr(word_number).c_str(),word.c_str());
    else
        s=formatString(getFraza("12"),inttostr(word_number).c_str(),word.c_str());
    MessagetoChannel(getConfigValW("botname"), getConfigValW("kanal"), strtows(s, CP_ACP));
    // MessageBoxA(0, s.c_str(),"kk", 0);
    current_words.clear();


    ///------------------
    CppSQLite3Query stmt = sqlite->execQuery("select zadano from statistic limit 1");
    if (stmt.eof())
    {
        sqlite->execDML("insert into statistic values(0)");
    }
    sqlite->execDML("update statistic set zadano=zadano+1");
    ///-------------------
    word_number++;
    lastAnswer="";

    sqlite->execDML("update users set replies=0");


    if  (getConfigVal("change_theme")=="1")
    {
        ChangeTheme(getConfigValW("botname"), getConfigValW("kanal"), strtows(getFraza("26")+word, CP_ACP));
    }

    if  (getConfigVal("show_privet")=="1") showstat_priv();


}

void registerUser(string user)
{
    int count= getCount("users", "name", user);
    if (count==0)
        sqlite->execDML("insert into users(name, inGame) values ('"+acp2utf(user)+"', 1)");
    else
    {
        sqlite->execDML("update users set inGame=1 where name='"+acp2utf(user)+"'");
        //  MessageBoxA(0, ("update users set inGame=1 where name='"+chcp(user, CP_ACP, CP_UTF8)+"'").c_str(),"kk", 0);
    }

}

void addReply(string user)
{
    registerUser(user);
    sqlite->execDML("update users set replies=replies+1 where name='"+acp2utf(user)+"'");
    sqlite->execDML("update users set vsego=vsego+1 where name='"+acp2utf(user)+"'");
}



void startgame(string user)
{
    string s;
    registerUser(user);
    int count= getCount("users", "inGame", "1");
    if (count<strtoint(getConfigVal("gamers_count")))
    {
        s=formatString(getFraza("16").c_str(),inttostr(count).c_str(), getConfigVal("gamers_count").c_str());
        MessagetoChannel(getConfigValW("botname"), getConfigValW("kanal"), strtows(s, CP_ACP));
        igraut=false;
    }
    else
    {
        quests_no_anwer=0;
        word_number=1;

        igraut=true;

        ChangeStatus(getConfigValW("botname"), getConfigValW("status_on"));

        s= user+getFraza("14");
        MessagetoChannel(getConfigValW("botname"), getConfigValW("kanal"), strtows(s, CP_ACP));

        timer_start->setInterval(strtoint(getConfigVal("start_interval"))*1000);
        timer_start->start();
        timer_check->setInterval (strtoint(getConfigVal("interval"))*1000);
    }
}


void stopgame()
{
    timer_start->stop();
    timer_check->stop();

    igraut=false;

    ChangeStatus(getConfigValW("botname"), getConfigValW("status_off"));
    sqlite->execDML("update users set inGame=0");
}

void next_command(string user)
{

    int ochki, minus;
    if (getCount("users", "name", user)==0) return;

    quests_no_anwer=0;

    minus= strtoint(getConfigVal("next_minus"));
    if (minus<0) minus=0;
    ochki=getIntFieldVal("users", "ochki", "name", user)-minus;
    sqlite->execDML("update users set ochki="+inttostr(ochki)+"  where name='"+acp2utf(user)+"'");

    setRang(user, ochki);

    string s=formatString(getFraza("11").c_str(), user.c_str(), inttostr(minus).c_str(),
                          inttostr(ochki).c_str(), getmesto(user).c_str());

    MessagetoChannel(getConfigValW("botname"), getConfigValW("kanal"),strtows(s, CP_ACP));

    timer_start->stop();
    timer_check->stop();
    timer_start->start();
}

double Round (double x)
{
    return (floor (x + 0.5));
}


int countOchki(string text)
{
    return trim(text).length()*strtoint(getConfigVal("scores"));
}


void checkquestion(string mess, string user)
{
    int plusochki,  ochki;
    string s;

    lastAnswer=mess;
    addReply(user);

    int max_replies = strtoint(getConfigVal("max_replies"));
    int replies = getIntFieldVal("users", "replies", "name", user);

    if (max_replies!=0)
    {
        if (replies==max_replies)
        {
            s=formatString(getFraza("5"), user.c_str());
            MessagetoChannel(getConfigValW("botname"), getConfigValW("kanal"),strtows(s, CP_ACP));
        }
        else if  (replies>max_replies) return;
    }


    if (slovopodhodit(mess, word) and stringExists(mess) and
            find(current_words.begin(), current_words.end(), toLowerCase(trim(mess)))== current_words.end())

    {
        current_words.push_back(toLowerCase(trim(mess)));

        quests_no_anwer=0;

        sqlite->execDML("update users set otgadal=otgadal+1 where name='"+acp2utf(user)+"'");
        plusochki= countOchki(mess);

        ochki=getIntFieldVal("users", "ochki", "name", user)+plusochki;
        sqlite->execDML("update users set ochki="+inttostr(ochki)+" where name='"+acp2utf(user)+"'");
        setRang(user, ochki);
        s= formatString(getFraza("23"), user.c_str(), inttostr(plusochki).c_str(), inttostr(ochki).c_str(), getmesto(user).c_str());
        MessagetoChannel(getConfigValW("botname"), getConfigValW("kanal"),strtows(s, CP_ACP));

    }
}

void adsuper()
{

    sqlite->execDML("update users set isadmin=0");
    string admins = getConfigVal("admins");
    replace_with(admins, "'", "''");
    vector<string> v;
    string::size_type beg=0, end=0;
    while (1)
    {
        beg = admins.find_first_not_of(",", end);
        if (beg==string::npos) break;
        end = admins.find_first_of(",", beg);
        if (end==string::npos)
        {
            v.push_back(trim(admins.substr(beg)));
            break;
        }
        else
            v.push_back(trim(admins.substr(beg, end-beg)));
    }

    int count;
    sqlite->execDML("BEGIN;");
    for(unsigned int i=0; i < v.size(); ++i)
    {
        count= getCount("users", "name", v.at(i));
        if (count==0)
            sqlite->execDML("insert into users(name, isadmin) values ('"+acp2utf(v.at(i))+"', 1)");
        else
            sqlite->execDML("update users set isadmin=1 where name='"+acp2utf(v.at(i))+"'");
    }
    sqlite->execDML("COMMIT;");
    v.clear();

}


void adrangs()
{
    sqlite->execDML("delete from rangs");
    string line, ochki, name;
    string::size_type pos1, pos2;
    string sql="";
    ifstream myfile ((AskPluginTempPath()+rangs).c_str());
    if (myfile.is_open())
    {
        sqlite->execDML("BEGIN;");
        while (!myfile.eof() )
        {
            getline (myfile,line);
            line = trim(line);
            if (line.length()>0)
            {
                pos1 = line.find_first_of(" ");
                if (pos1!=string::npos)
                {
                    ochki=line.substr(0, pos1);
                    pos2 = line.find_first_not_of(" ", pos1);
                    if (pos2!=string::npos)
                    {
                        name = line.substr(pos2);
                        sql = "insert into rangs(ochki, name) values ("+ochki+", '"+acp2utf(name)+"');";
                        sqlite->execDML(sql);
                    }
                }
            }
        }
        sqlite->execDML("COMMIT;");
        myfile.close();
    }

}

void clearStat()
{
    sqlite->execDML("update users set vsego=0, otgadal=0, ochki=0, rang=NULL");
    sqlite->execDML("update statistic set zadano=0");
}


void replace_with(std::string & src, const std::string & what, const std::string & with)
{
    if (what != with)
    {
        std::string temp;
        std::string::size_type prev_pos = 0, pos = src.find(what, 0);
        while ( std::string::npos != pos )
        {
            temp += std::string(src.begin() + prev_pos, src.begin() + pos) + with;
            prev_pos = pos + what.size();
            pos = src.find(what, prev_pos);
        }
        if ( !temp.empty() )
        {
            src = temp + std::string(src.begin() + prev_pos, src.end());
        }
    }
}

