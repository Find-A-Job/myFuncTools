#pragma once
#ifndef FUNC_ZMX_H
#define FUNC_ZMX_H

#include "stdafx.h"

/*�漰���ַ����ĺ���*/
int TcharToChar(TCHAR *, size_t , char *);
int CharToTchar(char *, size_t , TCHAR *);
int getFileLine_t(TCHAR *, int, TCHAR *);				//��ȡָ���ļ�ָ����
int compareString_t(TCHAR *, TCHAR *, int);				//�Ƚ������ַ���
int getSpeString_t(TCHAR *, TCHAR, int, TCHAR *);		//��ȡĳ���ַ�����ĳ���ַ�ǰ��������ַ�

/*
 *����
 *
 *
 */
struct LINKLIST
{
	void *another;		//ָ�������������ݵ�ָ��
	LINKLIST *next;		//ָ����һ�������ַ
	bool ExValue;		//�����ֶ�
	TCHAR *ExChar;		//�����ֶ�
};
typedef struct LINKLIST LK_ZMX;
typedef struct LINKLIST *PLK_ZMX;

int InitLinkListHead(LINKLIST *);
int AddDataToLinkList(LINKLIST *, void *);
int DeleteLinkList(LINKLIST *);

/*
*��־��¼
*
*
*/
int WriteLog_t(TCHAR *, TCHAR *);
int WriteLogEx_t(TCHAR *, TCHAR *);

#endif // !FUNC_ZMX_H
