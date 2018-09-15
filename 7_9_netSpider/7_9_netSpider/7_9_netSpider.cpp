// 7_9_netSpider.cpp : 定义控制台应用程序的入口点。
//

/*
 *作者：ZMX		此注解更新时间：2018-7-26
 *
 *关于此程序的一点心得
 *
 *一：采用的技术
 *	1.dll编写与dll显式调用
 *	2.socket套接字调用
 *	3.http协议报文格式
 *	4.TLS报文格式 （还未实现
 *
 *二：注意事项
 *	1.字符串或者是有些申请的内存或者是结构体，用ZeroMemory置0来保证安全
 *	2.文件打开方式--'t'`'b'， 熟练运用可以避免换行和回车问题
 *	3.设置recv（阻塞）超时避免等待时间过长
 *	4.若是以二进制方式处理字符，注意越界问题（char->UCHAR)
 *
 *三：印象比较深的函数
 *	1.ZeroMemory
 *	2.CopyMemory
 *	3.setsockopt
 *	4.getaddrinfo
 *	5.ftell
 *	6.fseek
 *	7._countof
 *	8.time
 *	9.CreateDirectory
 *
 *四：此程序结构如下
 *	1.动态链接库载入
 *	2.套接字创建
 *	3.URL分析
 *	4.DNS解析（域名转IP
 *	5.建立TCP连接
 *	6.发送http请求
 *	7.接收响应体
 *	8.分离响应头和主体
 *	9.分析响应头
 *	10.分析响应主体
 *	11.关闭文件，关闭TCP
 *
 */

#include "stdafx.h"

#pragma comment(lib, "ws2_32.lib")			//socket库

#define SIZE_URL		1024
#define	SIZE_PROTOCOL	8
#define SIZE_DOMAIN		64
#define SIZE_PORT		8
#define SIZE_DIR		128
#define SIZE_FILE		64
#define SIZE_ARGV		128
#define SIZE_ANCHOR		64	

#define RECV_NUM		1024				//一次recv字节数

typedef int(*aaf)(char *, char *, char *);	//文件名分离
typedef int(*aua)(char *, int, char *);		//URL处理

typedef int(*CTT)(char *, size_t, TCHAR *);	//char to TCHAR
typedef int(*GCH)(UCHAR *, int *);			//STL ClientHello

typedef int(*DTI)(char *, char *, SOCKADDR);

int _tmain(int argc, TCHAR **argv)
{
	//char mmmm[8];
	//TCHAR tttt[8];
	//sprintf_s(mmmm, _countof(mmmm), "ppp");
	//((CTT)(ctt))(mmmm, 3, tttt);
	//CreateDirectory(tttt, NULL);
	////printf(mmmm);
	//printf("%d\n", argc);
	//_tprintf(_T("%s\n"), argv[0]);
	//printf("%s\n", argv[0]);
	//system("pause");
	//exit(1);

	/*
	 *日志文件相关
	 *
	 */
	time_t time_begin=0;//开始时间
	time_t time_now=0;	//当前时间
	SOCKET cSock;		//套接字
	TCHAR msg2[256];
	char log_m[256];	//日志字符串
	FILE *logfile;		//日志文件
	
	time(&time_begin);	//开始计时
	fopen_s(&logfile, "record.log", "at");	//打开日志文件
	if (NULL == logfile) {
		printf("logfile err\n");
		system("pause");
		exit(1);
	}
	sprintf_s(log_m, 256, "---\n");
	fwrite(log_m, sizeof(char), strlen(log_m), logfile);	//日志记录

	/*
	 *动态链接库载入
	 *使用方式形如
	 *((aaf)(A_af))(file_in, file_name, file_suffix);
	 */
	HMODULE funAddr = LoadLibrary(_T("7-20-anlysis_file_name"));	//dll基址
	FARPROC A_af = GetProcAddress(funAddr, "A_name_suffix");		//分离文件名
	FARPROC A_ua = GetProcAddress(funAddr, "URL_anly");				//URL截取
	FARPROC ctt = GetProcAddress(funAddr, "CharToTchar");			//char转TCHAR
	FARPROC gch = GetProcAddress(funAddr, "getClientHello");		//STL ClientHello
	//FARPROC dti = GetProcAddress(funAddr, "domainToIp");			//DNS
	if (NULL == funAddr) {
		printf("loadlibrary err\n");
		system("pause");
		exit(1);
	}
	if (NULL == A_af || NULL == A_ua || NULL == ctt || NULL == gch) {
		printf("getprocaddr err\n");
		system("pause");
		exit(1);
	}

	//dll test
	//sockaddr caddr2;
	//ZeroMemory(&caddr2, 16);
	//int ssss=((DTI)(dti))("www.baidu.com\0", "80\0", caddr2);
	////((sockaddr_in *)caddr2)->sin_addr.S_un.S_addr;
	//printf("%s\n", caddr2.sa_data);
	//

	//system("pause");
	//exit(1);

	/*
	*socket初始化
	*socket创建
	*/
	//socket初始化
	WORD sockVer = MAKEWORD(2, 2);
	WSADATA wsadata;
	if (WSAStartup(sockVer, &wsadata) != 0) {
		_stprintf_s(msg2, 256, _T("GetLastError:%lu, errno:%d"), GetLastError(), errno);
		printf("WSAStartup error\n");
		_tprintf_s(msg2);
		fclose(logfile);
		system("pause");
		exit(1);
	}
	//创建socket
	cSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);		//return和exit之前要关闭cSock
	if (INVALID_SOCKET == cSock) {
		_stprintf_s(msg2, 256, _T("GetLastError:%lu, errno:%d"), GetLastError(), errno);
		printf("socket error\n");
		_tprintf_s(msg2);
		WSACleanup();
		fclose(logfile);
		system("pause");
		exit(1);
	}

	//URL注意溢出问题
	printf("分析URL\n");
	char full_URL[SIZE_URL];		//URL字符串'\0'结尾
	sprintf_s(full_URL, SIZE_URL, "http://www.ruanyifeng.com/blog/2018/04/weekly-issue-2.html");

	/*
	*分析URL
	*取出协议
	*取出域名
	*取出端口
	*取出虚拟目录
	*取出文件名
	*取出参数
	*取出锚点
	*/
	char protocol[SIZE_PROTOCOL];	//协议
	char domain[SIZE_DOMAIN];		//域名
	char port2[SIZE_PORT];			//端口
	char dir[SIZE_DIR];				//虚拟目录
	char file2[SIZE_FILE];			//文件名
	char argv2[SIZE_ARGV];			//参数
	char anchor[SIZE_ANCHOR];		//锚点

	((aua)(A_ua))(full_URL, 1, protocol);
	((aua)(A_ua))(full_URL, 2, domain);
	((aua)(A_ua))(full_URL, 3, port2);
	((aua)(A_ua))(full_URL, 4, dir);
	((aua)(A_ua))(full_URL, 5, file2);
	((aua)(A_ua))(full_URL, 6, argv2);
	((aua)(A_ua))(full_URL, 7, anchor);

	/*
	*端口去冒号
	*DNS处理
	*建立连接
	*设置接收超时
	*/
	char nport2[8];		//去冒号端口
	if (NULL == port2) {
		nport2[0] = '8';
		nport2[1] = '0';
		nport2[2] = '\0';
	}
	else {
		for (int i = 0; port2[i] != '\0'; ++i) {
			nport2[i] = port2[i + 1];
		}
	}
	//临时定义 可删
	//BOOL https_t = TRUE;
	nport2[0] = '8';
	nport2[1] = '0';
	nport2[2] = '\0';
	//nport2[3] = '\0';

	//char https_msg[256];
	//域名转ip
	ADDRINFOA ai;
	ZeroMemory(&ai, sizeof(ai));
	ai.ai_socktype = SOCK_STREAM;
	ai.ai_family = AF_INET;
	PADDRINFOA pai;
	if (getaddrinfo(domain, nport2, &ai, &pai) != 0) {
		char log_m[256];
		sprintf_s(log_m, _countof(log_m), "DNS err\n");
		fwrite(log_m, sizeof(char), strlen(log_m), logfile);
		printf("域名转ip失败\n");
		system("pause");
		exit(1);
	}
	else
	{
		char log_m[256];
		sprintf_s(log_m, _countof(log_m), "域名转ip完成\n");
		fwrite(log_m, sizeof(char), strlen(log_m), logfile);	//日志记录
		sprintf_s(log_m, _countof(log_m), "ip:%u.%u.%u.%u\n", ((UCHAR *)(pai->ai_addr->sa_data))[2], 
			((UCHAR *)(pai->ai_addr->sa_data))[3], ((UCHAR *)(pai->ai_addr->sa_data))[4], ((UCHAR *)(pai->ai_addr->sa_data))[5]);
		fwrite(log_m, sizeof(char), strlen(log_m), logfile);	//日志记录
		printf("域名转ip完成\n");
	}
	
	/*
	 *ADDRINFOA中地址和端口位置
	 *在ai_addr->sa_data字符串中(14位长度)
	 *前两位是端口80（内部存储顺序是 0x00 0x50）
	 *接着四位是地址128.1.83.59（内部顺序是 0x80 0x01 0x53 0x3b）
	 *后八位都是0
	 *
	 */
	//建立连接
	sockaddr caddre;
	ZeroMemory(&caddre, sizeof(caddre));
	((sockaddr_in *)&caddre)->sin_addr.S_un.S_addr = ((sockaddr_in *)(pai->ai_addr))->sin_addr.S_un.S_addr;
	((sockaddr_in *)&caddre)->sin_port = ((sockaddr_in *)(pai->ai_addr))->sin_port;
	caddre.sa_family = (pai->ai_addr)->sa_family;
	if (SOCKET_ERROR == connect(cSock, &caddre, sizeof(caddre))) {
		_stprintf_s(msg2, 256, _T("GetLastError:%lu, errno:%d"), GetLastError(), errno);
		printf("connect error\n");
		_tprintf_s(msg2);
		shutdown(cSock, SD_SEND);
		closesocket(cSock);
		WSACleanup();
		fclose(logfile);
		system("pause");
		exit(1);
	}
	printf("连接完成\n");

	int time_out_recv = 2000;	//超时时间（毫秒）
	setsockopt(cSock, SOL_SOCKET, SO_RCVTIMEO, (char *)&time_out_recv, sizeof(int));		//设置接收超时返回

	/*
	 *创建文件夹
	 *record记录文件
	 *ssl协商
	 *
	 *一：协商流程
	 *	1.client hello	to server
	 *	2.server hello	to client
	 *	3.
	 */
	//CreateDirectory(_T("STL_DATA"), NULL);
	//time(&time_now);
	//FILE *file_STL = NULL;				//全部记录
	//FILE *file_certificate = NULL;		//证书部分
	//char stl_data_m[128];

	//sprintf_s(stl_data_m, _countof(stl_data_m), "STL_DATA/%llu.tlsRecord", time_now);
	//fopen_s(&file_STL, stl_data_m, "ab+");
	//sprintf_s(stl_data_m, _countof(stl_data_m), "STL_DATA/%llu.cer", time_now);
	//fopen_s(&file_certificate, stl_data_m, "ab+");
	//if (NULL == file_STL) {
	//	printf("stl file err\n");
	//	system("pause");
	//	exit(1);
	//}
	//if (NULL == file_certificate) {
	//	printf("certificate file err\n");
	//	system("pause");
	//	exit(1);
	//}

	//UCHAR stl_m[256];
	//UCHAR recv_m[RECV_NUM];
	//BOOL con = FALSE;
	//int stl_m_l[1];
	//size_t stl_total = 0;
	//ZeroMemory(stl_m, 256);
	//((GCH)(gch))(stl_m, stl_m_l);		//获取clientHello报文

	//send(cSock, (char *)(void *)stl_m, stl_m_l[0], 0);
	//int recv_l = recv(cSock, (char *)(void *)recv_m, _countof(recv_m), 0);	//recv server hello/certificate/hello done
	//while (recv_l > 0) {
	//	printf("recv %d\n", recv_l);
	//	stl_total += recv_l;
	//	for (int i = 0; i < recv_l; ++i) {
	//		if (0 == i) {
	//			fwrite(recv_m, sizeof(UCHAR), recv_l, file_STL);
	//			if (RECV_NUM == recv_l) {
	//				con = TRUE;
	//				fwrite(recv_m, sizeof(UCHAR), recv_l, file_certificate);
	//			}
	//			else if (TRUE == con) {
	//				con = FALSE;
	//				fwrite(recv_m, sizeof(UCHAR), recv_l, file_certificate);
	//			}
	//		}
	//		printf("%02x", recv_m[i]);
	//		if (0 == (i + 1) % 8) {
	//			printf("\n");
	//		}
	//	}
	//	recv_l = recv(cSock, (char *)(void *)recv_m, _countof(recv_m), 0);	
	//}
	//printf("total %u\n", stl_total);
	//fclose(file_STL);			//关闭
	//fclose(file_certificate);

	//printf("\n");
	//system("pause");
	//fclose(logfile);
	//shutdown(cSock, SD_SEND);	//关闭
	//closesocket(cSock);
	//WSACleanup();
	//exit(1);
	/*
	 *存储文件准备
	 *
	 */
	//新建文件保存数据
	char datafile[32];
	TCHAR tdatafile[32];
	ZeroMemory(tdatafile, 32);
	sprintf_s(datafile, 32, "data");
	((CTT)(ctt))(datafile, strlen(datafile), tdatafile);
	CreateDirectory(tdatafile, NULL);	//创建文件夹
	char note_by_time_file[32];			//文件命名

	FILE *fo = NULL;		//记录通信
	FILE *fo_URL = NULL;	//存储分离出的URL
	FILE *fo_res = NULL;	//去除响应头的响应体

	time(&time_now);
	sprintf_s(note_by_time_file, _countof(note_by_time_file), "%s/%lld.txt", datafile, time_now);
	fopen_s(&fo, note_by_time_file, "wt+");
	sprintf_s(note_by_time_file, _countof(note_by_time_file), "%s/%lldURL.txt", datafile, time_now);
	fopen_s(&fo_URL, note_by_time_file, "wt+");
	if (NULL == file2) {
		sprintf_s(note_by_time_file, _countof(note_by_time_file), "%s/%lld.html", datafile, time_now);
	}
	else {
		char file_na[256];
		char file_su[256];
		((aaf)(A_af))(file2, file_na, file_su);
		sprintf_s(note_by_time_file, _countof(note_by_time_file), "%s/%lld.%s", datafile, time_now, file_su);
	}
	fopen_s(&fo_res, note_by_time_file, "wb+");
	if (NULL == fo) {
		printf("faile while open logfile\n");
		system("pause");
	}
	if (NULL == fo_URL) {
		printf("faile while open fo_URL\n");
		system("pause");
	}
	if (NULL == fo_res) {
		printf("faile while open fo_res\n");
		system("pause");
	}

	/*
	*请求头初始化
	*发送请求头
	*接收响应数据
	*分离响应体
	*/
	//方案三 无referer·无压缩
	char get_info[2048];		//
	char send_head[128];		//请求头
	char send_host[64];			//host字段
	char send_agent[128];		//agent字段
	char send_language[128];	//language
	char send_encoding[32];		//压缩方式
	char send_connection[32];	//连接方式
	char send_cache[32];		//缓存
	char send_ending[8];		//结束标志

	
	sprintf_s(send_head, 128, "GET /%s%s HTTP/1.1\r\n", dir, file2);
	sprintf_s(send_host, 64, "Host: %s\r\n", domain);
	sprintf_s(send_agent, 128, "User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:60.0) Gecko/20100101 Firefox/60.0\r\nAccept: */*\r\n");
	sprintf_s(send_language, 128, "Accept-Language: zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2\r\n");
	sprintf_s(send_encoding, 32, "Accept-Encoding:identity\r\n");
	sprintf_s(send_connection, 32, "Connection: keep-alive\r\n");
	sprintf_s(send_cache, 32, "Cache-Control: max-age=0\r\n");
	sprintf_s(send_ending, 8, "\r\n");

	sprintf_s(get_info, _countof(get_info), "%s%s%s%s%s%s%s%s", send_head, send_host, send_agent, send_language, send_encoding, send_connection, send_cache, send_ending);
	printf("发送请求头\n");
	size_t p = send(cSock, get_info, (int)(strlen(get_info)), 0);

	unsigned long long totalData = 0;		//接收总字节数
	unsigned long long actualData = 0;		//除去响应头的剩余部分
	if (strlen(get_info) == p) {
		printf("接收响应数据\n");
		BOOL isResouse = TRUE;
		char p2[RECV_NUM];			//存放接收的字节
		int s;						//实际接收字节数
		s = recv(cSock, p2, RECV_NUM, 0);
		if (-1 == s) {
			sprintf_s(log_m, _countof(log_m), "recv err:%llu\n", GetLastError());
			fwrite(log_m, sizeof(char), strlen(log_m), logfile);	//日志记录
			printf("recv err\n");
			system("pause");
			exit(1);
		}
		totalData += s;
		for (int x = 0; x < s; ++x) {
			if (0 == actualData) {
				if ('\r' == p2[x] && '\n' == p2[x + 1] && '\r' == p2[x + 2] && '\n' == p2[x + 3]) {
					actualData += (s - 4 - x);		//接收总字节数-响应头字节数
					if (TRUE == isResouse) {
						x += 3;
						continue;
					}
					else {
						break;
					}
				}
			}
			else {
				fwrite(&(p2[x]), sizeof(char), 1, fo_res);
			}
		}
		printf("分离响应头完毕\n");
		while (s) {
			fwrite(p2, sizeof(char), s, fo);

			s = recv(cSock, p2, RECV_NUM, 0);
			if (-1 == s) {
				s = 0;
			}
			if (TRUE == isResouse) {
				fwrite(p2, sizeof(char), s, fo_res);
			}
			totalData += s;
			actualData += s;
			printf(".");
		}
	}
	printf("\n\n");
	printf("totalData:%llu, actualData:%llu\n", totalData, actualData);
	//日志记录
	sprintf_s(log_m, _countof(log_m), "begin time:%lld, total size:%llu, data size:%llu\n", time_begin, totalData, actualData);
	fwrite(log_m, sizeof(char), strlen(log_m), logfile);	//日志记录
	printf("接收完毕\n");

	/*
	*分析状态码
	*提取URL
	*处理重定向
	*处理异常
	*
	*/
	printf("开始清洗数据\n");

	int statCode = 0;		//响应状态码
	char cc = 'e';			//字节读取
							//注：并未做读取检测，可能存在错误
	if (fseek(fo, (long)0, SEEK_SET) != 0) {
		printf("fseek err\n");
		system("pause");
		exit(1);
	}
	fread(&cc, sizeof(char), 1, fo);
	while (0 == feof(fo)) {
		if (0 == statCode) {
			if (' ' == cc) {
				fread(&cc, sizeof(char), 1, fo);
				if (feof(fo) != 0) {
					break;
				}
				statCode += ((int)cc - 48) * 100;
				fread(&cc, sizeof(char), 1, fo);
				if (feof(fo) != 0) {
					break;
				}
				statCode += ((int)cc - 48) * 10;
				fread(&cc, sizeof(char), 1, fo);
				if (feof(fo) != 0) {
					break;
				}
				statCode += ((int)cc - 48);
				break;
			}
		}
		fread(&cc, sizeof(char), 1, fo);
		if (feof(fo) != 0) {
			break;
		}
	}
	printf("\n");
	switch (statCode) {
	case 200: {
		char url_m[256];
		sprintf_s(url_m, _countof(url_m), ":%s\n", protocol);
		fwrite(url_m, sizeof(char), strlen(url_m), fo_URL);
		sprintf_s(url_m, _countof(url_m), "/:%s://%s%s\n", protocol, domain, port2);
		fwrite(url_m, sizeof(char), strlen(url_m), fo_URL);
		printf("有效的数据-状态码：%d\n", statCode);
		if (fseek(fo, (long)0, SEEK_SET) != 0) {
			printf("fseek err\n");
			system("pause");
		}
		//筛选URL href,src 只筛选双引号
		cc = 0;
		long fileCur;	//文件指针位置ftell()
		fread(&cc, sizeof(char), 1, fo);
		if (feof(fo) != 0) {
			break;
		}
		while (0 == feof(fo)) {
			if ('h' == cc) {
				fileCur = ftell(fo);
				fread(&cc, sizeof(char), 1, fo);
				if (feof(fo) != 0) {
					break;
				}
				if ('r' == cc) {
					fread(&cc, sizeof(char), 1, fo);
					if (feof(fo) != 0) {
						break;
					}
					if ('e' == cc) {
						fread(&cc, sizeof(char), 1, fo);
						if (feof(fo) != 0) {
							break;
						}
						if ('f' == cc) {
							fread(&cc, sizeof(char), 1, fo);
							if (feof(fo) != 0) {
								break;
							}
							//优化 空格处理
							//if ('=' == cc || ' '==cc) {
							//	fread(&cc, sizeof(char), 1, fo);
							//	if (feof(fo) != 0) {
							//		break;
							//	}
							int max_space = 0;
							while (max_space < 2) {
								if (' ' == cc) {
									max_space += 1;
									fread(&cc, sizeof(char), 1, fo);
									if (feof(fo) != 0) {
										break;
									}
									continue;
								}
								else if ('=' == cc) {
									max_space = 2;
									fread(&cc, sizeof(char), 1, fo);
									if (feof(fo) != 0) {
										break;
									}
								}
								else {
									break;
								}

								if ('\"' == cc) {
									int maxLen = 0;
									fread(&cc, sizeof(char), 1, fo);
									if (feof(fo) != 0) {
										break;
									}
									while (cc != '\"') {
										maxLen += 1;
										fwrite(&cc, sizeof(char), 1, fo_URL);
										fread(&cc, sizeof(char), 1, fo);
										if (feof(fo) != 0) {
											break;
										}
										if (maxLen > 128) {		//异常
											fseek(fo, fileCur, 0);
											break;
										}
									}
									cc = '\n';
									fwrite(&cc, sizeof(char), 1, fo_URL);
									fread(&cc, sizeof(char), 1, fo);
									if (feof(fo) != 0) {
										break;
									}
									continue;
								}
							}
						}
					}
				}
				fseek(fo, fileCur, 0);
				fread(&cc, sizeof(char), 1, fo);
				if (feof(fo) != 0) {
					break;
				}
				continue;
			}
			else if ('s' == cc) {
				fileCur = ftell(fo);
				fread(&cc, sizeof(char), 1, fo);
				if (feof(fo) != 0) {
					break;
				}
				if ('r' == cc) {
					fread(&cc, sizeof(char), 1, fo);
					if (feof(fo) != 0) {
						break;
					}
					if ('c' == cc) {
						fread(&cc, sizeof(char), 1, fo);
						if (feof(fo) != 0) {
							break;
						}
						if ('=' == cc) {
							fread(&cc, sizeof(char), 1, fo);
							if (feof(fo) != 0) {
								break;
							}
							if ('\"' == cc) {
								int maxLen = 0;
								fread(&cc, sizeof(char), 1, fo);
								if (feof(fo) != 0) {
									break;
								}
								while (cc != '\"') {
									maxLen += 1;
									fwrite(&cc, sizeof(char), 1, fo_URL);
									fread(&cc, sizeof(char), 1, fo);
									if (feof(fo) != 0) {
										break;
									}
									if (maxLen > 128) {		//异常
										fseek(fo, fileCur, 0);
										break;
									}
								}
								cc = '\n';
								fwrite(&cc, sizeof(char), 1, fo_URL);
								fread(&cc, sizeof(char), 1, fo);
								if (feof(fo) != 0) {
									break;
								}
								continue;
							}
						}
					}
				}
				fseek(fo, fileCur, 0);
				fread(&cc, sizeof(char), 1, fo);
				if (feof(fo) != 0) {
					break;
				}
				continue;
			}
			else {
				fread(&cc, sizeof(char), 1, fo);
				if (feof(fo) != 0) {
					break;
				}
				continue;
			}
		}
	}break;
	case 302: {
		printf("URL已被重定向-状态码：%d\n", statCode);
	}break;
	case 300:
	case 301:
	case 303:
	case 304:
	case 307:
	case 308: {
		printf("URL已被重定向-状态码：%d\n", statCode);
	}break;
	case 400: {
		printf("错误的请求-状态码：%d\n", statCode);
	}break;
	default: {
		printf("暂未设置处理方式-状态码：%d\n", statCode);
	}
	}

	sprintf_s(log_m, _countof(log_m), "状态码：%d\nURL:%s\n", statCode, full_URL);
	fwrite(log_m, sizeof(char), strlen(log_m), logfile);	//日志记录
	printf("数据清洗完毕\n");

	printf("清理内存\n");
	fclose(fo);
	fclose(fo_URL);
	fclose(fo_res);
	fclose(logfile);
	shutdown(cSock, SD_SEND);	//关闭
	closesocket(cSock);
	WSACleanup();
	printf("清理结束\n");

	system("pause");
    return 0;
}


//方案零 默认的请求文件
//sprintf_s(get_info, 256, "GET /index.html HTTP/1.1\r\nHost:%s\r\nConnection: Keep-Alive\r\n\r\n");
//方案一 极简
//	sprintf_s(get_info, 2048, "GET /wolfplan/p/6598583.html HTTP/1.1\r\nConnetion: Keep-Alive\r\n\r\n");
//方案二 带referer·无压缩
//	sprintf_s(get_info, 2048, "GET /res/delivery/0001/2018/07/06/201807061151261281.jpg HTTP/1.1\r\n\
//Host: ad.12306.cn\r\n\
//User - Agent : Mozilla / 5.0 (Windows NT 6.1; Win64; x64; rv:60.0) Gecko / 20100101 Firefox / 60.0\r\n\
//Accept : */*\r\n\
//Accept-Language: zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2\r\n\
//Accept-Encoding: identity\r\n\
//Referer: http://ad.12306.cn/\r\n\
//Connection: keep-alive\r\n\
//\r\n\r\n");

//https://pic002.cnblogs.com/images/2011/270176/2011092909392629.jpg
//https://blog.csdn.net/huangxy10/article/details/8120106
//https://www.cnblogs.com/wolfplan/p/6598583.html
//https://www.cnblogs.com/ranyonsue/p/5984001.html
//https://www.cnblogs.com/caoheyang911016/p/4110645.html
//http://www.12306.cn/mormhweb/
//https://ad.12306.cn/res/delivery/0001/2018/07/02/201807021522567470.jpg
//https://ad.12306.cn/res/delivery/0001/2018/07/06/201807061151261281.jpg
//ETag	"5b3ee790-4dd9"
//https://www.dahuatech.com/bocweb/web/img/logo.png
//http://www.sjiyou.com/
//https://docs.microsoft.com/zh-cn/windows/desktop/api/ws2tcpip/nf-ws2tcpip-getaddrinfo
//http://www.cnblogs.com/piyeyong/archive/2010/07/02/1770208.html
//https://pic002.cnblogs.com/img/piyeyong/201007/2010070220001133.png
//http://www.cnblogs.com/piyeyong/rss

////人为截断
//fclose(logfile);
//printf("protocol:%s\ndomain:%s\nport:%s\ndir:%s\nfile:%s\n", protocol, domain, "80", dir, file2);
//printf("%u, %u, %u, %u\n", index, index_end, port_index, port_index_end);
//system("pause");

//char node_name[32];
////sprintf_s(node_name, 32, "www.baidu.com");
//sprintf_s(node_name, 32, "www.sjiyou.com");
//char port_name[8];
//sprintf_s(port_name, 8, "80");

//旧URL分析程序
/*
BOOL analysis_success = TRUE;
BOOL find_protocol = FALSE;
BOOL find_domain = FALSE;
BOOL find_port = FALSE;
BOOL find_dir = FALSE;
BOOL find_file = FALSE;

UINT index = 0;
UINT index_end = 0;
UINT port_index = 0;
UINT port_index_end = 0;
UINT dir_index = 0;
UINT dir_b = 0;
UINT dir_e = 0;
UINT file_index = 0;

char protocol[SIZE_PROTOCOL];
char domain[SIZE_DOMAIN];
char port2[SIZE_PORT];
char dir[SIZE_DIR];
char file2[SIZE_FILE];
char argv2[SIZE_ARGV];
char anchor[SIZE_ANCHOR];

for (int i = 0;full_URL[i]!='\0'; ++i) {
	//取出协议
	if (FALSE == find_protocol) {
		protocol[i] = full_URL[i];
		if (':' == full_URL[i]) {
			protocol[i] = NULL;
			find_protocol = TRUE;
			printf("取出协议\n");
		}
		else {
			if (i >= SIZE_PROTOCOL) {	//溢出
				char log_m[256];
				sprintf_s(log_m, "protocol err\n");
				fwrite(log_m, sizeof(char), strlen(log_m), logfile);
				analysis_success = FALSE;
				break;
			}
			continue;
		}
	}
	//取出域名 取出端口
	if (FALSE == find_domain) {
		if (0 == index) {
			if ('/' == full_URL[i]) {
				if (full_URL[i + 1] != '\0'&&'/' == full_URL[i + 1]) {
					index = i + 1;
					i += 1;
					continue;
				}
				else {
					char log_m[256];
					sprintf_s(log_m, "cannot find \"//\"");
					fwrite(log_m, sizeof(char), strlen(log_m), logfile);
					analysis_success = FALSE;
					break;
				}
			}
		}
		else if (0 == index_end) {
			domain[i - index - 1] = full_URL[i];
			if (':' == full_URL[i]) {
				domain[i - index - 1] = NULL;
				index_end = i;
				find_domain = TRUE;
				printf("取出域名\n");
				i += 1;
				for (int j = i; j < i + 5; ++j) {
					port2[port_index] = full_URL[j];
					if ('\0' != full_URL[j]) {
						if ('/' == full_URL[j]) {
							port2[port_index] = NULL;
							port_index_end = j;
							find_port = TRUE;
							printf("取出端口\n");
							i = j;
							break;
						}
						else {
							port_index += 1;
						}
					}
					else {
						break;
					}
				}
				if (TRUE == find_port) {
					continue;
				}
				else {
					char log_m[256];
					sprintf_s(log_m, "cannot find \"port\"");
					fwrite(log_m, sizeof(char), strlen(log_m), logfile);
					analysis_success = FALSE;
					break;
				}
			}
			else if ('/' == full_URL[i]) {
				domain[i - index - 1] = NULL;
				index_end = i;
				find_domain = TRUE;
				printf("取出域名\n");
				continue;
			}
			else {
				if ((UINT)i >= index + SIZE_DOMAIN) {	//溢出
					char log_m[256];
					sprintf_s(log_m, "domain err\n");
					fwrite(log_m, sizeof(char), strlen(log_m), logfile);
					analysis_success = FALSE;
					break;
				}
				continue;
			}
		}
		continue;
	}
	//取出虚拟目录 文件名 需要优化！！！！！！
	if (FALSE == find_dir) {
		dir_b = i - 1;
		for (int j = i; full_URL[j] != '\0'; ++j) {
			if ('/' == full_URL[j]) {
				dir_e = j;
				for (int k = j + 1; full_URL[k] != '\0'; ++k) {
					if ('/' == full_URL[k]) {
						j = k;
						dir_e = k;
					}
					else {
						j = k;
						continue;
					}
				}
			}
			else {
				continue;
			}
		}
		find_dir = TRUE;
		printf("取出虚拟目录\n");
		if (0 == dir_e) {
			dir[0] = '\0';
			i -= 1;
			continue;
		}
		else {
			if ((dir_e - dir_b) > SIZE_DIR|| (dir_e - dir_b)<0) {
				char log_m[256];
				sprintf_s(log_m, "dir err\n");
				fwrite(log_m, sizeof(char), strlen(log_m), logfile);
				analysis_success = FALSE;
				break;
			}
			else {
				for (UINT l = dir_b, m=0; l <= dir_e; ++l) {
					dir[m] = full_URL[l];
					m += 1;
				}
			}
			i = dir_e;
			continue;
		}
	}
	if (FALSE == find_file) {
		file2[file_index] = full_URL[i];
		if ('?' == full_URL[i]||'#'==full_URL[i]) {
			file2[file_index] = '\0';
			find_file = TRUE;
			printf("取出文件名%s\n", file2);
			continue;
		}
		else if('\0' == full_URL[i + 1]){
			file2[file_index+1] = '\0';
			find_file = TRUE;
			printf("取出文件名%s\n", file2);
			continue;
		}
		else {
			file_index += 1;
			if (file_index >= SIZE_FILE - 1) {//溢出
				char log_m[256];
				sprintf_s(log_m, "file err\n");
				fwrite(log_m, sizeof(char), strlen(log_m), logfile);
				analysis_success = FALSE;
				break;
			}
			continue;
		}
	}
	//取出参数
	//取出锚点
}

if (FALSE == find_port) {
	sprintf_s(port2, 8, "80");
}
if (0 == dir_b) {
	dir[0]='\0';
}
if (FALSE == find_file) {
	;
}
printf("分析完毕\n");
//分析完毕
sprintf_s(log_m, 256, "URL处理完毕\n");
fwrite(log_m, sizeof(char), strlen(log_m), logfile);
if (FALSE == analysis_success) {
	printf("err while analysis\n");
	system("pause");
}
*/
//旧请求头处理
/**/
//	sprintf_s(get_info, 2048, "GET /%s%s HTTP/1.1\r\n\
//Host: %s\r\n\
//User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:60.0) Gecko/20100101 Firefox/60.0\r\nAccept: */*\r\n\
//Accept-Language: zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2\r\n\
//Accept-Encoding:identity\r\n\
//Connection: keep-alive\r\n\
//Cache-Control: max-age=0\
//\r\n\r\n", dir, file2, domain);