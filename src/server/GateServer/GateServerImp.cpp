//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Source File : GateServerImp.cpp                            //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ���ط���������ʵ��                                 //
//                                                              //
//////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GateServerImp.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGateServer
UInt CGateServer::Command(PCXStr pszCMD, uintptr_t utParam)
{
	// �������ļ������������Ƹ�����ز���
	UNREFERENCED_PARAMETER(pszCMD);
	UNREFERENCED_PARAMETER(utParam);
	return (UInt)RET_OKAY;
}

UInt CGateServer::Update(void)
{
	CheckConnectGameDBServer();
	CheckConnectGameServer();
	CheckUDPService();
	CheckTCPService();
	SyncServerInfo();
	return 0;
}
//--------------------------------------
UInt CGateServer::Init(CEventHandler& EventHandlerRef)
{
	if (m_nStatus == STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[���ط�����]���ط����ʼ����ʼ!"));
		m_pUIHandler = &EventHandlerRef;

		if ((InitLoadShare() == false) ||
			(InitLoadConfig() == false)) {
			return (UInt)RET_FAIL;
		}

		m_nStatus = STATUSC_INIT;
		LOG_INFO(m_FileLog, TF("[���ط�����]���ط����ʼ�����!"));
		return (UInt)RET_OKAY;
	}
	return (UInt)RET_FAIL;
}
// ��ȡ��������ö�����������
bool CGateServer::InitLoadShare(void)
{
	assert(m_pConfig == nullptr);
	m_pUIHandler->OnHandle(CServerConfig::CFG_DEFAULT_CONFIG, reinterpret_cast<uintptr_t>(&m_pConfig), DATA_INDEX_GATE);
	if (m_pConfig == nullptr) {
		LOG_ERROR(m_FileLog, TF("[���ط�����]�ӽ���ص��ӿڻ�ȡ���ö���ָ����Ч"));
		return false;
	}
	LOG_INFO(m_FileLog, TF("[���ط�����]�ӽ���ص��ӿڻ�ȡ���ö���ָ�����!"));
	m_pConfig->Dump(m_FileLog); // ������Ϣ�������־�ļ�
	// �Ӷ�̬�������ȡ���������ָ��
	assert(m_NetworkPtr == nullptr);
	CKeyValue::VALUE xValue = { 0 };
	m_pConfig->GetKeyValue()->GetValue(CServerConfig::NetworkPtr, xValue);
	if (xValue.pValue == nullptr) {
		LOG_ERROR(m_FileLog, TF("[���ط�����]�Ӷ�̬�������ȡ���������ָ����Ч"));
		return false;
	}
	LOG_INFO(m_FileLog, TF("[���ط�����]�Ӷ�̬�������ȡ���������ָ�����!"));
	m_NetworkPtr = *(reinterpret_cast<CNetworkPtr*>(xValue.pValue));
	return true;
}
// ��ʼ������
bool CGateServer::InitLoadConfig(void)
{
	assert(m_krConnectGameDB == nullptr);
	assert(m_krConnectGame == nullptr);
	assert(m_krUDPService == nullptr);
	assert(m_krTCPService == nullptr);

	assert(m_pShareGameDBSvr == nullptr);
	assert(m_pShareGameSvr == nullptr);

	m_ServerInfo.usId   = m_pConfig->GetServerId(CServerConfig::CFG_DEFAULT_GATE);
	m_ServerInfo.usIncr = m_pConfig->GetServerIncr(CServerConfig::CFG_DEFAULT_GATE);
	LOGV_INFO(m_FileLog, TF("[���ط�����]�����ö����ȡ������Id=%d, Incr=%d!"), m_ServerInfo.usId, m_ServerInfo.usIncr);
	if (m_pConfig->GetNetworkAttr() & ATTR_IPV6) {
		m_ServerInfo.NetAddr[GATEI_TCP].usAttr    = ATTR_IPV6;
		m_ServerInfo.NetAddr[GATEI_UDP].usAttr    = ATTR_IPV6;
		m_ServerInfo.NetAddr[GATEI_GAMEDB].usAttr = ATTR_IPV6;
	}
	UShort     usPort = 0;
	CStringKey strAddr;
	m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_GATE, 0, strAddr, usPort);
	m_NetworkPtr->TranslateAddr(strAddr, usPort, m_ServerInfo.NetAddr[GATEI_TCP]); // tcp

	m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_GATE, CServerConfig::CFG_DEFAULT_LOGIN, strAddr, usPort);
	m_NetworkPtr->TranslateAddr(strAddr, usPort, m_ServerInfo.NetAddr[GATEI_UDP]); // udp

	m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_GAMEDB, CServerConfig::CFG_DEFAULT_GATE, strAddr, usPort);
	if (usPort == 0) {
		m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_GAMEDB, 0, strAddr, usPort);
	}
	m_NetworkPtr->TranslateAddr(strAddr, usPort, m_ServerInfo.NetAddr[GATEI_GAMEDB]); // gamedb

	assert(m_GateRoutine == nullptr);
	m_GateRoutine = (ICommonRoutine*)CRTTI::CreateByName(TF("CGateRoutine"));
	if (m_GateRoutine != nullptr) {
		return (m_GateRoutine->Init() == (UInt)RET_OKAY);
	}
	LOG_ERROR(m_FileLog, TF("[���ط�����]�������������ʧ��"));
	return false;
}
//--------------------------------------
void CGateServer::Exit(void)
{
	if (m_nStatus != STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[���ط�����]���ط����˳�����ʼ!"));
		m_pUIHandler = nullptr;

		ExitUnloadConfig();
		ExitUnloadShare();

		m_nStatus = STATUSC_NONE;
		LOG_INFO(m_FileLog, TF("[���ط�����]���ط����˳��������!"));
	}
}
// �����������ö�����������
void CGateServer::ExitUnloadShare(void)
{
	m_pConfig    = nullptr;
	m_NetworkPtr = nullptr;
}
//
void CGateServer::ExitUnloadConfig(void)
{
	assert(m_krConnectGameDB == nullptr);
	assert(m_krConnectGame == nullptr);
	assert(m_krUDPService == nullptr);
	assert(m_krTCPService == nullptr);

	assert(m_pShareGameDBSvr == nullptr);
	assert(m_pShareGameSvr == nullptr);

	m_GateRoutine->Exit();
	m_GateRoutine = nullptr;
}
//--------------------------------------
bool CGateServer::Start(void)
{
	if (m_nStatus == STATUSC_INIT) {
		LOG_INFO(m_FileLog, TF("[���ط�����]���ط���������ʼ!"));

		if ((StartUDPService() == false) || // ��ǰ������ȡ��ַ��Ϣ
			(StartConnectGameDBServer() == false) ||
			(StartConnectGameServer() == false) ||
			(StartTCPService() == false)) {
			return false;
		}
		m_GateRoutine->Start(this);

		m_nStatus = STATUSC_RUN;
		LOG_INFO(m_FileLog, TF("[���ط�����]���ط����������!"));
	}
	return true;
}
// ���д���������ϷDB�����������Ӷ���
bool CGateServer::StartConnectGameDBServer(void)
{
	// ���غ���ϷDB�ڲ�ͬ����,  ��Ҫ����������ϷDB������
	if ((m_pConfig->GetLoadServers() & CServerConfig::CFG_DEFAULT_GAMEDB) == 0) {
		if (m_krConnectGameDB == nullptr) {
			UShort     usPort = 0;
			CStringKey strAddr;
			m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_GATE, CServerConfig::CFG_DEFAULT_GAMEDB, strAddr, usPort);
			m_krConnectGameDB = m_NetworkPtr->Create(*this, usPort, *strAddr);
		}
		if (m_krConnectGameDB != nullptr) {
			if (m_bGameDBCnnted == false) {
				LOG_INFO(m_FileLog, TF("[���ط�����]���ط���������ϷDB�������ڲ�ͬ����, ����������ϷDB������Socket�ɹ�"));
				if (m_NetworkPtr->Connect(m_krConnectGameDB, m_ServerInfo.NetAddr[GATEI_GAMEDB]) == false)
				{
					LOGV_ERROR(m_FileLog, TF("[���ط�����]������ϷDB����������ʧ��"));
					return false;
				}
				LOGV_INFO(m_FileLog, TF("[���ط�����]������ϷDB�������������"));
			}
		}
		else {
			LOG_ERROR(m_FileLog, TF("[���ط�����]���ط���������ϷDB�������ڲ�ͬ����, ����������ϷDB������Socketʧ��"));
			return false;
		}
	}
	else if (m_bGameDBCnnted == false) {
		assert(m_pShareGameDBSvr == nullptr);
		CKeyValue::VALUE xValue = { 0 };
		m_pConfig->GetKeyValue()->GetValue(CServerConfig::GameDBServer, xValue);
		if (xValue.pValue == nullptr) {
			LOG_ERROR(m_FileLog, TF("[���ط�����]�Ӷ�̬�������ȡͬ������ϷDB����������ָ����Ч"));
			return false;
		}
		m_pShareGameDBSvr = reinterpret_cast<ICommonServer*>(xValue.pValue);
		m_bGameDBCnnted   = true;

		LOG_INFO(m_FileLog, TF("[���ط�����]ͬ����ֱ��������ϷDB������"));
		m_ServerInfo.usStatus = STATUSU_LINK;

		CGateLink Link;
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
// ���д���������Ϸ�����������Ӷ���
bool CGateServer::StartConnectGameServer(void)
{
	// ���غ���Ϸ�ڲ�ͬ����,  ��Ҫ�����������ķ�����
	if ((m_pConfig->GetLoadServers() & CServerConfig::CFG_DEFAULT_GAME) == 0) {
		if (m_krConnectGame == nullptr) {
			UShort     usPort = 0;
			CStringKey strAddr;
			m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_GATE, CServerConfig::CFG_DEFAULT_GAME, strAddr, usPort);
			m_krConnectGame = m_NetworkPtr->Create(*this, usPort, *strAddr);
		}
		if (m_krConnectGame != nullptr) {
			if (m_bGameCnnted == false) {
				LOG_INFO(m_FileLog, TF("[���ط�����]���ط���������Ϸ�������ڲ�ͬ����, ����������Ϸ������Socket�ɹ�"));
				UShort      usPort = 0;
				CStringKey  strAddr;
				m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_GAME, CServerConfig::CFG_DEFAULT_GATE, strAddr, usPort);
				if (m_NetworkPtr->Connect(m_krConnectGame, usPort, *strAddr) == false) {
					LOGV_ERROR(m_FileLog, TF("[���ط�����]������Ϸ������[%s]:%d����ʧ��"), *strAddr, usPort);
					return false;
				}
				LOGV_INFO(m_FileLog, TF("[���ط�����]������Ϸ������[%s]:%d�������"), *strAddr, usPort);
			}
		}
		else {
			LOG_ERROR(m_FileLog, TF("[���ط�����]���ط���������Ϸ�������ڲ�ͬ����, ����������Ϸ������Socketʧ��"));
			return false;
		}
	}
	else if (m_bGameCnnted == false) {
		assert(m_pShareGameSvr == nullptr);
		CKeyValue::VALUE xValue = { 0 };
		m_pConfig->GetKeyValue()->GetValue(CServerConfig::GameServer, xValue);
		if (xValue.pValue == nullptr) {
			LOG_ERROR(m_FileLog, TF("[���ط�����]�Ӷ�̬�������ȡͬ������Ϸ����������ָ����Ч"));
			return false;
		}
		m_pShareGameSvr = reinterpret_cast<ICommonServer*>(xValue.pValue);
		m_bGameCnnted   = true;

		LOG_INFO(m_FileLog, TF("[���ط�����]ͬ����ֱ���������ķ�����"));
		m_ServerInfo.usStatus = STATUSU_LINK;

		CGateLink Link;
		Link.SetServerData(m_ServerInfo);
		Link.AdjustSize();

		CBufReadStream brs((PByte)m_ServerInfo.NetAddr, sizeof(m_ServerInfo.NetAddr));
		Link.SetStream(brs);

		m_bGameLinked = m_pShareGameSvr->OnShareRoutine(PAK_EVENT_LINK, Link, reinterpret_cast<uintptr_t>(this));
		m_pUIHandler->OnHandle(PAK_EVENT_LINK, 0, DATA_INDEX_GAME);
		return m_bGameLinked;
	}
	return true;
}
// ���д���UDP������½������У�����Ӷ���
bool CGateServer::StartUDPService(void)
{
	if (m_krUDPService == nullptr) {
		m_krUDPService = m_NetworkPtr->Create(*this, m_ServerInfo.NetAddr[GATEI_UDP], SOCKET_UDP);
		if (m_krUDPService != nullptr) {
			LOGV_INFO(m_FileLog, TF("[���ط�����]����UDP������½�������ɹ�"));
			if (m_ServerInfo.NetAddr[GATEI_UDP].usPort == 0) {
				m_NetworkPtr->GetAddr(m_krUDPService, m_ServerInfo.NetAddr[GATEI_UDP], false);
			}
			m_pUIHandler->OnHandle(PAK_EVENT_LINK, reinterpret_cast<uintptr_t>(m_ServerInfo.NetAddr + GATEI_UDP), DATA_INDEX_GATE);
		}
		else {
			LOGV_ERROR(m_FileLog, TF("[���ط�����]����UDP������½������ʧ��"));
			return false;
		}
	}
	return true;
}
// ���д���TCP�����ͻ������Ӷ���
bool CGateServer::StartTCPService(void)
{
	if (m_krTCPService != nullptr) {
		LOGV_INFO(m_FileLog, TF("[���ط�����]���������ͻ��˵������Ѿ�����"));
		return true;
	}
	bool bRet = true;
	m_krTCPService = m_NetworkPtr->Create(*this, m_ServerInfo.NetAddr[GATEI_TCP]);
	if (m_krTCPService != nullptr) {
		bRet = m_NetworkPtr->Listen(m_krTCPService);
		LOGV_INFO(m_FileLog, TF("[���ط�����]���������ͻ��˵����ӳɹ�, %s"), bRet ? TF("�������ӳɹ�") : TF("��������ʧ��"));
	}
	else {
		LOGV_ERROR(m_FileLog, TF("[���ط�����]���������ͻ��˵�����ʧ��"));
		bRet = false;
	}
	return bRet;
}
//--------------------------------------
bool CGateServer::Pause(bool bPause)
{
	if (bPause && (m_nStatus == STATUSC_RUN)) {
		m_nStatus = STATUSC_PAUSE; 
		LOG_INFO(m_FileLog, TF("[���ط�����]��ͣ����"));
		m_GateRoutine->Pause();
		return true;
	}
	else if ((bPause == false) && (m_nStatus == STATUSC_PAUSE)) {
		m_nStatus = STATUSC_RUN;   
		LOG_INFO(m_FileLog, TF("[���ط�����]�������"));
		m_GateRoutine->Pause(false);
		return true;
	}
	return false;
}
//--------------------------------------
void CGateServer::Stop(void)
{
	if (m_nStatus > STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[���ط�����]���ط���ֹͣ��ʼ!"));

		m_GateRoutine->Stop();

		StopUDPService();
		StopTCPService();
		StopConnectGameServer();
		StopConnectGameDBServer();

		m_ServerInfo.Zero();

		m_nStatus = STATUSC_INIT;
		LOG_INFO(m_FileLog, TF("[���ط�����]���ط���ֹͣ���!"));
	}
}
// ֹͣ������ϷDB������
void CGateServer::StopConnectGameDBServer(void)
{
	if (m_krConnectGameDB != nullptr) {
		m_NetworkPtr->Destroy(m_krConnectGameDB, false);
		m_krConnectGameDB = nullptr;
		LOG_INFO(m_FileLog, TF("[���ط�����]����������ϷDB�����������ӳɹ�"));
	}
	m_bGameDBCnnted   = false;
	m_bGameDBLinked   = false;
	m_pShareGameDBSvr = nullptr;
}
// ֹͣ������Ϸ������
void CGateServer::StopConnectGameServer(void)
{
	if (m_krConnectGame != nullptr) {
		m_NetworkPtr->Destroy(m_krConnectGame, false);
		m_krConnectGame = nullptr;
		LOG_INFO(m_FileLog, TF("[���ط�����]����������Ϸ�����������ӳɹ�"));
	}
	m_bGameCnnted   = false;
	m_bGameLinked   = false;
	m_pShareGameSvr = nullptr;
}
// ֹͣUDP������½������
void CGateServer::StopUDPService(void)
{
	if (m_krUDPService != nullptr) {
		m_NetworkPtr->Destroy(m_krUDPService, false);
		m_krUDPService = nullptr;
		LOG_INFO(m_FileLog, TF("[���ط�����]����UDP��������ӳɹ�"));
	}
}
// ֹͣTCP�����ͻ���
void CGateServer::StopTCPService(void)
{
	if (m_krTCPService != nullptr) {
		m_NetworkPtr->Destroy(m_krTCPService, false);
		m_krTCPService = nullptr;
		LOG_INFO(m_FileLog, TF("[���ط�����]���ټ����ͻ��˵����ӳɹ�"));
	}
}
//--------------------------------------
bool CGateServer::OnShareRoutine(Int nEvent, CEventBase& EventRef, LLong llParam, CEventQueue::EVENT_TYPE eType)
{
	assert((m_pConfig->GetLoadServers() & (CServerConfig::CFG_DEFAULT_GAMEDB|CServerConfig::CFG_DEFAULT_GAME)) != 0);
	switch (nEvent) {
	// gamedb
	case PAK_EVENT_GATE_SELECTACK:
	case PAK_EVENT_GATE_PLAYACK:
	case PAK_EVENT_GATE_CREATEACK:
	case PAK_EVENT_GATE_DELETEACK:
	case PAK_EVENT_GATE_RECOVERACK:
	// game
	case PAK_EVENT_LOGIN_SELECT_GAME:
		{
			m_GateRoutine->Add(nEvent, EventRef, llParam, eType);
		}
		break;
	default:
		{
			if ((nEvent > PAK_EVENT_REALM_BEGIN) && (nEvent < PAK_EVENT_REALM_END)) {
				m_GateRoutine->Add(nEvent, EventRef, llParam, eType);
			}
			else {
				LOGV_WARN(m_FileLog, TF("[���ط�����]ͬ���̷������޷�ʶ������������[event=%d]"), nEvent);
				assert(0);
			}
		}
	}
	return true;
}
//--------------------------------------
bool CGateServer::OnTcpDispatch(const PacketPtr& PktPtr, PTCP_PARAM pTcp)
{
	// PTCP_PARAM->pCache == nullptr, �ϲ���Ա���ӵײ�����Ļ�������
	// PTCP_PARAM->pCache, PTCP_PARAM->index�ǵײ�����Ļ������ݵ���ʼ��ַ��������
	// ����ϲ㱣�滺��, ����PTCP_PARAM->pDataΪnullptr֪ͨ����ײ�
	// �ͷŻ���ʹ��MObject::sMCFree(index, pCache)
	bool bRet = false;
	if (PktPtr->GetEvent() != PAK_EVENT_LIVE) {
		switch (PktPtr->GetType()) {
		case PAK_TYPE_CLIENT:
			{
				bRet = m_GateRoutine->Add((Int)PktPtr->GetEvent(), *PktPtr, (LLong)pTcp->krSocket);
			}
			break;
		case PAK_TYPE_GAMEDB:
			{
				bRet = DispatchGameDBServer(PktPtr, pTcp->krSocket);
			}
			break;
		case PAK_TYPE_GAME:
			{
				bRet = DispatchGameServer(PktPtr, pTcp->krSocket);
			}
			break;
		default:
			{
				LOGV_WARN(m_FileLog, TF("[���ط�����]%p�������������[%d]����ȷ"), pTcp->krSocket, PktPtr->GetType());
				bRet = false;
			}
		}
	}
	else {
		CPAKSimple<PAK_EVENT_LIVEACK, PAK_TYPE_GATE> LiveAck;
		LiveAck.AdjustSize();
		bRet = m_NetworkPtr->Send(pTcp->krSocket, LiveAck);
	}
	return bRet;
}
// ������ϷDB�������������
bool CGateServer::DispatchGameDBServer(const PacketPtr& PktPtr, KeyRef krSocket)
{
	switch (PktPtr->GetEvent()) {
	case PAK_EVENT_LINKACK:
		{
			m_bGameDBLinked = true;
			m_pUIHandler->OnHandle(PAK_EVENT_LINK, reinterpret_cast<uintptr_t>(m_ServerInfo.NetAddr + GATEI_GAMEDB), DATA_INDEX_GAMEDB);
			LOG_INFO(m_FileLog, TF("[���ط�����]�յ���ϷDB���������ӻظ���"));
		}
		break;
	case PAK_EVENT_UPDATEACK:
		{
			DEV_INFO(TF("[���ط�����]�յ���ϷDB���������»ظ���"));
		}
		break;
	case PAK_EVENT_UNLINKACK:
		{
			m_bGameDBLinked = false;
			LOG_INFO(m_FileLog, TF("[���ط�����]�յ���ϷDB�������Ͻӻظ���"));
		}
		break;
	case PAK_EVENT_LIVEACK:
		{
		}
		break;
	// gamedb
	case PAK_EVENT_GATE_SELECTACK:
	case PAK_EVENT_GATE_PLAYACK:
	case PAK_EVENT_GATE_CREATEACK:
	case PAK_EVENT_GATE_DELETEACK:
	case PAK_EVENT_GATE_RECOVERACK:
		{	
			m_GateRoutine->Add((Int)PktPtr->GetEvent(), *PktPtr);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[���ط�����]%p������Ϣ�޷�ʶ�����ϷDB���������������[event=%d]"), krSocket, PktPtr->GetEvent());
		}
	}
	return true;
}
// ������Ϸ�������������
bool CGateServer::DispatchGameServer(const PacketPtr& PktPtr, KeyRef krSocket)
{
	switch (PktPtr->GetEvent()) {
	case PAK_EVENT_LINKACK:
		{
			m_bGameLinked = true;

			NET_ADDR NetAddr;
			m_NetworkPtr->GetAddr(krSocket, NetAddr, false);
			m_pUIHandler->OnHandle(PAK_EVENT_LINK, reinterpret_cast<uintptr_t>(&NetAddr), DATA_INDEX_GAME);
			LOG_INFO(m_FileLog, TF("[���ط�����]�յ���Ϸ���������ӻظ���"));
		}
		break;
	case PAK_EVENT_UPDATEACK:
		{
			DEV_INFO(TF("[���ط�����]�յ���Ϸ���������»ظ���"));
		}
		break;
	case PAK_EVENT_UNLINKACK:
		{
			m_bGameLinked = false;
			LOG_INFO(m_FileLog, TF("[���ط�����]�յ���Ϸ�������Ͻӻظ���"));
		}
		break;
	case PAK_EVENT_LIVEACK:
		{
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[���ط�����]%p������Ϣ�޷�ʶ�����Ϸ���������������[event=%d]"), krSocket, PktPtr->GetEvent());
		}
	}
	return true;
}
//--------------------------------------
bool CGateServer::OnUdpDispatch(const PacketPtr& PktPtr, PUDP_PARAM pUdp)
{
	// PUDP_PARAM->pCache == nullptr, �ϲ���Ա���ӵײ�����Ļ�������
	// PUDP_PARAM->pCache, PUDP_PARAM->index�ǵײ�����Ļ������ݵ���ʼ��ַ��������
	// ����ϲ㱣�滺��, ����PUDP_PARAM->pDataΪnullptr֪ͨ����ײ�
	// �ͷŻ���ʹ��MObject::sMCFree(index, pCache)
	if (PktPtr->GetType() == PAK_TYPE_GAME) {
		if (PktPtr->GetEvent() == PAK_EVENT_LOGIN_SELECT_GAME) {
			m_GateRoutine->Add(PAK_EVENT_LOGIN_SELECT_GAME, *PktPtr);
		}
		else {
			LOGV_WARN(m_FileLog, TF("[���ط�����]�޷�ʶ��ķ�����UDP���������[event=%d]"), PktPtr->GetEvent());
		}
	}
	else {
		LOGV_WARN(m_FileLog, TF("[���ط�����]%p������Ϣ���������[%d]������Ϸ����������"), pUdp->krSocket, PktPtr->GetType());
	}
	return true;
}
//--------------------------------------
bool CGateServer::OnTcpAccept(KeyRef krAccept, KeyRef krListen)
{
	UNREFERENCED_PARAMETER(krListen);
	assert(krListen == m_krTCPService);
	if (m_nStatus == STATUSC_RUN) {
		NET_ADDR ClientAddr;
		m_NetworkPtr->GetAddr(krAccept, ClientAddr);
		if (m_GateRoutine->OnHandle(PAK_EVENT_LIVE, reinterpret_cast<uintptr_t>(&ClientAddr), TRUE)) {
			UShort     usPort = 0;
			CStringKey strAddr;
			m_NetworkPtr->GetAddr(krAccept, strAddr, usPort);

			DEV_INFO(TF("[���ط�����]�ͻ���[%s]:%d�������!"), *strAddr, usPort);
			m_ServerInfo.Incr();
			return true;
		}
	}
	else {
		LOGV_WARN(m_FileLog, TF("[���ط�����]���ط�����������ͣ״̬!"));
	}
	return false;
}
//--------------------------------------
bool CGateServer::OnTcpConnect(UInt uError, KeyRef krConnect)
{
	if (krConnect == m_krConnectGameDB) {
		m_bGameDBCnnted = (uError == 0);
		if (m_bGameDBCnnted) {
			LinkGameDBServer();
		}
		else {
			DEV_INFO(TF("[���ط�����]������ϷDB������ʧ��%X!"), uError);
			LOGV_WARN(m_FileLog, TF("[���ط�����]������ϷDB������ʧ��%X!"), uError);
		}
	}
	else if (krConnect == m_krConnectGame) {
		m_bGameCnnted = (uError == 0);
		if (m_bGameCnnted) {
			LinkGameServer();
		}
		else {
			DEV_INFO(TF("[���ط�����]������Ϸ������ʧ��%X!"), uError);
			LOGV_WARN(m_FileLog, TF("[���ط�����]������Ϸ������ʧ��%X!"), uError);
		}
	}
	return true;
}
// ������ϷDB������
void CGateServer::LinkGameDBServer(void)
{
	if (m_bGameDBCnnted && (m_bGameDBLinked == false)) {
		m_ServerInfo.usStatus = STATUSU_LINK;

		CGateLink Link;
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
			DEV_WARN(TF("[���ط�����]������ϷDB���������������������쳣!"));
			LOG_WARN(m_FileLog, TF("[���ط�����]������ϷDB���������������������쳣!"));
		}
	}
}
// ������ϷDB������
void CGateServer::UnlinkGameDBServer(void)
{
	if (m_bGameDBCnnted && m_bGameDBLinked)
	{
		m_bGameDBLinked = false;
		CPAKSimple<PAK_EVENT_UNLINK, PAK_TYPE_GATE> Unlink;
		Unlink.AdjustSize();
		m_NetworkPtr->Send(m_krConnectGameDB, Unlink);
	}
}
// ������Ϸ������
void CGateServer::LinkGameServer(void)
{
	if (m_bGameCnnted && (m_bGameLinked == false)) {
		m_ServerInfo.usStatus = STATUSU_LINK;

		CGateLink Link;
		Link.SetServerData(m_ServerInfo);
		Link.AdjustSize();

		CStreamScopePtr StreamPtr;
		m_NetworkPtr->AllocBuffer(StreamPtr);
		if (StreamPtr != nullptr) {
			Link.Serialize(*StreamPtr);
			m_ServerInfo.Addr(*StreamPtr);

			m_NetworkPtr->Send(m_krConnectGame, *StreamPtr);
		}
		else {
			DEV_WARN(TF("[���ط�����]������Ϸ���������������������쳣!"));
			LOG_WARN(m_FileLog, TF("[���ط�����]������Ϸ���������������������쳣!"));
		}
	}
}
// ������Ϸ������
void CGateServer::UnlinkGameServer(void)
{
	if (m_bGameCnnted && m_bGameLinked) {
		m_bGameLinked = false;
		CPAKSimple<PAK_EVENT_UNLINK, PAK_TYPE_GATE> Unlink;
		Unlink.AdjustSize();
		m_NetworkPtr->Send(m_krConnectGame, Unlink);
	}
}
//--------------------------------------
bool CGateServer::OnTcpClose(KeyRef krSocket, LLong llLiveData)
{
	if (krSocket == m_krConnectGameDB) {
		m_krConnectGameDB = nullptr;
		m_bGameDBCnnted   = false;
		m_bGameDBLinked   = false;
		m_pUIHandler->OnHandle(PAK_EVENT_UNLINK, 0, DATA_INDEX_GAMEDB);
		DEV_INFO(TF("[���ط�����]������ϷDB���������Ӷ���/�Ͽ�"));
	}
	else if (krSocket == m_krConnectGame) {
		m_krConnectGame = nullptr;
		m_bGameCnnted   = false;
		m_bGameLinked   = false;
		m_pUIHandler->OnHandle(PAK_EVENT_UNLINK, 0, DATA_INDEX_GAME);
		DEV_INFO(TF("[���ط�����]�������ķ��������Ӷ���/�Ͽ�"));
	}
	else if (krSocket == m_krTCPService) {
		m_krTCPService = nullptr;
		DEV_INFO(TF("[���ط�����]�����ͻ������Ӷ���/�Ͽ�"));
	}
	else { // ��������/�Ͽ�
		switch (llLiveData) {
		case PAK_TYPE_CLIENT:
		case PAK_TYPE_CLIENT_SELECT:
		case PAK_TYPE_CLIENT_QUEUE:
			{
				DEV_INFO(TF("[���ط�����]�ͻ���[%p]�Ͻ�"), krSocket);
				m_GateRoutine->OnHandle(PAK_EVENT_UNLINK, (uintptr_t)krSocket, llLiveData);
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
bool CGateServer::OnUdpClose(KeyRef krSocket, LLong)
{
	if (krSocket == m_krUDPService){
		m_krUDPService = nullptr;
		DEV_INFO(TF("[���ط�����]UDP������½�������ر�"));
	}
	return true;
}
//--------------------------------------
// ��ʱ���������ϷDB�����������Ӷ����Ƿ���Ч
bool CGateServer::CheckConnectGameDBServer(void)
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
// ��ʱ���������Ϸ�����������Ӷ����Ƿ���Ч
bool CGateServer::CheckConnectGameServer(void)
{
	if (m_bGameCnnted == false) {
		if (StartConnectGameServer() == false) {
			return false;
		}
	}
	if (m_bGameLinked == false) {
		LinkGameServer();
	}
	return true;
}
// ��ʱ���UDP������½�����������Ӷ����Ƿ���Ч
bool CGateServer::CheckUDPService(void)
{
	if (m_krUDPService == nullptr) {
		return StartUDPService();
	}
	return true;
}
// ��ʱ�������ͻ��˵����Ӷ����Ƿ���Ч
bool CGateServer::CheckTCPService(void)
{
	if (m_krTCPService == nullptr) {
		return StartTCPService();
	}
	return true;
}
//--------------------------------------
// ͬ����������Ϣ������
bool CGateServer::SyncServerInfo(void)
{
	m_GateRoutine->Update();
	if (m_ServerInfo.usStatus == STATUSU_SYNC) {
		CGateUpdate Update;
		Update.SetServerData(m_ServerInfo);
		Update.AdjustSize();
		if (m_pShareGameDBSvr != nullptr) {
			m_pShareGameDBSvr->OnShareRoutine(PAK_EVENT_UPDATE, Update, reinterpret_cast<LLong>(this));
		}
		else {
			m_NetworkPtr->Send(m_krConnectGameDB, Update);
		}
		if (m_pShareGameSvr != nullptr) {
			m_pShareGameSvr->OnShareRoutine(PAK_EVENT_UPDATE, Update, reinterpret_cast<LLong>(this));
		}
		else {
			m_NetworkPtr->Send(m_krConnectGame, Update);
		}
		m_ServerInfo.usStatus = STATUSU_OKAY;
		return m_pUIHandler->OnHandle(PAK_EVENT_SYNC, reinterpret_cast<uintptr_t>(&m_ServerInfo), DATA_INDEX_GATE);
	}
	else { // ��������
		if (m_pShareGameDBSvr == nullptr) { 
			CPAKSimple<PAK_EVENT_LIVE, PAK_TYPE_GATE> Live;
			Live.AdjustSize();
			m_NetworkPtr->Send(m_krConnectGameDB, Live);
		}
		if (m_pShareGameSvr == nullptr) { 
			CPAKSimple<PAK_EVENT_LIVE, PAK_TYPE_GATE> Live;
			Live.AdjustSize();
			m_NetworkPtr->Send(m_krConnectGame, Live);
		}
	}
	return true;
}



