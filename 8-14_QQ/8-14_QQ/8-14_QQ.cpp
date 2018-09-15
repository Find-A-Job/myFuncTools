// 8-14_QQ.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "8-14_QQ.h"
#include "MyMacro.h"

#pragma comment(lib, "ws2_32.lib")
#define MAX_LOADSTRING 100

#define MYDEBUGFORTEST

/*��̬���ӿ�*/
typedef int(*TTC)(TCHAR *, size_t, char *);				//���ַ�ת�ַ�
typedef int(*CTI)(char *, size_t, int *);				//�ַ���ת��ֵ
typedef int(*URLA)(char *, int, char *);				//�ַ�����ȡ�ؼ���
HMODULE funAddr=NULL;									//DLL��ַ
FARPROC ttc=NULL;										//������ַ
FARPROC cti=NULL;										//������ַ
FARPROC urla=NULL;		


/*�Զ���ȫ�ֱ���*/
char globalLogMsg[256] = { NULL };						//��־��¼�洢
long globalFuncStat = 0;								//��������״̬

HWND hStatic_TargetIp = NULL;							//��ʾIP
HWND hStatic_targetport = NULL;							//��ʾPORT
HWND hStatic_WorkMode = NULL;							//��ʾWORK MODE

HWND hComboBox_TargetIp = NULL;							//edit box���='Ŀ��ip'
HWND hEditBox_TargetPort = NULL;						//edit box���='Ŀ��port'
HWND hComboBox_WorkMode = NULL;							//��ʾWORK MODE
HWND hButton_StartUp = NULL;							//��ʾSTART UP
HWND hButton_Close = NULL;								//�Ͽ�����
HWND hListBox_Show = NULL;								//��ʾ��¼��Ϣ

TEXTMETRIC stTextMetric;								//ϵͳ������Ϣ	ע��δ����������Ϣ�ı���Ϣ�Զ�̬�޸�ȫ����ϢWM_MESSAGE

SOCKET stSock = NULL;									//����ͨ�ŵ��׽���
SOCKET stSock_server = NULL;							//�������׽���
BOOL connectStat = FALSE;								//����״̬


/*�Զ��庯��ǰ������*/
int myWinSockStartup(void);								//��װsock��غ���
int myWinSockAccept(u_short, int);						//�����ר��
int myWinSockConnect(u_short, char *);					//�ͻ���ר��
int sendFile_t(stSendType);								//�����ļ� ���ַ���
int sendDir_t(TCHAR *);									//�����ļ��� ���ַ���256�ֽڳ�
int recvFile_t(TCHAR *);								//�����ļ� ���ַ���
int recvDir_t(TCHAR *);									//�����ļ��� ������ ���ַ���
int getFileName_t(TCHAR *, int, TCHAR *, int *);		//��ȡ�ļ���
int myWinSockClose(SOCKET);								//�����׽���

int myLoadlibrary(void);								//���붯̬���ӿ�
int writeLog(char *, char *);							//��־��¼
int drawControl(HWND, UINT, WPARAM, LPARAM);			//���ƿؼ�
int initControl(void);									//�ؼ���ʼ��
int setControlStat(BOOL);								//���ÿؼ�״̬(����/����)
int myGetSysInfo(HWND);									//��ȡϵͳ��Ϣ

int myDragFile(HWND, UINT, WPARAM, LPARAM);				//������ҷ�ļ���Ϣ
int myPaintMsg(HWND, UINT, WPARAM, LPARAM);				//����paint��Ϣ
int myUserAdd1(HWND, UINT, WPARAM, LPARAM);				//�����Զ�����Ϣ
int myDestoryMsg(HWND, UINT, WPARAM, LPARAM);			//����destory��Ϣ

int dealWithCCII(HWND, UINT, WPARAM, LPARAM);			//����CHILD_COMBOBOX_IP_ID��Ϣ
int dealWithCEPI(HWND, UINT, WPARAM, LPARAM);			//����CHILD_EDIT_PORT_ID��Ϣ
int dealWithCCWI(HWND, UINT, WPARAM, LPARAM);			//����CHILD_COMBOBOX_WM_ID��Ϣ
int dealWithCBSI(HWND, UINT, WPARAM, LPARAM);			//����CHILD_BUTTON_SU_ID��Ϣ
int dealWithCBCL(HWND, UINT, WPARAM, LPARAM);			//����CHILD_BUTTON_CL_ID��Ϣ

/*�߳���ر���������*/
volatile char globalReady = 0;							//ͬ���ȴ� 0:δ�Ķ�, 1:׼������, -1:�������� 2:�ļ�׼������ 3:�ļ���׼������
HANDLE hGlobalMutex = NULL;								//���������

UINT __stdcall socketStartUp(void *);					//socket����
int useThread(void);									//�߳�����
UINT __stdcall recvSendRequest(void *);					//�����շ���Ϣ
//UINT __stdcall recvSendFILE(void *);					//�շ��ļ�

/*�����ļ�*/
TCHAR globalStorePath[256] = { NULL };					//�ļ��洢·��
TCHAR globalStorePathDefault[256] = { NULL };			//����ʱ��Ĭ��·��
TCHAR globalStoreDirDefault[256] = { NULL };			//����ʱ��Ĭ��Ŀ¼
int getFileLine_t(TCHAR *, int, TCHAR *);				//��ȡָ���ļ�ָ����
int compareString_t(TCHAR *, TCHAR *, int);				//�Ƚ������ַ���
int getSpeString_t(TCHAR *, TCHAR, int, TCHAR *);		//��ȡĳ���ַ�����ĳ���ַ�ǰ��������ַ�
int saveSuccessHistory(TCHAR *, saveSucHis *);			//����һ�γɹ����ӵļ�¼
int getFileTotalSize(TCHAR *, UINT64 *);				//��ȡ�ļ���С
int initFileJudge(void);								//���������ļ�

/*�ļ��з���*/
typedef TCHAR nameStore[256];							//
nameStore globalAllOfDir_t[16] = { NULL };				//��������ļ�������(��·��) ����16��
nameStore globalWithoutPath_dir_t[16] = { NULL };		//��������ļ�������(����·��) ����16��
int globalDirIndex = 0;									//�洢���
nameStore globalAllOfFile_t[16] = { NULL };				//��������ļ�����(��·��) ����16��
nameStore globalWithoutPath_file_t[16] = { NULL };		//��������ļ�����(����·��) ����16��
int globalFileIndex = 0;								//�洢���
volatile int globalFileStat = 0;									//�ļ�״̬ 0:���� 1:������ 2:������ 3: -1:����

int anlyDirAndFile(TCHAR *, int);						//�����ļ�
int resetGlobal(void);									//���ò���ȫ�ֱ���
UINT __stdcall anlySpeString(void *);					//�����ļ��ڲ�

// ȫ�ֱ���: 
HINSTANCE hInst = NULL;									// ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];							// �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];					// ����������

// �˴���ģ���а����ĺ�����ǰ������: 
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

	// TODO: �ڴ˷��ô��롣
	myLoadlibrary();

	// ��ʼ��ȫ���ַ���
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MY814_QQ, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY814_QQ));

	MSG msg;

	// ����Ϣѭ��: 
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


/*socket��غ���*/
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
		setControlStat(TRUE);				//����ʧ�ܣ��ָ���ť
		return WSAGetLastError();
	}
	else {
		stSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == stSock) {
			stSock = NULL;
			_stprintf_s(msg_t, _countof(msg_t), _T("myWinSockStartup, WSAGetLastError:%d"), WSAGetLastError());
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
			setControlStat(TRUE);			//����ʧ�ܣ��ָ���ť
			return WSAGetLastError();
		}
		else {
			;
		}
	}
	
	//int time_out_recv = RECVTIMELIMIT;	//��ʱʱ�䣨���룩
	//setsockopt(stSock, SOL_SOCKET, SO_RCVTIMEO, (char *)&time_out_recv, sizeof(int));

	return 0;
}

int myWinSockAccept(u_short port, int timeOut)
{
	/*�����ڲ�����*/
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
		setControlStat(TRUE);						//����ʧ�ܣ��ָ���ť
		return WSAGetLastError();
	}
	else {
		iFuncStat = listen(stSock_server, timeOut);
		if (SOCKET_ERROR == iFuncStat) {
			_stprintf_s(msg_t, _countof(msg_t), _T("myWinSockAccept, WSAGetLastError:%d"), WSAGetLastError());
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
			setControlStat(TRUE);					//����ʧ�ܣ��ָ���ť
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
				setControlStat(TRUE);				//����ʧ�ܣ��ָ���ť
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
	/*�����ڲ�����*/
	int iFuncStat = 0;
	SOCKADDR_IN cAddre;
	in_addr paddr[256];
	TCHAR msg_t[256] = { NULL };
	saveSucHis ssh_t = { NULL };

	inet_pton(AF_INET, ip, paddr);			//�����ֽ���ת�����ֽ���
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
	/*�����ڲ�����*/
	TCHAR msg_t[256] = { NULL };
	int iFuncStat = 0;
	size_t num = 0;
	UINT64 totalSize = 0;
	UINT64 gethhh = 0;
	UCHAR send_msg[65535] = { NULL };
	char temp = 0;							//�ļ��շ�״̬��
	size_t realSize = 0;

	/*�������*/
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
	getFileTotalSize(in_sst_msg.name, &totalSize);		//��ȡ�ļ���С
	//
	FILE *fileRead = NULL;
	_tfopen_s(&fileRead, in_sst_msg.name, _T("rb"));
	if (NULL == fileRead) {
		_stprintf_s(msg_t, _countof(msg_t), _T("open file err in sendFile_t"));
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		return -1;
	}

	/*���ȷ����ļ���������256��*/
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
	//�����Ƿ������������գ���֤���ݴ���Ŀɿ���
	iFuncStat = recv(stSock, &temp, 1, 0);				//�ȴ�
	if (SOCKET_ERROR == iFuncStat || 0 == temp) {
		fclose(fileRead);
		return -1;
	}

	/*�����ļ���С*/
	//totalSize = _filelength(_fileno(fileRead));		//��ȡ�ļ���С
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
	iFuncStat = recv(stSock, &temp, 1, 0);				//�ȴ�
	if (SOCKET_ERROR == iFuncStat || 0 == temp) {
		fclose(fileRead);
		return -1;
	}

	/*��ʼ�����ļ�����*/
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
		if (SOCKET_ERROR == iFuncStat) {				//socket�������⣬�����޷��������Է���ֱ�ӹر�
			_stprintf_s(msg_t, _countof(msg_t), _T("send err in sendFile_t"));
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
			fclose(fileRead);
			return -1;
		}
		iFuncStat = recv(stSock, &temp, 1, 0);			//���͹�������ɣ��ȴ��Է���Ӧ����ȷ���Ƿ����
		if (SOCKET_ERROR == iFuncStat || 0 == temp) {
			_stprintf_s(msg_t, _countof(msg_t), _T("recv err in sendFile_t"));
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
			fclose(fileRead);
			return -1;
		}
		if (totalSize == realSize) {					//��ȡ���ֽ����Ѵ�꣬�˳�
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
	/*�����ڲ�����*/
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
	/*�����ڲ�����*/
	INT32 recvLength = 0;
	UINT64 totalLength = 0;
	TCHAR msg_t[256] = { NULL };					//������Ϣ
	int iFuncStat = 0;								//��������ֵ
	TCHAR filename[256] = { NULL };					//�ļ�
	UINT64 fileSize = 0;								//���ֽ���
	FILE *fileWrite = NULL;							//д�ļ�
	UCHAR recvFile[65535] = { NULL };				//�����ֽ���
	TCHAR fileNameAddPath[512] = { NULL };			//ƴ��·��

	/*�������*/
	//
	_stprintf_s(msg_t, _countof(msg_t), _T("recvFile_t"));
	SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	//

	/*�����ļ���*/
	iFuncStat = recv(stSock, (char *)filename, (int)(_countof(filename) * 2), 0);
	if (SOCKET_ERROR == iFuncStat) {
		_stprintf_s(msg_t, _countof(msg_t), _T("recv err in recvFile_t,err code:%d"), WSAGetLastError());
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		send(stSock, "0", 1, 0);							//�����쳣֪ͨ,��ֹͨ��
		return -1;
	}
	send(stSock, "1", 1, 0);								//�������ȷ��

	/*�����ļ�*/
	_stprintf_s(fileNameAddPath, _countof(fileNameAddPath), _T("%s%s"), globalStorePath, filename);		//ƴ��·�����ļ���
	iFuncStat = _tfopen_s(&fileWrite, fileNameAddPath, _T("wb"));
	if (0 == iFuncStat) {
		;
	}
	else {
		_stprintf_s(msg_t, _countof(msg_t), _T("create file err in recvFile_t!error code:%d"), errno);
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		send(stSock, "0", 1, 0);							//�����쳣֪ͨ��ֹͨ��
		return -1;
	}

	/*�����ļ���С*/
	iFuncStat = recv(stSock, (char *)&fileSize, 8, 0);
	if (SOCKET_ERROR == iFuncStat) {
		_stprintf_s(msg_t, _countof(msg_t), _T("recv err in recvFile_t,err code:%d"), WSAGetLastError());
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		send(stSock, "0", 1, 0);							//�����쳣֪ͨ,��ֹͨ��
		fclose(fileWrite);
		return -1;
	}
	//
	//_stprintf_s(msg_t, _countof(msg_t), _T("recvsize: %llu"), fileSize);
	//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	//
	send(stSock, "1", 1, 0);								//�������ȷ��

	/*��ʼ�����ļ�*/
	while (1) {
		recvLength = recv(stSock, (char *)recvFile, _countof(recvFile), 0);
		iFuncStat = recvLength;
		if (SOCKET_ERROR == iFuncStat) {		//socket�����˴��󣬴�����ϢҲ�޷��������Է��ˣ�ֱ���˳�
			_stprintf_s(msg_t, _countof(msg_t), _T("recv err in recvFile_t"));
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
			fclose(fileWrite);
			return -1;
		}
		else {
			totalLength += recvLength;			//���ճ���ͳ��
		}
		iFuncStat = (int)fwrite(recvFile, sizeof(char), recvLength, fileWrite);
		if (iFuncStat != recvLength) {			//ʵ��д��������Ԥ�ڲ�һ�£����ش�����
			_stprintf_s(msg_t, _countof(msg_t), _T("fwrite err in recvFile_t"));
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
			send(stSock, "0", 1, 0);
			fclose(fileWrite);
			return -1;
		}
		send(stSock, "1", 1, 0);								//���ճɹ�����׼���ý�����һ�����������Է�һ��ȷ����
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
	/*�����ڲ�����*/
	TCHAR dirName[1024] = { NULL };
	TCHAR msg_t[256] = { NULL };
	TCHAR dirNameAddPath[512] = { NULL };
	int iFuncStat = 0;
	char recvType = 0;				//ͨ��״̬֪ͨ��

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
	_stprintf_s(dirNameAddPath, _countof(dirNameAddPath), _T("%s%s"), globalStorePath, dirName); //ƴ��·�����ļ�����
	iFuncStat = CreateDirectory(dirNameAddPath, NULL);		//�����ļ���
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

	setControlStat(TRUE);					//���ð�ť
	EnableWindow(hButton_Close, FALSE);		//���ð�ť
	return 0;
}

/*...*/
int myLoadlibrary(void)
{
	TCHAR msg_t[256] = { NULL };

	/*��̬���ӿ�*/
	funAddr = LoadLibrary(_T("7-20-anlysis_file_name"));		//freelibrary
	if (NULL == funAddr) {
		_stprintf_s(msg_t, _countof(msg_t), _T("myLoadlibrary, err"));
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
		exit(1);
	}
	ttc = GetProcAddress(funAddr, "TcharToChar");				//���롮���ַ�ת�ַ���
	cti = GetProcAddress(funAddr, "charToInt");					//���롮�ַ�ת��ֵ��
	//urla = GetProcAddress(funAddr, "URL_anly");					//���롮�ַ�����ȡ�ؼ��֡�
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
	fwrite(szTime_now, sizeof(char), strlen(szTime_now), logfile);		//��־ʱ��
	fwrite(logmsg, sizeof(char), strlen(logmsg), logfile);				//��־��Ϣ
	fwrite("\n", sizeof(char), 1, logfile);								//������־
	fclose(logfile);

	return 0;
}

int drawControl(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	/*����*/
	hComboBox_TargetIp = CreateWindow(_T("combobox"), CHILD_COMBOBOX_IP_TEXT, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | CBS_DROPDOWN,
		(int)CHILD_COMBOBOX_IP_COOR_X, (int)CHILD_COMBOBOX_IP_COOR_Y, (int)CHILD_COMBOBOX_IP_HOR, (int)CHILD_COMBOBOX_IP_VER,
		hWnd, (HMENU)(int)CHILD_COMBOBOX_IP_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	/*����*/
	hEditBox_TargetPort = CreateWindow(_T("edit"), CHILD_EDIT_PORT_TEXT, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		(int)CHILD_EDIT_PORT_COOR_X, (int)CHILD_EDIT_PORT_COOR_Y, (int)CHILD_EDIT_PORT_HOR, (int)CHILD_EDIT_PORT_VER,
		hWnd, (HMENU)(int)CHILD_EDIT_PORT_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	/*����*/
	hStatic_TargetIp = CreateWindow(_T("static"), CHILD_STATIC_PORT_TEXT, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_READONLY,
		(int)CHILD_STATIC_PORT_COOR_X, (int)CHILD_STATIC_PORT_COOR_Y, (int)CHILD_STATIC_PORT_HOR, (int)CHILD_STATIC_PORT_VER,
		hWnd, (HMENU)(int)CHILD_STATIC_PORT_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	/*����*/
	hStatic_targetport = CreateWindow(_T("static"), CHILD_STATIC_IP_TEXT, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_READONLY,
		(int)CHILD_STATIC_IP_COOR_X, (int)CHILD_STATIC_IP_COOR_Y, (int)CHILD_STATIC_IP_HOR, (int)CHILD_STATIC_IP_VER,
		hWnd, (HMENU)(int)CHILD_STATIC_IP_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	/*����*/
	hStatic_WorkMode = CreateWindow(_T("static"), CHILD_STATIC_WM_TEXT, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_READONLY,
		(int)CHILD_STATIC_WM_COOR_X, (int)CHILD_STATIC_WM_COOR_Y, (int)CHILD_STATIC_WM_HOR, (int)CHILD_STATIC_WM_VER,
		hWnd, (HMENU)(int)CHILD_STATIC_WM_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	/*����*/
	hComboBox_WorkMode = CreateWindow(_T("combobox"), CHILD_COMBOBOX_WM_TEXT, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | CBS_DROPDOWN,
		(int)CHILD_COMBOBOX_WM_COOR_X, (int)CHILD_COMBOBOX_WM_COOR_Y, (int)CHILD_COMBOBOX_WM_HOR, (int)CHILD_COMBOBOX_WM_VER,
		hWnd, (HMENU)(int)CHILD_COMBOBOX_WM_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	/*����*/
	hButton_StartUp = CreateWindow(_T("button"), CHILD_BUTTON_SU_TEXT, WS_CHILD | WS_VISIBLE | ES_CENTER,
		(int)CHILD_BUTTON_SU_COOR_X, (int)CHILD_BUTTON_SU_COOR_Y, (int)CHILD_BUTTON_SU_HOR, (int)CHILD_BUTTON_SU_VER,
		hWnd, (HMENU)(int)CHILD_BUTTON_SU_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	/*����*/
	hListBox_Show = CreateWindow(_T("listbox"), CHILD_LISTBOX_SHOW_TEXT, WS_CHILD | WS_VISIBLE | ES_CENTER | WS_VSCROLL | ES_READONLY |
		LBS_MULTICOLUMN  | LBS_DISABLENOSCROLL,
		(int)CHILD_LISTBOX_SHOW_COOR_X, (int)CHILD_LISTBOX_SHOW_COOR_Y, (int)CHILD_LISTBOX_SHOW_HOR, (int)CHILD_LISTBOX_SHOW_VER,
		hWnd, (HMENU)(int)CHILD_LISTBOX_SHOW_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	/*����*/
	hButton_Close = CreateWindow(_T("button"), CHILD_BUTTON_CL_TEXT, WS_CHILD | WS_VISIBLE | ES_CENTER,
		(int)CHILD_BUTTON_CL_COOR_X, (int)CHILD_BUTTON_CL_COOR_Y, (int)CHILD_BUTTON_CL_HOR, (int)CHILD_BUTTON_CL_VER,
		hWnd, (HMENU)(int)CHILD_BUTTON_CL_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	return 0;
}

int initControl(void)
{
	/*���������ַ�����*/
	SendMessage(hComboBox_TargetIp, CB_LIMITTEXT, 15, 0);
	SendMessage(hEditBox_TargetPort, EM_LIMITTEXT, 5, 0);

	/*Ϊ���������ѡ��*/
	SendMessage(hComboBox_WorkMode, CB_ADDSTRING, 0, (LPARAM)(_T("SERVER")));
	SendMessage(hComboBox_WorkMode, CB_ADDSTRING, 0, (LPARAM)(_T("CLIENT")));

	/*��ʼ����Ϣ*/
	SendMessage(hListBox_Show, LB_SETCOLUMNWIDTH, (int)CHILD_LISTBOX_SHOW_HOR, 0);
	SendMessage(hListBox_Show, LB_ADDSTRING, 0, (LPARAM)(_T("show results here!")));

	/*���ð�ť*/
	EnableWindow(hButton_Close, FALSE);

	/*�ؼ���ʼֵ*/
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

/*��Ϣ������غ���*/
int myDragFile(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	/*�����ڲ�����*/
	TCHAR msg_t[256] = { NULL };			//������Ϣ
	int iFuncStat = 0;						//����״̬
	HANDLE hThread = NULL;					//�߳̾��
	UINT32 id = 0;							//�߳�id
	BOOL bButtonStat = FALSE;				//��ť״̬

	/*...*/
	if (FALSE == connectStat) {
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(_T("Please click 'start up'!")));
		return 0;
	}
	//
	_stprintf_s(msg_t, _countof(msg_t), _T("myDragFile"));
	//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	//
	//�Ժ�Ӹ��ж� �Ƿ����������߳�ʵ��������
	if (0 != globalFileStat) {
		_stprintf_s(msg_t, _countof(msg_t), _T("sending already, stat:%d"), globalFileStat);
		//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		return -1;
	}
	/*�����̷߳����ļ�*/
	globalFileStat = 1;				//����״̬Ϊ������
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
	FreeLibrary(funAddr);					//�رն�̬���ӿ�
	myWinSockClose(stSock);					//�ر�socket
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
	/*�����ڲ�����*/
	HANDLE hThread = NULL;
	int iFuncStat = 0;
	UINT32 id = 0;
	TCHAR msg_t[256] = { NULL };

	/*...*/
	setControlStat(FALSE);				//���ÿؼ����ȴ�����

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
	/*�����ڲ�����*/
	char msgType = 0;

	/*...*/
	msgType = CLOSEWILL;
	send(stSock, &msgType, 1, 0);

	myWinSockClose(stSock);
	myWinSockClose(stSock_server);
	connectStat = FALSE;			//��������״̬

	return 0;
}

/*�߳���غ���*/
UINT __stdcall socketStartUp(void *argv) 
{
	//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(_T("Start up")));

	/*�����ڲ�����*/
	int iFuncStat = 0;						//����״̬
	TCHAR tcWorkMode_msg[256] = { NULL };	//�洢combobox text����ֵ
	TCHAR szIp[32];							//ip
	char szIp_c[32];						//תansi
	TCHAR szPort[8];						//port
	char szPort_c[8];						//תansi
	int iPort[1];							//ת��ֵ
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

	/*���ݵ���*/
	((TTC)ttc)(szIp, _tcslen(szIp), szIp_c);
	((TTC)ttc)(szPort, _tcslen(szIp), szPort_c);
	((CTI)cti)(szPort_c, strlen(szPort_c), iPort);

	/*��鹤��ģʽ*/
	iFuncStat = GetWindowText(hComboBox_WorkMode, tcWorkMode_msg, _countof(tcWorkMode_msg));		//��ȡcombobox��ֵ
	if (0 == iFuncStat) {
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(_T("Please chooice work mode!")));
		setControlStat(TRUE);
		return -1;
	}
	else {
		/*����socket*/
		iFuncStat = myWinSockStartup();
		//�ж�
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
				connectStat = TRUE;					//��������״̬
				EnableWindow(hButton_Close, TRUE);

				useThread();						//���������߳�
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
				connectStat = TRUE;					//��������״̬
				EnableWindow(hButton_Close, TRUE);

				useThread();						//���������߳�
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
 *useThread()����һ�����溯��
 *���ܣ���������recvSendRequest()�߳�
 */
int useThread(void) {
	/*�����ڲ�����*/
	UINT32 id = 0;
	HANDLE hThread = NULL;
	TCHAR msg_t[256] = { NULL };

	/*...*/
	//
	_stprintf_s(msg_t, _countof(msg_t), _T("useThread"));
	SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	//
	hThread =(HANDLE) _beginthreadex(NULL, 0, &recvSendRequest, NULL, 0, &id);	//�����߳�

	if (NULL == hThread) {
		_stprintf_s(msg_t, _countof(msg_t), _T("beginthreadex err in useThread"));
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
		return -1;
	}
	CloseHandle(hThread);											//��ʽ�ر��߳̾��

	return 0;
}

/*
 *recvSendRequest()����һ���̺߳���
 *���ܣ����ڻ������¼��������ж�
 *˵������socket���ӳɹ�����������̣߳����ա������ر��¼����м���
 *		������ϵ���¼����н��պ�᷵��һ������֪ͨ�������ӵ���һ��
 *		������ϵ���¼����н��պ���˳������̣߳�������ȫ�ֱ�����֪ͨ�����߳�
 *		��δ��������socket�շ����������̱߳�������
 *		�ڽ�������socket�շ�����ʱ�����̱߳����˳������������̲߳�����ʱ������߳�
 */
UINT __stdcall recvSendRequest(void *argv) {
	/*�������*/

	/*�����ڲ�����*/
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
			switch (msgType)						//��������
			{
			case SENDFILEREQUEST: {					//����ȷ��
				_stprintf_s(msg_t, _countof(msg_t), _T("recv SENDFILEREQUEST in recvSendRequest"));
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
				msgType = SENDFILEALLOW;
				send(stSock, &msgType, 1, 0);		//֪ͨ�Է������ؽ��վ���
				EnableWindow(hButton_Close, FALSE);
				recvFile_t(msg_t);					//��ʼ����
				EnableWindow(hButton_Close, TRUE);

				continue;
			}break;
			case SENDFILEALLOW: {					//�Է���ȷ��
				//...
				_stprintf_s(msg_t, _countof(msg_t), _T("recv SENDFILEALLOW in recvSendRequest"));
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
				globalReady = 2;

				_endthreadex(0);					//��ֹ���̣߳��ó�������Դ���Ժ���������߳��ٴο������߳�
				return 0;
			}break;
			case SENDDIRREQUEST: {					//����ȷ��
				_stprintf_s(msg_t, _countof(msg_t), _T("recv SENDDIRREQUEST in recvSendRequest"));
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
				msgType = SENDDIRALLOW;
				send(stSock, &msgType, 1, 0);		//֪ͨ�Է������ؽ��վ���
				EnableWindow(hButton_Close, FALSE);
				recvDir_t(msg_t);					//��ʼ����
				EnableWindow(hButton_Close, TRUE);

				continue;
			}break;
			case SENDDIRALLOW: {					//�Է���ȷ��
				//...
				_stprintf_s(msg_t, _countof(msg_t), _T("recv SENDDIRALLOW in recvSendRequest"));
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
				globalReady = 3;

				_endthreadex(0);					//��ֹ���̣߳��ó�������Դ���Ժ���������߳��ٴο������߳�
				return 0;
			}break;
			case CLOSEWILL: {						//׼���ر�
				_stprintf_s(msg_t, _countof(msg_t), _T("recv CLOSEWILL in recvSendRequest"));
				//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
				msgType = CLOSEALLOW;
				send(stSock, &msgType, 1, 0);
				myWinSockClose(stSock);

				_endthreadex(0);
				return 0;
			}break;
			case CLOSEALLOW: {						//�Է���ȷ��
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

/*�����ļ�����*/
int getFileLine_t(TCHAR *inTchar, int lineIndex, TCHAR *outTchar) {
	/*�����ڲ�����*/
	FILE *fileRead = NULL;
	TCHAR fr_t = 0;
	TCHAR msg_t[256] = { NULL };

	/*�������*/
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
		if (feof(fileRead) != 0) {		//�����ļ�βʱ����2�������1���ɹ��ҵ�ָ���У�2��δ�ҵ�ָ����
			if (0 == k) {
				outTchar[0] = '\0';
				fclose(fileRead);
				return -1;
			}
			outTchar[k - 1] = '\0';
			break;
		}
		if (lineIndex - 1 == j) {		//�ҵ�ָ����
			outTchar[k] = fr_t;
			k += 1;
		}
		if ('\n' == fr_t) {				//����ָ����
			j += 1;
		}
		if (j == lineIndex) {			//����ָ����
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
 *���ܣ��Ƚ������ַ���
 *˵�����˺���������ַ�������ʵ��ΪNULL������-1
 *		mode������ƥ������0x11111111,0x1��ȫ��ƥ�䣬0x10�ǲ����ִ�Сд��������δ����
 *		����ȷ���1����ȷ���0�����󷵻�-1
 */
int compareString_t(TCHAR *inTcharA, TCHAR *inTcharB, int mode) {
	/*�����ڲ�����*/
	TCHAR msg_t[256] = { NULL };
	int iFuncStat = 0;

	/*�������*/
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
 *���ܣ���ȡ�ַ�����ָ���ַ�ǰ��������ַ�
 *˵����inTcharA,inTcharB,mode�������������outTcharA���������
 *		mode:1��ʾ��-1��ʾǰ
 *		ʧ�ܷ���-1���ɹ�����0;���ص��ַ�����'\0'��β
 *
 *2018-9-3
 *��������ָ���ַ����滻ָ���ַ�ǰ��������ַ�
 *		mode��2��ʾ�滻��-2��ʾ�滻ǰ
 *		outTcharA���������Ϻ���ַ���
 *		outTcharA����Ĵ�С��Ҫ����(inTcharA�ַ����Ⱥ�outTcharAʵ���ַ���֮��)
 */
int getSpeString_t(TCHAR *inTcharA, TCHAR inTcharB, int mode, TCHAR *outTcharA) {
	/*�����ڲ�����*/
	TCHAR msg_t[256] = { NULL };
	int index = -1;
	TCHAR temp_t[256] = { NULL };

	/*�������*/
	if (NULL == inTcharA || 0 == inTcharB) {
		return -1;
	}

	/*...*/
	for (int i = 0; inTcharA[i] != '\0'; ++i) {		//���ҳ�ָ���ַ�λ��
		if (inTcharA[i] == inTcharB) {
			index = i;
			break;
		}
	}
	if (-1 == index) {								//δ�ҵ�ָ���ַ�λ��
		if (-1 == mode || 1 == mode) {
			outTcharA[0] = '\0';
		}
		return -1;
	}
	switch (mode)									//�жϹ���ģʽ
	{
	case 1: {										//��ȡ�������ַ�
		for (int i = index + 1;; ++i) {
			if (inTcharA[i] == '\0') {
				outTcharA[i - index - 1] = '\0';
				break;
			}
			outTcharA[i - index - 1] = inTcharA[i];
		}
	}break;
	case -1: {										//��ȡǰ�������ַ�
		for (int i = 0; i < index; ++i) {
			outTcharA[i] = inTcharA[i];
		}
		outTcharA[index] = '\0';
	}break;
	case 2: {										//�滻�������ַ�
		for (int i = index;; ++i) {
			if ('\0' == outTcharA[i - index]) {
				inTcharA[i] = '\0';
				break;
			}
			inTcharA[i] = outTcharA[i - index];
		}
	}break;
	case -2: {										//�滻ǰ�������ַ�
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
 *���ܣ����湤��ģʽ��ip���˿���ָ���ļ�
 *˵����
 */
int saveSuccessHistory(TCHAR *fileName, saveSucHis *inSSH) {
	/*�����ڲ�����*/
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
	/*�����ڲ�����*/
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
 *initFileJudge����
 *���ܣ�����Ĭ�ϵĳ�ʼ���ļ����ȡ�Ѵ��ڵĳ�ʼ���ļ�
 *˵����
 */
int initFileJudge(void) {
	/*�����ڲ�����*/
	FILE *FileInit = NULL;
	int iFuncStat = NULL;
	TCHAR msg_t[256] = { NULL };
	TCHAR temp_t[256] = { NULL };

	/*...*/
	CreateDirectory(_T("fileStore"), NULL);
	_stprintf_s(globalStoreDirDefault, _countof(globalStoreDirDefault), _T("fileStore"));						//����Ĭ��·��
	_stprintf_s(globalStorePathDefault, _countof(globalStorePathDefault), _T("%s\\"), globalStoreDirDefault);	//����Ĭ��·��
	iFuncStat = _tfopen_s(&FileInit, _T("init.ini"), _T("rt"));
	if (NULL == FileInit) {			//�ж������ļ��Ƿ����
		iFuncStat = _tfopen_s(&FileInit, _T("init.ini"), _T("wt"));	//���������򴴽�һ��
		if (NULL == FileInit) {
			_stprintf_s(msg_t, _countof(msg_t), _T("err in initFileJudge:%d"), iFuncStat);
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
			return -1;
		}
		fwrite("[WORKMODE]\nworkmode=SERVER\n[IP]\nip=192.168.188.117\n[PORT]\nport=12345\n[STOREPATH]\npath=fileStore\\\n", sizeof(char), 98, FileInit);
		fclose(FileInit);

		//������Ĭ�����ã�Ȼ���˳�
		_stprintf_s(globalStorePath, _countof(globalStorePath), _T("fileStore\\"));			//�ļ����·��

		return 0;
	}
	else {
		fclose(FileInit);
	}

	//�����ı�...
	getFileLine_t(_T("init.ini"), 1, msg_t);
	for (int i = 0;; ++i) {
		getFileLine_t(_T("init.ini"), i+1, msg_t);
		if ('\0' == msg_t[0]) {
			break;
		}
		//����洢·��
		if (0 == compareString_t(_T("[STOREPATH]"), msg_t, 1)) {
			getFileLine_t(_T("init.ini"), i + 2, msg_t);
			if ('\0' == msg_t[0]) {
				break;
			}
			getSpeString_t(msg_t, '=', -1, temp_t);					//��ȡ�Ⱥ�ǰ�����ַ���
			if (0 == compareString_t(_T("path"), temp_t, 1)) {
				i += 1;
				getSpeString_t(msg_t, '=', 1, temp_t);				//��ȡ�Ⱥź󲿷��ַ���
				_stprintf_s(globalStorePath, _countof(globalStorePath), _T("%s"), temp_t);	//ȫ�ֱ���globalStorePath��ֵ
			}
		}
		//����˿�
		else if (0 == compareString_t(_T("[PORT]"), msg_t, 1)) {
			getFileLine_t(_T("init.ini"), i + 2, msg_t);
			if ('\0' == msg_t[0]) {
				break;
			}
			getSpeString_t(msg_t, '=', -1, temp_t);					//��ȡ�Ⱥ�ǰ�����ַ���
			if (0 == compareString_t(_T("port"), temp_t, 1)) {
				i += 1;
				getSpeString_t(msg_t, '=', 1, temp_t);				//��ȡ�Ⱥź󲿷��ַ���
				SetWindowText(hEditBox_TargetPort, temp_t);			//����port���ı�
			}
		}
		//����ip
		else if (0 == compareString_t(_T("[IP]"), msg_t, 1)) {
			getFileLine_t(_T("init.ini"), i + 2, msg_t);
			if ('\0' == msg_t[0]) {
				break;
			}
			getSpeString_t(msg_t, '=', -1, temp_t);					//��ȡ�Ⱥ�ǰ�����ַ���
			if (0 == compareString_t(_T("ip"), temp_t, 1)) {
				i += 1;
				getSpeString_t(msg_t, '=', 1, temp_t);				//��ȡ�Ⱥź󲿷��ַ���
				SetWindowText(hComboBox_TargetIp, temp_t);			//����ip���ı�
				SendMessage(hComboBox_TargetIp, CB_ADDSTRING, 0, (LPARAM)(temp_t));
			}
		}
		//����workmode
		else if (0 == compareString_t(_T("[WORKMODE]"), msg_t, 1)) {
			getFileLine_t(_T("init.ini"), i + 2, msg_t);
			if ('\0' == msg_t[0]) {
				break;
			}
			getSpeString_t(msg_t, '=', -1, temp_t);					//��ȡ�Ⱥ�ǰ�����ַ���
			if (0 == compareString_t(_T("workmode"), temp_t, 1)) {
				i += 1;
				getSpeString_t(msg_t, '=', 1, temp_t);				//��ȡ�Ⱥź󲿷��ַ���
				SetWindowText(hComboBox_WorkMode, temp_t);			//����ip���ı�
			}
		}
	}

	return 0;
}

/*�ļ��з���*/
/*
 *anlyDirAndFile()����һ���ݹ麯��
 *���ܣ����ڵݹ�����ļ����ڵ��ļ����ļ���
 *˵�����˺����Ὣָ���ļ������ڲ��������ļ��С��ļ�·��������ȫ�ֱ���
 *		��ص�ȫ�ֱ�����6���ֱ���globalAllOfDir_t
 *		globalWithoutPath_dir_t��globalDirIndex
 *		globalAllOfFile_t��globalWithoutPath_file_t��globalFileIndex
 */
int anlyDirAndFile(TCHAR *filename, int inIndex) {
	/*�����ڲ�����*/
	TCHAR findFile_m[256] = { NULL };			//����_tfindfirst����
	__int64 fileHandle = 0;						//�ļ����	
	struct _tfinddata_t FileInfo;				//�ļ����Խṹ��
	TCHAR msg_t[256] = { NULL };				//������Ϣ
	TCHAR filename_new[256] = { NULL };			//��·�����ļ���

	/*...*/
	_stprintf_s(findFile_m, _countof(findFile_m), _T("%s\\*.*"), filename);//���ļ���·����ͨ��������ļ��淶
	fileHandle = _tfindfirst(findFile_m, &FileInfo);			//�ҳ���һ��ƥ�䵽���ļ����ļ��У���������ֵ��Ϊ_tfindnext�Ĳ���
	if (-1L == fileHandle) {
		//���ļ��л������ϻ����
		return 0;
	}
	if (_A_SUBDIR == (_A_SUBDIR & FileInfo.attrib)) {			//�ж��Ƿ�Ϊ�ļ���
		_tfindnext((intptr_t)fileHandle, &FileInfo);						//Ϊ������ƥ�䵽������Ĭ���ļ���
	}
	else {
		//ȫ�ļ�ƥ��ʱ������ƥ�䵽�����ļ��У����Ʒֱ���"."��".."����Ҫ���ų���
		_stprintf_s(msg_t, sizeof(msg_t), _T("not '.', so err in anlydirandfile"));
		SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
		//
		_findclose((intptr_t)fileHandle);									//�رվ��
		return -1;
	}
	
	/*������ʼ*/
	while (_tfindnext((intptr_t)fileHandle, &FileInfo) == 0)				//�������ļ����������ļ����ļ���
	{
		if (_A_SUBDIR == (_A_SUBDIR & FileInfo.attrib)) {		//�ж��Ƿ�Ϊ�ļ���
			_stprintf_s(filename_new, _countof(filename_new), _T("%s\\%s"), filename, FileInfo.name);//����Ŀ¼·��
			_stprintf_s(globalAllOfDir_t[globalDirIndex], sizeof(nameStore), filename_new);
			CopyMemory(&(globalWithoutPath_dir_t[globalDirIndex][0]), &((globalAllOfDir_t[globalDirIndex])[inIndex]), 2 * (_tcslen(globalAllOfDir_t[globalDirIndex]) - inIndex + 1));		//��һ��Ϊ�˽�'\0'Ҳ���ƹ�ȥ��ȥ������·�����ļ�������ȫ�ֱ���
			globalDirIndex += 1;
			if (globalDirIndex >= _countof(globalAllOfDir_t)) {
				_stprintf_s(msg_t, sizeof(msg_t), _T("out of dirstore"));
				SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));

				_findclose((intptr_t)fileHandle);							//�رվ��
				return -1;										//����Ԥ���ڴ棬�˳�
			}
			if (-1 == anlyDirAndFile(filename_new, inIndex)) {			//�˴��ǵݹ�ʽ���ã���������������������������������
				_findclose((intptr_t)fileHandle);							//�رվ��
				return -1;
			}
		}
		else {
			_stprintf_s(filename_new, _countof(filename_new), _T("%s\\%s"), filename, FileInfo.name);//����Ŀ¼·��
			_stprintf_s(globalAllOfFile_t[globalFileIndex], sizeof(nameStore), filename_new);			//�Ѵ�·�����ļ����ŵ�ȫ�ֱ�����
			CopyMemory(&(globalWithoutPath_file_t[globalFileIndex][0]), &((globalAllOfFile_t[globalFileIndex])[inIndex]), 2 * (_tcslen(globalAllOfFile_t[globalFileIndex]) - inIndex + 1));	//��һ��Ϊ�˽�'\0'Ҳ���ƹ�ȥ��ȥ������·�����ļ�������ȫ�ֱ���
			globalFileIndex += 1;
			if (globalFileIndex >= _countof(globalAllOfFile_t)) {
				_stprintf_s(msg_t, sizeof(msg_t), _T("out of filestore"));
				SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));

				_findclose((intptr_t)fileHandle);							//�رվ��
				return -1;										//����Ԥ���ڴ棬�˳�
			}
		}
	}
	_findclose((intptr_t)fileHandle);

	return 0;
}

/*���ò���ȫ�ֱ���*/
int resetGlobal(void) {
	globalDirIndex = 0;									//���±����
	ZeroMemory(globalWithoutPath_dir_t, (_countof(globalWithoutPath_dir_t) * sizeof(nameStore)));
	ZeroMemory(globalAllOfDir_t, (_countof(globalAllOfDir_t) * sizeof(nameStore)));

	globalFileIndex = 0;								//���±����
	ZeroMemory(globalWithoutPath_file_t, (_countof(globalWithoutPath_file_t) * sizeof(nameStore)));//ʵ����ֵ���2ԭ����TCHAR��char
	ZeroMemory(globalAllOfFile_t, (_countof(globalAllOfFile_t) * sizeof(nameStore)));
	EnableWindow(hButton_Close, TRUE);		//ͨ�Ž�����Ҫ����close��ť
	globalFileStat = 0;						//����Ϊ��ʼ״̬��ʹ����Խ����Ϸ��ļ�

	return 0;
}

/*
 *anlySpeString()����һ���̺߳���
 *���ܣ����������ļ����ļ���
 *˵��������������ȷ�����ҷ�������ļ�����δ�����쳣����ᷢ��һ��SENDϵ�е���Ϣ֪ͨ�����ӵ���һ����
 *		Ȼ��Է��᷵��һ�������ʧ�ܵ���Ϣ,�����̺߳���recvSendRequest()�յ�����֪ͨ����˳��Լ����̣߳�
 *		anlySpeString()������ȴ��̺߳���recvSendRequest()�˳���Ȼ����з��Ͳ�������ɺ������recvSendRequest()�������˳�����
 *		�ж��̺߳���recvSendRequest()�˳���������ȫ�ֱ���globalReady
 *		ע1���ļ���ȥ������·�����衣�ؼ��֣����·������ҷʱһ���ļ���ѡ�е��ļ��м��ļ���������ѡ�е��ļ����ڵ��ļ�����·�����ܲ�һ��
 *			����ҷ������ÿ���ļ�ִ�У���ȡ��ҷʱ����·�����ļ����ĳ��ȵõ�a��
 *			��ȡ��ҷʱ��·�����ļ������ȵõ�b��b��a�õ���ȥ�ļ�����·������c��
 *			�����ݹ�ʱ���ó���c
 */
UINT __stdcall anlySpeString(void *argv) {
	/*�����ڲ�����*/
	int iFuncStat = 0;						//����״̬
	int temp_i = 0;							//��ʱ���
	char msgType = 0;						//��Ϣ����
	UINT uSize = 0;							//�ļ�������
	UINT numOfFiles = 0;					//��ҷ�ļ�����
	TCHAR msg_t[256] = { NULL };			//������Ϣ
	TCHAR filename[256] = { NULL };			//�ļ���
	TCHAR tempmsg_t[256] = { NULL };		//��ʱ���
	WPARAM wParam = (WPARAM)argv;			//����DragQueryFile����
	__int64 fileHandle = 0;					//�ļ����
	struct _tfinddata_t FileInfo;			//�ļ���Ϣ�ṹ��
	/*ȫ�ֱ�������,ע����Դ����*/
	globalDirIndex = 0;
	globalFileIndex = 0;

	/*�ļ�������ʼ*/
	//
	_stprintf_s(msg_t, _countof(msg_t), _T("anlySpeString"));
	//SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)msg_t);
	//WideCharToMultiByte();
	//
	numOfFiles = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, NULL, 0);		//�ж���ҷ�ļ�����
	for (int i = 0; i < (int)numOfFiles; ++i) {
		//ȡ����i���ļ�
		uSize = DragQueryFile((HDROP)wParam, i, filename, _countof(filename));
		if (0 == uSize || _countof(filename) == uSize) {
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(_T("anlySpeString err!")));
			_endthreadex(0);
			return -1;
		}
		//ȥǰ׺,���±�
		getFileName_t(filename, (int)_tcslen(filename), tempmsg_t, &temp_i);
		temp_i = (int)_tcslen(filename) - temp_i;			//��ʼĿ¼���� ע1���ں���˵�������н���

		/*����ļ�����*/
		fileHandle = _tfindfirst(filename, &FileInfo);											//�ؼ�
		if (-1L == fileHandle) {
			SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(_T("err path in anlySpeString!")));
			resetGlobal();
			_endthreadex(0);
			return -1;
		}
		if (_A_SUBDIR == (_A_SUBDIR & FileInfo.attrib)) {
			_stprintf_s(globalAllOfDir_t[globalDirIndex], sizeof(nameStore), filename);//����·�����ļ��������Ƶ�ȫ�ֱ�����
			/*��ȥ����ʼĿ¼���·�����Ƶ��µ�������*/
			CopyMemory(&(globalWithoutPath_dir_t[globalDirIndex][0]), &((globalAllOfDir_t[globalDirIndex])[temp_i]), 2 * (_tcslen(globalAllOfDir_t[globalDirIndex]) - temp_i + 1));	//��һ��Ϊ�˽�'\0'Ҳ���ƹ�ȥ
			globalDirIndex += 1;
			if (globalDirIndex >= _countof(globalAllOfDir_t)) {
				_stprintf_s(msg_t, _countof(msg_t), _T("out of dirstore"));
				SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
				_findclose((intptr_t)fileHandle);					//�رվ��
				resetGlobal();
				_endthreadex(0);
				return -1;								//����Ԥ���ڴ棬�˳�
			}

			if (-1 == anlyDirAndFile(filename, temp_i)) {
				_findclose((intptr_t)fileHandle);					//�رվ��
				resetGlobal();
				_endthreadex(0);
				return -1;
			}
		}
		else {
			_stprintf_s(globalAllOfFile_t[globalFileIndex], sizeof(nameStore), filename);
			/*��ȥ����ʼĿ¼���·�����Ƶ��µ�������*/
			CopyMemory(&(globalWithoutPath_file_t[globalFileIndex][0]), &((globalAllOfFile_t[globalFileIndex])[temp_i]), 2 * (_tcslen(globalAllOfFile_t[globalFileIndex]) - temp_i + 1));	//��һ��Ϊ�˽�'\0'Ҳ���ƹ�ȥ
			globalFileIndex += 1;

			if (globalFileIndex >= _countof(globalAllOfFile_t)) {
				_stprintf_s(msg_t, _countof(msg_t), _T("out of filestore"));
				SendMessage(hListBox_Show, LB_INSERTSTRING, 0, (LPARAM)(msg_t));
				_findclose((intptr_t)fileHandle);					//�رվ��
				resetGlobal();
				_endthreadex(0);
				return -1;								//����Ԥ���ڴ棬�˳�
			}
			continue;
		}
		_findclose((intptr_t)fileHandle);							//�رվ��
	}
	DragFinish((HDROP)wParam);							//��ҷ�ļ�ʱ���ɵľ�����ر�
	/*���ϲ��裬�ļ��������*/
	globalFileStat = 2;									//����״̬Ϊ������

	EnableWindow(hButton_Close, FALSE);					//��ʼͨ����Ҫ����close��ť
	for (int i = 0; i < globalDirIndex; ++i) {

		msgType = SENDDIRREQUEST;
		globalReady = 0;
		send(stSock, &msgType, 1, 0);					//֪ͨ�Է����գ������յ��Է�����������Ϣ�󣬹ر�֮ǰ�ļ����̣߳��ɴ��߳̿�ʼͨ��
		while (1) {
			switch (globalReady) {						//�ж����֮��,�����¸�ֵ0
			case 0: {
				//�˴����쳣��ֱ����continue�����
				Sleep(10);
				continue;
			}break;
			case 3: {									//�ļ�����ȷ��
				globalReady = 0;
				msgType = 0;

				iFuncStat = sendDir_t(globalWithoutPath_dir_t[i]);	//�����ļ���
				useThread();							//���¿����߳̽��м���
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
			switch (globalReady) {						//�ж����֮��,�����¸�ֵ0
			case 0: {
				//�˴����쳣��ֱ����continue�����
				Sleep(10);
				continue;
			}break;
			case 2: {									//�ļ�����ȷ��
				globalReady = 0;
				msgType = 0;

				iFuncStat = sendFile_t(sst_msg);					//�����ļ�
				useThread();							//���¿����߳̽��м���
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
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
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
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

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
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE: {
		/*��ȡϵͳ��Ϣ*/
		myGetSysInfo(hWnd);
		/*���ƿؼ�*/
		drawControl(hWnd, message, wParam, lParam);
		/*��ʼ���ؼ�*/
		initControl();
		/*��ȡ�����ļ�*/
		initFileJudge();
	}break;
	case WM_DROPFILES: {
		myDragFile(hWnd, message, wParam, lParam);
	}break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);			//����ID��ʶ��
		int wmMsg = HIWORD(wParam);			//��Ϣ����
		// �����˵�ѡ��: 
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
		// TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
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