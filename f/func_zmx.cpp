#include "func_zmx.h"

/*�漰���ַ����ĺ���*/

/*�޸�ʱ�䣺2018-9-15
 *���ߣ�zmx
 *˵������������ὫTCHAR����ת��ΪCHAR����
 *		������Ҫ�������Զ��庯���ٰ�װһ��
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
/*�޸�ʱ�䣺2018-9-15
 *���ߣ�zmx
 *˵������������ὫCHAR����ת��ΪTCHAR����
 *		������Ҫ�������Զ��庯���ٰ�װһ��
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
 *��ȡָ���ļ�ָ����
 */
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
	for (int j = 0, k = 0;;) {
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
*����
*
*
*/

/*�޸�ʱ�䣺2018-9-13
 *���ߣ�zmx
 *˵��������������ʼ������ͷ��
 *		������Ҫ�������Զ��庯���ٰ�װһ��
 */
int InitLinkListHead(LINKLIST *LLHead) {
	if (NULL == LLHead) {
		return -1;
	}
	LLHead->another = NULL;
	LLHead->next = NULL;

	return 0;
}
/*�޸�ʱ�䣺2018-9-13
*���ߣ�zmx
*˵������������Ὣ�������������β��
*		������Ҫ�������Զ��庯���ٰ�װһ��
*/
int AddDataToLinkList(LINKLIST *LLHead, void *in_data) {
	if (NULL == LLHead) {
		return -1;
	}

	return 0;
}
/*�޸�ʱ�䣺2018-9-13
*���ߣ�zmx
*˵���������������������ͷ�������ڴ�
*		������Ҫ�������Զ��庯���ٰ�װһ��
*/
int DeleteLinkList(LINKLIST *LLHead) {
	if (NULL == LLHead) {
		return -1;
	}

	return 0;
}

/*
*��־��¼
*
*
*/

/*�޸�ʱ�䣺2018-9-13
*���ߣ�zmx
*˵������������Ὣ��Ϣд��ָ���ļ�
*		������Ҫ�������Զ��庯���ٰ�װһ��
*/
int WriteLog_t(TCHAR *FileName, TCHAR *msg_t) {
	if (NULL == FileName) {
		return -1;
	}
	/*�ڲ�����*/
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
/*�޸�ʱ�䣺2018-9-13
*���ߣ�zmx
*˵������������Ὣ��Ϣд��ָ���ļ�
*		������Ҫ�������Զ��庯���ٰ�װһ��
*/
int WriteLogEx_t(TCHAR *FileName, TCHAR *msg_t) {

	return 0;
}