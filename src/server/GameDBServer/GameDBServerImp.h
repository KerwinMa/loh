//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : GameDBServerImp.h                            //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ��ϷDB����������ʵ��                               //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __GAMEDB_SERVER_IMP_H__
#define __GAMEDB_SERVER_IMP_H__

#pragma once

#include "gamedbpacket.h"
#include "gamepacket.h"
#include "gatepacket.h"
#include "servermap.h"
#include "ServerConfig.h"
#include "CommonServer.h"
#include "CommonRoutine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGameDBServer : ��ϷDB������
// ��Ҫ���� :
// 1. ��ɫ��ص����ݿ����
// 2. �������˺ŵĽ�ɫ������ʱ�򻺴浽�ڴ�
// 3. ��������ݿ����, ��Ҫר�ŵ����������������
class CGameDBServer : public ICommonServer, public CPAKHandler
{
public:
	enum INFO_INDEX 
	{
		INFOI_GAMEDB,
		INFOI_GAME,
		INFOI_GATE,
		INFOI_MAX,
	};
public:
	CGameDBServer(void);
	virtual ~CGameDBServer(void);
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
	ICommonServer* GetShareGameServer(void);
	ICommonServer* GetShareGateServer(void);
	CFileLog&      GetFileLog(void);
	CNetworkPtr&   GetNetworkPtr(void);
private:
	CGameDBServer(const CGameDBServer&);
	CGameDBServer& operator=(const CGameDBServer&);
	// ��ȡ��������ö�����������
	bool  InitLoadShare(void);
	// ��ʼ������
	bool  InitLoadConfig(void);

	// �����������ö�����������
	void  ExitUnloadShare(void);
	// 
	void  ExitUnloadConfig(void);

	// ���д���������Ϸ���������ӵ����Ӷ���
	bool  StartListenGameServer(void);
	// ���д����������ط��������ӵ����Ӷ���
	bool  StartListenGateServer(void);

	// ֹͣ������Ϸ����������
	void  StopListenGameServer(void);
	// ֹͣ�������ط���������
	void  StopListenGateServer(void);

	// ��ʱ��������Ϸ���������ӵ����Ӷ����Ƿ���Ч
	bool  CheckListenGameServer(void);
	// ��ʱ���������ط��������ӵ����Ӷ����Ƿ���Ч
	bool  CheckListenGateServer(void);

	// ͬ����������Ϣ������
	bool  SyncServerInfo(void);

	// ͬ���̷���������
	bool  OnShareLink(CEventBase& EventRef, LLong llParam);
	bool  OnShareUpdate(CEventBase& EventRef, LLong llParam);
	// ����������
	bool  OnServerLink(CPAKLink* pLink, KeyRef krSocket);
	bool  OnServerUpdate(CPAKUpdate* pUpdate, KeyRef krSocket);
	bool  OnServerUnlink(CPAKHead* pUnlink, KeyRef krSocket);

	template <typename MAP_TYPE, DATA_INDEX DataIndex, INFO_INDEX InfoIndex>
	bool  ServerLink(CPAKLink* pLink, DataRef drServer, MAP_TYPE& MapRef);

	template <typename MAP_TYPE, DATA_INDEX DataIndex, INFO_INDEX InfoIndex>
	bool  ServerUpdate(CPAKUpdate* pUpdate, DataRef drServer, MAP_TYPE& MapRef);

	template <typename MAP_TYPE, DATA_INDEX DataIndex, INFO_INDEX InfoIndex>
	bool  ServerUnlink(KeyRef krSocket, MAP_TYPE& MapRef);

private:
	Int                    m_nStatus;         // ������״̬
	CEventHandler*         m_pUIHandler;      // ����ص��ӿ�
	CServerConfig*         m_pConfig;         // ���ö���
	KeyRef                 m_krListenGame;    // ����, ������Ϸ������
	KeyRef                 m_krListenGate;    // ����, �������ط�����
	ICommonServer*         m_pShareGameSvr;
	ICommonServer*         m_pShareGateSvr;
	CFileLog               m_FileLog;         // ���ı���־
	CNetworkPtr            m_NetworkPtr;      // �������
	CCommonRoutinePtr      m_GameDBRoutine;   // 
	SERVER_INFO            m_ServerInfo[INFOI_MAX]; // gamedb, game, gate
	SVR_GAME_MAP           m_GameSvrMap;      // ��Ϸ��������Ϣ
	SVR_GATE_MAP           m_GateSvrMap;      // ���ط�������Ϣ  
};

INLINE CGameDBServer::CGameDBServer(void)
: m_nStatus(STATUSC_NONE)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krListenGame(nullptr)
, m_krListenGate(nullptr)
, m_pShareGameSvr(nullptr)
, m_pShareGateSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::GameDBServer)
{
}

INLINE CGameDBServer::~CGameDBServer(void)
{
}

INLINE CGameDBServer::CGameDBServer(const CGameDBServer&)
: m_nStatus(STATUSC_NONE)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krListenGame(nullptr)
, m_krListenGate(nullptr)
, m_pShareGameSvr(nullptr)
, m_pShareGateSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::GameDBServer)
{
}

INLINE CGameDBServer& CGameDBServer::operator=(const CGameDBServer&)
{
	return (*this);
}

INLINE bool CGameDBServer::OnShareRoutine(Int, uintptr_t, LLong, CEventQueue::EVENT_TYPE)
{
	return false;
}

INLINE bool CGameDBServer::OnShareRoutine(Int, CStream&, LLong)
{
	return false;
}

INLINE bool CGameDBServer::OnUdpDispatch(const PacketPtr&, PUDP_PARAM)
{
	// PUDP_PARAM->pCache == nullptr, �ϲ���Ա���ӵײ�����Ļ�������
	// PUDP_PARAM->pCache, PUDP_PARAM->index�ǵײ�����Ļ������ݵ���ʼ��ַ��������
	// ����ϲ㱣�滺��, ����PUDP_PARAM->pDataΪnullptr֪ͨ����ײ�
	// �ͷŻ���ʹ��MObject::sMCFree(index, pCache)
	return false;
}

INLINE bool CGameDBServer::OnTcpConnect(UInt, KeyRef)
{
	return false;
}

INLINE bool CGameDBServer::OnUdpClose(KeyRef, LLong)
{
	return false;
}

INLINE CEventHandler* CGameDBServer::GetUIHandler(void)
{
	return m_pUIHandler;
}

INLINE CServerConfig* CGameDBServer::GetConfig(void)
{
	return m_pConfig;
}

INLINE ICommonServer* CGameDBServer::GetShareGameServer(void)
{
	return m_pShareGameSvr;
}

INLINE ICommonServer* CGameDBServer::GetShareGateServer(void)
{
	return m_pShareGateSvr;
}

INLINE CFileLog& CGameDBServer::GetFileLog(void)
{
	return m_FileLog;
}

INLINE CNetworkPtr& CGameDBServer::GetNetworkPtr(void)
{
	return m_NetworkPtr;
}

template <typename MAP_TYPE, DATA_INDEX DataIndex, CGameDBServer::INFO_INDEX InfoIndex>
INLINE bool CGameDBServer::ServerLink(CPAKLink* pLink, DataRef drServer, MAP_TYPE& MapRef)
{
	PINDEX index = nullptr;
	// 1.��ȡ��Ϣ
	MAP_TYPE::SVR_PAIR Pair;
	Pair.drKey = drServer;
	Pair.Value.Copy(pLink->GetServerData());
	assert(Pair.Value.usStatus == STATUSU_LINK);
	assert(pLink->CheckStream());
	Pair.Value.Addr(pLink->GetStream());
	{
		CSyncLockWaitScope scope(MapRef.GetLock());
		assert(MapRef.Find(drServer) == nullptr);
		index = MapRef.Add(drServer, Pair.Value);
	}
	if (index != nullptr) { // 2.���½���
		m_pUIHandler->OnHandle(PAK_EVENT_LINK, reinterpret_cast<uintptr_t>(&Pair), DataIndex);
		m_ServerInfo[InfoIndex].Incr(pLink->GetServerData());
		return true;
	}
	return false;
}

template <typename MAP_TYPE, DATA_INDEX DataIndex, CGameDBServer::INFO_INDEX InfoIndex>
INLINE bool CGameDBServer::ServerUpdate(CPAKUpdate* pUpdate, DataRef drServer, MAP_TYPE& MapRef)
{
	MAP_TYPE::SVR_PAIR Pair;
	Pair.drKey = drServer;
	{
		CSyncLockWaitScope scope(MapRef.GetLock());
		MAP_TYPE::SVR_MAP_PAIR* pPair = MapRef.Find(drServer);
		if ((pPair != nullptr) &&
			(pPair->m_V.usStatus > STATUSU_NONE) &&
			(pPair->m_V.usStatus < STATUSU_UNLINK)) {
			Pair.Value.Copy(pPair->m_V);
			pPair->m_V.Copy(pUpdate->GetServerData());
		}
	}
	if (Pair.Value.usStatus > STATUSU_NONE) {
		m_ServerInfo[InfoIndex].Diff(pUpdate->GetServerData(), Pair.Value);
		Pair.Value.Copy(pUpdate->GetServerData());
		m_pUIHandler->OnHandle(PAK_EVENT_UPDATE, reinterpret_cast<uintptr_t>(&Pair), DataIndex);// ���½���
		return true;
	}
	return false;
}

template <typename MAP_TYPE, DATA_INDEX DataIndex, CGameDBServer::INFO_INDEX InfoIndex>
INLINE bool CGameDBServer::ServerUnlink(KeyRef krSocket, MAP_TYPE& MapRef)
{
	// 2.���½���
	m_pUIHandler->OnHandle(PAK_EVENT_UNLINK, reinterpret_cast<uintptr_t>(krSocket), DataIndex);

	SERVER_DATA sd = { 0 };
	{
		CSyncLockWaitScope scope(MapRef.GetLock());
		MAP_TYPE::SVR_MAP_PAIR* pPair = MapRef.Find((DataRef)krSocket);
		if (pPair != nullptr) {
			sd = pPair->m_V;
			MapRef.RemoveAt(reinterpret_cast<PINDEX>(pPair));
		}
	}
	if (sd.usStatus > STATUSU_NONE) {
		m_ServerInfo[InfoIndex].Decr(sd);
		return true;
	}
	return false;
}
#endif // __GAMEDB_SERVER_IMP_H__
