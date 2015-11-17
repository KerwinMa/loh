//////////////////////////////////////////////////////////////////
//   The Legend of Heros                                        //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : GateServerImp.h                              //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ���ط���������ʵ��                                 //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __GATE_SERVER_IMP_H__
#define __GATE_SERVER_IMP_H__

#pragma once

#include "gamedbpacket.h"
#include "gamepacket.h"
#include "gatepacket.h"
#include "loginpacket.h"
#include "servermap.h"
#include "serversort.h"
#include "ServerConfig.h"
#include "CommonServer.h"
#include "CommonRoutine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGateServer : ���ط�����
// ��Ҫ���� :
// 1. ά����ͻ��˵�����
// 2. ��Ҫר�ŵ����������������, �߲���!!!
class CGateServer : public ICommonServer, public CPAKHandler
{
public:
	CGateServer(void);
	virtual ~CGateServer(void);
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
	virtual bool   OnUdpDispatch(const PacketPtr& PktPtr, PUDP_PARAM pUdp) OVERRIDE;
	virtual bool   OnTcpAccept(KeyRef krAccept, KeyRef krListen) OVERRIDE;
	virtual bool   OnTcpConnect(UInt uError, KeyRef krConnect) OVERRIDE;
	virtual bool   OnTcpClose(KeyRef krSocket, LLong llLiveData) OVERRIDE;
	virtual bool   OnUdpClose(KeyRef krSocket, LLong llLiveData) OVERRIDE;
	// ...
	CEventHandler* GetUIHandler(void);
	CServerConfig* GetConfig(void);
	ICommonServer* GetShareGameDBServer(void);
	ICommonServer* GetShareGameServer(void);
	KeyRef         GetConnectGameDB(void);
	KeyRef         GetConnectGame(void);
	KeyRef         GetUDPService(void);
	CFileLog&      GetFileLog(void);
	CNetworkPtr&   GetNetworkPtr(void);
	GATE_ADDR&     GetGateAddr(void);
private:
	CGateServer(const CGateServer&);
	CGateServer& operator=(const CGateServer&);
	// ��ȡ��������ö�����������
	bool  InitLoadShare(void);
	// ��ʼ������
	bool  InitLoadConfig(void);

	// �����������ö�����������
	void  ExitUnloadShare(void);
	// 
	void  ExitUnloadConfig(void);

	// ���д���������ϷDB�����������Ӷ���
	bool  StartConnectGameDBServer(void);
	// ���д���������Ϸ�����������Ӷ���
	bool  StartConnectGameServer(void);
	// ���д���UDP������½������У�����Ӷ���
	bool  StartUDPService(void);
	// ���д���TCP�����ͻ������Ӷ���
	bool  StartTCPService(void);

	// ֹͣ������ϷDB������
	void  StopConnectGameDBServer(void);
	// ֹͣ������Ϸ������
	void  StopConnectGameServer(void);
	// ֹͣUDP������½������
	void  StopUDPService(void);
	// ֹͣTCP�����ͻ���
	void  StopTCPService(void);

	// ������ϷDB�������������
	bool  DispatchGameDBServer(const PacketPtr& PktPtr, KeyRef krSocket);
	// ������Ϸ�������������
	bool  DispatchGameServer(const PacketPtr& PktPtr, KeyRef krSocket);

	// ������ϷDB������
	void  LinkGameDBServer(void);
	// ������ϷDB������
	void  UnlinkGameDBServer(void);
	// ������Ϸ������
	void  LinkGameServer(void);
	// ������Ϸ������
	void  UnlinkGameServer(void);

	// ��ʱ���������ϷDB�����������Ӷ����Ƿ���Ч
	bool  CheckConnectGameDBServer(void);
	// ��ʱ���������Ϸ�����������Ӷ����Ƿ���Ч
	bool  CheckConnectGameServer(void);
	// ��ʱ���UDP������½�����������Ӷ����Ƿ���Ч
	bool  CheckUDPService(void);
	// ��ʱ���TCP�����ͻ��˵����Ӷ����Ƿ���Ч
	bool  CheckTCPService(void);

	// ͬ����������Ϣ������
	bool  SyncServerInfo(void);
private:
	Int                 m_nStatus;          // ������״̬
	bool                m_bGameDBCnnted;
	bool                m_bGameDBLinked;
	bool                m_bGameCnnted;
	bool                m_bGameLinked;
	CEventHandler*      m_pUIHandler;       // ����ص��ӿ�
	CServerConfig*      m_pConfig;          // ���ö���
	KeyRef              m_krConnectGameDB;  // ����, ������ϷDB������
	KeyRef              m_krConnectGame;    // ����, ������Ϸ������
	KeyRef              m_krUDPService;     // ����, ������½������
	KeyRef              m_krTCPService;     // ����, �����ͻ���
	ICommonServer*      m_pShareGameDBSvr;
	ICommonServer*      m_pShareGameSvr;
	CFileLog            m_FileLog;          // ���ı���־
	CNetworkPtr         m_NetworkPtr;       // �������
	CCommonRoutinePtr   m_GateRoutine;      //
	GATE_ADDR           m_ServerInfo;       // ����������Ϣ 
};

INLINE CGateServer::CGateServer(void)
: m_nStatus(STATUSC_NONE)
, m_bGameDBCnnted(false)
, m_bGameDBLinked(false)
, m_bGameCnnted(false)
, m_bGameLinked(false)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krConnectGameDB(nullptr)
, m_krConnectGame(nullptr)
, m_krUDPService(nullptr)
, m_krTCPService(nullptr)
, m_pShareGameDBSvr(nullptr)
, m_pShareGameSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::GateServer)
{
}

INLINE CGateServer::~CGateServer(void)
{
}

INLINE CGateServer::CGateServer(const CGateServer&)
: m_nStatus(STATUSC_NONE)
, m_bGameDBCnnted(false)
, m_bGameDBLinked(false)
, m_bGameCnnted(false)
, m_bGameLinked(false)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krConnectGameDB(nullptr)
, m_krConnectGame(nullptr)
, m_krUDPService(nullptr)
, m_krTCPService(nullptr)
, m_pShareGameDBSvr(nullptr)
, m_pShareGameSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::GateServer)
{
}

INLINE CGateServer& CGateServer::operator=(const CGateServer&)
{
	return (*this);
}

INLINE Long CGateServer::Release(void)
{
	Long lRef = m_Counter.Reset();
	if (lRef == 0) {
		MDELETE this;
	}
	return lRef;
}

INLINE bool CGateServer::OnShareRoutine(Int, uintptr_t, LLong, CEventQueue::EVENT_TYPE)
{
	return false;
}

INLINE bool CGateServer::OnShareRoutine(Int, CStream&, LLong)
{
	return false;
}

INLINE CEventHandler* CGateServer::GetUIHandler(void)
{
	return m_pUIHandler;
}

INLINE CServerConfig* CGateServer::GetConfig(void)
{
	return m_pConfig;
}

INLINE ICommonServer* CGateServer::GetShareGameDBServer(void)
{
	return m_pShareGameDBSvr;
}

INLINE ICommonServer* CGateServer::GetShareGameServer(void)
{
	return m_pShareGameSvr;
}

INLINE KeyRef CGateServer::GetConnectGameDB(void)
{
	return m_krConnectGameDB;
}

INLINE KeyRef CGateServer::GetConnectGame(void)
{
	return m_krConnectGame;
}

INLINE KeyRef CGateServer::GetUDPService(void)
{
	return m_krUDPService;
}

INLINE CFileLog& CGateServer::GetFileLog(void)
{
	return m_FileLog;
}

INLINE CNetworkPtr& CGateServer::GetNetworkPtr(void)
{
	return m_NetworkPtr;
}

INLINE GATE_ADDR& CGateServer::GetGateAddr(void)
{
	return m_ServerInfo;
}

#endif // __GATE_SERVER_IMP_H__
