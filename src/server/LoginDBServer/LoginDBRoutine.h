//////////////////////////////////////////////////////////////////
//   The Legend of Heros                                        //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : LoginDBRoutine.h                             //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ��½DB����������ʵ��                               //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __LOGINDB_ROUTINE_H__
#define __LOGINDB_ROUTINE_H__

#pragma once

#include "CommonRoutine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLoginDBRoutine
// �˺�ע�ἰ��ֵ����һ�㶼��ҳ�洦��, ��������ֻ������Ϸ�ͻ�����صĵ�½�͵ǳ�����
class CLoginDBRoutine : public ICommonRoutine
{
	DECLARE_RTTI_CREATE( CLoginDBRoutine )
public:
	CLoginDBRoutine(void);
	virtual ~CLoginDBRoutine(void);
	// CEventHandler
	virtual bool OnHandle(Int nEvent, uintptr_t utParam, LLong llParam) OVERRIDE;//
	virtual bool OnHandle(Int nEvent, CEventBase&, LLong) OVERRIDE;
	virtual bool OnHandle(Int nEvent, CStream& Stream, LLong llParam) OVERRIDE;//
	virtual bool OnHandle(Int nEvent, UInt uCount) OVERRIDE;//
	// ICommonRoutine
	virtual bool Start(class ICommonServer* pServer) OVERRIDE;
	virtual bool Pause(bool bPause = true) OVERRIDE;
	virtual bool Update(void) OVERRIDE;
	virtual void Stop(void) OVERRIDE;

	virtual bool Add(Int nEvent, uintptr_t utParam = 0, LLong llParam = 0, CEventQueue::EVENT_TYPE eType = CEventQueue::EVENT_TYPE_NONE) OVERRIDE;//
	virtual bool Add(Int nEvent, CEventBase& EventRef, LLong llParam = 0, CEventQueue::EVENT_TYPE eType = CEventQueue::EVENT_TYPE_REFCOUNT) OVERRIDE;
	virtual bool Add(Int nEvent, CStream& Stream, LLong llParam = 0) OVERRIDE;//
private:
	class CLoginDBServer*   m_pServer;
	size_t                  m_stCount;         // ��ʱ��ID��
	CEventQueuePtr          m_EventQueuePtr;   // �¼�����  
	// 1. �ӷ�������չ�����ļ��ж�ȡ���ݿ���Ϣ
};

INLINE CLoginDBRoutine::CLoginDBRoutine(void)
: m_pServer(nullptr)
, m_stCount(0)
{
}

INLINE CLoginDBRoutine::~CLoginDBRoutine(void)
{
}

INLINE bool CLoginDBRoutine::OnHandle(Int, uintptr_t, LLong)
{ 
	return false;
}

INLINE bool CLoginDBRoutine::OnHandle(Int, CStream&, LLong)
{ 
	return false; 
}

INLINE bool CLoginDBRoutine::OnHandle(Int, UInt)
{ 
	return false;
}

INLINE bool CLoginDBRoutine::Add(Int, uintptr_t, LLong, CEventQueue::EVENT_TYPE)
{
	return false;
}

INLINE bool CLoginDBRoutine::Add(Int, CStream&, LLong)
{
	return false;
}

#endif // __LOGINDB_ROUTINE_H__
