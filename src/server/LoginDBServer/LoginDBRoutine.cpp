//////////////////////////////////////////////////////////////////
//   The Legend of Heros                                        //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Source File : LoginDBRoutine.cpp                           //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ��½DB����������ʵ��                               //
//                                                              //
//////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LoginDBServerImp.h"
#include "LoginDBRoutine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLoginDBRoutine
IMPLEMENT_RTTI_BEGIN_DUMP_CREATE( CLoginDBRoutine, CObject, CObjectTraits::OBJECTT_TRANSIENT, 3 )
DUMP_RTTI_PTR( m_pServer, VART_NONE, void )
DUMP_RTTI_STD( m_stCount, VART_SIZE_T, size_t )
DUMP_RTTI_DAT( m_EventQueuePtr, VART_REFCOUNT, CEventQueuePtr, sizeof(CEventQueuePtr) )
END_DUMP_RTTI( CLoginDBRoutine, CObject, 3 )

bool CLoginDBRoutine::OnHandle(Int nEvent, CEventBase& EventRef, LLong llParam)
{
	switch (nEvent) {
	case PAK_EVENT_LOGIN_LINK:
		{
			++m_stCount;

			CPAKLoginLink* pLink = static_cast<CPAKLoginLink*>(&EventRef);
			// ���ݿ��˺ŵ�½����
			LOGV_WARN(m_pServer->GetFileLog(), TF("[��½DB������]�ͻ����˺�:%s[����:%s]��½, session key=%llx!"),
					  *(pLink->GetUserId()), *(pLink->GetPassword()), pLink->GetSessionId());
			// ���󷵻�CPAKSessionAck, һ��ʱ���ε�½ʧ�ܴ���
			CPAKLoginLinkAck LinkAck(PAK_EVENT_LOGIN_LINKACK, (UInt)PAK_TYPE_LOGINDB);
			LinkAck.SetAck(DATAD_OKAY);
			LinkAck.SetSessionId(pLink->GetSessionId());
			LinkAck.SetUserId((LLong)m_stCount);
			// �����ݿ��ȡ�ϴε�¼�ͻ���IP+ʱ����ϴ����ӵ���Ϸ������Id
			LinkAck.AdjustSize();

			if (llParam == 0) {
				assert(m_pServer->GetShareServer() != nullptr);
				m_pServer->GetShareServer()->OnShareRoutine(PAK_EVENT_LOGIN_LINKACK, LinkAck);
			}
			else {
				m_pServer->GetNetworkPtr()->Send((KeyRef)llParam, LinkAck);
			}
		}
		break;
	case PAK_EVENT_LOGIN_UNLINK:
		{
			CPAKLoginUnlink* pUnlink = static_cast<CPAKLoginUnlink*>(&EventRef);
			// �˺ŵǳ�����
			LOGV_WARN(m_pServer->GetFileLog(), TF("[��½DB������]�ͻ���ID:%llx�ǳ�, session key=%llx!"), pUnlink->GetUserId(), pUnlink->GetSessionId());
			// ���ݿ�洢�ϴε�¼�ͻ���IP+ʱ����ϴ����ӵ���Ϸ������id
			if (pUnlink->GetSessionId() != 0) { // session=0��ʾ�Ͽ���, ����Ҫ�ظ�
				CPAKSessionAck UnlinkAck(PAK_EVENT_LOGIN_UNLINKACK, (UInt)PAK_TYPE_LOGINDB);
				UnlinkAck.SetAck(DATAD_OKAY);
				UnlinkAck.SetSessionId(pUnlink->GetSessionId());
				UnlinkAck.AdjustSize();

				if (llParam == 0) {
					assert(m_pServer->GetShareServer() != nullptr);
					m_pServer->GetShareServer()->OnShareRoutine(PAK_EVENT_LOGIN_UNLINKACK, UnlinkAck);
				}
				else {
					m_pServer->GetNetworkPtr()->Send((KeyRef)llParam, UnlinkAck);
				}
			}
		}
		break;
	default:
		{
		}
	}
	return true;
}

bool CLoginDBRoutine::Start(class ICommonServer* pServer)
{
	m_pServer = static_cast<CLoginDBServer*>(pServer);
	if (m_pServer != nullptr) {
		// Ĭ�϶��д����߳�Ϊ1
		if (CEventQueue::EventQueue(m_EventQueuePtr, *this, 0, m_pServer->GetConfig()->GetServerQueue(CServerConfig::CFG_DEFAULT_LOGINDB))) {
			return m_EventQueuePtr->Init();
		}
	}
	return false;
}

bool CLoginDBRoutine::Pause(bool)
{
	return false;
}

bool CLoginDBRoutine::Update(void)
{
	return false;
}

void CLoginDBRoutine::Stop(void)
{
	if (m_EventQueuePtr != nullptr) {
		m_EventQueuePtr->Exit();
		m_EventQueuePtr = nullptr;
	}
	m_pServer = nullptr;
	m_stCount = 0;
}

bool CLoginDBRoutine::Add(Int nEvent, CEventBase& EventRef, LLong llParam, CEventQueue::EVENT_TYPE eType)
{
	return m_EventQueuePtr->Add(nEvent, EventRef, llParam, eType);
}


