#include "func_zmx.h"

/*涉及到字符串的函数*/

/*修改时间：2018-9-15
 *作者：zmx
 *说明：这个函数会将TCHAR类型转换为CHAR类型
 *		并且需要由其他自定义函数再包装一次
 */
int TcharToChar(TCHAR *tcr, size_t tchLength, char *cr) {
	if (NULL == tcr) {
		cr[0] = '\0';
		return 1;
	}
	for (int i = 0; i < tchLength; ++i) {
		if (((char *)tcr)[i * 2 + 1] != '\0') {
			cr[0] = '\0';
			return -1;
		}
	}
	for (int i = 0; i < tchLength; ++i) {
		cr[i] = ((char *)tcr)[i * 2];
	}
	cr[tchLength] = '\0';
	return 0;
}
/*修改时间：2018-9-15
 *作者：zmx
 *说明：这个函数会将CHAR类型转换为TCHAR类型
 *		并且需要由其他自定义函数再包装一次
 */
int CharToTchar(char *cr, size_t tchLength, TCHAR *tcr) {
	if (NULL == cr) {
		return 1;
	}
	for (int i = 0; i < tchLength; ++i) {
		((char *)tcr)[2 * i] = cr[i];
		((char *)tcr)[2 * i + 1] = '\0';
	}
	((char *)tcr)[2 * tchLength] = '\0';
	return 0;
}
/*
 *
 *获取指定文件指定行
 */
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
	for (int j = 0, k = 0;;) {
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
		for (size_t i = 0; outTcharA[i] != '\0'; ++i) {
			temp_t[i] = outTcharA[i];
		}
		for (size_t i = _tcslen(outTcharA), j = index;; ++i, ++j) {
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
*链表
*
*
*/

/*修改时间：2018-9-13
 *作者：zmx
 *说明：这个函数会初始化链表头部
 *		并且需要由其他自定义函数再包装一次
 */
int InitLinkListHead(LINKLIST *LLHead) {
	if (NULL == LLHead) {
		return -1;
	}
	LLHead->another = NULL;
	LLHead->next = NULL;

	return 0;
}
/*修改时间：2018-9-13
*作者：zmx
*说明：这个函数会将数据添加至链表尾部
*		并且需要由其他自定义函数再包装一次
*/
int AddDataToLinkList(LINKLIST *LLHead, void *in_data) {
	if (NULL == LLHead) {
		return -1;
	}

	return 0;
}
/*修改时间：2018-9-13
*作者：zmx
*说明：这个函数会清空链表，释放申请的内存
*		并且需要由其他自定义函数再包装一次
*/
int DeleteLinkList(LINKLIST *LLHead) {
	if (NULL == LLHead) {
		return -1;
	}

	return 0;
}

/*
*日志记录
*
*
*/

/*修改时间：2018-9-13
*作者：zmx
*说明：这个函数会将信息写入指定文件
*		并且需要由其他自定义函数再包装一次
*/
int WriteLog_t(TCHAR *FileName, TCHAR *msg_t) {
	if (NULL == FileName) {
		return -1;
	}
	/*内部变量*/
	FILE *FileWrite = NULL;
	int iFuncStat = 0;
	char msg[256] = { NULL };

	/*...*/
	_tfopen_s(&FileWrite, FileName, _T("wt"));
	if (NULL == FileWrite) {
		return -1;
	}
	if (-1==TcharToChar(msg_t, _tcslen(msg_t), msg)) {
		fclose(FileWrite);
		return -1;
	}
	iFuncStat = fwrite(msg, sizeof(char), strlen(msg), FileWrite);
	if (iFuncStat > 0 && strlen(msg) == iFuncStat) {
		//success
	}
	else {
		fclose(FileWrite);
		return -1;
	}

	fclose(FileWrite);
	return 0;
}
/*修改时间：2018-9-13
*作者：zmx
*说明：这个函数会将信息写入指定文件
*		并且需要由其他自定义函数再包装一次
*/
int WriteLogEx_t(TCHAR *FileName, TCHAR *msg_t) {

	return 0;
}