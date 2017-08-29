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


//观察者基类 
class Observer    
{  
public:  
	Observer() {};  
	virtual ~Observer() {};  //通过Reporter的指针，上报状态
	virtual void Update(){};
	virtual void Update(CUACInfo objUACInfo,RouterInfo objRouterInfo){};
};  

//报告者基类
class Reporter
{  
public:  
	Reporter()
	{
		memset(m_szMsgType,0,32);
	};  
	virtual ~Reporter(){};
	void Attach(Observer *observer)//添加观察者  
	{ 
		m_observers.push_back(observer); 
	}; 
	void Remove(Observer *observer)//移除观察者 
	{ 
		m_observers.remove(observer); 
	};      
	void Notify1() //通知观察者方法1  
	{  
		std::list<Observer*>::iterator iter = m_observers.begin();  
		for(; iter != m_observers.end(); iter++)  
			(*iter)->Update();  
	};

	void Notify2() //通知观察者方法2
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
	std::list<Observer* > m_observers; //观察者链表  
protected:  
	CUACInfo    m_objUACInfo; //UAC1信息  
	RouterInfo  m_objRouterInfo;//src路由信息
	int         m_nstatus;  //
	char        m_szMsgType[32];
	CUACInfo    m_objDstUACInfo;//UAC2信息 
	RouterInfo  m_objDstRouterInfo;//dst路由信息
}; 
