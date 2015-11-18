//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : LoginDBServerImp.h                           //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ��½DB����������ʵ��                               //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __LOGINDB_SERVER_IMP_H__
#define __LOGINDB_SERVER_IMP_H__

#pragma once

#include "logindbpacket.h"
#include "loginpacket.h"
#include "servermap.h"
#include "ServerConfig.h"
#include "CommonServer.h"
#include "CommonRoutine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLoginDBServer : ��½DB������
// ��Ҫ���� :
// 1. �˺���ص����ݿ����
// 2. �������˺�������ʱ�򻺴浽�ڴ�
// 3. ��������ݿ����, ��Ҫר�ŵ����������������
class CLoginDBServer : public ICommonServer, public CPAKHandler
{
public:
	CLoginDBServer(void);
	virtual ~CLoginDBServer(void);
	// CTRefCount
	//virtual Long   AddRef(void) OVERRIDE;
	virtual Long   Release(void) OVERRIDE;
	// CComponent
	// Command to set param value
	virtual UInt   Command(PCXStr pszCMD, uintptr_t utParam) OVERRIDE;
	virtual UInt   Update(void) OVERRIDE;

	virtual UInt   Init(CEventHandler& EventHandlerRef) OVERRIDE;
	virtual void   Exit(void) OVERRIDE;

	virtual bool   Start(void) OVERRIDE;
	virtual bool   Pause(bool bPause = true) OVERRIDE;
	virtual void   Stop(void) OVERRIDE;

	virtual bool   OnShareRoutine(Int nEvent, uintptr_t utParam = 0, LLong llParam = 0, CEventQueue::EVENT_TYPE eType = CEventQueue::EVENT_TYPE_NONE) OVERRIDE;//
	virtual bool   OnShareRoutine(Int nEvent, CEventBase& EventRef, LLong llParam = 0, CEventQueue::EVENT_TYPE eType = CEventQueue::EVENT_TYPE_REFCOUNT) OVERRIDE;
	virtual bool   OnShareRoutine(Int nEvent, CStream& Stream, LLong llParam = 0) OVERRIDE;//
	// CPAKHandler
	virtual bool   OnTcpDispatch(const PacketPtr& PktPtr, PTCP_PARAM pTcp) OVERRIDE;
	virtual bool   OnUdpDispatch(const PacketPtr& PktPtr, PUDP_PARAM pUdp) OVERRIDE;//
	virtual bool   OnTcpAccept(KeyRef krAccept, KeyRef krListen) OVERRIDE;
	virtual bool   OnTcpConnect(UInt uError, KeyRef krConnect) OVERRIDE;//
	virtual bool   OnTcpClose(KeyRef krSocket, LLong llLiveData) OVERRIDE;
	virtual bool   OnUdpClose(KeyRef krSocket, LLong llLiveData) OVERRIDE;//
	// ...
	CEventHandler* GetUIHandler(void);
	CServerConfig* GetConfig(void);
	ICommonServer* GetShareServer(void);
	CFileLog&      GetFileLog(void);
	CNetworkPtr&   GetNetworkPtr(void);
private:
	CLoginDBServer(const CLoginDBServer&);
	CLoginDBServer& operator=(const CLoginDBServer&);
	// ��ȡ��������ö�����������
	bool  InitLoadShare(void);
	// ��ʼ������
	bool  InitLoadConfig(void);

	// �����������ö�����������
	void  ExitUnloadShare(void);
	// 
	void  ExitUnloadConfig(void);

	// ���д���������½���������ӵ����Ӷ���
	bool  StartListenLoginServer(void);

	// ֹͣ������½����������
	void  StopListenLoginServer(void);

	// ��ʱ��������½���������ӵ����Ӷ����Ƿ���Ч
	bool  CheckListenLoginServer(void);

	// ͬ����������Ϣ������
	bool  SyncServerInfo(void);

	// ͬ���̷���������
	bool  OnShareLink(CEventBase& EventRef, LLong llParam);
	bool  OnShareUpdate(CEventBase& EventRef, LLong llParam);
	// ����������
	bool  OnServerLink(CPAKLink* pLink, KeyRef krSocket);
	bool  OnServerUpdate(CPAKUpdate* pUpdate, KeyRef krSocket);
	bool  OnServerUnlink(CPAKHead* pUnlink, KeyRef krSocket);

	bool  ServerLink(CPAKLink* pLink, DataRef drServer);
	bool  ServerUpdate(CPAKUpdate* pUpdate, DataRef drServer);
	bool  ServerUnlink(KeyRef krSocket);
private:
	Int                    m_nStatus;         // ������״̬
	CEventHandler*         m_pUIHandler;      // ����ص��ӿ�
	CServerConfig*         m_pConfig;         // ���ö���
	KeyRef                 m_krListenLogin;   // ����, ������½������
	ICommonServer*         m_pShareLoginSvr;
	CFileLog               m_FileLog;         // ���ı���־
	CNetworkPtr            m_NetworkPtr;      // �������
	CCommonRoutinePtr      m_LoginDBRoutine;  // 
	SERVER_INFO            m_ServerInfo;      // ����������Ϣ
	SVR_LOGIN_MAP          m_LoginSvrMap;     // ��½��������Ϣ   
};

INLINE CLoginDBServer::CLoginDBServer(void)
: m_nStatus(STATUSC_NONE)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krListenLogin(nullptr)
, m_pShareLoginSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::LoginDBServer)
{
}

INLINE CLoginDBServer::~CLoginDBServer(void)
{
}

INLINE CLoginDBServer::CLoginDBServer(const CLoginDBServer&)
: m_nStatus(STATUSC_NONE)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krListenLogin(nullptr)
, m_pShareLoginSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::LoginDBServer)
{
}

INLINE CLoginDBServer& CLoginDBServer::operator=(const CLoginDBServer&)
{
	return (*this);
}

INLINE Long CLoginDBServer::Release(void)
{
	Long lRef = m_Counter.Reset();
	if (lRef == 0) {
		MDELETE this;
	}
	return lRef;
}

INLINE bool CLoginDBServer::OnShareRoutine(Int, uintptr_t, LLong, CEventQueue::EVENT_TYPE)
{
	return false;
}

INLINE bool CLoginDBServer::OnShareRoutine(Int, CStream&, LLong)
{
	return false;
}

INLINE bool CLoginDBServer::OnUdpDispatch(const PacketPtr&, PUDP_PARAM)
{
	// PUDP_PARAM->pCache == nullptr, �ϲ���Ա���ӵײ�����Ļ�������
	// PUDP_PARAM->pCache, PUDP_PARAM->index�ǵײ�����Ļ������ݵ���ʼ��ַ��������
	// ����ϲ㱣�滺��, ����PUDP_PARAM->pDataΪnullptr֪ͨ����ײ�
	// �ͷŻ���ʹ��MObject::sMCFree(index, pCache)
	return false;
}

INLINE bool CLoginDBServer::OnTcpConnect(UInt, KeyRef)
{
	return false;
}

INLINE bool CLoginDBServer::OnUdpClose(KeyRef, LLong)
{
	return false;
}

INLINE CEventHandler* CLoginDBServer::GetUIHandler(void)
{
	return m_pUIHandler;
}

INLINE CServerConfig* CLoginDBServer::GetConfig(void)
{
	return m_pConfig;
}

INLINE ICommonServer* CLoginDBServer::GetShareServer(void)
{
	return m_pShareLoginSvr;
}

INLINE CFileLog& CLoginDBServer::GetFileLog(void)
{ 
	return m_FileLog;
}

INLINE CNetworkPtr& CLoginDBServer::GetNetworkPtr(void)
{
	return m_NetworkPtr;
}

INLINE bool CLoginDBServer::ServerLink(CPAKLink* pLink, DataRef drServer)
{
	PINDEX index = nullptr;
	// 1.��ȡ��Ϣ
	SVR_LOGIN_MAP::SVR_PAIR Pair;
	Pair.drKey = drServer;
	Pair.Value.Copy(pLink->GetServerData());
	assert(Pair.Value.usStatus == STATUSU_LINK);
	assert(pLink->CheckStream());
	Pair.Value.Addr(pLink->GetStream());
	{
		CSyncLockWaitScope scope(m_LoginSvrMap.GetLock());
		assert(m_LoginSvrMap.Find(drServer) == nullptr);
		index = m_LoginSvrMap.Add(drServer, Pair.Value);
	}
	if (index != nullptr) { // 2.���½���
		m_pUIHandler->OnHandle(PAK_EVENT_LINK, reinterpret_cast<uintptr_t>(&Pair), DATA_INDEX_LOGIN);
		return true;
	}
	return false;
}

INLINE bool CLoginDBServer::ServerUpdate(CPAKUpdate* pUpdate, DataRef drServer)
{
	SVR_LOGIN_MAP::SVR_PAIR Pair;
	Pair.drKey = drServer;
	Pair.Value.Copy(pUpdate->GetServerData());
	m_pUIHandler->OnHandle(PAK_EVENT_UPDATE, reinterpret_cast<uintptr_t>(&Pair), DATA_INDEX_LOGIN);// ���½���
	{
		CSyncLockWaitScope scope(m_LoginSvrMap.GetLock());
		SVR_LOGIN_MAP::SVR_MAP_PAIR* pPair = m_LoginSvrMap.Find(drServer);
		if ((pPair != nullptr) &&
			(pPair->m_V.usStatus > STATUSU_NONE) &&
			(pPair->m_V.usStatus < STATUSU_UNLINK)) {
			pPair->m_V.Copy(pUpdate->GetServerData());
			return true;
		}
	}
	return false;
}

INLINE bool CLoginDBServer::ServerUnlink(KeyRef krSocket)
{
	// 1.���½���
	m_pUIHandler->OnHandle(PAK_EVENT_UNLINK, reinterpret_cast<uintptr_t>(krSocket), DATA_INDEX_LOGIN);
	{
		CSyncLockWaitScope scope(m_LoginSvrMap.GetLock());
		return m_LoginSvrMap.Remove((DataRef)krSocket);
	}
}

#endif // __LOGINDB_SERVER_IMP_H__
