//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Source File : GameServerImp.cpp                            //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ��Ϸ����������ʵ��                                 //
//                                                              //
//////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameServerImp.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGameServer
UInt CGameServer::Command(PCXStr pszCMD, uintptr_t utParam)
{
	// �������ļ������������Ƹ�����ز���
	UNREFERENCED_PARAMETER(pszCMD);
	UNREFERENCED_PARAMETER(utParam);
	return (UInt)RET_OKAY;
}

UInt CGameServer::Update(void)
{
	CheckConnectCenterServer();
	CheckConnectGameDBServer();
	CheckListenZoneServer();
	CheckListenGateServer();
	CheckUDPService();
	SyncServersInfo();
	return 0;
}
//--------------------------------------
UInt CGameServer::Init(CEventHandler& EventHandlerRef)
{
	if (m_nStatus == STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[��Ϸ������]��Ϸ�����ʼ����ʼ!"));
		m_pUIHandler = &EventHandlerRef;

		if ((InitLoadShare() == false) ||
			(InitLoadConfig() == false)) {
			return (UInt)RET_FAIL;
		}

		m_nStatus = STATUSC_INIT;
		LOG_INFO(m_FileLog, TF("[��Ϸ������]��Ϸ�����ʼ�����!"));
		return (UInt)RET_OKAY;
	}
	return (UInt)RET_FAIL;
}
// ��ȡ��������ö�����������
bool CGameServer::InitLoadShare(void)
{
	assert(m_pConfig == nullptr);
	m_pUIHandler->OnHandle(CServerConfig::CFG_DEFAULT_CONFIG, reinterpret_cast<uintptr_t>(&m_pConfig), DATA_INDEX_GAME);
	if (m_pConfig == nullptr) {
		LOG_ERROR(m_FileLog, TF("[��Ϸ������]�ӽ���ص��ӿڻ�ȡ���ö���ָ����Ч"));
		return false;
	}
	LOG_INFO(m_FileLog, TF("[��Ϸ������]�ӽ���ص��ӿڻ�ȡ���ö���ָ�����!"));
	m_pConfig->Dump(m_FileLog); // ������Ϣ�������־�ļ�
	// �Ӷ�̬�������ȡ���������ָ��
	assert(m_NetworkPtr == nullptr);
	CKeyValue::VALUE xValue = { 0 };
	m_pConfig->GetKeyValue()->GetValue(CServerConfig::NetworkPtr, xValue);
	if (xValue.pValue == nullptr) {
		LOG_ERROR(m_FileLog, TF("[��Ϸ������]�Ӷ�̬�������ȡ���������ָ����Ч"));
		return false;
	}
	LOG_INFO(m_FileLog, TF("[��Ϸ������]�Ӷ�̬�������ȡ���������ָ�����!"));
	m_NetworkPtr = *(reinterpret_cast<CNetworkPtr*>(xValue.pValue));
	// ������Ϸ������ָ�뵽��̬������
	LOGV_INFO(m_FileLog, TF("[��Ϸ������]������Ϸ������ָ�뵽��̬���������[key=%s]!"), CServerConfig::GameServer);
	m_pConfig->GetKeyValue()->AddItem(CServerConfig::GameServer, this);
	return true;
}
// ��ʼ������
bool CGameServer::InitLoadConfig(void)
{
	assert(m_krConnectCenter == nullptr);
	assert(m_krConnectGameDB == nullptr);
	assert(m_krListenZone == nullptr);
	assert(m_krListenGate == nullptr);
	assert(m_krUDPService == nullptr);

	assert(m_pShareCenterSvr == nullptr);
	assert(m_pShareGameDBSvr == nullptr);
	assert(m_pShareZoneSvr == nullptr);
	assert(m_pShareGateSvr == nullptr);

	assert(m_ZoneSvrMap.Size() == 0);
	assert(m_ZoneIdMap.Size() == 0);
	assert(m_GateSvrMap.Size() == 0);

	m_ServerInfo.usId   = m_pConfig->GetServerId(CServerConfig::CFG_DEFAULT_GAME);
	m_ServerInfo.usIncr = m_pConfig->GetServerIncr(CServerConfig::CFG_DEFAULT_GAME);
	LOGV_INFO(m_FileLog, TF("[��Ϸ������]�����ö����ȡ������Id=%d, Incr=%d!"), m_ServerInfo.usId, m_ServerInfo.usIncr);
	if (m_pConfig->GetNetworkAttr() & ATTR_IPV6) {
		m_ServerInfo.NetAddr[GAMEI_GATE].usAttr        = ATTR_IPV6;
		m_ServerInfo.NetAddr[GAMEI_ZONE].usAttr        = ATTR_IPV6;
		m_ServerInfo.NetAddr[GAMEI_UDP].usAttr         = ATTR_IPV6;
		m_ServerInfo.NetAddr[GAMEI_CENTER].usAttr      = ATTR_IPV6;
		m_ServerInfo.NetAddr[GAMEI_GAMEDB].usAttr      = ATTR_IPV6;
		m_ServerInfo.NetAddr[GAMEI_GAMEDB_ADDR].usAttr = ATTR_IPV6;
	}
	Int        nPort = 0;
	CStringKey strAddr;
	m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_GAME, 0, strAddr, nPort);
	UShort     usPort = (UShort)nPort;
	m_NetworkPtr->TranslateAddr(strAddr, usPort, m_ServerInfo.NetAddr[GAMEI_GATE]); // gate

	m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_GAME, CServerConfig::CFG_DEFAULT_ZONE, strAddr, nPort);
	usPort = (UShort)nPort;
	m_NetworkPtr->TranslateAddr(strAddr, usPort, m_ServerInfo.NetAddr[GAMEI_ZONE]); // zone

	m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_GAME, CServerConfig::CFG_DEFAULT_LOGIN, strAddr, nPort);
	usPort = (UShort)nPort;
	m_NetworkPtr->TranslateAddr(strAddr, usPort, m_ServerInfo.NetAddr[GAMEI_UDP]); // udp

	m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_GAME, CServerConfig::CFG_DEFAULT_CENTER, strAddr, nPort);
	usPort = (UShort)nPort;
	m_NetworkPtr->TranslateAddr(strAddr, usPort, m_ServerInfo.NetAddr[GAMEI_CENTER]); // connect center 

	m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_GAME, CServerConfig::CFG_DEFAULT_GAMEDB, strAddr, nPort);
	usPort = (UShort)nPort;
	m_NetworkPtr->TranslateAddr(strAddr, usPort, m_ServerInfo.NetAddr[GAMEI_GAMEDB]); // connect gamedb

	m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_GAMEDB, 0, strAddr, nPort);
	usPort = (UShort)nPort;
	m_NetworkPtr->TranslateAddr(strAddr, usPort, m_ServerInfo.NetAddr[GAMEI_GAMEDB_ADDR]); // gamedb
	return true;
}
//--------------------------------------
void CGameServer::Exit(void)
{
	if (m_nStatus != STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[��Ϸ������]��Ϸ�����˳�����ʼ!"));
		m_pUIHandler = nullptr;

		ExitUnloadConfig();
		ExitUnloadShare();

		m_nStatus = STATUSC_NONE;
		LOG_INFO(m_FileLog, TF("[��Ϸ������]��Ϸ�����˳��������!"));
	}
}
// �����������ö�����������
void CGameServer::ExitUnloadShare(void)
{
	m_pConfig    = nullptr;
	m_NetworkPtr = nullptr;
}
//
void CGameServer::ExitUnloadConfig(void)
{
	assert(m_krConnectCenter == nullptr);
	assert(m_krConnectGameDB == nullptr);
	assert(m_krListenZone == nullptr);
	assert(m_krListenGate == nullptr);
	assert(m_krUDPService == nullptr);

	assert(m_pShareCenterSvr == nullptr);
	assert(m_pShareGameDBSvr == nullptr);
	assert(m_pShareZoneSvr == nullptr);
	assert(m_pShareGateSvr == nullptr);

	assert(m_ZoneSvrMap.Size() == 0);
	assert(m_ZoneIdMap.Size() == 0);
	assert(m_GateSvrMap.Size() == 0);

	m_pConfig->GetKeyValue()->Remove(CServerConfig::GameServer);
}
//--------------------------------------
bool CGameServer::Start(void)
{
	if (m_nStatus == STATUSC_INIT) {
		LOG_INFO(m_FileLog, TF("[��Ϸ������]��Ϸ����������ʼ!"));

		if ((StartConnectCenterServer() == false) ||
			(StartConnectGameDBServer() == false) ||
			(StartListenZoneServer() == false) ||
			(StartListenGateServer() == false) ||
			(StartUDPService() == false)) {
			return false;
		}

		m_nStatus = STATUSC_RUN;
		LOG_INFO(m_FileLog, TF("[��Ϸ������]��Ϸ�����������!"));
	}
	return true;
}
// ���д����������ķ����������Ӷ���
bool CGameServer::StartConnectCenterServer(void)
{
	// ��Ϸ�������ڲ�ͬ����,  ��Ҫ�����������ķ�����
	if ((m_pConfig->GetLoadServers() & CServerConfig::CFG_DEFAULT_CENTER) == 0) {
		if (m_krConnectCenter == nullptr) {
			m_krConnectCenter = m_NetworkPtr->Create(*this, m_ServerInfo.NetAddr[GAMEI_CENTER]);
		}
		if (m_krConnectCenter != nullptr) {
			if (m_bCenterCnnted == false) {
				LOG_INFO(m_FileLog, TF("[��Ϸ������]��Ϸ�����������ķ������ڲ�ͬ����, �����������ķ�����Socket�ɹ�"));
				Int         nPort = 0;
				CStringKey  strAddr;
				m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_CENTER, CServerConfig::CFG_DEFAULT_GAME, strAddr, nPort);
				if (m_NetworkPtr->Connect(m_krConnectCenter, (UShort)nPort, *strAddr) == false)
				{
					LOGV_ERROR(m_FileLog, TF("[��Ϸ������]�������ķ�����[%s]:%d����ʧ��"), *strAddr, nPort);
					return false;
				}
				LOGV_INFO(m_FileLog, TF("[��Ϸ������]�������ķ�����[%s]:%d�������"), *strAddr, nPort);
			}
		}
		else {
			LOG_ERROR(m_FileLog, TF("[��Ϸ������]��Ϸ�����������ķ������ڲ�ͬ����, �����������ķ�����Socketʧ��"));
			return false;
		}
	}
	else if (m_bCenterCnnted == false) {
		assert(m_pShareCenterSvr == nullptr);
		CKeyValue::VALUE xValue = { 0 };
		m_pConfig->GetKeyValue()->GetValue(CServerConfig::CenterServer, xValue);
		if (xValue.pValue == nullptr) {
			LOG_ERROR(m_FileLog, TF("[��Ϸ������]�Ӷ�̬�������ȡͬ�������ķ���������ָ����Ч"));
			return false;
		}
		m_pShareCenterSvr = reinterpret_cast<ICommonServer*>(xValue.pValue);
		m_bCenterCnnted   = true;

		LOG_INFO(m_FileLog, TF("[��Ϸ������]ͬ����ֱ���������ķ�����"));
		m_ServerInfo.usStatus = STATUSU_LINK;
		m_ServerInfo.NetAddr[GAMEI_CENTER].usPort = 0; // 0 == ͬ���̹���

		CGameLink Link;
		Link.SetServerData(m_ServerInfo);
		Link.AdjustSize();

		CBufReadStream brs((PByte)m_ServerInfo.NetAddr, sizeof(m_ServerInfo.NetAddr));
		Link.SetStream(brs);

		m_bCenterLinked = m_pShareCenterSvr->OnShareRoutine(PAK_EVENT_LINK, Link, reinterpret_cast<uintptr_t>(this));
		m_pUIHandler->OnHandle(PAK_EVENT_LINK, 0, DATA_INDEX_CENTER);
		return m_bCenterLinked;
	}
	return true;
}
// ���д���������ϷDB�����������Ӷ���
bool CGameServer::StartConnectGameDBServer(void)
{
	// ��Ϸ����ϷDB�ڲ�ͬ����,  ��Ҫ����������ϷDB������
	if ((m_pConfig->GetLoadServers() & CServerConfig::CFG_DEFAULT_GAMEDB) == 0) {
		if (m_krConnectGameDB == nullptr) {
			m_krConnectGameDB = m_NetworkPtr->Create(*this, m_ServerInfo.NetAddr[GAMEI_GAMEDB]);
		}
		if (m_krConnectGameDB != nullptr) {
			if (m_bGameDBCnnted == false) {
				LOG_INFO(m_FileLog, TF("[��Ϸ������]��Ϸ����������ϷDB�������ڲ�ͬ����, ����������ϷDB������Socket�ɹ�"));
				if (m_NetworkPtr->Connect(m_krConnectGameDB, m_ServerInfo.NetAddr[GAMEI_GAMEDB_ADDR]) == false)
				{
					LOGV_ERROR(m_FileLog, TF("[��Ϸ������]������ϷDB����������ʧ��"));
					return false;
				}
				LOGV_INFO(m_FileLog, TF("[��Ϸ������]������ϷDB�������������"));
			}
		}
		else {
			LOG_ERROR(m_FileLog, TF("[��Ϸ������]��Ϸ����������ϷDB�������ڲ�ͬ����, ����������ϷDB������Socketʧ��"));
			return false;
		}
	}
	else if (m_bGameDBCnnted == false) {
		assert(m_pShareGameDBSvr == nullptr);
		CKeyValue::VALUE xValue = { 0 };
		m_pConfig->GetKeyValue()->GetValue(CServerConfig::GameDBServer, xValue);
		if (xValue.pValue == nullptr) {
			LOG_ERROR(m_FileLog, TF("[��Ϸ������]�Ӷ�̬�������ȡͬ������ϷDB����������ָ����Ч"));
			return false;
		}
		m_pShareGameDBSvr = reinterpret_cast<ICommonServer*>(xValue.pValue);
		m_bGameDBCnnted   = true;

		LOG_INFO(m_FileLog, TF("[��Ϸ������]ͬ����ֱ��������ϷDB������"));
		m_ServerInfo.usStatus = STATUSU_LINK;
		m_ServerInfo.NetAddr[GAMEI_GAMEDB].usPort = 0; // 0 == ͬ���̹���

		CGameLink Link;
		Link.SetServerData(m_ServerInfo);
		Link.AdjustSize();

		CBufReadStream brs((PByte)m_ServerInfo.NetAddr, sizeof(m_ServerInfo.NetAddr));
		Link.SetStream(brs);

		m_bGameDBLinked = m_pShareGameDBSvr->OnShareRoutine(PAK_EVENT_LINK, Link, reinterpret_cast<uintptr_t>(this));
		m_pUIHandler->OnHandle(PAK_EVENT_LINK, 0, DATA_INDEX_GAMEDB);
		return m_bGameDBLinked;
	}
	return true;
}
// ���д���������ͼ���������ӵ����Ӷ���
bool CGameServer::StartListenZoneServer(void)
{
	if (m_krListenZone != nullptr) {
		LOGV_INFO(m_FileLog, TF("[��Ϸ������]����������ͼ�������������Ѿ�����"));
		return true;
	}
	m_krListenZone = m_NetworkPtr->Create(*this, m_ServerInfo.NetAddr[GAMEI_ZONE]);
	if (m_krListenZone != nullptr) {
		LOGV_INFO(m_FileLog, TF("[��Ϸ������]����������ͼ�����������ӳɹ�"));
		if (m_NetworkPtr->Listen(m_krListenZone) == false) {
			LOG_ERROR(m_FileLog, TF("[��Ϸ������]������ͼ����������ʧ��"));
			return false;
		}
		else {
			LOG_INFO(m_FileLog, TF("[��Ϸ������]������ͼ���������ӳɹ�"));
		}
	}
	else {
		LOGV_ERROR(m_FileLog, TF("[��Ϸ������]����������ͼ������������ʧ��"));
		return false;
	}
	return true;
}
// ���д����������ط��������ӵ����Ӷ���
bool CGameServer::StartListenGateServer(void)
{
	if (m_krListenGate != nullptr) {
		LOGV_INFO(m_FileLog, TF("[��Ϸ������]�����������ط������������Ѿ�����"));
		return true;
	}
	m_krListenGate = m_NetworkPtr->Create(*this, m_ServerInfo.NetAddr[GAMEI_GATE]);
	if (m_krListenGate != nullptr) {
		LOGV_INFO(m_FileLog, TF("[��Ϸ������]�����������ط����������ӳɹ�"));
		if (m_NetworkPtr->Listen(m_krListenGate) == false) {
			LOG_ERROR(m_FileLog, TF("[��Ϸ������]�������ط���������ʧ��"));
			return false;
		}
		else {
			LOG_INFO(m_FileLog, TF("[��Ϸ������]�������ط��������ӳɹ�"));
		}
	}
	else {
		LOGV_ERROR(m_FileLog, TF("[��Ϸ������]�����������ط�����������ʧ��"));
		return false;
	}
	return true;
}
// ���д���UDP������½������У�����Ӷ���
bool CGameServer::StartUDPService(void)
{
	if (m_krUDPService == nullptr) {
		m_krUDPService = m_NetworkPtr->Create(*this, m_ServerInfo.NetAddr[GAMEI_UDP], SOCKET_UDP);
		if (m_krUDPService != nullptr) {
			LOGV_INFO(m_FileLog, TF("[��Ϸ������]����UDP������½�������ɹ�"));
		}
		else {
			LOGV_ERROR(m_FileLog, TF("[��Ϸ������]����UDP������½������ʧ��"));
			return false;
		}
	}
	return (m_krUDPService != nullptr);
}
//--------------------------------------
bool CGameServer::Pause(bool bPause)
{
	if (bPause && (m_nStatus == STATUSC_RUN)) {
		m_nStatus = STATUSC_PAUSE; 
		LOG_INFO(m_FileLog, TF("[��Ϸ������]��ͣ����"));
		return true;
	}
	else if ((bPause == false) && (m_nStatus == STATUSC_PAUSE)) {
		m_nStatus = STATUSC_RUN;   
		LOG_INFO(m_FileLog, TF("[��Ϸ������]�������"));
		return true;
	}
	return false;
}
//--------------------------------------
void CGameServer::Stop(void)
{
	if (m_nStatus > STATUSC_INIT) {
		LOG_INFO(m_FileLog, TF("[��Ϸ������]��Ϸ����ֹͣ��ʼ!"));

		StopUDPService();
		StopListenGateServer();
		StopListenZoneServer();
		StopConnectGameDBServer();
		StopConnectCenterServer();

		m_ServerInfo.Zero();
		m_OthersInfo[INFOI_ZONE].Reset();
		m_OthersInfo[INFOI_GATE].Reset();
		m_ZoneSvrMap.RemoveAll();
		m_ZoneIdMap.RemoveAll();
		m_GateSvrMap.RemoveAll();
		m_SortChain.Reset();

		m_nStatus = STATUSC_INIT;
		LOG_INFO(m_FileLog, TF("[��Ϸ������]��Ϸ����ֹͣ���!"));
	}
}
// ֹͣ�������ķ�����
void CGameServer::StopConnectCenterServer(void)
{
	if (m_krConnectCenter != nullptr) {
		m_NetworkPtr->Destroy(m_krConnectCenter, false);
		m_krConnectCenter = nullptr;
		LOG_INFO(m_FileLog, TF("[��Ϸ������]�����������ķ����������ӳɹ�"));
	}
	m_bCenterCnnted   = false;
	m_bCenterLinked   = false;
	m_pShareCenterSvr = nullptr;
}
// ֹͣ������ϷDB������
void CGameServer::StopConnectGameDBServer(void)
{
	if (m_krConnectGameDB != nullptr) {
		m_NetworkPtr->Destroy(m_krConnectGameDB, false);
		m_krConnectGameDB = nullptr;
		LOG_INFO(m_FileLog, TF("[��Ϸ������]����������ϷDB�����������ӳɹ�"));
	}
	m_bGameDBCnnted   = false;
	m_bGameDBLinked   = false;
	m_pShareGameDBSvr = nullptr;
}
// ֹͣ������ͼ����������
void CGameServer::StopListenZoneServer(void)
{
	if (m_krListenZone != nullptr) {
		m_NetworkPtr->Destroy(m_krListenZone, false);
		m_krListenZone = nullptr;
		LOG_INFO(m_FileLog, TF("[��Ϸ������]���ټ�����ͼ�����������ӳɹ�"));
	}
	m_pShareZoneSvr = nullptr;
}
// ֹͣ�������ط���������
void CGameServer::StopListenGateServer(void)
{
	if (m_krListenGate != nullptr) {
		m_NetworkPtr->Destroy(m_krListenGate, false);
		m_krListenGate = nullptr;
		LOG_INFO(m_FileLog, TF("[��Ϸ������]���ټ������ط����������ӳɹ�"));
	}
	m_pShareGateSvr = nullptr;
}
// ֹͣUDP������½������
void CGameServer::StopUDPService(void)
{
	if (m_krUDPService != nullptr) {
		m_NetworkPtr->Destroy(m_krUDPService, false);
		m_krUDPService = nullptr;
		LOG_INFO(m_FileLog, TF("[��Ϸ������]����UDP��������ӳɹ�"));
	}
}
//--------------------------------------
bool CGameServer::OnShareRoutine(Int nEvent, CEventBase& EventRef, LLong llParam, CEventQueue::EVENT_TYPE)
{
	assert((m_pConfig->GetLoadServers() & (CServerConfig::CFG_DEFAULT_CENTER|CServerConfig::CFG_DEFAULT_GAMEDB|CServerConfig::CFG_DEFAULT_ZONE|CServerConfig::CFG_DEFAULT_GATE)) != 0);
	switch (nEvent) {
	case PAK_EVENT_LINK:
		{
			return OnShareLink(EventRef, llParam);
		}
		break;
	case PAK_EVENT_UPDATE:
		{
			return OnShareUpdate(EventRef, llParam);
		}
		break;
	//case PAK_EVENT_UNLINK: // ͬ���̷���������, ˵������ֹͣ, ����Ҫ����
	//	break;
	case PAK_EVENT_LOGIN_SELECT_GAME:
		{
			CPAKLoginSelectGame* pSelect = static_cast<CPAKLoginSelectGame*>(&EventRef);
			if (SelectGame(pSelect, llParam) == false) {
				pSelect->SetAck(LOGIN_ERROR_NONE_GATE);
				reinterpret_cast<ICommonServer*>(llParam)->OnShareRoutine(PAK_EVENT_LOGIN_SELECT_GAME, *pSelect);
				DEV_INFO(TF("[��Ϸ������]ͬ������Ϸ�������յ��ͻ��˵�½��Ϸ����������, ����û�����ط�����"));
			}
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��Ϸ������]ͬ���̷������޷�ʶ������������[event=%d]"), nEvent);
			assert(0);
		}
	}
	return true;
}
// ͬ���̷���������
bool CGameServer::OnShareLink(CEventBase& EventRef, LLong llParam)
{
	CPAKLink* pLink = static_cast<CPAKLink*>(&EventRef);
	if (pLink->GetType() == PAK_TYPE_ZONE) {
		if (m_pShareZoneSvr == nullptr) {
			// 0.��ù���ָ��
			m_pShareZoneSvr = reinterpret_cast<ICommonServer*>(llParam);
			LOGV_INFO(m_FileLog, TF("[��Ϸ������]ͬ���̵�ͼ������[%p]����"), m_pShareZoneSvr);
			return ServerLink<SVR_ZONE_INDEX_MAP, DATA_INDEX_ZONE, INFOI_ZONE>(pLink, (DataRef)llParam, m_ZoneSvrMap);
		}
		else {
			LOGV_WARN(m_FileLog, TF("[��Ϸ������]ͬ���̵�ͼ�����������Ѿ�����[%p-%p]"), m_pShareZoneSvr, llParam);
		}
	}
	else if (pLink->GetType() == PAK_TYPE_GATE) {
		if (m_pShareGateSvr == nullptr) {
			// 0.��ù���ָ��
			m_pShareGateSvr = reinterpret_cast<ICommonServer*>(llParam);
			LOGV_INFO(m_FileLog, TF("[��Ϸ������]ͬ�������ط�����[%p]����"), m_pShareGateSvr);
			return ServerLink<SVR_GATE_MAP, DATA_INDEX_GATE, INFOI_GATE>(pLink, (DataRef)llParam, m_GateSvrMap);
		}
		else {
			LOGV_WARN(m_FileLog, TF("[��Ϸ������]ͬ�������ط����������Ѿ�����[%p-%p]"), m_pShareGateSvr, llParam);
		}
	}
	return false;
}

bool CGameServer::OnShareUpdate(CEventBase& EventRef, LLong llParam)
{
	CPAKUpdate* pUpdate = static_cast<CPAKUpdate*>(&EventRef);
	if (pUpdate->GetType() == PAK_TYPE_ZONE) {
		if (m_pShareZoneSvr != nullptr) {
			DEV_INFO(TF("[��Ϸ������]ͬ���̵�ͼ������[%p]����"), m_pShareZoneSvr);
			assert(m_pShareZoneSvr == reinterpret_cast<ICommonServer*>(llParam));
			return ServerUpdate<SVR_ZONE_INDEX_MAP, DATA_INDEX_ZONE, INFOI_ZONE>(pUpdate, (DataRef)llParam, m_ZoneSvrMap);
		}
		else {
			LOGV_WARN(m_FileLog, TF("[��Ϸ������]ͬ���̵�ͼ���������󲻴���"));
		}
	}
	else if (pUpdate->GetType() == PAK_TYPE_GATE) {
		if (m_pShareGateSvr != nullptr) {
			DEV_INFO(TF("[��Ϸ������]ͬ�������ط�����[%p]����"), m_pShareGateSvr);
			assert(m_pShareGateSvr == reinterpret_cast<ICommonServer*>(llParam));
			return ServerUpdate<SVR_GATE_MAP, DATA_INDEX_GATE, INFOI_GATE>(pUpdate, (DataRef)llParam, m_GateSvrMap);
		}
		else {
			LOGV_WARN(m_FileLog, TF("[��Ϸ������]ͬ�������ط��������󲻴���"));
		}
	}
	return false;
}
//--------------------------------------
bool CGameServer::OnTcpDispatch(const PacketPtr& PktPtr, PTCP_PARAM pTcp)
{
	// PTCP_PARAM->pCache == nullptr, �ϲ���Ա���ӵײ�����Ļ�������
	// PTCP_PARAM->pCache, PTCP_PARAM->index�ǵײ�����Ļ������ݵ���ʼ��ַ��������
	// ����ϲ㱣�滺��, ����PTCP_PARAM->pDataΪnullptr֪ͨ����ײ�
	// �ͷŻ���ʹ��MObject::sMCFree(index, pCache)
	bool bRet = false;
	if (PktPtr->GetEvent() != PAK_EVENT_LIVE) {
		switch (PktPtr->GetType()) {
		case PAK_TYPE_GATE:
			{
				bRet = DispatchGateServer(PktPtr, pTcp->krSocket);
			}
			break;
		case PAK_TYPE_ZONE:
			{
				bRet = DispatchZoneServer(PktPtr, pTcp->krSocket);
			}
			break;
		case PAK_TYPE_GAMEDB:
			{
				bRet = DispatchGameDBServer(PktPtr, pTcp->krSocket);
			}
			break;
		case PAK_TYPE_CENTER:
			{
				bRet = DispatchCenterServer(PktPtr, pTcp->krSocket);
			}
			break;
		default:
			{
				LOGV_WARN(m_FileLog, TF("[��Ϸ������]%p�������������������[%d]����ȷ"), pTcp->krSocket, PktPtr->GetType());
				bRet = false;
			}
		}
	}
	else {
		CPAKSimple<PAK_EVENT_LIVEACK, PAK_TYPE_GAME> LiveAck;
		LiveAck.AdjustSize();
		bRet = m_NetworkPtr->Send(pTcp->krSocket, LiveAck);
	}
	return bRet;
}
// �������ķ������������
bool CGameServer::DispatchCenterServer(const PacketPtr& PktPtr, KeyRef krSocket)
{
	switch (PktPtr->GetEvent()) {
	case PAK_EVENT_LINKACK:
		{
			m_bCenterLinked = true;
			m_pUIHandler->OnHandle(PAK_EVENT_LINK, reinterpret_cast<uintptr_t>(m_ServerInfo.NetAddr + GAMEI_CENTER), DATA_INDEX_CENTER);
			LOG_INFO(m_FileLog, TF("[��Ϸ������]�յ����ķ��������ӻظ���"));
		}
		break;
	case PAK_EVENT_UPDATEACK:
		{
			DEV_INFO(TF("[��Ϸ������]�յ����ķ��������»ظ���"));
		}
		break;
	case PAK_EVENT_UNLINKACK:
		{
			m_bCenterLinked = false;
			LOG_INFO(m_FileLog, TF("[��Ϸ������]�յ����ķ������Ͻӻظ���"));
		}
		break;
	case PAK_EVENT_LIVEACK:
		{
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��Ϸ������]%p������Ϣ�޷�ʶ������ķ��������������[event=%d]"), krSocket, PktPtr->GetEvent());
		}
	}
	return true;
}
// ������ϷDB�������������
bool CGameServer::DispatchGameDBServer(const PacketPtr& PktPtr, KeyRef krSocket)
{
	switch (PktPtr->GetEvent()) {
	case PAK_EVENT_LINKACK:
		{
			m_bGameDBLinked = true;
			m_pUIHandler->OnHandle(PAK_EVENT_LINK, reinterpret_cast<uintptr_t>(m_ServerInfo.NetAddr + GAMEI_GAMEDB), DATA_INDEX_GAMEDB);
			LOG_INFO(m_FileLog, TF("[��Ϸ������]�յ���ϷDB���������ӻظ���"));
		}
		break;
	case PAK_EVENT_UPDATEACK:
		{
			DEV_INFO(TF("[��Ϸ������]�յ���ϷDB���������»ظ���"));
		}
		break;
	case PAK_EVENT_UNLINKACK:
		{
			m_bGameDBLinked = false;
			LOG_INFO(m_FileLog, TF("[��Ϸ������]�յ���ϷDB�������Ͻӻظ���"));
		}
		break;
	case PAK_EVENT_LIVEACK:
		{
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��Ϸ������]%p������Ϣ�޷�ʶ�����ϷDB���������������[event=%d]"), krSocket, PktPtr->GetEvent());
		}
	}
	return true;
}
// �����ͼ�������������
bool CGameServer::DispatchZoneServer(const PacketPtr& PktPtr, KeyRef krSocket)
{
	bool bRet = false;
	switch (PktPtr->GetEvent()) {
	case PAK_EVENT_LINK:
		{
			bRet = OnServerLink(static_cast<CPAKLink*>(PktPtr.Get()), krSocket);
		}
		break;
	case PAK_EVENT_UPDATE:
		{
			bRet = OnServerUpdate(static_cast<CPAKUpdate*>(PktPtr.Get()), krSocket);
		}
		break;
	case PAK_EVENT_UNLINK:
		{
			bRet = OnServerUnlink(PktPtr.Get(), krSocket);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��Ϸ������]%p������Ϣ�޷�ʶ��ĵ�ͼ���������������[event=%d]"), krSocket, PktPtr->GetEvent());
		}
	}
	return bRet;
}
// �������ط������������
bool CGameServer::DispatchGateServer(const PacketPtr& PktPtr, KeyRef krSocket)
{
	bool bRet = false;
	switch (PktPtr->GetEvent()) {
	case PAK_EVENT_LINK:
		{
			bRet = OnServerLink(static_cast<CPAKLink*>(PktPtr.Get()), krSocket);
		}
		break;
	case PAK_EVENT_UPDATE:
		{
			bRet = OnServerUpdate(static_cast<CPAKUpdate*>(PktPtr.Get()), krSocket);
		}
		break;
	case PAK_EVENT_UNLINK:
		{
			bRet = OnServerUnlink(PktPtr.Get(), krSocket);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��Ϸ������]%p������Ϣ�޷�ʶ������ط��������������[event=%d]"), krSocket, PktPtr->GetEvent());
		}
	}
	return bRet;
}
// ����������
bool CGameServer::OnServerLink(CPAKLink* pLink, KeyRef krSocket)
{
	bool bRet = false;
	switch (pLink->GetType()) {
	case PAK_TYPE_ZONE:
		{
			LOGV_INFO(m_FileLog, TF("[��Ϸ������]��ͼ������[%p]����"), krSocket);
			bRet = ServerLink<SVR_ZONE_INDEX_MAP, DATA_INDEX_ZONE, INFOI_ZONE>(pLink, (DataRef)krSocket, m_ZoneSvrMap);
		}
		break;
	case PAK_TYPE_GATE:
		{
			LOGV_INFO(m_FileLog, TF("[��Ϸ������]���ط�����[%p]����"), krSocket);
			bRet = ServerLink<SVR_GATE_MAP, DATA_INDEX_GATE, INFOI_GATE>(pLink, (DataRef)krSocket, m_GateSvrMap);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��Ϸ������]%p������Ϣ�޷�ʶ���%X���������������"), krSocket, pLink->GetType());
		}
	}
	if (bRet) {
		// 3.��������Ϊѡ�����������
		m_NetworkPtr->SetAttr(krSocket, pLink->GetType());
		// 4.�������ӻ�ִ��
		CPAKSimple<PAK_EVENT_LINKACK, PAK_TYPE_GAME> LinkAck;
		LinkAck.AdjustSize();
		m_NetworkPtr->Send(krSocket, LinkAck);
	}
	return bRet;
}

bool CGameServer::OnServerUpdate(CPAKUpdate* pUpdate, KeyRef krSocket)
{
	bool bRet = false;
	switch (pUpdate->GetType()) {
	case PAK_TYPE_ZONE:
		{
			DEV_INFO(TF("[��Ϸ������]��ͼ������[%p]����"), krSocket);
			bRet = ServerUpdate<SVR_ZONE_INDEX_MAP, DATA_INDEX_ZONE, INFOI_ZONE>(pUpdate, (DataRef)krSocket, m_ZoneSvrMap);
		}
		break;
	case PAK_TYPE_GATE:
		{
			DEV_INFO(TF("[��Ϸ������]���ط�����[%p]����"), krSocket);
			bRet = ServerUpdate<SVR_GATE_MAP, DATA_INDEX_GATE, INFOI_GATE>(pUpdate, (DataRef)krSocket, m_GateSvrMap);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��Ϸ������]%p������Ϣ�޷�ʶ���%X���������������"), krSocket, pUpdate->GetType());
		}
	}
	if (bRet) {
		// ���͸��»�ִ��
		CPAKSimple<PAK_EVENT_UPDATEACK, PAK_TYPE_GAME> UpdateAck;
		UpdateAck.AdjustSize();
		m_NetworkPtr->Send(krSocket, UpdateAck);
	}
	return bRet;
}

bool CGameServer::OnServerUnlink(CPAKHead* pUnlink, KeyRef krSocket)
{
	bool bRet = false;
	switch (pUnlink->GetType()) {
	case PAK_TYPE_ZONE:
		{
			LOGV_INFO(m_FileLog, TF("[��Ϸ������]��ͼ������[%p]�Ͻ�"), krSocket);
			bRet = ServerUnlink<SVR_ZONE_INDEX_MAP, DATA_INDEX_ZONE, INFOI_ZONE>(krSocket, m_ZoneSvrMap);
		}
		break;
	case PAK_TYPE_GATE:
		{
			LOGV_INFO(m_FileLog, TF("[��Ϸ������]���ط�����[%p]�Ͻ�"), krSocket);
			bRet = ServerUnlink<SVR_GATE_MAP, DATA_INDEX_GATE, INFOI_GATE>(krSocket, m_GateSvrMap);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��Ϸ������]%p������Ϣ�޷�ʶ���%X���������������"), krSocket, pUnlink->GetType());
		}
	}
	if (bRet) {
		// 3.��������Ϊ��Ч����������
		m_NetworkPtr->SetAttr(krSocket, PAK_TYPE_NONE);
		// 4.���Ͷ�����ִ��
		CPAKSimple<PAK_EVENT_UNLINKACK, PAK_TYPE_GAME> UnlinkAck;
		UnlinkAck.AdjustSize();
		m_NetworkPtr->Send(krSocket, UnlinkAck);
	}
	return bRet;
}
//--------------------------------------
bool CGameServer::OnUdpDispatch(const PacketPtr& PktPtr, PUDP_PARAM pUdp)
{
	// PUDP_PARAM->pCache == nullptr, �ϲ���Ա���ӵײ�����Ļ�������
	// PUDP_PARAM->pCache, PUDP_PARAM->index�ǵײ�����Ļ������ݵ���ʼ��ַ��������
	// ����ϲ㱣�滺��, ����PUDP_PARAM->pDataΪnullptr֪ͨ����ײ�
	// �ͷŻ���ʹ��MObject::sMCFree(index, pCache)
	assert(PktPtr->GetEvent() == PAK_EVENT_LOGIN_SELECT_GAME);
	CPAKLoginSelectGame* pSelect = static_cast<CPAKLoginSelectGame*>(PktPtr.Get());
	if (SelectGame(pSelect) == false) {
		pSelect->SetAck(LOGIN_ERROR_NONE_GATE);
		m_NetworkPtr->SendTo(pUdp->krSocket, *pSelect, pUdp->NetAddr);
		LOG_INFO(m_FileLog, TF("[��Ϸ������]��Ϸ�������յ��ͻ��˵�½��Ϸ����������, ����û�����ط�����"));
	}
	return true;
}

bool CGameServer::SelectGame(CPAKLoginSelectGame* pSelect, LLong llParam)
{
	assert(pSelect->GetType() == PAK_TYPE_LOGIN);
	pSelect->SetType(PAK_TYPE_GAME);
	// ʵ����Ҫ�����Ż�����ͨ��·PINGֵΪ�ο�������
	PSORT_RESULT pSortResult = m_SortChain.GetCur();
	if (pSortResult->lCount > 0) {
		Long lIndex = pSortResult->Index();
		if ((DataRef)m_pShareGateSvr == pSortResult->Sort[lIndex].drServer) {
			m_pShareGateSvr->OnShareRoutine(PAK_EVENT_LOGIN_SELECT_GAME, *pSelect, llParam);
		}
		else {
			m_NetworkPtr->SendTo(m_krUDPService, *pSelect, pSortResult->Sort[lIndex].Addr);
		}
		DEV_INFO(TF("[��Ϸ������]ѡȡ��%d�����ط�����"), lIndex);
		return true;
	}
	return false;
}
//--------------------------------------
bool CGameServer::OnTcpAccept(KeyRef krAccept, KeyRef krListen)
{
	if (m_nStatus == STATUSC_RUN) {
		UShort     usPort = 0;
		CStringKey strAddr;
		m_NetworkPtr->GetAddr(krAccept, strAddr, usPort);

		if (krListen == m_krListenZone) {
			DEV_INFO(TF("[��Ϸ������]��ͼ������[%s]:%d�������!"), *strAddr, usPort);
		}
		else if (krListen == m_krListenGate) {
			DEV_INFO(TF("[��Ϸ������]���ط�����[%s]:%d�������!"), *strAddr, usPort);
		}
		m_ServerInfo.Incr();
		return true;
	}
	else {
		LOGV_WARN(m_FileLog, TF("[��Ϸ������]��Ϸ������������ͣ״̬!"));
	}
	return false;
}
//--------------------------------------
bool CGameServer::OnTcpConnect(UInt uError, KeyRef krConnect)
{
	if (krConnect == m_krConnectCenter) {
		m_bCenterCnnted = (uError == 0);
		if (m_bCenterCnnted) {
			m_NetworkPtr->GetAddr(krConnect, m_ServerInfo.NetAddr[GAMEI_CENTER], false);
		}
		else {
			DEV_INFO(TF("[��Ϸ������]�������ķ�����ʧ��%X!"), uError);
			LOGV_WARN(m_FileLog, TF("[��Ϸ������]�������ķ�����ʧ��%X!"), uError);
		}
	}
	else if (krConnect == m_krConnectGameDB) {
		m_bGameDBCnnted = (uError == 0);
		if (m_bGameDBCnnted) {
			m_NetworkPtr->GetAddr(krConnect, m_ServerInfo.NetAddr[GAMEI_GAMEDB], false);
		}
		else {
			DEV_INFO(TF("[��Ϸ������]������ϷDB������ʧ��%X!"), uError);
			LOGV_WARN(m_FileLog, TF("[��Ϸ������]������ϷDB������ʧ��%X!"), uError);
		}
	}
	if (m_bCenterCnnted && m_bGameDBCnnted) {
		LinkCenterServer();
		LinkGameDBServer();
	}
	return true;
}
// �������ķ�����
void CGameServer::LinkCenterServer(void)
{
	if (m_bCenterCnnted && (m_bCenterLinked == false)) {
		m_ServerInfo.usStatus = STATUSU_LINK;

		CGameLink Link;
		Link.SetServerData(m_ServerInfo);
		Link.AdjustSize();

		CStreamScopePtr StreamPtr;
		m_NetworkPtr->AllocBuffer(StreamPtr);
		if (StreamPtr != nullptr) {
			Link.Serialize(*StreamPtr);
			m_ServerInfo.Addr(*StreamPtr);

			m_NetworkPtr->Send(m_krConnectCenter, *StreamPtr);
		}
		else {
			DEV_WARN(TF("[��Ϸ������]�������ķ��������������������쳣!"));
			LOG_WARN(m_FileLog, TF("[��Ϸ������]�������ķ��������������������쳣!"));
		}
	}
}
// �������ķ�����
void CGameServer::UnlinkCenterServer(void)
{
	if (m_bCenterCnnted && m_bCenterLinked) {
		m_bCenterLinked = false;
		CPAKSimple<PAK_EVENT_UNLINK, PAK_TYPE_GAME> Unlink;
		Unlink.AdjustSize();
		m_NetworkPtr->Send(m_krConnectCenter, Unlink);
	}
}
// ������ϷDB������
void CGameServer::LinkGameDBServer(void)
{
	if (m_bGameDBCnnted && (m_bGameDBLinked == false)) {
		m_ServerInfo.usStatus = STATUSU_LINK;

		CGameLink Link;
		Link.SetServerData(m_ServerInfo);
		Link.AdjustSize();

		CStreamScopePtr StreamPtr;
		m_NetworkPtr->AllocBuffer(StreamPtr);
		if (StreamPtr != nullptr) {
			Link.Serialize(*StreamPtr);
			m_ServerInfo.Addr(*StreamPtr);

			m_NetworkPtr->Send(m_krConnectGameDB, *StreamPtr);
		}
		else {
			DEV_WARN(TF("[��Ϸ������]������ϷDB���������������������쳣!"));
			LOG_WARN(m_FileLog, TF("[��Ϸ������]������ϷDB���������������������쳣!"));
		}
	}
}
// ������ϷDB������
void CGameServer::UnlinkGameDBServer(void)
{
	if (m_bGameDBCnnted && m_bGameDBLinked)
	{
		m_bGameDBLinked = false;
		CPAKSimple<PAK_EVENT_UNLINK, PAK_TYPE_GAME> Unlink;
		Unlink.AdjustSize();
		m_NetworkPtr->Send(m_krConnectGameDB, Unlink);
	}
}
//--------------------------------------
bool CGameServer::OnTcpClose(KeyRef krSocket, LLong llLiveData)
{
	if (krSocket == m_krConnectCenter) {
		m_krConnectCenter = nullptr;
		m_bCenterCnnted   = false;
		m_bCenterLinked   = false;
		m_pUIHandler->OnHandle(PAK_EVENT_UNLINK, 0, DATA_INDEX_CENTER);
		DEV_INFO(TF("[��Ϸ������]�������ķ��������Ӷ���/�Ͽ�"));
	}
	else if (krSocket == m_krConnectGameDB) {
		m_krConnectGameDB = nullptr;
		m_bGameDBCnnted   = false;
		m_bGameDBLinked   = false;
		m_pUIHandler->OnHandle(PAK_EVENT_UNLINK, 0, DATA_INDEX_GAMEDB);
		DEV_INFO(TF("[��Ϸ������]������ϷDB���������Ӷ���/�Ͽ�"));
	}
	else if (krSocket == m_krListenZone) {
		m_krListenZone = nullptr;
		DEV_INFO(TF("[��Ϸ������]������ͼ���������ӹر�"));
	}
	else if (krSocket == m_krListenGate) {
		m_krListenGate = nullptr;
		DEV_INFO(TF("[��Ϸ������]�������ط��������ӹر�"));
	}
	else { // ��������������/�Ͽ�
		switch (llLiveData) {
		case PAK_TYPE_ZONE:
			{
				DEV_INFO(TF("[��ϷDB������]��ͼ������[%p]�Ͻ�"), krSocket);
				ServerUnlink<SVR_ZONE_INDEX_MAP, DATA_INDEX_ZONE, INFOI_ZONE>(krSocket, m_ZoneSvrMap);
			}
			break;
		case PAK_TYPE_GATE:
			{
				DEV_INFO(TF("[��ϷDB������]���ط�����[%p]�Ͻ�"), krSocket);
				ServerUnlink<SVR_GATE_MAP, DATA_INDEX_GATE, INFOI_GATE>(krSocket, m_GateSvrMap);
			}
			break;
		default:
			{
			}
		}
		m_ServerInfo.Decr();
	}
	return true;
}
//--------------------------------------
bool CGameServer::OnUdpClose(KeyRef krSocket, LLong)
{
	if (krSocket == m_krUDPService){
		m_krUDPService = nullptr;
		DEV_INFO(TF("[��Ϸ������]UDP������½�������ر�"));
	}
	return true;
}
//--------------------------------------
// ��ʱ����������ķ����������Ӷ����Ƿ���Ч
bool CGameServer::CheckConnectCenterServer(void)
{
	if (m_bCenterCnnted == false) {
		if (StartConnectCenterServer() == false) {
			return false;
		}
	}
	if (m_bCenterLinked == false) {
		LinkCenterServer();
	}
	return true;
}
// ��ʱ���������ϷDB�����������Ӷ����Ƿ���Ч
bool CGameServer::CheckConnectGameDBServer(void)
{
	if (m_bGameDBCnnted == false) {
		if (StartConnectGameDBServer() == false) {
			return false;
		}
	}
	if (m_bGameDBLinked == false) {
		LinkGameDBServer();
	}
	return true;
}
// ��ʱ��������ͼ���������ӵ����Ӷ����Ƿ���Ч
bool CGameServer::CheckListenZoneServer(void)
{
	if (m_krListenZone == nullptr) {
		return StartListenZoneServer();
	}
	return true;
}
// ��ʱ���������ط��������ӵ����Ӷ����Ƿ���Ч
bool CGameServer::CheckListenGateServer(void)
{
	if (m_krListenGate == nullptr) {
		return StartListenGateServer();
	}
	return true;
}
// ��ʱ���UDP������Ϸ�����������Ӷ����Ƿ���Ч
bool CGameServer::CheckUDPService(void)
{
	if (m_krUDPService == nullptr) {
		return StartUDPService();
	}
	return true;
}
//--------------------------------------
// ͬ����������Ϣ������
bool CGameServer::SyncServersInfo(void)
{
	SyncGameServerInfo();
	SyncZoneServerInfo();
	SyncGateSortInfo();
	return true;
}
// ͬ����Ϸ��������Ϣ������
bool CGameServer::SyncGameServerInfo(void)
{
	if (m_ServerInfo.usStatus == STATUSU_SYNC) {
		CGameUpdate Update;
		Update.SetServerData(m_ServerInfo);
		Update.AdjustSize();
		if (m_pShareCenterSvr != nullptr) {
			m_pShareCenterSvr->OnShareRoutine(PAK_EVENT_UPDATE, Update, reinterpret_cast<LLong>(this));
		}
		else {
			m_NetworkPtr->Send(m_krConnectCenter, Update);
		}
		if (m_pShareGameDBSvr != nullptr) {
			m_pShareGameDBSvr->OnShareRoutine(PAK_EVENT_UPDATE, Update, reinterpret_cast<LLong>(this));
		}
		else {
			m_NetworkPtr->Send(m_krConnectGameDB, Update);
		}
		m_ServerInfo.usStatus = STATUSU_OKAY;
		return m_pUIHandler->OnHandle(PAK_EVENT_SYNC, reinterpret_cast<uintptr_t>(&m_ServerInfo), DATA_INDEX_GAME);
	}
	else { // ��������
		if (m_pShareCenterSvr == nullptr) { 
			CPAKSimple<PAK_EVENT_LIVE, PAK_TYPE_GAME> Live;
			Live.AdjustSize();
			m_NetworkPtr->Send(m_krConnectCenter, Live);
		}
		if (m_pShareGameDBSvr == nullptr) { 
			CPAKSimple<PAK_EVENT_LIVE, PAK_TYPE_GAME> Live;
			Live.AdjustSize();
			m_NetworkPtr->Send(m_krConnectGameDB, Live);
		}
	}
	return true;
}
// ͬ����ͼ��������Ϣ������
bool CGameServer::SyncZoneServerInfo(void)
{
	if (m_OthersInfo[INFOI_ZONE].usStatus == STATUSU_SYNC) {
		m_OthersInfo[INFOI_ZONE].usStatus = STATUSU_OKAY;
		return m_pUIHandler->OnHandle(PAK_EVENT_SYNC, reinterpret_cast<uintptr_t>(m_OthersInfo + INFOI_ZONE), DATA_INDEX_ZONE);
	}
	return true;
}
// ͬ�����ط���������������鲢ͬ��������
bool CGameServer::SyncGateSortInfo(void)
{ 
	if (m_OthersInfo[INFOI_GATE].usStatus == STATUSU_SYNC) {
		m_OthersInfo[INFOI_GATE].usStatus = STATUSU_OKAY;
		m_pUIHandler->OnHandle(PAK_EVENT_SYNC, reinterpret_cast<uintptr_t>(m_OthersInfo + INFOI_GATE), DATA_INDEX_GATE);

		Int nCount = 0;
		PSORT_RESULT pSortResult = m_SortChain.GetNext();
		{
			CSyncLockWaitScope scope(m_GateSvrMap.GetLock());

			PINDEX index = m_GateSvrMap.First();
			while ((index != nullptr) && (nCount < SORT_RESULT::SORTC_LIMIT)) {
				SVR_GATE_MAP::SVR_MAP_PAIR* pPair = m_GateSvrMap.Next(index);
				if ((pPair->m_V.usStatus > STATUSU_NONE) && (pPair->m_V.usStatus < STATUSU_UNLINK)) {
					pSortResult->Sort[nCount].usBusy   = pPair->m_V.usBusy;
					pSortResult->Sort[nCount].usIncr   = pPair->m_V.usIncr;
					pSortResult->Sort[nCount].drServer = pPair->m_K;
					pSortResult->Sort[nCount].Addr     = pPair->m_V.NetAddr[GATEI_UDP];
					++nCount;
				}
			}
		}
		if (nCount > 1) {
			// ʵ����Ҫ�����Ż�����ͨ��·PINGֵΪ�ο�������
			if (nCount > 2) {
				SERVER_SORT::QuickSort(pSortResult->Sort, nCount);
			}
			else if (pSortResult->Sort[0].usBusy > pSortResult->Sort[1].usBusy) {
				SERVER_SORT ss = pSortResult->Sort[0];
				pSortResult->Sort[0] = pSortResult->Sort[1];
				pSortResult->Sort[1] = ss;
			}
		}
		pSortResult->lCount = (Long)nCount;
		pSortResult->lIndex = 0;
		m_SortChain.Swap(); // �����������������, ����ǰ���µ���Ϊ��Ч����
		LOGV_INFO(m_FileLog, TF("[��Ϸ������]�������ط������������������%d�����ط�������Ϣ"), nCount);
	}
	return true;
}

bool CGameServer::AddZoneServerIds(CStream& Stream, PINDEX index, DataRef drServer)
{
	Int nSize = 0;
	Stream >> nSize;
	if (nSize <= 0) {
		return false;
	}
	CSyncLockWaitScope scope(m_ZoneIdMap.GetLock()); // ������server-map�� ������id-map
	Int nCount = 0;
	for (; nCount < nSize; ++nCount) {
		UInt uId = (UInt)-1;
		Stream >> uId;
		if (uId != (UInt)-1) {
			assert(m_ZoneIdMap.Find(uId) == nullptr);
			m_ZoneSvrMap.At(index)->m_V.Index.Add(m_ZoneIdMap.Add(uId, drServer));
			continue;
		}
		break;
	}
	return (nCount == nSize);
}

bool CGameServer::RemoveZoneServerIds(PINDEX index)
{
	ZONE_ADDR_INDEX::ARY_INDEX& Index = m_ZoneSvrMap.At(index)->m_V.Index;
	if (Index.GetSize() == 0) {
		return true;
	}
	CSyncLockWaitScope scope(m_ZoneIdMap.GetLock()); // ������server-map�� ������id-map
	Int nCount = Index.GetSize();
	for (Int i = 0; i < nCount; ++i) {
		m_ZoneIdMap.RemoveAt(Index[i]);
	}
	Index.RemoveAll();
	return true;
}





