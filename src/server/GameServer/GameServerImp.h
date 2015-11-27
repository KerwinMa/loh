//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : GameServerImp.h                              //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update : 2015-11-25     version 0.0.0.5                    //
//   Detail : ��Ϸ������ʵ��                                     //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __GAME_SERVER_IMP_H__
#define __GAME_SERVER_IMP_H__

#pragma once

#include "gamedbpacket.h"
#include "gamepacket.h"
#include "zonepacket.h"
#include "gatepacket.h"
#include "loginpacket.h"
#include "servermap.h"
#include "serversort.h"
#include "ServerConfig.h"
#include "CommonServer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ��Ϸ������ʵ��
/// - ��Ҫ���� :
/// -# ά�����ط���������Ϸ�����������ת��
/// -# ά����ͼ����������Ϸ������Ӧ��ת��
/// -# �����, ȫ��ʹ�������̺߳Ͷ�ʱ�߳�������
class CGameServer : public ICommonServer, public CPAKHandler
{
public:
	/// �����ͷ�������ͳ����Ϣ������
	enum INFO_INDEX {
		INFOI_ZONE,
		INFOI_GATE,
		INFOI_MAX,
	};
public:
	CGameServer(void);
	virtual ~CGameServer(void);
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
private:
	CGameServer(const CGameServer&);
	CGameServer& operator=(const CGameServer&);
	/// ��ȡ��������ö�����������
	bool  InitLoadShare(void);
	/// ��ʼ������
	bool  InitLoadConfig(void);

	/// �����������ö�����������
	void  ExitUnloadShare(void);
	/// �������
	void  ExitUnloadConfig(void);

	/// �����������ķ���������
	bool  StartConnectCenterServer(void);
	/// ����������ϷDB����������
	bool  StartConnectGameDBServer(void);
	/// ����������ͼ����������
	bool  StartListenZoneServer(void);
	/// �����������ط���������
	bool  StartListenGateServer(void);
	/// ����UDP������½����������
	bool  StartUDPService(void);

	/// ֹͣ�������ķ�����
	void  StopConnectCenterServer(void);
	/// ֹͣ������ϷDB������
	void  StopConnectGameDBServer(void);
	/// ֹͣ������ͼ������
	void  StopListenZoneServer(void);
	/// ֹͣ�������ط�����
	void  StopListenGateServer(void);
	/// ֹͣUDP������½������
	void  StopUDPService(void);

	/// �������ķ������������
	bool  DispatchCenterServer(const PacketPtr& PktPtr, KeyRef krSocket);
	/// ������ϷDB�������������
	bool  DispatchGameDBServer(const PacketPtr& PktPtr, KeyRef krSocket);
	/// �����ͼ�������������
	bool  DispatchZoneServer(const PacketPtr& PktPtr, KeyRef krSocket);
	/// �������ط������������
	bool  DispatchGateServer(const PacketPtr& PktPtr, KeyRef krSocket);

	/// �����ķ�����ע��
	void  LinkCenterServer(void);
	/// �����ķ�����ע��
	void  UnlinkCenterServer(void);
	/// ����ϷDB������ע��
	void  LinkGameDBServer(void);
	/// ����ϷDB������ע��
	void  UnlinkGameDBServer(void);

	/// ��ʱ����������ķ����������Ƿ���Ч
	bool  CheckConnectCenterServer(void);
	/// ��ʱ���������ϷDB�����������Ƿ���Ч
	bool  CheckConnectGameDBServer(void);
	/// ��ʱ��������ͼ�����������Ƿ���Ч
	bool  CheckListenZoneServer(void);
	/// ��ʱ���������ط����������Ƿ���Ч
	bool  CheckListenGateServer(void);
	/// ��ʱ���UDP������½�����������Ƿ���Ч
	bool  CheckUDPService(void);

	/// ͬ����������Ϣ������
	bool  SyncServersInfo(void);
	/// ͬ����Ϸ��������Ϣ������
	bool  SyncGameServerInfo(void);
	/// ͬ����ͼ��������Ϣ������
	bool  SyncZoneServerInfo(void);
	/// ͬ�����ط��������������ͬ��������
	bool  SyncGateSortInfo(void);

	/// ͬ���̷���������-ע��
	bool  OnShareLink(CEventBase& EventRef, LLong llParam);
	/// ͬ���̷���������-����
	bool  OnShareUpdate(CEventBase& EventRef, LLong llParam);
	/// �������������-ע��
	bool  OnServerLink(CPAKLink* pLink, KeyRef krSocket);
	/// �������������-����
	bool  OnServerUpdate(CPAKUpdate* pUpdate, KeyRef krSocket);
	/// �������������-ע��
	bool  OnServerUnlink(CPAKHead* pUnlink, KeyRef krSocket);
	/// ע�����������
	template <typename MAP_TYPE, DATA_INDEX DataIndex, INFO_INDEX InfoIndex>
	bool  ServerLink(CPAKLink* pLink, DataRef drServer, MAP_TYPE& MapRef);
	/// ���·���������
	template <typename MAP_TYPE, DATA_INDEX DataIndex, INFO_INDEX InfoIndex>
	bool  ServerUpdate(CPAKUpdate* pUpdate, DataRef drServer, MAP_TYPE& MapRef);
	/// ע������������
	template <typename MAP_TYPE, DATA_INDEX DataIndex, INFO_INDEX InfoIndex>
	bool  ServerUnlink(KeyRef krSocket, MAP_TYPE& MapRef);
	/// ѡ����ʵ����ط�����������������Ϸ����
	bool  SelectGame(CPAKLoginSelectGame* pSelect, LLong llParam = 0);
	/// ע��ĵ�ͼ����������͵�ͼ��ż����Ӧ��
	bool  AddZoneServerIds(CStream& Stream, PINDEX index, DataRef drServer);
	/// ע���ĵ�ͼ����������͵�ͼ��ŴӶ�Ӧ��ɾ��
	bool  RemoveZoneServerIds(PINDEX index);
private:
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ZONE_ID
	typedef tagDATA_MAP<UInt, DataRef>   ZONE_ID_MAP, *PZONE_ID_MAP;
private:
	Int                 m_nStatus;          ///< ������״̬
	bool                m_bCenterCnnted;	///< �Ƿ��������ķ��������
	bool                m_bCenterLinked;	///< �Ƿ������ķ�����ע�����
	bool                m_bGameDBCnnted;	///< �Ƿ�������ϷDB���������
	bool                m_bGameDBLinked;	///< �Ƿ�����ϷDB������ע�����
	CEventHandler*      m_pUIHandler;       ///< ����ص��ӿ�
	CServerConfig*      m_pConfig;          ///< ���ö���
	KeyRef              m_krConnectCenter;  ///< ����, �������ķ�����
	KeyRef              m_krConnectGameDB;  ///< ����, ������ϷDB������
	KeyRef              m_krListenZone;     ///< ����, ������ͼ������
	KeyRef              m_krListenGate;     ///< ����, �������ط�����
	KeyRef              m_krUDPService;     ///< ����, ������½������
	ICommonServer*      m_pShareCenterSvr;  ///< ͬ�������ķ�����
	ICommonServer*      m_pShareGameDBSvr;  ///< ͬ������ϷDB������
	ICommonServer*      m_pShareZoneSvr;    ///< ͬ���̵�ͼ������
	ICommonServer*      m_pShareGateSvr;    ///< ͬ�������ط�����
	CFileLog            m_FileLog;          ///< ���ı���־
	CNetworkPtr         m_NetworkPtr;       ///< �������
	GAME_ADDR           m_ServerInfo;       ///< ����������Ϣ
	SERVER_INFO         m_OthersInfo[INFOI_MAX]; ///< ��������ͳ������
	SVR_ZONE_INDEX_MAP  m_ZoneSvrMap;       ///< ��ͼ��������Ϣ
	ZONE_ID_MAP         m_ZoneIdMap;        ///< ��ͼ����������͵�ͼ��Ŷ�Ӧ��
	SVR_GATE_MAP        m_GateSvrMap;       ///< ���ط�������Ϣ
	RESULT_CHAIN        m_SortChain;        ///< ���ط����������� 
};

INLINE CGameServer::CGameServer(void)
: m_nStatus(STATUSC_NONE)
, m_bCenterCnnted(false)
, m_bCenterLinked(false)
, m_bGameDBCnnted(false)
, m_bGameDBLinked(false)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krConnectCenter(nullptr)
, m_krConnectGameDB(nullptr)
, m_krListenZone(nullptr)
, m_krListenGate(nullptr)
, m_krUDPService(nullptr)
, m_pShareCenterSvr(nullptr)
, m_pShareGameDBSvr(nullptr)
, m_pShareZoneSvr(nullptr)
, m_pShareGateSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::GameServer)
{
}

INLINE CGameServer::~CGameServer(void)
{
}

INLINE CGameServer::CGameServer(const CGameServer&)
: m_nStatus(STATUSC_NONE)
, m_bCenterCnnted(false)
, m_bCenterLinked(false)
, m_bGameDBCnnted(false)
, m_bGameDBLinked(false)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krConnectCenter(nullptr)
, m_krConnectGameDB(nullptr)
, m_krListenZone(nullptr)
, m_krListenGate(nullptr)
, m_krUDPService(nullptr)
, m_pShareCenterSvr(nullptr)
, m_pShareGameDBSvr(nullptr)
, m_pShareZoneSvr(nullptr)
, m_pShareGateSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::GameServer)
{
}

INLINE CGameServer& CGameServer::operator=(const CGameServer&)
{
	return (*this);
}

INLINE bool CGameServer::OnShareRoutine(Int, uintptr_t, LLong, CEventQueue::EVENT_TYPE)
{
	return false;
}

INLINE bool CGameServer::OnShareRoutine(Int, CStream&, LLong)
{
	return false;
}

template <typename MAP_TYPE, DATA_INDEX DataIndex, CGameServer::INFO_INDEX InfoIndex>
INLINE bool CGameServer::ServerLink(CPAKLink* pLink, DataRef drServer, MAP_TYPE& MapRef)
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
		if ((DataIndex == DATA_INDEX_ZONE) && (index != nullptr)) {
			AddZoneServerIds(pLink->GetStream(), index, drServer);// ����ӵ��ǵ�ͼ����������ĵ�ͼ��������
		}
	}
	if (index != nullptr) { // 2.���½���
		m_pUIHandler->OnHandle(PAK_EVENT_LINK, reinterpret_cast<uintptr_t>(&Pair), DataIndex);
		m_OthersInfo[InfoIndex].Incr(pLink->GetServerData());
		return true;
	}
	return false;
}

template <typename MAP_TYPE, DATA_INDEX DataIndex, CGameServer::INFO_INDEX InfoIndex>
INLINE bool CGameServer::ServerUpdate(CPAKUpdate* pUpdate, DataRef drServer, MAP_TYPE& MapRef)
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
		m_OthersInfo[InfoIndex].Diff(pUpdate->GetServerData(), Pair.Value);
		Pair.Value.Copy(pUpdate->GetServerData());
		m_pUIHandler->OnHandle(PAK_EVENT_UPDATE, reinterpret_cast<uintptr_t>(&Pair), DataIndex);// ���½���
		return true;
	}
	return false;
}

template <typename MAP_TYPE, DATA_INDEX DataIndex, CGameServer::INFO_INDEX InfoIndex>
INLINE bool CGameServer::ServerUnlink(KeyRef krSocket, MAP_TYPE& MapRef)
{
	// 2.���½���
	m_pUIHandler->OnHandle(PAK_EVENT_UNLINK, reinterpret_cast<uintptr_t>(krSocket), DataIndex);

	SERVER_DATA sd;
	{
		CSyncLockWaitScope scope(MapRef.GetLock());
		MAP_TYPE::SVR_MAP_PAIR* pPair = MapRef.Find((DataRef)krSocket);
		if (pPair != nullptr) {
			sd = pPair->m_V;
			if (DataIndex == PAK_TYPE_ZONE) {
				RemoveZoneServerIds(reinterpret_cast<PINDEX>(pPair)); // ɾ����Ӧ�ĵ�ͼid������Ϣ
			}
			MapRef.RemoveAt(reinterpret_cast<PINDEX>(pPair));
		}
	}
	if (sd.usStatus > STATUSU_NONE) {
		m_OthersInfo[InfoIndex].Decr(sd);
		return true;
	}
	return false;
}

#endif // __GAME_SERVER_IMP_H__
