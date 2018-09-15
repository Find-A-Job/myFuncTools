#pragma once
#ifndef FUNC_ZMX_H
#define FUNC_ZMX_H

#include "stdafx.h"

/*涉及到字符串的函数*/
int TcharToChar(TCHAR *, size_t , char *);
int CharToTchar(char *, size_t , TCHAR *);
int getFileLine_t(TCHAR *, int, TCHAR *);				//获取指定文件指定行
int compareString_t(TCHAR *, TCHAR *, int);				//比较两个字符串
int getSpeString_t(TCHAR *, TCHAR, int, TCHAR *);		//获取某个字符串中某个字符前或后部所有字符

/*
 *链表
 *
 *
 */
struct LINKLIST
{
	void *another;		//指向其他类型数据的指针
	LINKLIST *next;		//指向下一个链表地址
	bool ExValue;		//保留字段
	TCHAR *ExChar;		//保留字段
};
typedef struct LINKLIST LK_ZMX;
typedef struct LINKLIST *PLK_ZMX;

int InitLinkListHead(LINKLIST *);
int AddDataToLinkList(LINKLIST *, void *);
int DeleteLinkList(LINKLIST *);

/*
*日志记录
*
*
*/
int WriteLog_t(TCHAR *, TCHAR *);
int WriteLogEx_t(TCHAR *, TCHAR *);

#endif // !FUNC_ZMX_H
