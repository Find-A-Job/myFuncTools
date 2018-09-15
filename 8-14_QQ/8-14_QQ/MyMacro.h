/***************************************************************
*版权所有 (C)2014,公司名称。
*
*文件名称：
*内容摘要：
*其它说明：
*当前版本：
*作   者：
*完成日期：
*
*修改记录1：  //修改历史记录，包括修改日期、版本号、修改人及修改内容等
*   修改日期：
*   版本号：
*   修改人：
*   修改内容：
***************************************************************/

#pragma once
#ifndef MYMACRO_H
#define MYMACRO_H

/**************************************************************
相关宏定义
**************************************************************/
#define MAIN_WINSIZE_HOR			500																	//主窗口宽 自定义数据
#define MAIN_WINSIZE_VER			500																	//主窗口高 自定义数据
#define MAIN_WINCOOR_X				(((GetSystemMetrics(SM_CXSCREEN) - MAIN_WINSIZE_HOR)) / 2)			//主窗口起始坐标x
#define MAIN_WINCOOR_Y				(((GetSystemMetrics(SM_CYSCREEN) - MAIN_WINSIZE_VER)) / 2)			//主窗口起始坐标y

#define CHILD_COMBOBOX_IP_ID		0x1																	//标识
#define CHILD_COMBOBOX_IP_TEXT		NULL																//TEXT内容
#define CHILD_COMBOBOX_IP_COOR_X	(stTextMetric.tmAveCharWidth * 1.5 * 9 + 5 + 10)					//x坐标 自定义数据
#define CHILD_COMBOBOX_IP_COOR_Y	(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4 + 5 + 5)//y坐标 自定义数据
#define CHILD_COMBOBOX_IP_HOR		(stTextMetric.tmAveCharWidth * 1.5 * 15)							//宽  注：该数值参考自《windows程序设计第五版》
#define CHILD_COMBOBOX_IP_VER		(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4 + 60)	//高  注：窄边框属性需要额外加4个单位以显示完整的边框效果

#define CHILD_EDIT_PORT_ID			0x2																	//标识
#define CHILD_EDIT_PORT_TEXT		NULL																//TEXT内容
#define CHILD_EDIT_PORT_COOR_X		(stTextMetric.tmAveCharWidth * 1.5 * 9 + 5 + 10 + (stTextMetric.tmAveCharWidth * 1.5 * 15) + 10)//x坐标 自定义数据
#define CHILD_EDIT_PORT_COOR_Y		(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4 + 5 + 5)//y坐标 自定义数据
#define CHILD_EDIT_PORT_HOR			(stTextMetric.tmAveCharWidth * 1.5 * 5)								//宽  注：该数值参考自《windows程序设计第五版》
#define CHILD_EDIT_PORT_VER			(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4)		//高  注：窄边框属性需要额外加4个单位以显示完整的边框效果

#define CHILD_STATIC_PORT_ID		0x3																	//标识
#define CHILD_STATIC_PORT_TEXT		(_T("PORT"))														//TEXT内容
#define CHILD_STATIC_PORT_COOR_X	(stTextMetric.tmAveCharWidth * 1.5 * 9 + 5 + 10 + (stTextMetric.tmAveCharWidth * 1.5 * 15) + 10)	//x坐标 自定义数据
#define CHILD_STATIC_PORT_COOR_Y	5																	//y坐标 自定义数据
#define CHILD_STATIC_PORT_HOR		(stTextMetric.tmAveCharWidth * 1.5 * 5)								//宽  注：该数值参考自《windows程序设计第五版》
#define CHILD_STATIC_PORT_VER		(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4)		//高  注：窄边框属性需要额外加4个单位以显示完整的边框效果

#define CHILD_STATIC_IP_ID			0x4																	//标识
#define CHILD_STATIC_IP_TEXT		(_T("IP"))															//TEXT内容
#define CHILD_STATIC_IP_COOR_X		(stTextMetric.tmAveCharWidth * 1.5 * 9 + 5 + 10)					//x坐标 自定义数据
#define CHILD_STATIC_IP_COOR_Y		5																	//y坐标 自定义数据
#define CHILD_STATIC_IP_HOR			(stTextMetric.tmAveCharWidth * 1.5 * 2)								//宽  注：该数值参考自《windows程序设计第五版》
#define CHILD_STATIC_IP_VER			(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4)		//高  注：窄边框属性需要额外加4个单位以显示完整的边框效果

#define CHILD_STATIC_WM_ID			0x5																	//标识
#define CHILD_STATIC_WM_TEXT		(_T("Work Mode"))													//TEXT内容
#define CHILD_STATIC_WM_COOR_X		5																	//x坐标 自定义数据
#define CHILD_STATIC_WM_COOR_Y		5																	//y坐标 自定义数据
#define CHILD_STATIC_WM_HOR			(stTextMetric.tmAveCharWidth * 1.5 * 9)								//宽  注：该数值参考自《windows程序设计第五版》
#define CHILD_STATIC_WM_VER			(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4)		//高  注：窄边框属性需要额外加4个单位以显示完整的边框效果

#define CHILD_COMBOBOX_WM_ID		0x6																	//标识
#define CHILD_COMBOBOX_WM_TEXT		(_T("Work Mode"))													//TEXT内容
#define CHILD_COMBOBOX_WM_COOR_X	5																	//x坐标 自定义数据
#define CHILD_COMBOBOX_WM_COOR_Y	(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4 + 5 + 5)//y坐标 自定义数据
#define CHILD_COMBOBOX_WM_HOR		(stTextMetric.tmAveCharWidth * 1.5 * 9)								//宽  注：该数值参考自《windows程序设计第五版》
#define CHILD_COMBOBOX_WM_VER		(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4 + 60)	//高  注：窄边框属性需要额外加4个单位以显示完整的边框效果 另：combobox 类型有下拉框属性导致额外的高度

#define CHILD_BUTTON_SU_ID			0x7																	//静态编辑框标识
#define CHILD_BUTTON_SU_TEXT		(_T("Start up"))													//TEXT内容
#define CHILD_BUTTON_SU_COOR_X		(stTextMetric.tmAveCharWidth * 1.5 * 9 + 5 + 10 + (stTextMetric.tmAveCharWidth * 1.5 * 15) + 10 +(stTextMetric.tmAveCharWidth * 1.5 * 5) + 10)//x坐标 自定义数据
#define CHILD_BUTTON_SU_COOR_Y		5																	//y坐标 自定义数据
#define CHILD_BUTTON_SU_HOR			(stTextMetric.tmAveCharWidth * 1.5 * 9)								//宽  注：该数值参考自《windows程序设计第五版》
#define CHILD_BUTTON_SU_VER			(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4)		//高  注：窄边框属性需要额外加4个单位以显示完整的边框效果

#define CHILD_LISTBOX_SHOW_ID		0x8																	//静态编辑框标识
#define CHILD_LISTBOX_SHOW_TEXT		(_T("SHOW"))														//TEXT内容
#define CHILD_LISTBOX_SHOW_COOR_X	5																	//x坐标 自定义数据
#define CHILD_LISTBOX_SHOW_COOR_Y	300																	//y坐标 自定义数据
#define CHILD_LISTBOX_SHOW_HOR		(MAIN_WINSIZE_HOR - 25)							//宽 自定义数据
#define CHILD_LISTBOX_SHOW_VER		((stTextMetric.tmHeight + stTextMetric.tmExternalLeading) * 10)		//高  注：窄边框属性需要额外加4个单位以显示完整的边框效果

#define CHILD_BUTTON_CL_ID			0x9																	//静态编辑框标识
#define CHILD_BUTTON_CL_TEXT		(_T("Close"))														//TEXT内容
#define CHILD_BUTTON_CL_COOR_X		(stTextMetric.tmAveCharWidth * 1.5 * 9 + 5 + 10 + (stTextMetric.tmAveCharWidth * 1.5 * 15) + 10 +(stTextMetric.tmAveCharWidth * 1.5 * 5) + 10)//x坐标 自定义数据
#define CHILD_BUTTON_CL_COOR_Y		(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4 + 5 + 5)//y坐标 自定义数据
#define CHILD_BUTTON_CL_HOR			(stTextMetric.tmAveCharWidth * 1.5 * 9)								//宽  注：该数值参考自《windows程序设计第五版》
#define CHILD_BUTTON_CL_VER			(stTextMetric.tmHeight + stTextMetric.tmExternalLeading + 4)		//高  注：窄边框属性需要额外加4个单位以显示完整的边框效果

#define LOGFILE						("log.log")															//日志文件
#define FLOWPATH					("flowpath.log")													//流程记录文件

#define RECVTIMELIMIT				(5000)																//recv超时设置（单位：毫秒）

#define MUTEXNAME					(_T("MNNETT"))														//互斥锁名字

#define SENDFILEREQUEST				0x10																//请求进行'sendfile'动作
#define SENDFILEALLOW				0x11																//允许进行'sendfile'动作
#define CLOSEWILL					0x12																//将要进行'close'动作
#define CLOSEALLOW					0x13																//允许进行'close'动作
#define SENDDIRREQUEST				0x14																//请求进行'senddir'动作
#define SENDDIRALLOW				0x15																//允许进行'senddir'动作
#define	ERRLEVEL1					0x1A																//错误等级1 警告
#define ERRLEVEL2					0x1B																//错误等级2 严重
#define	ERRLEVEL3					0x1C																//错误等级3 致命
#define NOTALLOW					0x1F																//不允许进行其他动作

/**************************************************************
相关结构体定义
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
本程序中出现的函数的声明
**************************************************************/

#endif // !MYMACRO_H