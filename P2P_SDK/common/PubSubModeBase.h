#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <stdio.h>
#include <list>   
#include <numeric>   
#include <algorithm> 
#include "UACInfo.h"
#include "RouterInfo.h"


//�۲��߻��� 
class Observer    
{  
public:  
	Observer() {};  
	virtual ~Observer() {};  //ͨ��Reporter��ָ�룬�ϱ�״̬
	virtual void Update(){};
	virtual void Update(CUACInfo objUACInfo,RouterInfo objRouterInfo){};
};  

//�����߻���
class Reporter
{  
public:  
	Reporter()
	{
		memset(m_szMsgType,0,32);
	};  
	virtual ~Reporter(){};
	void Attach(Observer *observer)//��ӹ۲���  
	{ 
		m_observers.push_back(observer); 
	}; 
	void Remove(Observer *observer)//�Ƴ��۲��� 
	{ 
		m_observers.remove(observer); 
	};      
	void Notify1() //֪ͨ�۲��߷���1  
	{  
		std::list<Observer*>::iterator iter = m_observers.begin();  
		for(; iter != m_observers.end(); iter++)  
			(*iter)->Update();  
	};

	void Notify2() //֪ͨ�۲��߷���2
	{  
		std::list<Observer*>::iterator iter = m_observers.begin();  
		for(; iter != m_observers.end(); iter++)  
			(*iter)->Update(m_objUACInfo,m_objRouterInfo);  
	};
    void Reset()
	{
         m_nstatus=-100; 
	   //m_objDstRouterInfo.Reset();
       //m_objRouterInfo.Reset();

	};
	virtual void SetUACInfo(CUACInfo obj){ m_objUACInfo = obj; };   
	virtual CUACInfo GetUACInfo() { return m_objUACInfo; } ; 

	virtual void SetDstUACInfo(CUACInfo obj)
	{
		m_objDstUACInfo = obj; 
	};   
	virtual CUACInfo GetDstUACInfo() { return m_objDstUACInfo; } ;   

	virtual void SetRouterInfo(RouterInfo obj){ m_objRouterInfo = obj; };   
	virtual RouterInfo GetRouterInfo() { return m_objRouterInfo; };    
	
	virtual void SetStatus(int nStatus){ m_nstatus = nStatus; };   
	virtual int GetStatus() { return m_nstatus; }; 

	virtual void SetMsgType(char* szMsgType){ strcpy(m_szMsgType,szMsgType); };   
	virtual char* GetMsgType() { return m_szMsgType; }; 

	virtual void SetDstRouterInfo(RouterInfo obj)
	{ 
		m_objDstRouterInfo =obj; 
	};  
	virtual RouterInfo GetDstRouterInfoVec() { return m_objDstRouterInfo; };    
    
private:  
	std::list<Observer* > m_observers; //�۲�������  
protected:  
	CUACInfo    m_objUACInfo; //UAC1��Ϣ  
	RouterInfo  m_objRouterInfo;//src·����Ϣ
	int         m_nstatus;  //
	char        m_szMsgType[32];
	CUACInfo    m_objDstUACInfo;//UAC2��Ϣ 
	RouterInfo  m_objDstRouterInfo;//dst·����Ϣ
}; 
