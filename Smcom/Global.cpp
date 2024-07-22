#include "stdafx.h"
#include "Global.h"

#include "../Public/Function.h"

using namespace std;
#include <bitset>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char* ConvertToAnsi(OLECHAR* szW)
{
	const ULONG ulCnt = WideCharToMultiByte(CP_ACP, 0, szW, -1, nullptr, 0, nullptr, nullptr)+1;

	char* pStr = new char[ulCnt];

	WideCharToMultiByte(CP_ACP, 0, szW, -1, pStr, ulCnt, nullptr, nullptr);

	return pStr;
}

BOOL HasWideChar(char* szA)// �ж�һ���ַ����Ƿ��к���
{
	const ULONG ulCnt1 = strlen(szA);
	const OLECHAR* data = MultiByteToUnicode(CP_ACP, szA);
	const ULONG ulCnt2 = wcslen(data);
	delete[] data;

	if(ulCnt1==ulCnt2)
		return FALSE;
	else
		return TRUE;
}

int atohex(char* p, int cnt)
{
	int val = 0;
	char ch = 0;
	int pow = 1;

	for(int i = cnt; i>0; i--)
	{
		int v = 0;
		ch = *(p+i-1);
		if('A'<=ch&&'Z'>=ch)
		{
			v = 10+(ch-'A');
		}
		else if('a'<=ch&&'z'>=ch)
		{
			v = 10+(ch-'a');
		}
		else if('0'<=ch&&'9'>=ch)
			v = ch-'0';

		if(i==cnt)
			pow = 1;
		else
			pow *= 16;

		val += v*pow;
	}

	return val;
}

char* SepToOctets(char* p, int cnt)
{
	if(cnt<2)
		return nullptr;

	cnt /= 2;
	char* result = new char[cnt*8/7+1];
	char* point = result;

	unsigned char buf1 = 0X00;
	unsigned char buf2 = 0X00;
	int index = 0;
	while(index<cnt)
	{
		const int i = index%7;
		const char value = atohex(p, 2);
		p += 2;

		buf1 = value;
		buf1 = (buf1<<i);
		buf2 = (buf2>>(8-i));
		buf1 = (buf1|buf2);
		buf1 = (buf1&0X7F);
		*point = buf1;
		point++;

		buf2 = value;
		buf2 = (buf2>>(7-i));
		buf2 = (buf2<<(7-i));

		if(i==6)
		{
			buf2 = (buf2>>1);
			buf2 = (buf2&0X7F);
			*point = buf2;
			point++;
			buf2 = 0X00;
		}

		index++;
	}
	*point = 0;

	return result;
}

char* OctToSeptects(char* p, int cnt)// octets into septetcs in ASCII coding p: 8λseptects�ִ�����ָ�� cnt: Ҫת����ʮ�������ַ�����
{
	int size = cnt*4;
	char bits[] = {"0000000100100011010001010110011110001001101010111100110111101111"};
	char* septects = new char[size+1];
	memset(septects, 0, size+1);
	char ch;

	// ת��λ01�ִ�
	int mm = 0;
	for(int i = 0; i<cnt; i++)
	{
		int v = 0;
		ch = *(p+i);
		if('A'<=ch&&'Z'>=ch)
		{
			v = 10+(ch-'A');
		}
		else if('a'<=ch&&'z'>=ch)
		{
			v = 10+(ch-'a');
		}
		else if('0'<=ch&&'9'>=ch)
			v = ch-'0';

		if(0==i%2)
		{
			memcpy((septects+mm), (bits+v*4)+1, 3);
			mm += 3;
		}
		else
		{
			memcpy((septects+mm), (bits+v*4), 4);
			mm += 4;
		}
	}

	// ת����octects to septects
	int bs = 1;
	const int len = strlen(septects);
	if(len<14)
	{
		char* content = new char[5];
		memset(content, 0, 5);
		const string str(septects);
		const bitset<8> set(str, 0, len);
		sprintf(content, "%02X", set.to_ulong());
		return content;
	}

	char* start = septects;
	char* end = septects+14;
	while(1)
	{
		if(7<end-start)
			ConvertBits(start, end, bs);

		bs++;

		start = start+8;

		if(1<bs&&0==(bs-1)%7)
		{
			bs = 1;
			end += 14;
		}
		else
			end += 7;

		if(len<=(end-septects))
			end = septects+len;

		if(len<(start-septects))
			break;
	}

	// bit�ִ�ת��Ϊ����ֵ
	size = len/8;
	char* content = new char[size*2+3];
	memset(content, 0, size*2+3);
	int i;
	for(i = 0; i<size; i++)
	{
		char ch[3];
		memset(ch, 0, 3);
		string str(septects);
		bitset<8> set(str, i*8, 8);
		sprintf(ch, "%02X", set.to_ulong());
		sprintf(content, "%s%s", content, ch);
	}

	if(0!=len%8) // ����һ���ֽڵ����
	{
		char ch[3];
		memset(ch, 0, 3);
		int ii = len-i*8;
		const string str(septects);
		const bitset<8> set(str, i*8, len-i*8);
		sprintf(ch, "%02X", set.to_ulong());
		sprintf(content, "%s%s", content, ch);
	}

	delete[] septects;

	return content;
}

long GetSafeArraySize(SAFEARRAY* psa)
{
	if(psa==nullptr)
		return 0;

	long ubound;
	const HRESULT hr1 = SafeArrayGetUBound(psa, 1, &ubound);
	if(FAILED(hr1))
		return 0;

	long lbound;
	const HRESULT hr2 = SafeArrayGetLBound(psa, 1, &lbound);
	if(FAILED(hr2))
		return 0;
	const long arraysize = ubound-lbound+1;
	return arraysize;
}

void ConvertPDUPhoneNo(char* PhoneNo, int len)
{
	char ch = 0;

	// 8613910829120F -> 683119809221F0
	// 683119809221F0 -> 8613910829120F
	for(int i = 0; i<len; i++)
	{
		if(i%2==0)
		{
			ch = PhoneNo[i];
		}
		else
		{
			PhoneNo[i-1] = PhoneNo[i];
			PhoneNo[i] = ch;
		}
	}
}

// ��һ��λ����ǰ�벿��Ų��bitsλ����벿��
// start: λ���Ŀ�ʼ
// end:   λ���ĵĽ���
// bits:  Ų����λ��(���λ�����ֽڵĳ���:16)
void MoveBits(char* start, char* end, BYTE bits)
{
	// ����ǰ���ֵ
	char ch[16];
	for(int i = 0; i<bits; i++)
		ch[i] = start[i];

	/*#ifdef _LOG_FILE
		char temp[50];
		memset(temp, 0, 50);
		memcpy(temp, start, end-start);
		Report("Befor Convert[%d]/[%d]: %s", bits, end-start, temp);
	#endif*/

	// �ƶ��������е�ֵ
	const int size = end-start-bits;
	for(int i = 0; i<size; i++)
		start[i] = start[i+bits];

	//�������ǰ���ֵ׷�ӵ�����
	for(int i = 0; i<bits; i++)
	{
		start[i+size] = ch[i];
	}
	/*#ifdef _LOG_FILE
		memset(temp, 0, 50);
		memcpy(temp, start, end-start);
		Report("After Convert[%d]/[%d]: %s", bits, end-start, temp);
	#endif*/
}

// ��һ��λ���ĺ�벿��Ų��bitsλ��ǰ�벿��
// start: λ���Ŀ�ʼ
// end:   λ���ĵĽ���
// bits:  Ų����λ��(���λ�����ֽڵĳ���:16)
void ConvertBits(char* start, char* end, BYTE bits)
{
	// ����ǰ���ֵ
	char ch[16];
	int size = end-start;
	int j = 0;
	for(int i = size-bits; i<size; i++)
		ch[j++] = start[i];

	// ���ƶ�ǰ�����е�ֵ
	size = size-bits;
	for(int i = size-1; i>=0; i--)
		start[i+bits] = start[i];

	// ������ĺ����ֵ׷�ӵ�ǰ��
	for(int i = 0; i<bits; i++)
		start[i] = ch[i];
}
