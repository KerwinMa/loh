//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : CenterServerImp.h                            //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ���ķ���������ʵ��                                 //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __CENTER_SERVER_IMP_H__
#define __CENTER_SERVER_IMP_H__

#pragma once

#include "centerpacket.h"
#include "servermap.h"
#include "ServerConfig.h"
#include "CommonServer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CCenterServer : ���ķ�����
// ��Ҫ���� :
// 1. ά������ѡ��, ��½����Ϸ��������Ϣ, 
// 2. ��ʱͬ����½��������Ϣ��ѡ�������, ��ʱͬ����Ϸ��������Ϣ����½������, ��ʱͬ����½��������Ϣ����Ϸ������
// 3. �����, ȫ��ʹ�������̺߳Ͷ�ʱ�߳�������
class CCenterServer : public ICommonServer, public CPAKHandler
{
public:
	enum INFO_INDEX
	{
		INFOI_CENTER,
		INFOI_SELECT,
		INFOI_LOGIN,
		INFOI_GAME,
		INFOI_MAX,
	};
public:
	CCenterServer(void);
	virtual ~CCenterServer(void);
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
private:
	CCenterServer(const CCenterServer&);
	CCenterServer& operator=(const CCenterServer&);
	// ��ȡ��������ö�����������
	bool  InitLoadShare(void);
	// ��ʼ������
	bool  InitLoadConfig(void);

	// �����������ö�����������
	void  ExitUnloadShare(void);
	// 
	void  ExitUnloadConfig(void);

	// ���д�������ѡ��, ��½����Ϸ���������ӵ����Ӷ���
	bool  StartListenServers(void);
	// ���д�������������������
	bool  StartListenServer(KeyRef& krListen, const CStringKey& strAddr, UShort usPort);

	// ֹͣ��������������
	void  StopListenServers(void);

	// ��ʱ���������������ӵ����Ӷ����Ƿ���Ч
	bool  CheckListenServers(void);
	// ��ʱ������ѡ����������ӵ����Ӷ����Ƿ���Ч
	bool  CheckListenSelectServer(void);
	// ��ʱ��������½���������ӵ����Ӷ����Ƿ���Ч
	bool  CheckListenLoginServer(void);
	// ��ʱ��������Ϸ���������ӵ����Ӷ����Ƿ���Ч
	bool  CheckListenGameServer(void);

	// ͬ����������Ϣ
	bool  SyncServersInfo(void);
	bool  SyncCenterServerInfo(void);
	bool  SyncSelectServerInfo(void);
	bool  SyncLoginServerInfo(void);
	bool  SyncGameServerInfo(void);
	bool  SendLoginServerInfo(KeyRef krSocket = nullptr);
	bool  SendGameServerInfo(KeyRef krSocket = nullptr);

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
	Int                    m_nStatus;        // ������״̬
	CEventHandler*         m_pUIHandler;     // ����ص��ӿ�
	CServerConfig*         m_pConfig;        // ���ö���
	KeyRef                 m_krListenSelect; // ����, ����ѡ�������
	KeyRef                 m_krListenLogin;  // ����, ������½������
	KeyRef                 m_krListenGame;   // ����, ������Ϸ������
	ICommonServer*         m_pShareSelectSvr;
	ICommonServer*         m_pShareLoginSvr;
	ICommonServer*         m_pShareGameSvr;
	CFileLog               m_FileLog;        // ���ı���־
	CNetworkPtr            m_NetworkPtr;     // �������
	SERVER_INFO            m_ServerInfo[INFOI_MAX];  // center, select, login, game
	SVR_SELECT_MAP         m_SelectSvrMap;   // ѡ���������Ϣ
	SVR_LOGIN_MAP          m_LoginSvrMap;    // ��½��������Ϣ
	SVR_GAME_MAP           m_GameSvrMap;     // ��Ϸ��������Ϣ
	// 1. �ӷ�������չ�����ļ��ж�ȡ���ݿ���Ϣ, ��ʱд��ͳ������
};

INLINE CCenterServer::CCenterServer(void)
: m_nStatus(STATUSC_NONE)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krListenSelect(nullptr)
, m_krListenLogin(nullptr)
, m_krListenGame(nullptr)
, m_pShareSelectSvr(nullptr)
, m_pShareLoginSvr(nullptr)
, m_pShareGameSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::CenterServer)
{
}

INLINE CCenterServer::~CCenterServer(void)
{
}

INLINE CCenterServer::CCenterServer(const CCenterServer&)
: m_nStatus(STATUSC_NONE)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krListenSelect(nullptr)
, m_krListenLogin(nullptr)
, m_krListenGame(nullptr)
, m_pShareSelectSvr(nullptr)
, m_pShareLoginSvr(nullptr)
, m_pShareGameSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::CenterServer)
{
}

INLINE CCenterServer& CCenterServer::operator=(const CCenterServer&)
{
	return (*this);
}

INLINE bool CCenterServer::OnShareRoutine(Int, uintptr_t, LLong, CEventQueue::EVENT_TYPE)
{
	return false;
}

INLINE bool CCenterServer::OnShareRoutine(Int, CStream&, LLong)
{
	return false;
}

INLINE bool CCenterServer::OnUdpDispatch(const PacketPtr&, PUDP_PARAM)
{
	// PUDP_PARAM->pCache == nullptr, �ϲ���Ա���ӵײ�����Ļ�������
	// PUDP_PARAM->pCache, PUDP_PARAM->index�ǵײ�����Ļ������ݵ���ʼ��ַ��������
	// ����ϲ㱣�滺��, ����PUDP_PARAM->pDataΪnullptr֪ͨ����ײ�
	// �ͷŻ���ʹ��MObject::sMCFree(index, pCache)
	return false;
}

INLINE bool CCenterServer::OnTcpConnect(UInt, KeyRef)
{
	return false;
}

INLINE bool CCenterServer::OnUdpClose(KeyRef, LLong)
{
	return false;
}

template <typename MAP_TYPE, DATA_INDEX DataIndex, CCenterServer::INFO_INDEX InfoIndex>
INLINE bool CCenterServer::ServerLink(CPAKLink* pLink, DataRef drServer, MAP_TYPE& MapRef)
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
		MAP_TYPE::SVR_MAP_PAIR* pPair = MapRef.Find(drServer);
		if (pPair == nullptr) {
			index = MapRef.Add(drServer, Pair.Value);
		}
		else if (pPair->m_V.usStatus == STATUSU_UNLINK) { // ��������Ϣδͬ��������������
			pPair->m_V = Pair.Value;
			index = reinterpret_cast<PINDEX>(pPair);
		}
	}
	if (index != nullptr) { // 2.���½���
		m_pUIHandler->OnHandle(PAK_EVENT_LINK, reinterpret_cast<uintptr_t>(&Pair), DataIndex);
		m_ServerInfo[InfoIndex].Incr(pLink->GetServerData());
		return true;
	}
	return false;
}

template <typename MAP_TYPE, DATA_INDEX DataIndex, CCenterServer::INFO_INDEX InfoIndex>
INLINE bool CCenterServer::ServerUpdate(CPAKUpdate* pUpdate, DataRef drServer, MAP_TYPE& MapRef)
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

template <typename MAP_TYPE, DATA_INDEX DataIndex, CCenterServer::INFO_INDEX InfoIndex>
INLINE bool CCenterServer::ServerUnlink(KeyRef krSocket, MAP_TYPE& MapRef)
{
	// 2.���½���
	m_pUIHandler->OnHandle(PAK_EVENT_UNLINK, reinterpret_cast<uintptr_t>(krSocket), DataIndex);

	SERVER_DATA sd = { 0 };
	{
		CSyncLockWaitScope scope(MapRef.GetLock());
		MAP_TYPE::SVR_MAP_PAIR* pPair = MapRef.Find((DataRef)krSocket);
		if (pPair != nullptr) {
			sd = pPair->m_V;
			// 3.�޸�״̬
			if ((DataIndex == DATA_INDEX_SELECT) || (pPair->m_V.usStatus <= STATUSU_LINK)) {
				MapRef.RemoveAt(reinterpret_cast<PINDEX>(pPair));// ��½�������ӵ�½������/�Ͽ�, ��Ϣ��δͬ��������������, ֱ����Ϊ��Ч��Ϣɾ��
			}
			else if ((pPair->m_V.usStatus == STATUSU_OKAY) || (pPair->m_V.usStatus == STATUSU_SYNC)) {
				pPair->m_V.usStatus = STATUSU_UNLINK; // ��Ҫͬ����ѡ��������Ժ���ɾ��
			}
		}
	}
	if (sd.usStatus > STATUSU_NONE) {
		m_ServerInfo[InfoIndex].Decr(sd);
		return true;
	}
	return false;
}

#endif // __CENTER_SERVER_IMP_H__
