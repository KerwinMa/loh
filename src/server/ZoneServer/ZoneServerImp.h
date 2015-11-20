//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : ZoneServerImp.h                              //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ��ͼ����������ʵ��                                 //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __ZONE_SERVER_IMP_H__
#define __ZONE_SERVER_IMP_H__

#pragma once

#include "zonepacket.h"
#include "servermap.h"
#include "ServerConfig.h"
#include "CommonServer.h"
#include "CommonRoutine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoneServer : ��ͼ������
// ��Ҫ���� :
// 1. ����ͻ��˵���Ϸҵ��
class CZoneServer : public ICommonServer, public CPAKHandler
{
public:
	CZoneServer(void);
	virtual ~CZoneServer(void);
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
	virtual bool   OnTcpAccept(KeyRef krAccept, KeyRef krListen) OVERRIDE;//
	virtual bool   OnTcpConnect(UInt uError, KeyRef krConnect) OVERRIDE;
	virtual bool   OnTcpClose(KeyRef krSocket, LLong llLiveData) OVERRIDE;
	virtual bool   OnUdpClose(KeyRef krSocket, LLong llLiveData) OVERRIDE;
private:
	CZoneServer(const CZoneServer&);
	CZoneServer& operator=(const CZoneServer&);
	// ��ȡ��������ö�����������
	bool  InitLoadShare(void);
	// ��ʼ������
	bool  InitLoadConfig(void);

	// �����������ö�����������
	void  ExitUnloadShare(void);
	// 
	void  ExitUnloadConfig(void);

	// ���д���������Ϸ�����������Ӷ���
	bool  StartConnectGameServer(void);

	// ֹͣ������Ϸ������
	void  StopConnectGameServer(void);
	// ������Ϸ������
	void  LinkGameServer(void);
	// ������Ϸ������
	void  UnlinkGameServer(void);

	// ��ʱ���������Ϸ�����������Ӷ����Ƿ���Ч
	bool  CheckConnectGameServer(void);

	// ͬ����������Ϣ������
	bool  SyncServerInfo(void);
private:
	Int                    m_nStatus;         // ������״̬
	bool                   m_bGameCnnted;
	bool                   m_bGameLinked;
	CEventHandler*         m_pUIHandler;      // ����ص��ӿ�
	CServerConfig*         m_pConfig;         // ���ö���
	KeyRef                 m_krConnectGame;    // ����, ������Ϸ������
	ICommonServer*         m_pShareGameSvr;
	CFileLog               m_FileLog;         // ���ı���־
	CNetworkPtr            m_NetworkPtr;      // �������
	ZONE_ADDR              m_ServerInfo;      // ����������Ϣ
};

INLINE CZoneServer::CZoneServer(void)
: m_nStatus(STATUSC_NONE)
, m_bGameCnnted(false)
, m_bGameLinked(false)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krConnectGame(nullptr)
, m_pShareGameSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::ZoneServer)
{
}

INLINE CZoneServer::~CZoneServer(void)
{
}

INLINE CZoneServer::CZoneServer(const CZoneServer&)
: m_nStatus(STATUSC_NONE)
, m_bGameCnnted(false)
, m_bGameLinked(false)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krConnectGame(nullptr)
, m_pShareGameSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::ZoneServer)
{
}

INLINE CZoneServer& CZoneServer::operator=(const CZoneServer&)
{
	return (*this);
}

INLINE bool CZoneServer::OnTcpAccept(KeyRef, KeyRef)
{
	return false;
}

#endif // __ZONE_SERVER_IMP_H__
