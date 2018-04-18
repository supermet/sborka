//---------------------------------------------------------------------------
#include "functions.h"
//------------------
bool plugin_started;
//----------------------


DWORD dwPluginID;

typedef VOID (__stdcall *typeCommFortProcess)(DWORD dwPluginID, DWORD dID, BYTE * bOutBuffer, DWORD dwOutBufferSize);
typedef DWORD (__stdcall *typeCommFortGetData)(DWORD dwPluginID, DWORD dwID, BYTE * bInBuffer, DWORD dwInBufferSize, BYTE * bOutBuffer, DWORD dwOutBufferSize);

typeCommFortProcess CommFortProcess;
typeCommFortGetData CommFortGetData;


#ifdef __cplusplus
extern "C" {
#endif
    __declspec(dllexport) BOOL __stdcall PluginStart(DWORD dwThisPluginID, typeCommFortProcess, typeCommFortGetData);
    __declspec(dllexport) VOID __stdcall PluginStop();

    __declspec(dllexport) VOID __stdcall PluginProcess(DWORD dwID, BYTE * bInBuffer, DWORD dwInBufferSize);
    __declspec(dllexport) DWORD __stdcall PluginGetData(DWORD dwID, BYTE * bInBuffer, DWORD dwInBufferSize, BYTE * bOutBuffer, DWORD dwOutBufferSize);

    __declspec(dllexport) VOID __stdcall PluginShowOptions();
    __declspec(dllexport) VOID __stdcall PluginShowAbout();
#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
    return 1;
}
//---------------------------------------------------------------------------


BOOL __stdcall PluginStart (DWORD dwThisPluginID, typeCommFortProcess func1, typeCommFortGetData func2)
{
    //try {
    dwPluginID = dwThisPluginID;
    //При инициализации планину присваивается уникальный идентификатор
    //его необходимо обязательно сохранить, и указывать
    //в качестве первого параметра при инициировании событий и получении данных

    CommFortProcess = func1;
    //указываем функцию обратного вызова,
    //с помощью которой плагин сможет инициировать события

    CommFortGetData = func2;
    //указываем функцию обратного вызова,
    if (!fExists(AskPluginTempPath()+database))
    {
        MessageBoxA(0, (AskPluginTempPath()+database+ " not exists!").c_str(),"Error",MB_ICONSTOP);
        return false;
    }

    dllCreate();
//    }
//    catch (std::exception ex)
//    {
//        MessageBoxA(0, ex.what(),"",0);
//    }
    ConnectUser(getConfigValW("botname"), getConfigValW("ip"), getConfigValW("password"), strtoint(getConfigVal("icon")));

    InviteUser(getConfigValW("botname"), getConfigValW("kanal"));
    ChangeStatus(getConfigValW("botname"), getConfigValW("status_off"));
    plugin_started=true;

    //Возвращаемые значения:
    //TRUE - запуск прошел успешно
    //FALSE - запуск невозможен

    return TRUE;
}
//---------------------------------------------------------------------------
VOID __stdcall PluginStop()
{

    dllDestroy();
    plugin_started = false;

    //данная функция вызывается при завершении работы плагина
}
//---------------------------------------------------------------------------
VOID __stdcall PluginProcess(DWORD dwID, BYTE * bInBuffer, DWORD dwInBufferSize)
{
    //Функция приема событий
    //Параметры:
    //dwID - идентификатор события
    //bInBuffer - указатель на данные
    //dwInBufferSize - объем данных в байтах

    int iReadOffset = 0;

    if((dwID==1070) or (dwID==5)) //сообщение в общий канал
    {
        //Получаем данные о событии
        if (AskProgramType()==0)
        {
            wstring uVirtualUserLogin = fReadText(bInBuffer, &iReadOffset);
        }
        wstring uSenderLogin = fReadText(bInBuffer, &iReadOffset);
        wstring uSenderIP = fReadText(bInBuffer, &iReadOffset);
        int iSenderIcon = fReadInteger(bInBuffer, &iReadOffset);
        wstring uChannel = fReadText(bInBuffer, &iReadOffset);
        int iMessageMode = fReadInteger(bInBuffer, &iReadOffset);
        wstring uText = fReadText(bInBuffer, &iReadOffset);

        string SenderLogin = wstostr(uSenderLogin, CP_ACP);
        string SenderLoginFirst = SenderLogin;
        replace_with(SenderLogin, "'", "''");

        if (uChannel!=getConfigValW("kanal")) return;
        if (getConfigVal("botname")==SenderLoginFirst) return;
        if (iMessageMode!=0) return;

        if (uText.substr(0,6)==L"!старт")
        {
            if (GetIgraut())
            {
                MessagetoChannel(getConfigValW("botname"), getConfigValW("kanal"), strtows(SenderLogin+getFraza("1"), CP_ACP));
                return;
            }
            //		adsuper();
            if ((getConfigVal("start_admin")=="1") && (!isadmin(SenderLogin)))
            {
                MessagetoChannel(getConfigValW("botname"), getConfigValW("kanal"), strtows(SenderLogin+">"+getFraza("8"), CP_ACP));
                return;
            }
            else
            {
                startgame(SenderLogin);
                return;
            }
        }


        if (uText.substr(0,5)==L"!стоп")
        {
            if (!GetIgraut())
            {
                MessagetoChannel(getConfigValW("botname"), getConfigValW("kanal"),strtows(SenderLogin+">"+getFraza("2"), CP_ACP));
                return;
            }
            if ((getConfigVal("stop_admin")=="1") && (!isadmin(SenderLogin)))
            {
                MessagetoChannel(getConfigValW("botname"), getConfigValW("kanal"), strtows(SenderLogin+">"+getFraza("8"), CP_ACP));
                return;
            }
            else
            {
                stopgame();
                MessagetoChannel(getConfigValW("botname"), getConfigValW("kanal"), strtows(SenderLogin+getFraza("7"), CP_ACP));
                return;
            }
        }


        if (uText.substr(0,5)==L"!след" && GetIgraut())
        {
            next_command(SenderLogin);
            return;
        }

        if (uText.substr(0,5)==L"!стат")
        {
            string parametr = getParam(uText);
            if (parametr.length()>0)
            {
                replace_with(parametr, "'", "''");
                personalStatistic(parametr, SenderLoginFirst);
            }
            else
                personalStatistic(SenderLogin, SenderLoginFirst);
            return;
        }


        if (uText.substr(0,4)==L"!топ")
        {
            string parametr = getParam(uText);
            //replace_with(parametr, "'", "''");
            //	MessageBoxA(0, parametr.c_str(), "", 0);
            int nparam = strtoint(parametr);
            if (parametr.length()>0)
            {
                if ((nparam>0) && (nparam<=200))
                    showtop(nparam, SenderLoginFirst, true);
            }
            else
                showtop(10, "", false);
            return;
        }

        if (uText.substr(0,6)==L"!сброс")
        {
            if (isadmin(SenderLogin))
            {
                if (GetIgraut())
                {
                    MessagetoChannel(getConfigValW("botname"), getConfigValW("kanal"), strtows(SenderLogin+">"+getFraza("49"), CP_ACP));
                }
                else
                {
                    clearStat();
                    MessagetoChannel(getConfigValW("botname"), getConfigValW("kanal"), strtows(getFraza("48"), CP_ACP));
                }
            }
            else
                MessagetoChannel(getConfigValW("botname"), getConfigValW("kanal"), strtows(SenderLogin+">"+getFraza("8"), CP_ACP));
            return;
        }

        if (uText.substr(0,8)==L"!команды")
        {
            wstring s= strtows("\n"+getFraza("50"), CP_ACP);
            PrivatMessage(getConfigValW("botname"), strtows(SenderLoginFirst, CP_ACP), s);
            return;
        }


        if (GetIgraut())  checkquestion(wstostr(uText, CP_ACP), SenderLogin);

    }

}


//---------------------------------------------------------------------------
DWORD __stdcall PluginGetData(DWORD dwID, BYTE * bInBuffer, DWORD dwInBufferSize, BYTE * bOutBuffer, DWORD dwOutBufferSize)
{
    //функция передачи данных программе

    int iReadOffset = 0; //вспомогательные переменные для упрощения работы с блоком данных
    int iWriteOffset = 0;

    //при значении dwOutBufferSize равным нулю функция должна вернуть объем данных, ничего не записывая

    if(dwID==2800) //предназначение плагина
    {
        if(dwOutBufferSize==0)
            return 4; //объем памяти в байтах, которую необходимо выделить программе

        fWriteInteger(bOutBuffer,&iWriteOffset,0);  //плагин подходит куда угодно

        return 4;//объем заполненного буфера в байтах
    }
    else if(dwID==2810) //название плагина (отображается в списке)
    {
        wstring uName =  L"Сборка 1.4.1";//название плагина
        int iSize = uName.length()*2+4;

        if(dwOutBufferSize==0)
            return iSize; //объем памяти в байтах, которую необходимо выделить программе

        fWriteText(bOutBuffer, &iWriteOffset, uName);

        return iSize;//объем заполненного буфера в байтах
    }

    return 0;//возвращаемое значение - объем записанных данных
}
//---------------------------------------------------------------------------
VOID __stdcall PluginShowOptions()
{
    //данная функция вызывается при нажатии кнопки "Настроить" в списке плагинов
    //если Вы не желаете чтобы активировалась кнопка "Настроить", просто сотрите данную функцию
    if (plugin_started)
        ShellExecuteA(0, "open", (AskPluginTempPath()+inifile).c_str(),0,0, SW_SHOWNORMAL);
    else
        MessageBoxW(0, L"Запустите плагин", L"Инфо", MB_ICONSTOP);

}
//---------------------------------------------------------------------------
VOID __stdcall PluginShowAbout()
{
    //данная функция вызывается при нажатии кнопки "О плагине" в списке плагинов
    //если Вы не желаете чтобы активировалась кнопка "Настроить", просто сотрите данную функцию

    MessageBoxW(0, L"Игра сборка\nРазработал GaRrY(supermet@list.ru)", L"Информация", 0);
}


void ConnectUser(wstring Name, wstring ip, wstring password, int icon) //для сервера
{
    if (AskProgramType()!=0) return; //для клиента выход
    string ads = fTextTostring(Name)+  //имя
                 fTextTostring(ip)+ //IP-адрес
                 fIntegerTostring(0)+  //пароль в открытом виде
                 fTextTostring(password)+   //пароль
                 fIntegerTostring(icon);
    (*CommFortProcess)(dwPluginID,1001,(BYTE*)ads.c_str(),ads.length());
}

// ID: 1026
//--текст(имя виртуального пользователя) + текст(название канала) + число(видимость) + число(тип входа)
//ID: 67
//Блок данных: число(режим) + текст(канал)
void InviteUser(wstring Name, wstring Chanel)
{
    DWORD id;
    string ads;
    if (AskProgramType()==0)
    {
        ads = fTextTostring(Name)+ fTextTostring(Chanel)+ fIntegerTostring(1)+ fIntegerTostring(0);
        id=1026;
    }
    else
    {
        ads = fIntegerTostring(1)+ fTextTostring(Chanel);
        id=67;
    }
    (*CommFortProcess)(dwPluginID, id, (BYTE*)ads.c_str(), ads.length());
}
//ID: 1020
//Блок данных: текст(имя виртуального пользователя) + число(режим) + текст(канал) + текст(сообщение)
//ID: 50
//Блок данных: текст(название канала) + число(тип сообщения) + текст(сообщение)
void MessagetoChannel (wstring Name, wstring Chanel, wstring text)
{
    DWORD id;
    string ads;
    string txt=wstostr(text, CP_ACP);
    replace_with(txt, "''", "'");
    text=strtows(txt, CP_ACP);
    if (AskProgramType()==0)
    {
        ads = fTextTostring(Name)+ fIntegerTostring(0)+ fTextTostring(Chanel)+ fTextTostring(text);
        id=1020;
    }
    else
    {
        ads = fTextTostring(Chanel) + fIntegerTostring(0) + fTextTostring(text);
        id=50;
    }
    (*CommFortProcess)(dwPluginID, id, (BYTE*)ads.c_str(), ads.length());
}
//ID: 1021
//Блок данных: текст(имя виртуального пользователя) + число(режим) + текст(имя получателя) + текст(сообщение)
//ID: 63
//Блок данных: текст(имя пользователя) + число(тип сообщения) + текст(сообщение)
void PrivatMessage (wstring Name, wstring Poluchatel, wstring text)
{
    DWORD id;
    string ads;
    if (AskProgramType()==0)
    {
        ads = fTextTostring(Name)+ fIntegerTostring(0)+ fTextTostring(Poluchatel)+ fTextTostring(text);
        id=1021;
    }
    else
    {
        ads = fTextTostring(Poluchatel) + fIntegerTostring(0)+ fTextTostring(text);
        id=63;
    }
    (*CommFortProcess)(dwPluginID, id, (BYTE*)ads.c_str(), ads.length());
}
//ID: 1023
//Блок данных: текст(имя виртуального пользователя) + текст(канал) + текст(новая тема)
//ID: 61
//Блок данных: текст(название канала) + текст(новая тема)
void ChangeTheme (wstring Name, wstring Chanel, wstring text)
{
    DWORD id;
    string ads;
    if (AskProgramType()==0)
    {
        ads = fTextTostring(Name)+ fTextTostring(Chanel)+ fTextTostring(text);
        id=1023;
    }
    else
    {
        ads = fTextTostring(Chanel)+ fTextTostring(text);
        id=61;
    }
    (*CommFortProcess)(dwPluginID, id, (BYTE*)ads.c_str(), ads.length());
}
//-----------------
//ID: 1024
//Блок данных: текст(имя виртуального пользователя) + текст(канал) + текст(новая тема)
//ID: 62
//Блок данных: текст(название канала) + текст(новое приветствие)
void ChangePrivet (wstring Name, wstring Chanel, wstring text)
{
    DWORD id;
    string ads;
    if (AskProgramType()==0)
    {
        ads = fTextTostring(Name)+ fTextTostring(Chanel)+ fTextTostring(text);
        id=1024;
    }
    else
    {
        ads = fTextTostring(Chanel)+ fTextTostring(text);
        id = 62;
    }
    (*CommFortProcess)(dwPluginID, id, (BYTE*)ads.c_str(), ads.length());
}
//ID: 1025
//Блок данных: текст(имя виртуального пользователя) + текст(новое состояние)
//ID: 53
//Блок данных: текст(новое состояние) + число(флаг важности сообщений) + число(звуки) + число(всплывающие сообщения в трее)
void ChangeStatus (wstring Name, wstring text)
{
    DWORD id;
    string ads;
    if (AskProgramType()==0)
    {
        ads = fTextTostring(Name)+fTextTostring(text);
        id=1025;
    }
    else
    {
        ads = fTextTostring(text)+fIntegerTostring(0)+fIntegerTostring(0)+fIntegerTostring(0);
        id=53;
    }
    (*CommFortProcess)(dwPluginID, id, (BYTE*)ads.c_str(), ads.length());
}


string AskPluginTempPath()
{
    // return "c:\\ProgramData\\CommFort_server\\PluginsData\\";
    DWORD iSize;

    iSize = CommFortGetData(dwPluginID, 2010, 0, 0, 0, 0);
    BYTE* buf = new BYTE[iSize];
    CommFortGetData(dwPluginID, 2010, buf, iSize, 0, 0);
    memcpy(&iSize, buf, 4);
    wstring uRet;
    uRet.resize(iSize);
    memcpy((void*)uRet.c_str(), buf+4, iSize*2);
    delete [] buf;
    return wstostr(uRet, CP_ACP);
}

DWORD AskProgramType()
{
    DWORD iSize;
    iSize =CommFortGetData(dwPluginID, 2000, 0, 0, 0, 0);
    BYTE* buf = new BYTE[iSize];
    CommFortGetData(dwPluginID, 2000, buf, iSize, 0, 0);
    memcpy(&iSize, buf, 4);
    delete [] buf;
    return iSize;
}

//int main()
//{
//
////dllCreate();
////show_next_slovo();
//    return 0;
//}

