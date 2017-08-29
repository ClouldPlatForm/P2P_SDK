#ifndef _COMMON_UTIL_H_
#define _COMMON_UTIL_H_
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <string.h>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdio>
using namespace std;
typedef std::pair<char*,int> PAIR;
typedef enum paramType
{
	UNKNOW = -2,
	SECOND_ROOT_TYPE =-1,
	ROOT_TYPE=0,
	ROOT_INT_TYPE =1,
	ROOT_CHAR_TYPE =2,
	ROOT_STRING_TYPE=3,
	ROOT_PCHAR_TYPE=4,
	ROOT_INTP_TYPE=5,
	SECOND_INT_TYPE =6,
	SECOND_CHAR_TYPE =7,
	SECOND_STRING_TYPE =8,
	SECOND_PCHAR_TYPE =9,
	SECOND_INTP_TYPE =10,
};
static int str2int(const char *str)
{
	int temp = 0;
	const char *ptr = str;  //ptr����str�ַ�����ͷ	 
	if (*str == '-' || *str == '+')  //�����һ���ַ��������ţ�
	{  //���Ƶ���һ���ַ�
		str++;
	}
	while(*str != 0)
	{
		if ((*str < '0') || (*str > '9'))  //�����ǰ�ַ���������
		{  //���˳�ѭ��
			break;
		}
		temp = temp * 10 + (*str - '0'); //�����ǰ�ַ��������������ֵ
		str++;      //�Ƶ���һ���ַ�
	}  
	if (*ptr == '-')     //����ַ������ԡ�-����ͷ����ת�������෴��
	{
		temp = -temp;
	}

	return temp;
}

static void int2str(int n, char *str)
{
	if (n == 0)
	{
		str[0] = '0';
		str[1]='\0';
		return;
	}
	
	int nlen = 10;
	char buf[10] = "";
	int i = 0;
	int len = 0;
	int temp = n < 0 ? -n: n;  // tempΪn�ľ���ֵ

	if (str == NULL)
	{
		return;
	}
	while(temp)
	{
		buf[i++] = (temp % nlen) + '0';  //��temp��ÿһλ�ϵ�������buf
		temp = temp / nlen;
	}

	len = n < 0 ? ++i: i;  //���n�Ǹ����������Ҫһλ���洢����
	str[i] = 0;            //ĩβ�ǽ�����0
	while(1)
	{
		i--;
		if (buf[len-i-1] ==0)
		{
			break;
		}
		str[i] = buf[len-i-1];  //��buf��������ַ������ַ���
	}
	if (i == 0 )
	{
		str[i] = '-';          //����Ǹ��������һ������
	}
}

//map����valueֵ����
static int cmp(const PAIR& x, const PAIR& y)    
{    
	return abs(x.second) < abs(y.second);    
}    

static void sortMapByValue(std::map< char*, int >& tMap,vector<PAIR >& tVector)    
{    
	for (map<char*, int>::iterator curr = tMap.begin(); curr != tMap.end(); curr++)     
		tVector.push_back(make_pair(curr->first, curr->second));      

	sort(tVector.begin(), tVector.end(), cmp);    
}


//�ַ����ָ��  
static std::vector<std::string> split(std::string str,std::string pattern)  
{  
	std::string::size_type pos;  
	std::vector<std::string> result;  
	str+=pattern;//��չ�ַ����Է������  
	int size=str.size();  

	for(int i=0; i<size; i++)  
	{  
		pos=str.find(pattern,i);  
		if(pos<size)  
		{  
			std::string s=str.substr(i,pos-i);  
			result.push_back(s);  
			i=pos+pattern.size()-1;  
		}  
	}  
	return result;  
}  

#endif
