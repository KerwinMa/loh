//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : GateServerImp.h                              //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update : 2015-11-25     version 0.0.0.5                    //
//   Detail : ���ط�����ʵ��                                     //
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
/// ���ط�����ʵ��
/// - ��Ҫ���� :
/// -# ά����ͻ��˵�����
/// -# ��Ҫר�ŵ����������������, �߲���!!!
/// -# �����������������������
class CGateServer : public ICommonServer, public CPAKHandler
{
public:
	CGateServer(void);
	virtual ~CGateServer(void);
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
	Int            GetGameId(void);
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
	/// ��ȡ��������ö�����������
	bool  InitLoadShare(void);
	/// ��ʼ������
	bool  InitLoadConfig(void);

	/// �����������ö�����������
	void  ExitUnloadShare(void);
	/// �������
	void  ExitUnloadConfig(void);

	/// ����������ϷDB����������
	bool  StartConnectGameDBServer(void);
	/// ����������Ϸ����������
	bool  StartConnectGameServer(void);
	/// ����UDP������½����������
	bool  StartUDPService(void);
	/// ����TCP�����ͻ��˶���
	bool  StartTCPService(void);

	/// ֹͣ������ϷDB������
	void  StopConnectGameDBServer(void);
	/// ֹͣ������Ϸ������
	void  StopConnectGameServer(void);
	/// ֹͣUDP������½������
	void  StopUDPService(void);
	/// ֹͣTCP�����ͻ���
	void  StopTCPService(void);

	/// ������ϷDB�������������
	bool  DispatchGameDBServer(const PacketPtr& PktPtr, KeyRef krSocket);
	/// ������Ϸ�������������
	bool  DispatchGameServer(const PacketPtr& PktPtr, KeyRef krSocket);

	/// ����ϷDB������ע��
	void  LinkGameDBServer(void);
	/// ����ϷDB������ע��
	void  UnlinkGameDBServer(void);
	/// ����Ϸ������ע��
	void  LinkGameServer(void);
	/// ����Ϸ������ע��
	void  UnlinkGameServer(void);

	/// ��ʱ���������ϷDB�����������Ƿ���Ч
	bool  CheckConnectGameDBServer(void);
	/// ��ʱ���������Ϸ�����������Ƿ���Ч
	bool  CheckConnectGameServer(void);
	/// ��ʱ���UDP������½�����������Ƿ���Ч
	bool  CheckUDPService(void);
	/// ��ʱ���TCP�����ͻ��˶����Ƿ���Ч
	bool  CheckTCPService(void);

	// ͬ����������Ϣ������
	bool  SyncServerInfo(void);
private:
	Int                 m_nStatus;          ///< ������״̬
	Int                 m_nGameId;          ///< ���ӵ���Ϸ������Id
	bool                m_bGameDBCnnted;    ///< �Ƿ�������ϷDB���������
	bool                m_bGameDBLinked;    ///< �Ƿ�����ϷDB������ע�����
	bool                m_bGameCnnted;      ///< �Ƿ�������Ϸ���������
	bool                m_bGameLinked;      ///< �Ƿ�����Ϸ������ע�����
	CEventHandler*      m_pUIHandler;       ///< ����ص��ӿ�
	CServerConfig*      m_pConfig;          ///< ���ö���
	KeyRef              m_krConnectGameDB;  ///< ����, ������ϷDB������
	KeyRef              m_krConnectGame;    ///< ����, ������Ϸ������
	KeyRef              m_krUDPService;     ///< ����, ������½������
	KeyRef              m_krTCPService;     ///< ����, �����ͻ���
	ICommonServer*      m_pShareGameDBSvr;  ///< ͬ������ϷDB������
	ICommonServer*      m_pShareGameSvr;    ///< ͬ������Ϸ������
	CFileLog            m_FileLog;          ///< ���ı���־
	CNetworkPtr         m_NetworkPtr;       ///< �������
	CCommonRoutinePtr   m_GateRoutine;      ///< ��Ϸ������ 
	GATE_ADDR           m_ServerInfo;       ///< ����������Ϣ 
};

INLINE CGateServer::CGateServer(void)
: m_nStatus(STATUSC_NONE)
, m_nGameId(0)
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
, m_nGameId(0)
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

INLINE bool CGateServer::OnShareRoutine(Int, uintptr_t, LLong, CEventQueue::EVENT_TYPE)
{
	return false;
}

INLINE bool CGateServer::OnShareRoutine(Int, CStream&, LLong)
{
	return false;
}

INLINE Int CGateServer::GetGameId(void)
{
	return m_nGameId;
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
