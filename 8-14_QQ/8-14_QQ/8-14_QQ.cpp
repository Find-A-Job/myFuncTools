// 8-14_QQ.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "8-14_QQ.h"
#include "MyMacro.h"

#pragma comment(lib, "ws2_32.lib")
#define MAX_LOADSTRING 100

#define MYDEBUGFORTEST

/*动态链接库*/
typedef int(*TTC)(TCHAR *, size_t, char *);				//宽字符转字符
typedef int(*CTI)(char *, size_t, int *);				//字符串转数值
typedef int(*URLA)(char *, int, char *);				//字符串提取关键字
HMODULE funAddr=NULL;									//DLL基址
FARPROC ttc=NULL;										//函数地址
FARPROC cti=NULL;										//函数地址
FARPROC urla=NULL;		


/*自定义全局变量*/
char globalLogMsg[256] = { NULL };						//日志记录存储
long globalFuncStat = 0;								//函数返回状态

HWND hStatic_TargetIp = NULL;							//显示IP
HWND hStatic_targetport = NULL;							//显示PORT
HWND hStatic_WorkMode = NULL;							//显示WORK MODE

HWND hComboBox_TargetIp = NULL;							//edit box句柄='目标ip'
HWND hEditBox_TargetPort = NULL;						//edit box句柄='目标port'
HWND hComboBox_WorkMode = NULL;							//显示WORK MODE
HWND hButton_StartUp = NULL;							//显示START UP
HWND hButton_Close = NULL;								//断开连接
HWND hListBox_Show = NULL;								//显示记录信息

TEXTMETRIC stTextMetric;								//系统字体信息	注：未设置字体信息改变消息以动态修改全局信息WM_MESSAGE

SOCKET stSock = NULL;									//用于通信的套接字
SOCKET stSock_server = NULL;							//服务器套接字
BOOL connectStat = FALSE;								//连接状态


/*自定义函数前置声明*/
int myWinSockStartup(void);								//封装sock相关函数
int myWinSockAccept(u_short, int);						//服务端专用
int myWinSockConnect(u_short, char *);					//客户端专用
int sendFile_t(stSendType);								//发送文件 宽字符版
int sendDir_t(TCHAR *);									//发送文件夹 宽字符版256字节长
int recvFile_t(TCHAR *);								//接收文件 宽字符版
int recvDir_t(TCHAR *);									//发送文件夹 并创建 宽字符版
int getFileName_t(TCHAR *, int, TCHAR *, int *);		//提取文件名
int myWinSockClose(SOCKET);								//结束套接字

int myLoadlibrary(void);								//载入动态链接库
int writeLog(char *, char *);							//日志记录
int drawControl(HWND, UINT, WPARAM, LPARAM);			//绘制控件
int initControl(void);									//控件初始化
int setControlStat(BOOL);								//设置控件状态(禁用/启用)
int myGetSysInfo(HWND);									//获取系统信息

int myDragFile(HWND, UINT, WPARAM, LPARAM);				//处理拖曳文件消息
int myPaintMsg(HWND, UINT, WPARAM, LPARAM);				//处理paint消息
int myUserAdd1(HWND, UINT, WPARAM, LPARAM);				//处理自定义消息
int myDestoryMsg(HWND, UINT, WPARAM, LPARAM);			//处理destory消息

int dealWithCCII(HWND, UINT, WPARAM, LPARAM);			//处理CHILD_COMBOBOX_IP_ID消息
int dealWithCEPI(HWND, UINT, WPARAM, LPARAM);			//处理CHILD_EDIT_PORT_ID消息
int dealWithCCWI(HWND, UINT, WPARAM, LPARAM);			//处理CHILD_COMBOBOX_WM_ID消息
int dealWithCBSI(HWND, UINT, WPARAM, LPARAM);			//处理CHILD_BUTTON_SU_ID消息
int dealWithCBCL(HWND, UINT, WPARAM, LPARAM);			//处理CHILD_BUTTON_CL_ID消息

/*线程相关变量及函数*/
volatile char globalReady = 0;							//同步等待 0:未改动, 1:准备就绪, -1:发生错误 2:文件准备就绪 3:文件夹准备就绪
HANDLE hGlobalMutex = NULL;								//互斥锁句柄

UINT __stdcall socketStartUp(void *);					//socket连接
int useThread(void);									//线程启动
UINT __stdcall recvSendRequest(void *);					//处理收发消息
//UINT __stdcall recvSendFILE(void *);					//收发文件

/*配置文件*/
TCHAR globalStorePath[256] = { NULL };					//文件存储路径
TCHAR globalStorePathDefault[256] = { NULL };			//出错时的默认路径
TCHAR globalStoreDirDefault[256] = { NULL };			//出错时的默认目录
int getFileLine_t(TCHAR *, int, TCHAR *);				//获取指定文件指定行
int compareString_t(TCHAR *, TCHAR *, int);				//比较两个字符串
int getSpeString_t(TCHAR *, TCHAR, int, TCHAR *);		//获取某个字符串中某个字符前或后部所有字符
int saveSuccessHistory(TCHAR *, saveSucHis *);			//保存一次成功连接的记录
int getFileTotalSize(TCHAR *, UINT64 *);				//获取文件大小
int initFileJudge(void);								//处理配置文件

/*文件夹分析*/
typedef TCHAR nameStore[256];							//
nameStore globalAllOfDir_t[16] = { NULL };				//存放所有文件夹名称(带路径) 上限16个
nameStore globalWithoutPath_dir_t[16] = { NULL };		//存放所有文件夹名称(不带路径) 上限16个
int globalDirIndex = 0;									//存储情况
nameStore globalAllOfFile_t[16] = { NULL };				//存放所有文件名称(带路径) 上限16个
nameStore globalWithoutPath_file_t[16] = { NULL };		//存放所有文件名称(不带路径) 上限16个
int globalFileIndex = 0;								//存储情况
volatile int globalFileStat = 0;									//文件状态 0:就绪 1:分析中 2:发送中 3: -1:出错

int anlyDirAndFile(TCHAR *, int);						//遍历文件
int resetGlobal(void);									//重置部分全局变量
UINT __stdcall anlySpeString(void *);					//分析文件内部

// 全局变量: 
HINSTANCE hInst = NULL;									// 当前实例
WCHAR szTitle[MAX_LOADSTRING];							// 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];					// 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此放置代码。
	myLoadlibrary();

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MY814_QQ, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY814_QQ));

	MSG msg;

	// 主消息循环: 
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


/*socket相关函数*/
int myWinSockStartup(void)
{
	int iFuncStat = 0;
	WORD sockVer = MAKEWORD(2, 2);
	WSADATA wsaData;
	TCHAR msg_t[256] = { NULL };

	iFuncStat = WSAStartup(sockVer, &wsaData);
	if (iFuncStat != 0) {
		stSock = NULL;
		_stprintf_s(msg_t, _countof(msg_t), _T("myWinSockStartup, WSAGetLastError:%d"), WSAGetLastError());
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
		setControlStat(TRUE);				//连接失败，恢复按钮
		return WSAGetLastError();
	}
	else {
		stSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == stSock) {
			stSock = NULL;
			_stprintf_s(msg_t, _countof(msg_t), _T("myWinSockStartup, WSAGetLastError:%d"), WSAGetLastError());
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
			setControlStat(TRUE);			//连接失败，恢复按钮
			return WSAGetLastError();
		}
		else {
			;
		}
	}
	
	//int time_out_recv = RECVTIMELIMIT;	//超时时间（毫秒）
	//setsockopt(stSock, SOL_SOCKET, SO_RCVTIMEO, (char *)&time_out_recv, sizeof(int));

	return 0;
}

int myWinSockAccept(u_short port, int timeOut)
{
	/*函数内部变量*/
	int iFuncStat = 0;
	stSock_server = stSock;
	SOCKADDR_IN sAddre;
	TCHAR msg_t[256] = { NULL };
	saveSucHis ssh_t = { NULL };

	/*...*/
	sAddre.sin_family = AF_INET;
	sAddre.sin_port = htons(port);
	sAddre.sin_addr.S_un.S_addr = INADDR_ANY;
	iFuncStat = bind(stSock_server, (LPSOCKADDR)&sAddre, sizeof(sAddre));
	if (SOCKET_ERROR == iFuncStat) {
		_stprintf_s(msg_t, _countof(msg_t), _T("myWinSockAccept, WSAGetLastError:%d"), WSAGetLastError());
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
		setControlStat(TRUE);						//连接失败，恢复按钮
		return WSAGetLastError();
	}
	else {
		iFuncStat = listen(stSock_server, timeOut);
		if (SOCKET_ERROR == iFuncStat) {
			_stprintf_s(msg_t, _countof(msg_t), _T("myWinSockAccept, WSAGetLastError:%d"), WSAGetLastError());
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
			setControlStat(TRUE);					//连接失败，恢复按钮
			return WSAGetLastError();
		}
		else {
			SOCKADDR_IN cAddre;
			int len = sizeof(cAddre);
			//
			_stprintf_s(msg_t, _countof(msg_t), _T("ready to connect"));
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
			//
			stSock = accept(stSock_server, (sockaddr *)&cAddre, &len);
			if (INVALID_SOCKET == stSock) {
				stSock = NULL;
				_stprintf_s(msg_t, _countof(msg_t), _T("myWinSockAccept, WSAGetLastError:%d"), WSAGetLastError());
				SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
				setControlStat(TRUE);				//连接失败，恢复按钮
				return WSAGetLastError();
			}
			else {
				ZeroMemory(msg_t, _countof(msg_t));
				GetWindowText(hComboBox_WorkMode, msg_t, _countof(msg_t));
				_stprintf_s(ssh_t.workmode, _countof(ssh_t.workmode), _T("%s"), msg_t);
				GetWindowText(hComboBox_TargetIp, msg_t, _countof(msg_t));
				_stprintf_s(ssh_t.ip, _countof(ssh_t.ip), _T("%s"), msg_t);
				GetWindowText(hEditBox_TargetPort, msg_t, _countof(msg_t));
				_stprintf_s(ssh_t.port, _countof(ssh_t.port), _T("%s"), msg_t);
				_stprintf_s(ssh_t.storepath, _countof(ssh_t.workmode), _T("%s"), globalStorePath);
				saveSuccessHistory(_T("init.ini"), &ssh_t);
			}
		}
	}

	return 0;
}

int myWinSockConnect(u_short port, char *ip)
{
	/*函数内部变量*/
	int iFuncStat = 0;
	SOCKADDR_IN cAddre;
	in_addr paddr[256];
	TCHAR msg_t[256] = { NULL };
	saveSucHis ssh_t = { NULL };

	inet_pton(AF_INET, ip, paddr);			//主机字节序转网络字节序
	cAddre.sin_family = AF_INET;
	cAddre.sin_port = htons(port);			//
	cAddre.sin_addr.S_un.S_addr = paddr->S_un.S_addr;
	iFuncStat = connect(stSock, (const sockaddr *)&cAddre, sizeof(cAddre));
	if (SOCKET_ERROR == iFuncStat) {
		_stprintf_s(msg_t, _countof(msg_t), _T("myWinSockConnect, WSAGetLastError:%d"), WSAGetLastError());
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
		return WSAGetLastError();
	}
	else {
		//
		_stprintf_s(msg_t, _countof(msg_t), _T("connect success"));
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
		//
		ZeroMemory(msg_t, _countof(msg_t));
		GetWindowText(hComboBox_WorkMode, msg_t, _countof(msg_t));
		_stprintf_s(ssh_t.workmode, _countof(ssh_t.workmode), _T("%s"), msg_t);
		GetWindowText(hComboBox_TargetIp, msg_t, _countof(msg_t));
		_stprintf_s(ssh_t.ip, _countof(ssh_t.ip), _T("%s"), msg_t);
		GetWindowText(hEditBox_TargetPort, msg_t, _countof(msg_t));
		_stprintf_s(ssh_t.port, _countof(ssh_t.port), _T("%s"), msg_t);
		_stprintf_s(ssh_t.storepath, _countof(ssh_t.workmode), _T("%s"), globalStorePath);
		saveSuccessHistory(_T("init.ini"), &ssh_t);
	}

	return 0;
}

int sendFile_t(stSendType in_sst_msg) {
	/*函数内部变量*/
	TCHAR msg_t[256] = { NULL };
	int iFuncStat = 0;
	size_t num = 0;
	UINT64 totalSize = 0;
	UINT64 gethhh = 0;
	UCHAR send_msg[65535] = { NULL };
	char temp = 0;							//文件收发状态码
	size_t realSize = 0;

	/*参数检查*/
	//
	_stprintf_s(msg_t, _countof(msg_t), _T("sendFile_t"));
	SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	//
	if (NULL == in_sst_msg.name || NULL == in_sst_msg.nameWithoutPath) {
		_stprintf_s(msg_t, _countof(msg_t), _T("empty filename in sendFile_t"));
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		return -1;
	}
	//
	getFileTotalSize(in_sst_msg.name, &totalSize);		//获取文件大小
	//
	FILE *fileRead = NULL;
	_tfopen_s(&fileRead, in_sst_msg.name, _T("rb"));
	if (NULL == fileRead) {
		_stprintf_s(msg_t, _countof(msg_t), _T("open file err in sendFile_t"));
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		return -1;
	}

	/*首先发送文件名，长度256，*/
	if (_tcslen(in_sst_msg.nameWithoutPath) >= 256) {
		_stprintf_s(msg_t, _countof(msg_t), _T("filename length more than 256 in sendFile_t"));
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		fclose(fileRead);
		return -1;
	}

	iFuncStat = send(stSock, (char *)in_sst_msg.nameWithoutPath, (int)(_countof(in_sst_msg.nameWithoutPath) * 2), 0);
	if (SOCKET_ERROR == iFuncStat) {
		_stprintf_s(msg_t, _countof(msg_t), _T("send err in sendFile_t,err code:%d"), WSAGetLastError());
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		sprintf_s(globalLogMsg, _countof(globalLogMsg), "send err in sendFile_t,err code:%d", WSAGetLastError());

		fclose(fileRead);
		return -1;
	}
	//考虑是否设置阻塞接收，保证数据传输的可靠性
	iFuncStat = recv(stSock, &temp, 1, 0);				//等待
	if (SOCKET_ERROR == iFuncStat || 0 == temp) {
		fclose(fileRead);
		return -1;
	}

	/*发送文件大小*/
	//totalSize = _filelength(_fileno(fileRead));		//获取文件大小
	iFuncStat = send(stSock, (char *)&totalSize, 8, 0);
	//
	//_stprintf_s(msg_t, _countof(msg_t), _T("_filelength:%llu, getFileTotalSize:%llu"), totalSize, gethhh);
	//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	//
	if (SOCKET_ERROR == iFuncStat) {
		_stprintf_s(msg_t, _countof(msg_t), _T("send err in sendFile_t,err code:%d"), WSAGetLastError());
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		fclose(fileRead);
		return -1;
	}
	iFuncStat = recv(stSock, &temp, 1, 0);				//等待
	if (SOCKET_ERROR == iFuncStat || 0 == temp) {
		fclose(fileRead);
		return -1;
	}

	/*开始发送文件内容*/
	_stprintf_s(msg_t, _countof(msg_t), _T("totalSize:%llu"), totalSize);
	SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	while (1) {
		num = fread(send_msg, sizeof(char), _countof(send_msg), fileRead);
		if (ferror(fileRead) != 0) {
			_stprintf_s(msg_t, _countof(msg_t), _T("read err in sendFile_t"));
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
			fclose(fileRead);
			return -1;
		}
		iFuncStat = send(stSock, (char *)send_msg, (int)num, 0);
		realSize += iFuncStat;
		if (SOCKET_ERROR == iFuncStat) {				//socket出现问题，错误无法反馈给对方，直接关闭
			_stprintf_s(msg_t, _countof(msg_t), _T("send err in sendFile_t"));
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
			fclose(fileRead);
			return -1;
		}
		iFuncStat = recv(stSock, &temp, 1, 0);			//发送工作已完成，等待对方响应，以确定是否继续
		if (SOCKET_ERROR == iFuncStat || 0 == temp) {
			_stprintf_s(msg_t, _countof(msg_t), _T("recv err in sendFile_t"));
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
			fclose(fileRead);
			return -1;
		}
		if (totalSize == realSize) {					//读取的字节数已达标，退出
			break;
		}
	}
	//
	_stprintf_s(msg_t, _countof(msg_t), _T("send finish"));
	SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
	//
	fclose(fileRead);
	return 0;
}

int sendDir_t(TCHAR *in_dir) {
	/*函数内部变量*/
	TCHAR msg_t[256] = { NULL };
	int iFuncStat = 0;
	int iFuncLen = 0;
	char recvType = 0;

	/*...*/
	//
	_stprintf_s(msg_t, _countof(msg_t), _T("sendDir_t"));
	SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	//
	iFuncStat = send(stSock, (char *)in_dir, 256 * 2, 0);
	if (SOCKET_ERROR == iFuncStat) {
		_stprintf_s(msg_t, _countof(msg_t), _T("send err in sendDir_t:%d, %s"), WSAGetLastError(), in_dir);
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);

		return -1;
	}
	iFuncStat = recv(stSock, &recvType, 1, 0);
	if (SOCKET_ERROR == iFuncStat || recvType != 1) {
		_stprintf_s(msg_t, _countof(msg_t), _T("recv err in sendDir_t:%d, %s"), WSAGetLastError(), in_dir);
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);

		return -1;
	}
	//
	_stprintf_s(msg_t, _countof(msg_t), _T("send finish"));
	SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	//
	return 0;
}

int recvFile_t(TCHAR *out_info) {
	/*函数内部变量*/
	INT32 recvLength = 0;
	UINT64 totalLength = 0;
	TCHAR msg_t[256] = { NULL };					//调试信息
	int iFuncStat = 0;								//函数返回值
	TCHAR filename[256] = { NULL };					//文件
	UINT64 fileSize = 0;								//总字节数
	FILE *fileWrite = NULL;							//写文件
	UCHAR recvFile[65535] = { NULL };				//接收字节用
	TCHAR fileNameAddPath[512] = { NULL };			//拼接路径

	/*参数检查*/
	//
	_stprintf_s(msg_t, _countof(msg_t), _T("recvFile_t"));
	SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	//

	/*接收文件名*/
	iFuncStat = recv(stSock, (char *)filename, (int)(_countof(filename) * 2), 0);
	if (SOCKET_ERROR == iFuncStat) {
		_stprintf_s(msg_t, _countof(msg_t), _T("recv err in recvFile_t,err code:%d"), WSAGetLastError());
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		send(stSock, "0", 1, 0);							//发送异常通知,终止通信
		return -1;
	}
	send(stSock, "1", 1, 0);								//接受完毕确认

	/*创建文件*/
	_stprintf_s(fileNameAddPath, _countof(fileNameAddPath), _T("%s%s"), globalStorePath, filename);		//拼接路径加文件名
	iFuncStat = _tfopen_s(&fileWrite, fileNameAddPath, _T("wb"));
	if (0 == iFuncStat) {
		;
	}
	else {
		_stprintf_s(msg_t, _countof(msg_t), _T("create file err in recvFile_t!error code:%d"), errno);
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		send(stSock, "0", 1, 0);							//发送异常通知终止通信
		return -1;
	}

	/*接收文件大小*/
	iFuncStat = recv(stSock, (char *)&fileSize, 8, 0);
	if (SOCKET_ERROR == iFuncStat) {
		_stprintf_s(msg_t, _countof(msg_t), _T("recv err in recvFile_t,err code:%d"), WSAGetLastError());
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		send(stSock, "0", 1, 0);							//发送异常通知,终止通信
		fclose(fileWrite);
		return -1;
	}
	//
	//_stprintf_s(msg_t, _countof(msg_t), _T("recvsize: %llu"), fileSize);
	//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	//
	send(stSock, "1", 1, 0);								//接受完毕确认

	/*开始接收文件*/
	while (1) {
		recvLength = recv(stSock, (char *)recvFile, _countof(recvFile), 0);
		iFuncStat = recvLength;
		if (SOCKET_ERROR == iFuncStat) {		//socket发生了错误，错误信息也无法反馈给对方了，直接退出
			_stprintf_s(msg_t, _countof(msg_t), _T("recv err in recvFile_t"));
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
			fclose(fileWrite);
			return -1;
		}
		else {
			totalLength += recvLength;			//接收长度统计
		}
		iFuncStat = (int)fwrite(recvFile, sizeof(char), recvLength, fileWrite);
		if (iFuncStat != recvLength) {			//实际写入数字与预期不一致，返回错误码
			_stprintf_s(msg_t, _countof(msg_t), _T("fwrite err in recvFile_t"));
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
			send(stSock, "0", 1, 0);
			fclose(fileWrite);
			return -1;
		}
		send(stSock, "1", 1, 0);								//接收成功，已准备好进行下一步，反馈给对方一个确认码
		if (totalLength == fileSize) {
			_stprintf_s(msg_t, _countof(msg_t), _T("totalsize:%llu"), totalLength);
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
			
			break;
		}
	}
	//
	_stprintf_s(msg_t, _countof(msg_t), _T("recv finish"));
	SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	fclose(fileWrite);
	//
	return 0;
}

int recvDir_t(TCHAR *in_dir) {
	/*函数内部变量*/
	TCHAR dirName[1024] = { NULL };
	TCHAR msg_t[256] = { NULL };
	TCHAR dirNameAddPath[512] = { NULL };
	int iFuncStat = 0;
	char recvType = 0;				//通信状态通知码

	/*...*/
	//
	_stprintf_s(msg_t, _countof(msg_t), _T("recvDir_t"));
	SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	//
	iFuncStat = recv(stSock, (char *)dirName, 256 * 2, 0);
	if (SOCKET_ERROR == iFuncStat) {
		_stprintf_s(msg_t, _countof(msg_t), _T("err in recvDir_t:%d"), WSAGetLastError());
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);

		recvType = 0;
		send(stSock, &recvType, 1, 0);
		return - 1;
	}
	_stprintf_s(dirNameAddPath, _countof(dirNameAddPath), _T("%s%s"), globalStorePath, dirName); //拼接路径加文件夹名
	iFuncStat = CreateDirectory(dirNameAddPath, NULL);		//创建文件夹
	if (0 == iFuncStat) {
		if (ERROR_ALREADY_EXISTS != GetLastError()) {
			_stprintf_s(msg_t, _countof(msg_t), _T("createdir err in recvDir_t:%lu"), GetLastError());
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);

			recvType = 0;
			send(stSock, &recvType, 1, 0);
			return -1;
		}
	}

	recvType = 1;
	send(stSock, &recvType, 1, 0);
	//
	_stprintf_s(msg_t, _countof(msg_t), _T("recv finish"));
	SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	//
	return 0;
}

int getFileName_t(TCHAR *in_ch, int in_i, TCHAR *out_ch, int *out_i) {
	int index = 0;
	*out_i = 0;

	for (int i = 0; i < in_i; ++i) {
		if ('/' == in_ch[i] || '\\' == in_ch[i]) {
			index = i;
		}
	}
	if (0 == index) {
		out_ch = NULL;
		*out_i = 0;
	}
	for (int i = index + 1; i < in_i; ++i) {
		out_ch[i - index - 1] = in_ch[i];
		*out_i += 1;
	}
	out_ch[*out_i] = '\0';

	return 0;
}

int myWinSockClose(SOCKET stSock)
{
	if (NULL == stSock) {
		return 0;
	}
	char recv_msg[8];
	shutdown(stSock, SD_SEND);
	recv(stSock, recv_msg, 1, 0);
	closesocket(stSock);
	WSACleanup();
	stSock = NULL;

	setControlStat(TRUE);					//设置按钮
	EnableWindow(hButton_Close, FALSE);		//设置按钮
	return 0;
}

/*...*/
int myLoadlibrary(void)
{
	TCHAR msg_t[256] = { NULL };

	/*动态链接库*/
	funAddr = LoadLibrary(_T("7-20-anlysis_file_name"));		//freelibrary
	if (NULL == funAddr) {
		_stprintf_s(msg_t, _countof(msg_t), _T("myLoadlibrary, err"));
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
		exit(1);
	}
	ttc = GetProcAddress(funAddr, "TcharToChar");				//载入‘宽字符转字符’
	cti = GetProcAddress(funAddr, "charToInt");					//载入‘字符转数值’
	//urla = GetProcAddress(funAddr, "URL_anly");					//载入‘字符串提取关键字’
	if (NULL == ttc || NULL == cti) {
		_stprintf_s(msg_t, _countof(msg_t), _T("myLoadlibrary, err"));
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
		FreeLibrary(funAddr);
		funAddr = NULL;
		exit(1);
	}

	return 0;
}

int writeLog(char * filename, char * logmsg)
{
	if (NULL == filename || NULL == logmsg) {
		return -1;
	}
	time_t time_now = 0;
	char szTime_now[256];
	time(&time_now);
	ctime_s(szTime_now, _countof(szTime_now), &time_now);
	FILE *logfile;
	fopen_s(&logfile, filename, "at");
	fwrite(szTime_now, sizeof(char), strlen(szTime_now), logfile);		//日志时间
	fwrite(logmsg, sizeof(char), strlen(logmsg), logfile);				//日志信息
	fwrite("\n", sizeof(char), 1, logfile);								//结束标志
	fclose(logfile);

	return 0;
}

int drawControl(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	/*创建*/
	hComboBox_TargetIp = CreateWindow(_T("combobox"), CHILD_COMBOBOX_IP_TEXT, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | CBS_DROPDOWN,
		(int)CHILD_COMBOBOX_IP_COOR_X, (int)CHILD_COMBOBOX_IP_COOR_Y, (int)CHILD_COMBOBOX_IP_HOR, (int)CHILD_COMBOBOX_IP_VER,
		hWnd, (HMENU)(int)CHILD_COMBOBOX_IP_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	/*创建*/
	hEditBox_TargetPort = CreateWindow(_T("edit"), CHILD_EDIT_PORT_TEXT, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		(int)CHILD_EDIT_PORT_COOR_X, (int)CHILD_EDIT_PORT_COOR_Y, (int)CHILD_EDIT_PORT_HOR, (int)CHILD_EDIT_PORT_VER,
		hWnd, (HMENU)(int)CHILD_EDIT_PORT_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	/*创建*/
	hStatic_TargetIp = CreateWindow(_T("static"), CHILD_STATIC_PORT_TEXT, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_READONLY,
		(int)CHILD_STATIC_PORT_COOR_X, (int)CHILD_STATIC_PORT_COOR_Y, (int)CHILD_STATIC_PORT_HOR, (int)CHILD_STATIC_PORT_VER,
		hWnd, (HMENU)(int)CHILD_STATIC_PORT_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	/*创建*/
	hStatic_targetport = CreateWindow(_T("static"), CHILD_STATIC_IP_TEXT, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_READONLY,
		(int)CHILD_STATIC_IP_COOR_X, (int)CHILD_STATIC_IP_COOR_Y, (int)CHILD_STATIC_IP_HOR, (int)CHILD_STATIC_IP_VER,
		hWnd, (HMENU)(int)CHILD_STATIC_IP_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	/*创建*/
	hStatic_WorkMode = CreateWindow(_T("static"), CHILD_STATIC_WM_TEXT, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_READONLY,
		(int)CHILD_STATIC_WM_COOR_X, (int)CHILD_STATIC_WM_COOR_Y, (int)CHILD_STATIC_WM_HOR, (int)CHILD_STATIC_WM_VER,
		hWnd, (HMENU)(int)CHILD_STATIC_WM_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	/*创建*/
	hComboBox_WorkMode = CreateWindow(_T("combobox"), CHILD_COMBOBOX_WM_TEXT, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | CBS_DROPDOWN,
		(int)CHILD_COMBOBOX_WM_COOR_X, (int)CHILD_COMBOBOX_WM_COOR_Y, (int)CHILD_COMBOBOX_WM_HOR, (int)CHILD_COMBOBOX_WM_VER,
		hWnd, (HMENU)(int)CHILD_COMBOBOX_WM_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	/*创建*/
	hButton_StartUp = CreateWindow(_T("button"), CHILD_BUTTON_SU_TEXT, WS_CHILD | WS_VISIBLE | ES_CENTER,
		(int)CHILD_BUTTON_SU_COOR_X, (int)CHILD_BUTTON_SU_COOR_Y, (int)CHILD_BUTTON_SU_HOR, (int)CHILD_BUTTON_SU_VER,
		hWnd, (HMENU)(int)CHILD_BUTTON_SU_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	/*创建*/
	hListBox_Show = CreateWindow(_T("listbox"), CHILD_LISTBOX_SHOW_TEXT, WS_CHILD | WS_VISIBLE | ES_CENTER | WS_VSCROLL | ES_READONLY |
		LBS_MULTICOLUMN  | LBS_DISABLENOSCROLL,
		(int)CHILD_LISTBOX_SHOW_COOR_X, (int)CHILD_LISTBOX_SHOW_COOR_Y, (int)CHILD_LISTBOX_SHOW_HOR, (int)CHILD_LISTBOX_SHOW_VER,
		hWnd, (HMENU)(int)CHILD_LISTBOX_SHOW_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	/*创建*/
	hButton_Close = CreateWindow(_T("button"), CHILD_BUTTON_CL_TEXT, WS_CHILD | WS_VISIBLE | ES_CENTER,
		(int)CHILD_BUTTON_CL_COOR_X, (int)CHILD_BUTTON_CL_COOR_Y, (int)CHILD_BUTTON_CL_HOR, (int)CHILD_BUTTON_CL_VER,
		hWnd, (HMENU)(int)CHILD_BUTTON_CL_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	return 0;
}

int initControl(void)
{
	/*限制输入字符个数*/
	SendMessage(hComboBox_TargetIp, CB_LIMITTEXT, 15, 0);
	SendMessage(hEditBox_TargetPort, EM_LIMITTEXT, 5, 0);

	/*为下拉框添加选项*/
	SendMessage(hComboBox_WorkMode, CB_ADDSTRING, 0, (LPARAM)(_T("SERVER")));
	SendMessage(hComboBox_WorkMode, CB_ADDSTRING, 0, (LPARAM)(_T("CLIENT")));

	/*初始化信息*/
	SendMessage(hListBox_Show, LB_SETCOLUMNWIDTH, (int)CHILD_LISTBOX_SHOW_HOR, 0);
	SendMessage(hListBox_Show, LB_ADDSTRING, 0, (LPARAM)(_T("show results here!")));

	/*禁用按钮*/
	EnableWindow(hButton_Close, FALSE);

	/*控件初始值*/
	//SetWindowText(hComboBox_TargetIp, _T("192.168.188.117"));
	//SetWindowText(hEditBox_TargetPort, _T("12345"));
	return 0;
}

int setControlStat(BOOL bStat) {
	if (TRUE == bStat) {
		EnableWindow(hEditBox_TargetPort, TRUE);
		EnableWindow(hComboBox_TargetIp, TRUE);
		EnableWindow(hComboBox_WorkMode, TRUE);
		EnableWindow(hButton_StartUp, TRUE);
	}
	else {
		EnableWindow(hEditBox_TargetPort, FALSE);
		EnableWindow(hComboBox_TargetIp, FALSE);
		EnableWindow(hComboBox_WorkMode, FALSE);
		EnableWindow(hButton_StartUp, FALSE);
	}

	return 0;
}

int myGetSysInfo(HWND hWnd)
{
	GetTextMetrics(GetDC(hWnd), &stTextMetric);

	return 0;
}

/*消息处理相关函数*/
int myDragFile(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	/*函数内部变量*/
	TCHAR msg_t[256] = { NULL };			//调试信息
	int iFuncStat = 0;						//函数状态
	HANDLE hThread = NULL;					//线程句柄
	UINT32 id = 0;							//线程id
	BOOL bButtonStat = FALSE;				//按钮状态

	/*...*/
	if (FALSE == connectStat) {
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(_T("Please click 'start up'!")));
		return 0;
	}
	//
	_stprintf_s(msg_t, _countof(msg_t), _T("myDragFile"));
	//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	//
	//稍后加个判断 是否还有其他的线程实例在运行
	if (0 != globalFileStat) {
		_stprintf_s(msg_t, _countof(msg_t), _T("sending already, stat:%d"), globalFileStat);
		//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		return -1;
	}
	/*启动线程分析文件*/
	globalFileStat = 1;				//设置状态为分析中
	hThread = (HANDLE)_beginthreadex(NULL, 0, &anlySpeString, (void *)wParam, 1, &id);
	if (NULL == hThread) {
		_stprintf_s(msg_t, _countof(msg_t), _T("anlySpeString"));
		//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		return -1;
	}
	CloseHandle(hThread);
	return 0;
}

int myPaintMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

int myUserAdd1(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	return 0;
}

int myDestoryMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	FreeLibrary(funAddr);					//关闭动态链接库
	myWinSockClose(stSock);					//关闭socket
	myWinSockClose(stSock_server);
	return 0;
}

int dealWithCCII(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

int dealWithCEPI(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

int dealWithCCWI(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

int dealWithCBSI(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	/*函数内部变量*/
	HANDLE hThread = NULL;
	int iFuncStat = 0;
	UINT32 id = 0;
	TCHAR msg_t[256] = { NULL };

	/*...*/
	setControlStat(FALSE);				//禁用控件，等待返回

	hThread = (HANDLE)_beginthreadex(NULL, 0, &socketStartUp, NULL, 1, &id);
	if (NULL == hThread) {
		_stprintf_s(msg_t, _countof(msg_t), _T("_beginthreadex err in dealWithCBSI"));
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		return -1;
	}
	CloseHandle(hThread);
	
	return 0;
}

int dealWithCBCL(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	/*函数内部变量*/
	char msgType = 0;

	/*...*/
	msgType = CLOSEWILL;
	send(stSock, &msgType, 1, 0);

	myWinSockClose(stSock);
	myWinSockClose(stSock_server);
	connectStat = FALSE;			//设置连接状态

	return 0;
}

/*线程相关函数*/
UINT __stdcall socketStartUp(void *argv) 
{
	//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(_T("Start up")));

	/*函数内部变量*/
	int iFuncStat = 0;						//函数状态
	TCHAR tcWorkMode_msg[256] = { NULL };	//存储combobox text属性值
	TCHAR szIp[32];							//ip
	char szIp_c[32];						//转ansi
	TCHAR szPort[8];						//port
	char szPort_c[8];						//转ansi
	int iPort[1];							//转数值
	iPort[0] = 0;

	/*...*/
	iFuncStat = GetWindowText(hComboBox_TargetIp, szIp, _countof(szIp));
	if (0 == iFuncStat) {
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(_T("Please type ip")));
		setControlStat(TRUE);
		return -1;
	}
	iFuncStat = GetWindowText(hEditBox_TargetPort, szPort, _countof(szPort));
	if (0 == iFuncStat) {
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(_T("Please type port")));
		setControlStat(TRUE);
		return -1;
	}

	/*数据调整*/
	((TTC)ttc)(szIp, _tcslen(szIp), szIp_c);
	((TTC)ttc)(szPort, _tcslen(szIp), szPort_c);
	((CTI)cti)(szPort_c, strlen(szPort_c), iPort);

	/*检查工作模式*/
	iFuncStat = GetWindowText(hComboBox_WorkMode, tcWorkMode_msg, _countof(tcWorkMode_msg));		//获取combobox的值
	if (0 == iFuncStat) {
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(_T("Please chooice work mode!")));
		setControlStat(TRUE);
		return -1;
	}
	else {
		/*启动socket*/
		iFuncStat = myWinSockStartup();
		//判断
		switch (tcWorkMode_msg[0]) {
		case 'S': {
			iFuncStat = myWinSockAccept(iPort[0], 5);
			if (iFuncStat != 0) {
				TCHAR msg[256];
				_stprintf_s(msg, _countof(msg), _T("accept getlasterr :%d"), iFuncStat);
				SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg));
				break;
			}
			else {
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(_T("accept one connect msg")));
				connectStat = TRUE;					//设置连接状态
				EnableWindow(hButton_Close, TRUE);

				useThread();						//启动监听线程
			}
		}break;
		case 'C': {
			iFuncStat = myWinSockConnect(iPort[0], szIp_c);
			if (iFuncStat != 0) {
				TCHAR msg[256];
				_stprintf_s(msg, _countof(msg), _T("connect getlasterr :%d"), iFuncStat);
				SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg));
				setControlStat(TRUE);
				break;
			}
			else {
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(_T("connect success")));
				connectStat = TRUE;					//设置连接状态
				EnableWindow(hButton_Close, TRUE);

				useThread();						//启动监听线程
			}
		}break;
		default: {
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(_T("err in work mode!")));
		}
		}
	}

	_endthreadex(0);
	return 0;
}

/*
 *useThread()这是一个常规函数
 *功能：用来启动recvSendRequest()线程
 */
int useThread(void) {
	/*函数内部变量*/
	UINT32 id = 0;
	HANDLE hThread = NULL;
	TCHAR msg_t[256] = { NULL };

	/*...*/
	//
	_stprintf_s(msg_t, _countof(msg_t), _T("useThread"));
	SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	//
	hThread =(HANDLE) _beginthreadex(NULL, 0, &recvSendRequest, NULL, 0, &id);	//创建线程

	if (NULL == hThread) {
		_stprintf_s(msg_t, _countof(msg_t), _T("beginthreadex err in useThread"));
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		return -1;
	}
	CloseHandle(hThread);											//显式关闭线程句柄

	return 0;
}

/*
 *recvSendRequest()这是一个线程函数
 *功能：用于基本的事件监听·判断
 *说明：在socket连接成功后会启动此线程，对收·发·关闭事件进行监听
 *		对请求系列事件进行接收后会返回一个允许通知给此连接的另一方
 *		对允许系列事件进行接收后会退出自身线程，并设置全局变量，通知其他线程
 *		若未进行其他socket收发操作，此线程必须运行
 *		在进行其他socket收发操作时，此线程必须退出，且在其他线程不工作时激活此线程
 */
UINT __stdcall recvSendRequest(void *argv) {
	/*参数检查*/

	/*函数内部变量*/
	char msgType = 0;
	int recvStat = 0;
	char msg[256] = { NULL };
	TCHAR msg_t[256] = { NULL };

	/*...*/
	while (1) {
		recvStat = recv(stSock, &msgType, 1, 0);
		if (SOCKET_ERROR == recvStat) {
			_stprintf_s(msg_t, _countof(msg_t), _T("SOCKET_ERROR in recvSendRequest:%d"), WSAGetLastError());
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
			_endthreadex(0);
			return 0;
		}
		else {
			switch (msgType)						//分析请求
			{
			case SENDFILEREQUEST: {					//接收确认
				_stprintf_s(msg_t, _countof(msg_t), _T("recv SENDFILEREQUEST in recvSendRequest"));
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
				msgType = SENDFILEALLOW;
				send(stSock, &msgType, 1, 0);		//通知对方，本地接收就绪
				EnableWindow(hButton_Close, FALSE);
				recvFile_t(msg_t);					//开始接收
				EnableWindow(hButton_Close, TRUE);

				continue;
			}break;
			case SENDFILEALLOW: {					//对方已确认
				//...
				_stprintf_s(msg_t, _countof(msg_t), _T("recv SENDFILEALLOW in recvSendRequest"));
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
				globalReady = 2;

				_endthreadex(0);					//终止该线程，让出网络资源，稍后会由其他线程再次开启此线程
				return 0;
			}break;
			case SENDDIRREQUEST: {					//接收确认
				_stprintf_s(msg_t, _countof(msg_t), _T("recv SENDDIRREQUEST in recvSendRequest"));
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
				msgType = SENDDIRALLOW;
				send(stSock, &msgType, 1, 0);		//通知对方，本地接收就绪
				EnableWindow(hButton_Close, FALSE);
				recvDir_t(msg_t);					//开始接收
				EnableWindow(hButton_Close, TRUE);

				continue;
			}break;
			case SENDDIRALLOW: {					//对方已确认
				//...
				_stprintf_s(msg_t, _countof(msg_t), _T("recv SENDDIRALLOW in recvSendRequest"));
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
				globalReady = 3;

				_endthreadex(0);					//终止该线程，让出网络资源，稍后会由其他线程再次开启此线程
				return 0;
			}break;
			case CLOSEWILL: {						//准备关闭
				_stprintf_s(msg_t, _countof(msg_t), _T("recv CLOSEWILL in recvSendRequest"));
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
				msgType = CLOSEALLOW;
				send(stSock, &msgType, 1, 0);
				myWinSockClose(stSock);

				_endthreadex(0);
				return 0;
			}break;
			case CLOSEALLOW: {						//对方已确认
				_stprintf_s(msg_t, _countof(msg_t), _T("recv CLOSEALLOW in recvSendRequest"));
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
				myWinSockClose(stSock);

				_endthreadex(0);
				return 0;
			}break;
			case ERRLEVEL1: {
				_stprintf_s(msg_t, _countof(msg_t), _T("ERRLEVEL1 in recvSendRequest:%d"), WSAGetLastError());
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
				_endthreadex(0);
				return 0;
			}break;
			case ERRLEVEL2: {
				_stprintf_s(msg_t, _countof(msg_t), _T("ERRLEVEL2 in recvSendRequest:%d"), WSAGetLastError());
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
				_endthreadex(0);
				return 0;
			}break;
			case ERRLEVEL3: {
				sprintf_s(msg, _countof(msg), "ERRLEVEL3 in recvSendRequest:%d", WSAGetLastError());
				_stprintf_s(msg_t, _countof(msg_t), _T("ERRLEVEL3 in recvSendRequest:%d"), WSAGetLastError());
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
				_endthreadex(0);
				return 0;
			}break;
			case 0: {
				_stprintf_s(msg_t, _countof(msg_t), _T("case 0"));
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
				continue;
			}
			default: {
				_stprintf_s(msg_t, _countof(msg_t), _T("recv unknow in recvSendRequest:%d"), msgType);
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
				msgType = ERRLEVEL1;
				Sleep(10000);
				send(stSock, &msgType, 1, 0);
				msgType = 0;
			}break;
			}
		}
	}
	_endthreadex(0);
	return 0;
}

/*配置文件管理*/
int getFileLine_t(TCHAR *inTchar, int lineIndex, TCHAR *outTchar) {
	/*函数内部变量*/
	FILE *fileRead = NULL;
	TCHAR fr_t = 0;
	TCHAR msg_t[256] = { NULL };

	/*参数检查*/
	if (NULL == outTchar) {
		return -1;
	}

	/*...*/
	*outTchar = NULL;
	_tfopen_s(&fileRead, inTchar, _T("rt"));
	if (NULL == fileRead) {

		return -1;
	}
	fread(&fr_t, sizeof(char), 1, fileRead);
	for (int j = 0, k=0;;) {
		if (feof(fileRead) != 0) {		//读到文件尾时，有2种情况，1：成功找到指定行，2：未找到指定行
			if (0 == k) {
				outTchar[0] = '\0';
				fclose(fileRead);
				return -1;
			}
			outTchar[k - 1] = '\0';
			break;
		}
		if (lineIndex - 1 == j) {		//找到指定行
			outTchar[k] = fr_t;
			k += 1;
		}
		if ('\n' == fr_t) {				//超过指定行
			j += 1;
		}
		if (j == lineIndex) {			//超过指定行
			outTchar[k - 1] = '\0';
			break;
		}
		fread(&fr_t, sizeof(char), 1, fileRead);
	}

	fclose(fileRead);
	return 0;
}

/*
 *compareString_t()
 *功能：比较两个字符串
 *说明：此函数处理宽字符串，若实参为NULL，返回-1
 *		mode参数是匹配条件0x11111111,0x1是全字匹配，0x10是不区分大小写，其他暂未定义
 *		不相等返回1，相等返回0，错误返回-1
 */
int compareString_t(TCHAR *inTcharA, TCHAR *inTcharB, int mode) {
	/*函数内部变量*/
	TCHAR msg_t[256] = { NULL };
	int iFuncStat = 0;

	/*参数检查*/
	if (NULL == inTcharA || NULL == inTcharB) {
		return -1;
	}
	/*...*/
	//case mode & 0x1
	if (_tcslen(inTcharA) != _tcslen(inTcharB)) {
		return 1;
	}
	for (int i = 0; i < (int)_tcslen(inTcharA); ++i) {
		if (inTcharA[i] != inTcharB[i]) {
			return 1;
		}
	}

	return 0;
}

/*
 *getSpeString_t()
 *功能：获取字符串中指定字符前或后部所有字符
 *说明：inTcharA,inTcharB,mode均是输入参数。outTcharA是输出参数
 *		mode:1表示后，-1表示前
 *		失败返回-1；成功返回0;返回的字符串以'\0'结尾
 *
 *2018-9-3
 *新增：用指定字符串替换指定字符前或后部所有字符
 *		mode：2表示替换后，-2表示替换前
 *		outTcharA存放重新组合后的字符串
 *		outTcharA数组的大小需要大于(inTcharA字符长度和outTcharA实际字符数之和)
 */
int getSpeString_t(TCHAR *inTcharA, TCHAR inTcharB, int mode, TCHAR *outTcharA) {
	/*函数内部变量*/
	TCHAR msg_t[256] = { NULL };
	int index = -1;
	TCHAR temp_t[256] = { NULL };

	/*参数检查*/
	if (NULL == inTcharA || 0 == inTcharB) {
		return -1;
	}

	/*...*/
	for (int i = 0; inTcharA[i] != '\0'; ++i) {		//查找出指定字符位置
		if (inTcharA[i] == inTcharB) {
			index = i;
			break;
		}
	}
	if (-1 == index) {								//未找到指定字符位置
		if (-1 == mode || 1 == mode) {
			outTcharA[0] = '\0';
		}
		return -1;
	}
	switch (mode)									//判断工作模式
	{
	case 1: {										//提取后部所有字符
		for (int i = index + 1;; ++i) {
			if (inTcharA[i] == '\0') {
				outTcharA[i - index - 1] = '\0';
				break;
			}
			outTcharA[i - index - 1] = inTcharA[i];
		}
	}break;
	case -1: {										//提取前部所有字符
		for (int i = 0; i < index; ++i) {
			outTcharA[i] = inTcharA[i];
		}
		outTcharA[index] = '\0';
	}break;
	case 2: {										//替换后部所有字符
		for (int i = index;; ++i) {
			if ('\0' == outTcharA[i - index]) {
				inTcharA[i] = '\0';
				break;
			}
			inTcharA[i] = outTcharA[i - index];
		}
	}break;
	case -2: {										//替换前部所有字符
		for (size_t i = 0;outTcharA[i]!='\0'; ++i) {
			temp_t[i] = outTcharA[i];
		}
		for (size_t i = _tcslen(outTcharA), j=index;; ++i, ++j) {
			if ('\0' == inTcharA[j]) {
				temp_t[i] = '\0';
				break;
			}
			temp_t[i] = inTcharA[j];
		}
		_stprintf_s(outTcharA, 256, _T("%s"), temp_t);
	}break;
	default:
		return -1;
		break;
	}

	return 0;
}

/*
 *
 *功能：保存工作模式，ip，端口至指定文件
 *说明：
 */
int saveSuccessHistory(TCHAR *fileName, saveSucHis *inSSH) {
	/*函数内部变量*/
	TCHAR msg_t[256] = { NULL };
	FILE *fileInit = NULL;
	char msg_a[256] = { NULL };
	char workmode[256] = { NULL };
	char ip[256] = { NULL };
	char port[256] = { NULL };
	char storepath[256] = { NULL };

	/*...*/
	_tfopen_s(&fileInit, fileName, _T("wt"));
	if (NULL == fileInit) {
		_stprintf_s(msg_t, _countof(msg_t), _T("err in saveSuccessHistory"));
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
		return -1;
	}
	((TTC)ttc)(inSSH->workmode, _tcslen(inSSH->workmode), workmode);
	((TTC)ttc)(inSSH->ip, _tcslen(inSSH->ip), ip);
	((TTC)ttc)(inSSH->port, _tcslen(inSSH->port), port);
	((TTC)ttc)(inSSH->storepath, _tcslen(inSSH->storepath), storepath);
	sprintf_s(msg_a, _countof(msg_a), "[WORKMODE]\nworkmode=%s\n[IP]\nip=%s\n[PORT]\nport=%s\n[STOREPATH]\npath=%s\n", workmode, ip, port, storepath);
	fwrite(msg_a, sizeof(char), strlen(msg_a), fileInit);
	fclose(fileInit);

	return 0;
}

int getFileTotalSize(TCHAR *inTcharA, UINT64 *outLen) {
	/*函数内部变量*/
	TCHAR msg_t[256];
	FILE *fileRead = NULL;
	char readString[102400] = { NULL };
	size_t readSize = 0;
	UINT64 totalSize = 0;

	/*...*/
	_tfopen_s(&fileRead, inTcharA, _T("rb"));
	if (NULL == fileRead) {
		*outLen = 0;
		//
		_stprintf_s(msg_t, _countof(msg_t), _T("openfile err in getFileTotalSize"));
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
		//
		return -1;
	}
	while (1) {
		readSize = fread(readString, sizeof(char), _countof(readString), fileRead);
		if (0 == readSize) {
			if (ferror(fileRead)) {
				*outLen = 0;
				fclose(fileRead);
				return -1;
			}
			if (feof(fileRead)) {
				*outLen = totalSize;
				fclose(fileRead);
				return 0;
			}
		}
		else {
			totalSize += readSize;
			if (readSize < _countof(readString)) {
				*outLen = totalSize;
				break;
			}
		}
	}

	fclose(fileRead);
	return 0;
}

/*
 *initFileJudge（）
 *功能：建立默认的初始化文件或读取已存在的初始化文件
 *说明：
 */
int initFileJudge(void) {
	/*函数内部变量*/
	FILE *FileInit = NULL;
	int iFuncStat = NULL;
	TCHAR msg_t[256] = { NULL };
	TCHAR temp_t[256] = { NULL };

	/*...*/
	CreateDirectory(_T("fileStore"), NULL);
	_stprintf_s(globalStoreDirDefault, _countof(globalStoreDirDefault), _T("fileStore"));						//建立默认路径
	_stprintf_s(globalStorePathDefault, _countof(globalStorePathDefault), _T("%s\\"), globalStoreDirDefault);	//建立默认路径
	iFuncStat = _tfopen_s(&FileInit, _T("init.ini"), _T("rt"));
	if (NULL == FileInit) {			//判断配置文件是否存在
		iFuncStat = _tfopen_s(&FileInit, _T("init.ini"), _T("wt"));	//若不存在则创建一个
		if (NULL == FileInit) {
			_stprintf_s(msg_t, _countof(msg_t), _T("err in initFileJudge:%d"), iFuncStat);
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
			return -1;
		}
		fwrite("[WORKMODE]\nworkmode=SERVER\n[IP]\nip=192.168.188.117\n[PORT]\nport=12345\n[STOREPATH]\npath=fileStore\\\n", sizeof(char), 98, FileInit);
		fclose(FileInit);

		//均采用默认配置，然后退出
		_stprintf_s(globalStorePath, _countof(globalStorePath), _T("fileStore\\"));			//文件存放路径

		return 0;
	}
	else {
		fclose(FileInit);
	}

	//处理文本...
	getFileLine_t(_T("init.ini"), 1, msg_t);
	for (int i = 0;; ++i) {
		getFileLine_t(_T("init.ini"), i+1, msg_t);
		if ('\0' == msg_t[0]) {
			break;
		}
		//处理存储路径
		if (0 == compareString_t(_T("[STOREPATH]"), msg_t, 1)) {
			getFileLine_t(_T("init.ini"), i + 2, msg_t);
			if ('\0' == msg_t[0]) {
				break;
			}
			getSpeString_t(msg_t, '=', -1, temp_t);					//提取等号前部分字符串
			if (0 == compareString_t(_T("path"), temp_t, 1)) {
				i += 1;
				getSpeString_t(msg_t, '=', 1, temp_t);				//提取等号后部分字符串
				_stprintf_s(globalStorePath, _countof(globalStorePath), _T("%s"), temp_t);	//全局变量globalStorePath赋值
			}
		}
		//处理端口
		else if (0 == compareString_t(_T("[PORT]"), msg_t, 1)) {
			getFileLine_t(_T("init.ini"), i + 2, msg_t);
			if ('\0' == msg_t[0]) {
				break;
			}
			getSpeString_t(msg_t, '=', -1, temp_t);					//提取等号前部分字符串
			if (0 == compareString_t(_T("port"), temp_t, 1)) {
				i += 1;
				getSpeString_t(msg_t, '=', 1, temp_t);				//提取等号后部分字符串
				SetWindowText(hEditBox_TargetPort, temp_t);			//设置port框文本
			}
		}
		//处理ip
		else if (0 == compareString_t(_T("[IP]"), msg_t, 1)) {
			getFileLine_t(_T("init.ini"), i + 2, msg_t);
			if ('\0' == msg_t[0]) {
				break;
			}
			getSpeString_t(msg_t, '=', -1, temp_t);					//提取等号前部分字符串
			if (0 == compareString_t(_T("ip"), temp_t, 1)) {
				i += 1;
				getSpeString_t(msg_t, '=', 1, temp_t);				//提取等号后部分字符串
				SetWindowText(hComboBox_TargetIp, temp_t);			//设置ip框文本
				SendMessage(hComboBox_TargetIp, CB_ADDSTRING, 0, (LPARAM)(temp_t));
			}
		}
		//处理workmode
		else if (0 == compareString_t(_T("[WORKMODE]"), msg_t, 1)) {
			getFileLine_t(_T("init.ini"), i + 2, msg_t);
			if ('\0' == msg_t[0]) {
				break;
			}
			getSpeString_t(msg_t, '=', -1, temp_t);					//提取等号前部分字符串
			if (0 == compareString_t(_T("workmode"), temp_t, 1)) {
				i += 1;
				getSpeString_t(msg_t, '=', 1, temp_t);				//提取等号后部分字符串
				SetWindowText(hComboBox_WorkMode, temp_t);			//设置ip框文本
			}
		}
	}

	return 0;
}

/*文件夹分析*/
/*
 *anlyDirAndFile()这是一个递归函数
 *功能：用于递归遍历文件夹内的文件和文件夹
 *说明：此函数会将指定文件及其内部的所有文件夹、文件路径拷贝至全局变量
 *		相关的全局变量共6个分别是globalAllOfDir_t
 *		globalWithoutPath_dir_t、globalDirIndex
 *		globalAllOfFile_t、globalWithoutPath_file_t、globalFileIndex
 */
int anlyDirAndFile(TCHAR *filename, int inIndex) {
	/*函数内部变量*/
	TCHAR findFile_m[256] = { NULL };			//用作_tfindfirst参数
	__int64 fileHandle = 0;						//文件句柄	
	struct _tfinddata_t FileInfo;				//文件属性结构体
	TCHAR msg_t[256] = { NULL };				//调试信息
	TCHAR filename_new[256] = { NULL };			//带路径的文件名

	/*...*/
	_stprintf_s(findFile_m, _countof(findFile_m), _T("%s\\*.*"), filename);//将文件夹路径用通配符进行文件规范
	fileHandle = _tfindfirst(findFile_m, &FileInfo);			//找出第一个匹配到的文件或文件夹，并将返回值作为_tfindnext的参数
	if (-1L == fileHandle) {
		//空文件夹或遍历完毕或出错
		return 0;
	}
	if (_A_SUBDIR == (_A_SUBDIR & FileInfo.attrib)) {			//判断是否为文件夹
		_tfindnext((intptr_t)fileHandle, &FileInfo);						//为了跳过匹配到的两个默认文件夹
	}
	else {
		//全文件匹配时，会先匹配到两个文件夹，名称分别是"."和".."，需要先排除掉
		_stprintf_s(msg_t, sizeof(msg_t), _T("not '.', so err in anlydirandfile"));
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
		//
		_findclose((intptr_t)fileHandle);									//关闭句柄
		return -1;
	}
	
	/*遍历开始*/
	while (_tfindnext((intptr_t)fileHandle, &FileInfo) == 0)				//遍历该文件夹内所有文件和文件夹
	{
		if (_A_SUBDIR == (_A_SUBDIR & FileInfo.attrib)) {		//判断是否为文件夹
			_stprintf_s(filename_new, _countof(filename_new), _T("%s\\%s"), filename, FileInfo.name);//带上目录路径
			_stprintf_s(globalAllOfDir_t[globalDirIndex], sizeof(nameStore), filename_new);
			CopyMemory(&(globalWithoutPath_dir_t[globalDirIndex][0]), &((globalAllOfDir_t[globalDirIndex])[inIndex]), 2 * (_tcslen(globalAllOfDir_t[globalDirIndex]) - inIndex + 1));		//加一是为了将'\0'也复制过去，去掉部分路径的文件名放入全局变量
			globalDirIndex += 1;
			if (globalDirIndex >= _countof(globalAllOfDir_t)) {
				_stprintf_s(msg_t, sizeof(msg_t), _T("out of dirstore"));
				SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));

				_findclose((intptr_t)fileHandle);							//关闭句柄
				return -1;										//超出预设内存，退出
			}
			if (-1 == anlyDirAndFile(filename_new, inIndex)) {			//此处是递归式调用！！！！！！！！！！！！！！！！！
				_findclose((intptr_t)fileHandle);							//关闭句柄
				return -1;
			}
		}
		else {
			_stprintf_s(filename_new, _countof(filename_new), _T("%s\\%s"), filename, FileInfo.name);//带上目录路径
			_stprintf_s(globalAllOfFile_t[globalFileIndex], sizeof(nameStore), filename_new);			//把带路径的文件名放到全局变量中
			CopyMemory(&(globalWithoutPath_file_t[globalFileIndex][0]), &((globalAllOfFile_t[globalFileIndex])[inIndex]), 2 * (_tcslen(globalAllOfFile_t[globalFileIndex]) - inIndex + 1));	//加一是为了将'\0'也复制过去，去掉部分路径的文件名放入全局变量
			globalFileIndex += 1;
			if (globalFileIndex >= _countof(globalAllOfFile_t)) {
				_stprintf_s(msg_t, sizeof(msg_t), _T("out of filestore"));
				SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));

				_findclose((intptr_t)fileHandle);							//关闭句柄
				return -1;										//超出预设内存，退出
			}
		}
	}
	_findclose((intptr_t)fileHandle);

	return 0;
}

/*重置部分全局变量*/
int resetGlobal(void) {
	globalDirIndex = 0;									//将下标归零
	ZeroMemory(globalWithoutPath_dir_t, (_countof(globalWithoutPath_dir_t) * sizeof(nameStore)));
	ZeroMemory(globalAllOfDir_t, (_countof(globalAllOfDir_t) * sizeof(nameStore)));

	globalFileIndex = 0;								//将下标归零
	ZeroMemory(globalWithoutPath_file_t, (_countof(globalWithoutPath_file_t) * sizeof(nameStore)));//实际数值会乘2原因是TCHAR和char
	ZeroMemory(globalAllOfFile_t, (_countof(globalAllOfFile_t) * sizeof(nameStore)));
	EnableWindow(hButton_Close, TRUE);		//通信结束需要开启close按钮
	globalFileStat = 0;						//设置为初始状态，使其可以接收拖放文件

	return 0;
}

/*
 *anlySpeString()这是一个线程函数
 *功能：用来发送文件和文件夹
 *说明：这个函数会先分析拖曳进来的文件，若未发生异常，则会发送一个SEND系列的消息通知此连接的另一方，
 *		然后对方会返回一个允许或失败的消息,本地线程函数recvSendRequest()收到允许通知后会退出自己的线程，
 *		anlySpeString()函数会等待线程函数recvSendRequest()退出，然后进行发送操作，完成后会启动recvSendRequest()函数并退出自身
 *		判断线程函数recvSendRequest()退出的依据是全局变量globalReady
 *		注1：文件名去除部分路径步骤。关键字：相对路径，拖曳时一级文件（选中的文件夹及文件，不包括选中的文件夹内的文件）的路径可能不一样
 *			对拖曳进来的每个文件执行，获取拖曳时不带路径的文件名的长度得到a，
 *			获取拖曳时带路径的文件名长度得到b，b减a得到除去文件名的路径长度c，
 *			后续递归时沿用长度c
 */
UINT __stdcall anlySpeString(void *argv) {
	/*函数内部变量*/
	int iFuncStat = 0;						//函数状态
	int temp_i = 0;							//临时存放
	char msgType = 0;						//消息类型
	UINT uSize = 0;							//文件名长度
	UINT numOfFiles = 0;					//拖曳文件个数
	TCHAR msg_t[256] = { NULL };			//调试信息
	TCHAR filename[256] = { NULL };			//文件名
	TCHAR tempmsg_t[256] = { NULL };		//临时存放
	WPARAM wParam = (WPARAM)argv;			//用作DragQueryFile参数
	__int64 fileHandle = 0;					//文件句柄
	struct _tfinddata_t FileInfo;			//文件信息结构体
	/*全局变量归零,注意资源竞争*/
	globalDirIndex = 0;
	globalFileIndex = 0;

	/*文件分析开始*/
	//
	_stprintf_s(msg_t, _countof(msg_t), _T("anlySpeString"));
	//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	//WideCharToMultiByte();
	//
	numOfFiles = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, NULL, 0);		//判断拖曳文件数量
	for (int i = 0; i < (int)numOfFiles; ++i) {
		//取出第i个文件
		uSize = DragQueryFile((HDROP)wParam, i, filename, _countof(filename));
		if (0 == uSize || _countof(filename) == uSize) {
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(_T("anlySpeString err!")));
			_endthreadex(0);
			return -1;
		}
		//去前缀,找下标
		getFileName_t(filename, (int)_tcslen(filename), tempmsg_t, &temp_i);
		temp_i = (int)_tcslen(filename) - temp_i;			//初始目录长度 注1：在函数说明部分有讲解

		/*检查文件属性*/
		fileHandle = _tfindfirst(filename, &FileInfo);											//关键
		if (-1L == fileHandle) {
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(_T("err path in anlySpeString!")));
			resetGlobal();
			_endthreadex(0);
			return -1;
		}
		if (_A_SUBDIR == (_A_SUBDIR & FileInfo.attrib)) {
			_stprintf_s(globalAllOfDir_t[globalDirIndex], sizeof(nameStore), filename);//将带路径的文件夹名复制到全局变量中
			/*将去掉初始目录后的路径复制到新的数组中*/
			CopyMemory(&(globalWithoutPath_dir_t[globalDirIndex][0]), &((globalAllOfDir_t[globalDirIndex])[temp_i]), 2 * (_tcslen(globalAllOfDir_t[globalDirIndex]) - temp_i + 1));	//加一是为了将'\0'也复制过去
			globalDirIndex += 1;
			if (globalDirIndex >= _countof(globalAllOfDir_t)) {
				_stprintf_s(msg_t, _countof(msg_t), _T("out of dirstore"));
				SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
				_findclose((intptr_t)fileHandle);					//关闭句柄
				resetGlobal();
				_endthreadex(0);
				return -1;								//超出预设内存，退出
			}

			if (-1 == anlyDirAndFile(filename, temp_i)) {
				_findclose((intptr_t)fileHandle);					//关闭句柄
				resetGlobal();
				_endthreadex(0);
				return -1;
			}
		}
		else {
			_stprintf_s(globalAllOfFile_t[globalFileIndex], sizeof(nameStore), filename);
			/*将去掉初始目录后的路径复制到新的数组中*/
			CopyMemory(&(globalWithoutPath_file_t[globalFileIndex][0]), &((globalAllOfFile_t[globalFileIndex])[temp_i]), 2 * (_tcslen(globalAllOfFile_t[globalFileIndex]) - temp_i + 1));	//加一是为了将'\0'也复制过去
			globalFileIndex += 1;

			if (globalFileIndex >= _countof(globalAllOfFile_t)) {
				_stprintf_s(msg_t, _countof(msg_t), _T("out of filestore"));
				SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
				_findclose((intptr_t)fileHandle);					//关闭句柄
				resetGlobal();
				_endthreadex(0);
				return -1;								//超出预设内存，退出
			}
			continue;
		}
		_findclose((intptr_t)fileHandle);							//关闭句柄
	}
	DragFinish((HDROP)wParam);							//拖曳文件时生成的句柄，关闭
	/*以上步骤，文件分析完毕*/
	globalFileStat = 2;									//设置状态为发送中

	EnableWindow(hButton_Close, FALSE);					//开始通信需要禁用close按钮
	for (int i = 0; i < globalDirIndex; ++i) {

		msgType = SENDDIRREQUEST;
		globalReady = 0;
		send(stSock, &msgType, 1, 0);					//通知对方接收，本地收到对方传回来的信息后，关闭之前的监听线程，由此线程开始通信
		while (1) {
			switch (globalReady) {						//判断完成之后,需重新赋值0
			case 0: {
				//此处有异常，直接用continue会出错
				Sleep(10);
				continue;
			}break;
			case 3: {									//文件接收确认
				globalReady = 0;
				msgType = 0;

				iFuncStat = sendDir_t(globalWithoutPath_dir_t[i]);	//发送文件夹
				useThread();							//重新开启线程进行监听
				if (-1 == iFuncStat) {
					resetGlobal();

					_endthreadex(0);
					return -1;
				}
			}break;
			default: {
				globalReady = 0;
				msgType = 0;
				_stprintf_s(msg_t, _countof(msg_t), _T("globalRead unknow in recvSendFILE"));
				SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);

				resetGlobal();
				_endthreadex(0);
				return 0;
			}
			}
			break;
		}
	}
	for (int i = 0; i < globalFileIndex; ++i) {
		stSendType sst_msg;
		CopyMemory(sst_msg.name, globalAllOfFile_t[i], (_tcslen(globalAllOfFile_t[i]) + 1) * 2);
		CopyMemory(sst_msg.nameWithoutPath, globalWithoutPath_file_t[i], (_tcslen(globalWithoutPath_file_t[i]) + 1) * 2);
		temp_i = 0;

		globalReady = 0;
		msgType = SENDFILEREQUEST;
		send(stSock, &msgType, 1, 0);
		while (1) {
			switch (globalReady) {						//判断完成之后,需重新赋值0
			case 0: {
				//此处有异常，直接用continue会出错
				Sleep(10);
				continue;
			}break;
			case 2: {									//文件接收确认
				globalReady = 0;
				msgType = 0;

				iFuncStat = sendFile_t(sst_msg);					//发送文件
				useThread();							//重新开启线程进行监听
				if (-1 == iFuncStat) {
					resetGlobal();

					_endthreadex(0);
					return -1;
				}
			}break;
			default: {
				globalReady = 0;
				msgType = 0;
				_stprintf_s(msg_t, _countof(msg_t), _T("globalRead unknow in recvSendFILE"));
				SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);

				resetGlobal();
				_endthreadex(0);
				return 0;
			}
			}
			break;
		}
	}
	//
	_stprintf_s(msg_t, _countof(msg_t), _T("totalDir:%d, totalFile:%d"), globalDirIndex, globalFileIndex);
	SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	//
	resetGlobal();

	_endthreadex(0);
	return 0;
}

//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY814_QQ));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL; //MAKEINTRESOURCEW(IDC_MY814_QQ);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hWnd = CreateWindowEx(WS_EX_ACCEPTFILES, szWindowClass, szTitle, WS_CAPTION | WS_POPUP | WS_SYSMENU,
		MAIN_WINCOOR_X, MAIN_WINCOOR_Y, MAIN_WINSIZE_HOR, MAIN_WINSIZE_VER, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE: {
		/*获取系统信息*/
		myGetSysInfo(hWnd);
		/*绘制控件*/
		drawControl(hWnd, message, wParam, lParam);
		/*初始化控件*/
		initControl();
		/*读取配置文件*/
		initFileJudge();
	}break;
	case WM_DROPFILES: {
		myDragFile(hWnd, message, wParam, lParam);
	}break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);			//窗口ID标识符
		int wmMsg = HIWORD(wParam);			//消息类型
		// 分析菜单选择: 
		switch (wmId)
		{
		case CHILD_COMBOBOX_IP_ID: {
			dealWithCCII(hWnd, message, wParam, lParam);
		}break;
		case CHILD_EDIT_PORT_ID: {
			dealWithCEPI(hWnd, message, wParam, lParam);
		}break;
		case CHILD_COMBOBOX_WM_ID: {
			dealWithCCWI(hWnd, message, wParam, lParam);
		}break;
		case CHILD_BUTTON_SU_ID: {
			dealWithCBSI(hWnd, message, wParam, lParam);
		}break;
		case CHILD_BUTTON_CL_ID: {
			dealWithCBCL(hWnd, message, wParam, lParam);
		}break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		myPaintMsg(hWnd, message, wParam, lParam);

		EndPaint(hWnd, &ps);
	}
	break;
	//case WM_USER + 1: {
	//	myUserAdd1(hWnd, message, wParam, lParam);
	//}break;
	case WM_DESTROY:
		myDestoryMsg(hWnd, message, wParam, lParam);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}