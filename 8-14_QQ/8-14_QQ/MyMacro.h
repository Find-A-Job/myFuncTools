/***************************************************************
*��Ȩ���� (C)2014,��˾���ơ�
*
*�ļ����ƣ�
*����ժҪ��
*����˵����
*��ǰ�汾��
*��   �ߣ�
*������ڣ�
*
*�޸ļ�¼1��  //�޸���ʷ��¼�������޸����ڡ��汾�š��޸��˼��޸����ݵ�
*   �޸����ڣ�
*   �汾�ţ�
*   �޸��ˣ�
*   �޸����ݣ�
***************************************************************/

#pragma once
#ifndef MYMACRO_H
#define MYMACRO_H

/**************************************************************
��غ궨��
**************************************************************/
#define MAIN_WINSIZE_HOR			500																	//�����ڿ� �Զ�������
#define MAIN_WINSIZE_VER			500																	//�����ڸ� �Զ�������
#define MAIN_WINCOOR_X				(((GetSystemMetrics(SM_CXSCREEN) - MAIN_WINSIZE_HOR)) / 2)			//��������ʼ����x
#define MAIN_WINCOOR_Y				(((GetSystemMetrics(SM_CYSCREEN) - MAIN_WINSIZE_VER)) / 2)			//��������ʼ����y

#define CHILD_COMBOBOX_IP_ID		0x1																	//��ʶ
#define CHILD_COMBOBOX_IP_TEXT		NULL																//TEXT����
#define CHILD_COMBOBOX_IP_COOR_X	(stTextMetric.tmAveCharWidth * 1.5 * 9 + 5 + 10)					//x���� �Զ�������
#define CHILD_COMBOBOX_IP_COOR_Y	(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4 + 5 + 5)//y���� �Զ�������
#define CHILD_COMBOBOX_IP_HOR		(stTextMetric.tmAveCharWidth * 1.5 * 15)							//��  ע������ֵ�ο��ԡ�windows������Ƶ���桷
#define CHILD_COMBOBOX_IP_VER		(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4 + 60)	//��  ע��խ�߿�������Ҫ�����4����λ����ʾ�����ı߿�Ч��

#define CHILD_EDIT_PORT_ID			0x2																	//��ʶ
#define CHILD_EDIT_PORT_TEXT		NULL																//TEXT����
#define CHILD_EDIT_PORT_COOR_X		(stTextMetric.tmAveCharWidth * 1.5 * 9 + 5 + 10 + (stTextMetric.tmAveCharWidth * 1.5 * 15) + 10)//x���� �Զ�������
#define CHILD_EDIT_PORT_COOR_Y		(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4 + 5 + 5)//y���� �Զ�������
#define CHILD_EDIT_PORT_HOR			(stTextMetric.tmAveCharWidth * 1.5 * 5)								//��  ע������ֵ�ο��ԡ�windows������Ƶ���桷
#define CHILD_EDIT_PORT_VER			(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4)		//��  ע��խ�߿�������Ҫ�����4����λ����ʾ�����ı߿�Ч��

#define CHILD_STATIC_PORT_ID		0x3																	//��ʶ
#define CHILD_STATIC_PORT_TEXT		(_T("PORT"))														//TEXT����
#define CHILD_STATIC_PORT_COOR_X	(stTextMetric.tmAveCharWidth * 1.5 * 9 + 5 + 10 + (stTextMetric.tmAveCharWidth * 1.5 * 15) + 10)	//x���� �Զ�������
#define CHILD_STATIC_PORT_COOR_Y	5																	//y���� �Զ�������
#define CHILD_STATIC_PORT_HOR		(stTextMetric.tmAveCharWidth * 1.5 * 5)								//��  ע������ֵ�ο��ԡ�windows������Ƶ���桷
#define CHILD_STATIC_PORT_VER		(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4)		//��  ע��խ�߿�������Ҫ�����4����λ����ʾ�����ı߿�Ч��

#define CHILD_STATIC_IP_ID			0x4																	//��ʶ
#define CHILD_STATIC_IP_TEXT		(_T("IP"))															//TEXT����
#define CHILD_STATIC_IP_COOR_X		(stTextMetric.tmAveCharWidth * 1.5 * 9 + 5 + 10)					//x���� �Զ�������
#define CHILD_STATIC_IP_COOR_Y		5																	//y���� �Զ�������
#define CHILD_STATIC_IP_HOR			(stTextMetric.tmAveCharWidth * 1.5 * 2)								//��  ע������ֵ�ο��ԡ�windows������Ƶ���桷
#define CHILD_STATIC_IP_VER			(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4)		//��  ע��խ�߿�������Ҫ�����4����λ����ʾ�����ı߿�Ч��

#define CHILD_STATIC_WM_ID			0x5																	//��ʶ
#define CHILD_STATIC_WM_TEXT		(_T("Work Mode"))													//TEXT����
#define CHILD_STATIC_WM_COOR_X		5																	//x���� �Զ�������
#define CHILD_STATIC_WM_COOR_Y		5																	//y���� �Զ�������
#define CHILD_STATIC_WM_HOR			(stTextMetric.tmAveCharWidth * 1.5 * 9)								//��  ע������ֵ�ο��ԡ�windows������Ƶ���桷
#define CHILD_STATIC_WM_VER			(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4)		//��  ע��խ�߿�������Ҫ�����4����λ����ʾ�����ı߿�Ч��

#define CHILD_COMBOBOX_WM_ID		0x6																	//��ʶ
#define CHILD_COMBOBOX_WM_TEXT		(_T("Work Mode"))													//TEXT����
#define CHILD_COMBOBOX_WM_COOR_X	5																	//x���� �Զ�������
#define CHILD_COMBOBOX_WM_COOR_Y	(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4 + 5 + 5)//y���� �Զ�������
#define CHILD_COMBOBOX_WM_HOR		(stTextMetric.tmAveCharWidth * 1.5 * 9)								//��  ע������ֵ�ο��ԡ�windows������Ƶ���桷
#define CHILD_COMBOBOX_WM_VER		(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4 + 60)	//��  ע��խ�߿�������Ҫ�����4����λ����ʾ�����ı߿�Ч�� ��combobox ���������������Ե��¶���ĸ߶�

#define CHILD_BUTTON_SU_ID			0x7																	//��̬�༭���ʶ
#define CHILD_BUTTON_SU_TEXT		(_T("Start up"))													//TEXT����
#define CHILD_BUTTON_SU_COOR_X		(stTextMetric.tmAveCharWidth * 1.5 * 9 + 5 + 10 + (stTextMetric.tmAveCharWidth * 1.5 * 15) + 10 +(stTextMetric.tmAveCharWidth * 1.5 * 5) + 10)//x���� �Զ�������
#define CHILD_BUTTON_SU_COOR_Y		5																	//y���� �Զ�������
#define CHILD_BUTTON_SU_HOR			(stTextMetric.tmAveCharWidth * 1.5 * 9)								//��  ע������ֵ�ο��ԡ�windows������Ƶ���桷
#define CHILD_BUTTON_SU_VER			(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4)		//��  ע��խ�߿�������Ҫ�����4����λ����ʾ�����ı߿�Ч��

#define CHILD_LISTBOX_SHOW_ID		0x8																	//��̬�༭���ʶ
#define CHILD_LISTBOX_SHOW_TEXT		(_T("SHOW"))														//TEXT����
#define CHILD_LISTBOX_SHOW_COOR_X	5																	//x���� �Զ�������
#define CHILD_LISTBOX_SHOW_COOR_Y	300																	//y���� �Զ�������
#define CHILD_LISTBOX_SHOW_HOR		(MAIN_WINSIZE_HOR - 25)							//�� �Զ�������
#define CHILD_LISTBOX_SHOW_VER		((stTextMetric.tmHeight + stTextMetric.tmExternalLeading) * 10)		//��  ע��խ�߿�������Ҫ�����4����λ����ʾ�����ı߿�Ч��

#define CHILD_BUTTON_CL_ID			0x9																	//��̬�༭���ʶ
#define CHILD_BUTTON_CL_TEXT		(_T("Close"))														//TEXT����
#define CHILD_BUTTON_CL_COOR_X		(stTextMetric.tmAveCharWidth * 1.5 * 9 + 5 + 10 + (stTextMetric.tmAveCharWidth * 1.5 * 15) + 10 +(stTextMetric.tmAveCharWidth * 1.5 * 5) + 10)//x���� �Զ�������
#define CHILD_BUTTON_CL_COOR_Y		(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4 + 5 + 5)//y���� �Զ�������
#define CHILD_BUTTON_CL_HOR			(stTextMetric.tmAveCharWidth * 1.5 * 9)								//��  ע������ֵ�ο��ԡ�windows������Ƶ���桷
#define CHILD_BUTTON_CL_VER			(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4)		//��  ע��խ�߿�������Ҫ�����4����λ����ʾ�����ı߿�Ч��

#define LOGFILE						("log.log")															//��־�ļ�
#define FLOWPATH					("flowpath.log")													//���̼�¼�ļ�

#define RECVTIMELIMIT				(5000)																//recv��ʱ���ã���λ�����룩

#define MUTEXNAME					(_T("MNNETT"))														//����������

#define SENDFILEREQUEST				0x10																//�������'sendfile'����
#define SENDFILEALLOW				0x11																//�������'sendfile'����
#define CLOSEWILL					0x12																//��Ҫ����'close'����
#define CLOSEALLOW					0x13																//�������'close'����
#define SENDDIRREQUEST				0x14																//�������'senddir'����
#define SENDDIRALLOW				0x15																//�������'senddir'����
#define	ERRLEVEL1					0x1A																//����ȼ�1 ����
#define ERRLEVEL2					0x1B																//����ȼ�2 ����
#define	ERRLEVEL3					0x1C																//����ȼ�3 ����
#define NOTALLOW					0x1F																//�����������������

/**************************************************************
��ؽṹ�嶨��
**************************************************************/
struct stSendType
{
	TCHAR name[256];
	TCHAR nameWithoutPath[256];	
};

struct saveSucHis
{
	TCHAR workmode[256];
	TCHAR ip[256];
	TCHAR port[256];
	TCHAR storepath[256];
};

struct GLOBAL_TESTSTRUCT1
{
	int argv;
	CRITICAL_SECTION cs;
};

/**************************************************************
�������г��ֵĺ���������
**************************************************************/

#endif // !MYMACRO_H