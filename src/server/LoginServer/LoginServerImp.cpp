//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Source File : LoginServerImp.cpp                           //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ��½����������ʵ��                                 //
//                                                              //
//////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LoginServerImp.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLoginServer
UInt CLoginServer::Command(PCXStr pszCMD, uintptr_t utParam)
{
	// �������ļ������������Ƹ�����ز���
	UNREFERENCED_PARAMETER(pszCMD);
	UNREFERENCED_PARAMETER(utParam);
	return (UInt)RET_OKAY;
}

UInt CLoginServer::Update(void)
{
	CheckSessionTimeout();
	CheckConnectCenterServer();
	CheckConnectLoginDBServer();
	CheckUDPService();
	CheckTCPService();
	SyncServerInfo();
	SyncGameServerInfo();
	return 0;
}
//--------------------------------------
UInt CLoginServer::Init(CEventHandler& EventHandlerRef)
{
	if (m_nStatus == STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[��½������]��½�����ʼ����ʼ!"));
		m_pUIHandler = &EventHandlerRef;

		if ((InitLoadShare() == false) ||
			(InitLoadConfig() == false)) {
			return (UInt)RET_FAIL;
		}

		m_nStatus = STATUSC_INIT;
		LOG_INFO(m_FileLog, TF("[��½������]��½�����ʼ�����!"));
		return (UInt)RET_OKAY;
	}
	return (UInt)RET_FAIL;
}
// ��ȡ��������ö�����������
bool CLoginServer::InitLoadShare(void)
{
	assert(m_pConfig == nullptr);
	m_pUIHandler->OnHandle(CServerConfig::CFG_DEFAULT_CONFIG, reinterpret_cast<uintptr_t>(&m_pConfig), DATA_INDEX_LOGIN);
	if (m_pConfig == nullptr) {
		LOG_ERROR(m_FileLog, TF("[��½������]��ͬ���̹������ݻص��ӿڻ�ȡ���ö���ָ����Ч"));
		return false;
	}
	LOG_INFO(m_FileLog, TF("[��½������]�ӽ���ص��ӿڻ�ȡ���ö���ָ�����!"));
	m_pConfig->Dump(m_FileLog); // ������Ϣ�������־�ļ�
	// �Ӷ�̬�������ȡ���������ָ��
	assert(m_NetworkPtr == nullptr);
	CKeyValue::VALUE xValue = { 0 };
	m_pConfig->GetKeyValue()->GetValue(CServerConfig::NetworkPtr, xValue);
	if (xValue.pValue == nullptr) {
		LOG_ERROR(m_FileLog, TF("[��½������]�Ӷ�̬�������ȡ���������ָ����Ч"));
		return false;
	}
	LOG_INFO(m_FileLog, TF("[��½������]�Ӷ�̬�������ȡ���������ָ�����!"));
	m_NetworkPtr = *(reinterpret_cast<CNetworkPtr*>(xValue.pValue));
	return true;
}
// ��ʼ������
bool CLoginServer::InitLoadConfig(void)
{
	assert(m_krConnectCenter == nullptr);
	assert(m_krConnectLoginDB == nullptr);
	assert(m_krUDPService == nullptr);
	assert(m_krTCPService == nullptr);

	assert(m_pShareCenterSvr == nullptr);
	assert(m_pShareLoginDBSvr == nullptr);
	assert(m_pShareGameSvr == nullptr);

	assert(m_GameSvrMap.Size() == 0);
	assert(m_SessionMap.Size() == 0);

	m_ServerInfo.usId   = m_pConfig->GetServerId(CServerConfig::CFG_DEFAULT_LOGIN);
	m_ServerInfo.usIncr = m_pConfig->GetServerIncr(CServerConfig::CFG_DEFAULT_LOGIN);
	LOGV_INFO(m_FileLog, TF("[��½������]�����ö����ȡ������Id=%d, Incr=%d!"), m_ServerInfo.usId, m_ServerInfo.usIncr);
	if (m_pConfig->GetNetworkAttr() & ATTR_IPV6) {
		m_ServerInfo.NetAddr[LOGINI_TCP].usAttr     = ATTR_IPV6;
		m_ServerInfo.NetAddr[LOGINI_UDP].usAttr     = ATTR_IPV6;
		m_ServerInfo.NetAddr[LOGINI_LOGINDB].usAttr = ATTR_IPV6;
	}
	UShort     usPort = 0;
	CStringKey strAddr;
	m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_LOGIN, 0, strAddr, usPort);
	m_NetworkPtr->TranslateAddr(strAddr, usPort, m_ServerInfo.NetAddr[LOGINI_TCP]); // client tcp

	m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_LOGIN, CServerConfig::CFG_DEFAULT_GAME, strAddr, usPort);
	m_NetworkPtr->TranslateAddr(strAddr, usPort, m_ServerInfo.NetAddr[LOGINI_UDP]); // game udp

	m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_LOGINDB, 0, strAddr, usPort);
	m_NetworkPtr->TranslateAddr(strAddr, usPort, m_ServerInfo.NetAddr[LOGINI_LOGINDB]); // logindb
	return true;
}
//--------------------------------------
void CLoginServer::Exit(void)
{
	if (m_nStatus != STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[��½������]��½�����˳�����ʼ!"));
		m_pUIHandler = nullptr;

		ExitUnloadConfig();
		ExitUnloadShare();

		m_nStatus = STATUSC_NONE;
		LOG_INFO(m_FileLog, TF("[��½������]��½�����˳��������!"));
	}
}
// �����������ö�����������
void CLoginServer::ExitUnloadShare(void)
{
	m_pConfig    = nullptr;
	m_NetworkPtr = nullptr;
}
//
void CLoginServer::ExitUnloadConfig(void)
{
	assert(m_krConnectCenter == nullptr);
	assert(m_krConnectLoginDB == nullptr);
	assert(m_krUDPService == nullptr);
	assert(m_krTCPService == nullptr);

	assert(m_pShareCenterSvr == nullptr);
	assert(m_pShareLoginDBSvr == nullptr);
	assert(m_pShareGameSvr == nullptr);

	assert(m_GameSvrMap.Size() == 0);
	assert(m_SessionMap.Size() == 0);
}
//--------------------------------------
bool CLoginServer::Start(void)
{
	if (m_nStatus == STATUSC_INIT) {
		LOG_INFO(m_FileLog, TF("[��½������]��½����������ʼ!"));

		if ((StartUDPService() == false) || // ��ǰ������ȡ��ַ��Ϣ
			(StartConnectCenterServer() == false)  ||
			(StartConnectLoginDBServer() == false) ||
			(StartTCPService() == false)) {
			return false;
		}

		m_nStatus = STATUSC_RUN;
		LOG_INFO(m_FileLog, TF("[��½������]��½�����������!"));
	}
	return true;
}
// ���д����������ķ����������Ӷ���
bool CLoginServer::StartConnectCenterServer(void)
{
	// ��½�������ڲ�ͬ����,  ��Ҫ�����������ķ�����
	if ((m_pConfig->GetLoadServers() & CServerConfig::CFG_DEFAULT_CENTER) == 0) {
		if (m_krConnectCenter == nullptr) {
			UShort      usPort = 0;
			CStringKey  strAddr;
			m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_LOGIN, CServerConfig::CFG_DEFAULT_CENTER, strAddr, usPort);
			m_krConnectCenter = m_NetworkPtr->Create(*this, usPort, *strAddr);
		}
		if (m_krConnectCenter != nullptr) {
			if (m_bCenterCnnted == false) {
				LOG_INFO(m_FileLog, TF("[��½������]��½�����������ķ������ڲ�ͬ����, �����������ķ�����Socket�ɹ�"));
				UShort      usPort = 0;
				CStringKey  strAddr;
				m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_CENTER, CServerConfig::CFG_DEFAULT_LOGIN, strAddr, usPort);
				if (m_NetworkPtr->Connect(m_krConnectCenter, usPort, *strAddr) == false) {
					LOGV_ERROR(m_FileLog, TF("[��½������]�������ķ�����[%s]:%d����ʧ��"), *strAddr, usPort);
					return false;
				}
				LOGV_INFO(m_FileLog, TF("[��½������]�������ķ�����[%s]:%d�������"), *strAddr, usPort);
			}
		}
		else {
			LOG_ERROR(m_FileLog, TF("[��½������]��½�����������ķ������ڲ�ͬ����, �����������ķ�����Socketʧ��"));
			return false;
		}
	}
	else if (m_bCenterCnnted == false) {
		assert(m_pShareCenterSvr == nullptr);
		CKeyValue::VALUE xValue = { 0 };
		m_pConfig->GetKeyValue()->GetValue(CServerConfig::CenterServer, xValue);
		if (xValue.pValue == nullptr) {
			LOG_ERROR(m_FileLog, TF("[��½������]�Ӷ�̬�������ȡͬ�������ķ���������ָ����Ч"));
			return false;
		}
		m_pShareCenterSvr = reinterpret_cast<ICommonServer*>(xValue.pValue);
		m_bCenterCnnted = true;

		LOG_INFO(m_FileLog, TF("[��½������]ͬ����ֱ���������ķ�����"));
		m_ServerInfo.usStatus = STATUSU_LINK;

		CLoginLink Link;
		Link.SetServerData(m_ServerInfo);
		Link.AdjustSize();

		CBufReadStream brs((PByte)m_ServerInfo.NetAddr, sizeof(m_ServerInfo.NetAddr));
		Link.SetStream(brs);

		m_bCenterLinked = m_pShareCenterSvr->OnShareRoutine(PAK_EVENT_LINK, Link, reinterpret_cast<LLong>(this));
		m_pUIHandler->OnHandle(PAK_EVENT_LINK, 0, DATA_INDEX_CENTER);
		return m_bCenterLinked;
	}
	return true;
}
// ���д������ӵ�½DB�����������Ӷ���
bool CLoginServer::StartConnectLoginDBServer(void)
{
	// ��½�͵�½DB�ڲ�ͬ����,  ��Ҫ����������½DB������
	if ((m_pConfig->GetLoadServers() & CServerConfig::CFG_DEFAULT_LOGINDB) == 0) {
		if (m_krConnectLoginDB == nullptr) {
			UShort      usPort = 0;
			CStringKey  strAddr;
			m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_LOGIN, CServerConfig::CFG_DEFAULT_LOGINDB, strAddr, usPort);
			m_krConnectLoginDB = m_NetworkPtr->Create(*this, usPort, *strAddr);
		}
		if (m_krConnectLoginDB != nullptr) {
			if (m_bLoginDBCnnted == false) {
				LOG_INFO(m_FileLog, TF("[��½������]��½�������͵�½DB�������ڲ�ͬ����, �������ӵ�½DB������Socket�ɹ�"));
				if (m_NetworkPtr->Connect(m_krConnectLoginDB, m_ServerInfo.NetAddr[LOGINI_LOGINDB]) == false) {
					LOGV_ERROR(m_FileLog, TF("[��½������]���ӵ�½DB����������ʧ��"));
					return false;
				}
				LOGV_INFO(m_FileLog, TF("[��½������]���ӵ�½DB�������������"));
			}
		}
		else {
			LOG_ERROR(m_FileLog, TF("[��½������]��½�������͵�½DB�������ڲ�ͬ����, �����������ķ�����Socketʧ��"));
			return false;
		}
	}
	else if (m_bLoginDBCnnted == false) {
		assert(m_pShareLoginDBSvr == nullptr);
		CKeyValue::VALUE xValue = { 0 };
		m_pConfig->GetKeyValue()->GetValue(CServerConfig::LoginDBServer, xValue);
		if (xValue.pValue == nullptr) {
			LOG_ERROR(m_FileLog, TF("[��½������]�Ӷ�̬�������ȡͬ���̵�½DB����������ָ����Ч"));
			return false;
		}
		m_pShareLoginDBSvr = reinterpret_cast<ICommonServer*>(xValue.pValue);
		m_bLoginDBCnnted = true;

		LOG_INFO(m_FileLog, TF("[��½������]ͬ����ֱ�����ӵ�½DB������"));
		m_ServerInfo.usStatus = STATUSU_LINK;

		CLoginLink Link;
		Link.SetServerData(m_ServerInfo);
		Link.AdjustSize();

		CBufReadStream brs((PByte)m_ServerInfo.NetAddr, sizeof(m_ServerInfo.NetAddr));
		Link.SetStream(brs);

		m_bLoginDBLinked = m_pShareLoginDBSvr->OnShareRoutine(PAK_EVENT_LINK, Link, reinterpret_cast<LLong>(this));
		m_pUIHandler->OnHandle(PAK_EVENT_LINK, 0, DATA_INDEX_LOGINDB);
		return m_bLoginDBLinked;
	}
	return true;
}
// ���д���UDP������Ϸ������У�����Ӷ���
bool CLoginServer::StartUDPService(void)
{
	if (m_krUDPService == nullptr) {
		m_krUDPService = m_NetworkPtr->Create(*this, m_ServerInfo.NetAddr[LOGINI_UDP], SOCKET_UDP);
		if (m_krUDPService != nullptr) {
			LOGV_INFO(m_FileLog, TF("[��½������]����UDP������Ϸ�������ɹ�"));
			if (m_ServerInfo.NetAddr[LOGINI_UDP].usPort == 0) {
				m_NetworkPtr->GetAddr(m_krUDPService, m_ServerInfo.NetAddr[LOGINI_UDP], false);
			}
			m_pUIHandler->OnHandle(PAK_EVENT_LINK, reinterpret_cast<uintptr_t>(m_ServerInfo.NetAddr + LOGINI_UDP), DATA_INDEX_LOGIN);
		}
		else {
			LOGV_ERROR(m_FileLog, TF("[��½������]����UDP������Ϸ������ʧ��"));
			return false;
		}
	}
	if (((m_pConfig->GetLoadServers() & CServerConfig::CFG_DEFAULT_GAME) != 0) &&
		(m_pShareGameSvr == nullptr)) {
		CKeyValue::VALUE xValue = { 0 };
		m_pConfig->GetKeyValue()->GetValue(CServerConfig::GameServer, xValue);
		if (xValue.pValue == nullptr) {
			LOG_ERROR(m_FileLog, TF("[��½������]�Ӷ�̬�������ȡͬ������Ϸ����������ָ����Ч"));
			return false;
		}
		m_pShareGameSvr = reinterpret_cast<ICommonServer*>(xValue.pValue);
	}
	return true;
}

// ���д��������ͻ������Ӷ���
bool CLoginServer::StartTCPService(void)
{
	if (m_krTCPService != nullptr) {
		LOGV_INFO(m_FileLog, TF("[��½������]���������ͻ��˵������Ѿ�����"));
		return true;
	}
	bool bRet = true;
	m_krTCPService = m_NetworkPtr->Create(*this, m_ServerInfo.NetAddr[LOGINI_TCP]);
	if (m_krTCPService != nullptr) {
		bRet = m_NetworkPtr->Listen(m_krTCPService);
		LOGV_INFO(m_FileLog, TF("[��½������]���������ͻ��˵����ӳɹ�, %s"), bRet ? TF("�������ӳɹ�") : TF("��������ʧ��"));
	}
	else {
		LOGV_ERROR(m_FileLog, TF("[��½������]���������ͻ��˵�����ʧ��"));
		bRet = false;
	}
	return bRet;
}
//--------------------------------------
bool CLoginServer::Pause(bool bPause)
{
	if (bPause && (m_nStatus == STATUSC_RUN)) {
		m_nStatus = STATUSC_PAUSE; 
		LOG_INFO(m_FileLog, TF("[��½������]��ͣ����"));
		return true;
	}
	else if ((bPause == false) && (m_nStatus == STATUSC_PAUSE)) {
		m_nStatus = STATUSC_RUN;   
		LOG_INFO(m_FileLog, TF("[��½������]�������"));
		return true;
	}
	return false;
}
//--------------------------------------
void CLoginServer::Stop(void)
{
	if (m_nStatus > STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[��½������]��½����ֹͣ��ʼ!"));

		StopUDPService();
		StopTCPService();
		StopConnectLoginDBServer();
		StopConnectCenterServer();

		m_ServerInfo.Zero();
		m_GameInfo.Reset();
		m_GameSvrMap.RemoveAll();
		m_SessionMap.RemoveAll();

		m_nStatus = STATUSC_INIT;
		LOG_INFO(m_FileLog, TF("[��½������]��½����ֹͣ���!"));
	}
}
// ֹͣ�������ķ�����
void CLoginServer::StopConnectCenterServer(void)
{
	if (m_krConnectCenter != nullptr) {
		m_NetworkPtr->Destroy(m_krConnectCenter, false);
		m_krConnectCenter = nullptr;
		LOG_INFO(m_FileLog, TF("[��½������]�����������ķ����������ӳɹ�"));
	}
	m_bCenterCnnted   = false;
	m_bCenterLinked   = false;
	m_pShareCenterSvr = nullptr;
}
// ֹͣ���ӵ�½DB������
void CLoginServer::StopConnectLoginDBServer(void)
{
	if (m_krConnectLoginDB != nullptr) {
		m_NetworkPtr->Destroy(m_krConnectLoginDB, false);
		m_krConnectLoginDB = nullptr;
		LOG_INFO(m_FileLog, TF("[��½������]�������ӵ�½DB�����������ӳɹ�"));
	}
	m_bLoginDBCnnted   = false;
	m_bLoginDBLinked   = false;
	m_pShareLoginDBSvr = nullptr;
}
// ֹͣUDP������Ϸ������
void CLoginServer::StopUDPService(void)
{
	if (m_krUDPService != nullptr) {
		m_NetworkPtr->Destroy(m_krUDPService, false);
		m_krUDPService = nullptr;
		LOG_INFO(m_FileLog, TF("[��½������]����UDP��������ӳɹ�"));
	}
	m_pShareGameSvr = nullptr;
}
// ֹͣ�����ͻ�������
void CLoginServer::StopTCPService(void)
{
	if (m_krTCPService != nullptr) {
		m_NetworkPtr->Destroy(m_krTCPService, false);
		m_krTCPService = nullptr;
		LOG_INFO(m_FileLog, TF("[��½������]���ټ����ͻ��˵����ӳɹ�"));
	}
}
//--------------------------------------
bool CLoginServer::OnShareRoutine(Int nEvent, CEventBase& EventRef, LLong, CEventQueue::EVENT_TYPE)
{
	assert((m_pConfig->GetLoadServers() & (CServerConfig::CFG_DEFAULT_LOGINDB|CServerConfig::CFG_DEFAULT_GAME)) != 0);
	switch (nEvent) {
	// logindb
	case PAK_EVENT_LOGIN_LINKACK:
		{
			GameLinkAck(static_cast<CPAKSessionAck*>(&EventRef));
		}
		break;
	case PAK_EVENT_LOGIN_UNLINKACK:
		{
			GameUnlinkAck(reinterpret_cast<CPAKSessionAck*>(&EventRef));
		}
		break;
	// game or gate
	case PAK_EVENT_LOGIN_SELECT_GAME:
		{
			CPAKSessionAck* pAck = reinterpret_cast<CPAKSessionAck*>(&EventRef);
			assert((pAck->GetType() == PAK_TYPE_GAME) || (pAck->GetType() == PAK_TYPE_GATE));
			SelectGameAck(pAck);
		}
		break;
	case PAK_EVENT_LOGIN_LINK_GAME:
		{
			CPAKLoginLinkGame* pLink = reinterpret_cast<CPAKLoginLinkGame*>(&EventRef);
			assert(pLink->GetType() == PAK_TYPE_GATE);
			LinkGameAck(pLink);
		}
		break;
	case PAK_EVENT_LOGIN_QUEUE_PLAY:
		{
			CPAKSessionAck* pAck = reinterpret_cast<CPAKSessionAck*>(&EventRef);
			assert(pAck->GetType() == PAK_TYPE_GATE);
			PlayGameAck(pAck);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��½������]ͬ���̷������޷�ʶ������������[event=%d]"), nEvent);
			assert(0);
		}
	}
	return true;
}

bool CLoginServer::OnShareRoutine(Int nEvent, CStream& Stream, LLong)
{
	assert((m_pConfig->GetLoadServers() & CServerConfig::CFG_DEFAULT_CENTER) != 0);
	if (nEvent == PAK_EVENT_SYNC) {
		LOGV_INFO(m_FileLog, TF("[��½������]ͬ����ͬ����Ϸ��������Ϣ"));
		CStreamScopePtr StreamPtr;
		if (m_NetworkPtr->ReferBuffer(StreamPtr, Stream)) { // ͬ���̵�Stream��дģʽ, ����һ����ģʽ������
			CPAKSync Sync;
			Sync.Serialize(*StreamPtr);

			m_GameInfo.Copy(Sync.GetServerData());
			SyncGameServerInfo(*StreamPtr);
		}
		else {
			LOGV_WARN(m_FileLog, TF("[��½������]ͬ����ͬ����Ϸ��������Ϣ����"));
		}
	}
	else {
		LOGV_WARN(m_FileLog, TF("[��½������]ͬ���̷������޷�ʶ������������[event=%d]"), nEvent);
		assert(0);
	}
	return true;
}
//--------------------------------------
bool CLoginServer::OnTcpDispatch(const PacketPtr& PktPtr, PTCP_PARAM pTcp)
{
	// PTCP_PARAM->pCache == nullptr, �ϲ���Ա���ӵײ�����Ļ�������
	// PTCP_PARAM->pCache, PTCP_PARAM->index�ǵײ�����Ļ������ݵ���ʼ��ַ��������
	// ����ϲ㱣�滺��, ����PTCP_PARAM->pDataΪnullptr֪ͨ����ײ�
	// �ͷŻ���ʹ��MObject::sMCFree(index, pCache)
	bool bRet = false;
	if (PktPtr->GetEvent() != PAK_EVENT_LIVE) {
		switch (PktPtr->GetType()) {
		case PAK_TYPE_CLIENT_LOGIN:
			{
				bRet = DispatchGameService(PktPtr, pTcp->krSocket);
			}
			break;
		case PAK_TYPE_LOGINDB:
			{
				bRet = DispatchLoginDBServer(PktPtr, pTcp->krSocket);
			}
			break;
		case PAK_TYPE_CENTER:
			{
				bRet = DispatchCenterServer(PktPtr, pTcp->krSocket);
			}
			break;
		default:
			{
				LOGV_WARN(m_FileLog, TF("[��½������]%p�������������[%d]����ȷ"), pTcp->krSocket, PktPtr->GetType());
				bRet = false;
			}
		}
	}
	else {
		CPAKSimple<PAK_EVENT_LIVEACK, PAK_TYPE_LOGIN> LiveAck;
		LiveAck.AdjustSize();
		bRet = m_NetworkPtr->Send(pTcp->krSocket, LiveAck);
	}
	return bRet;
}
// �������ķ������������
bool CLoginServer::DispatchCenterServer(const PacketPtr& PktPtr, KeyRef krSocket)
{
	switch (PktPtr->GetEvent()) {
	case PAK_EVENT_LINKACK:
		{
			m_bCenterLinked = true;

			NET_ADDR NetAddr;
			m_NetworkPtr->GetAddr(krSocket, NetAddr, false);
			m_pUIHandler->OnHandle(PAK_EVENT_LINK, reinterpret_cast<uintptr_t>(&NetAddr), DATA_INDEX_CENTER);
			LOG_INFO(m_FileLog, TF("[��½������]�յ����ķ��������ӻظ���"));
		}
		break;
	case PAK_EVENT_UPDATEACK:
		{
			DEV_INFO(TF("[��½������]�յ����ķ��������»ظ���"));
		}
		break;
	case PAK_EVENT_SYNC:
		{
			DEV_INFO(TF("[��½������]���ķ�����ͬ����Ϸ��������Ϣ"));
			CPAKSync* pSync = static_cast<CPAKSync*>(PktPtr.Get());
			m_GameInfo.Copy(pSync->GetServerData());

			if (pSync->CheckStream()) {
				SyncGameServerInfo(pSync->GetStream());
			}
			// ����ͬ����ִ��
			CPAKSimple<PAK_EVENT_SYNCACK, PAK_TYPE_LOGIN> SyncAck;
			SyncAck.AdjustSize();
			m_NetworkPtr->Send(krSocket, SyncAck);
		}
		break;
	case PAK_EVENT_UNLINKACK:
		{
			m_bCenterLinked = false;
			LOG_INFO(m_FileLog, TF("[��½������]�յ����ķ������Ͻӻظ���"));
		}
		break;
	case PAK_EVENT_LIVEACK:
		{
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��½������]%p������Ϣ�޷�ʶ������ķ��������������[event=%d]"), krSocket, PktPtr->GetEvent());
		}
	}
	return true;
}
// �����½DB�������������
bool CLoginServer::DispatchLoginDBServer(const PacketPtr& PktPtr, KeyRef krSocket)
{
	switch (PktPtr->GetEvent()) {
	case PAK_EVENT_LINKACK:
		{
			m_bLoginDBLinked = true;

			NET_ADDR NetAddr;
			m_NetworkPtr->GetAddr(krSocket, NetAddr, false);
			m_pUIHandler->OnHandle(PAK_EVENT_LINK, reinterpret_cast<uintptr_t>(&NetAddr), DATA_INDEX_LOGINDB);
			LOG_INFO(m_FileLog, TF("[��½������]�յ���½DB���������ӻظ���"));
		}
		break;
	case PAK_EVENT_UPDATEACK:
		{
			DEV_INFO(TF("[��½������]�յ���½DB���������»ظ���"));
		}
		break;
	case PAK_EVENT_UNLINKACK:
		{
			m_bLoginDBLinked = false;
			LOG_INFO(m_FileLog, TF("[��½������]�յ���½DB�������Ͻӻظ���"));
		}
		break;
	case PAK_EVENT_LOGIN_LINKACK:
		{
			GameLinkAck(static_cast<CPAKSessionAck*>(PktPtr.Get()));
		}
		break;
	case PAK_EVENT_LOGIN_UNLINKACK:
		{
			GameUnlinkAck(static_cast<CPAKSessionAck*>(PktPtr.Get()));
		}
		break;
	case PAK_EVENT_LIVEACK:
		{
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��½������]%p������Ϣ�޷�ʶ��ĵ�½DB���������������[event=%d]"), krSocket, PktPtr->GetEvent());
		}
	}
	return true;
}
// ������Ϸ�ͻ��˵������
bool CLoginServer::DispatchGameService(const PacketPtr& PktPtr, KeyRef krSocket)
{
	bool bRet = false;
	switch (PktPtr->GetEvent()) {
	case PAK_EVENT_LOGIN_LINK:
		{
			bRet = OnClientLink(static_cast<CPAKLoginLink*>(PktPtr.Get()), krSocket);
		}
		break;
	case PAK_EVENT_LOGIN_UNLINK:
		{
			bRet = OnClientUnlink(static_cast<CPAKLoginUnlink*>(PktPtr.Get()), krSocket);
		}
		break;
	case PAK_EVENT_LOGIN_SELECT_GAME:
		{
			bRet = OnClientSelectGame(static_cast<CPAKLoginSelectGame*>(PktPtr.Get()), krSocket);
		}
		break;
	case PAK_EVENT_LINK:
		{	// �ͻ�����Ҫ�������������������ACK�Ͽ�
			m_NetworkPtr->SetAttr(krSocket, PAK_TYPE_CLIENT_READY);
			CPAKSimple<PAK_EVENT_LINKACK, PAK_TYPE_LOGIN> LinkAck;
			LinkAck.AdjustSize();
			bRet = m_NetworkPtr->Send(krSocket, LinkAck);
		}
		break;
	case PAK_EVENT_UNLINK: // �ͻ��������Ͽ�����
		{
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��½������]%p������Ϣ�޷�ʶ��Ŀͻ������������[event=%d]"), krSocket, PktPtr->GetEvent());
		}
	}
	return bRet;
}

bool CLoginServer::OnClientLink(CPAKLoginLink* pLink, KeyRef krSocket)
{
	UInt uAck = LOGIN_ERROR_SESSION;

	SESSION_DATA sd;
	sd.llTimeout = CPlatform::GetRunningTime();
	sd.nStatus   = SESSION_DATA::SESSION_STATUS_LINK;
	{
		CSyncLockWaitScope scope(m_SessionMap.GetLock());
		SESSION_MAP::DATA_MAP_PAIR* pPair = m_SessionMap.Find((DataRef)krSocket);
		if (pPair == nullptr) {
			m_SessionMap.Add((DataRef)krSocket, sd);
			uAck = DATAD_OKAY;
		}
		else if (pPair->m_V.nStatus == SESSION_DATA::SESSION_STATUS_NONE) { // ��½ʧ��
			pPair->m_V.nStatus   = SESSION_DATA::SESSION_STATUS_LINK;
			pPair->m_V.llTimeout = sd.llTimeout;
			uAck = DATAD_OKAY;
		}
	}
	if (uAck == DATAD_OKAY) {
		pLink->SetType(PAK_TYPE_LOGIN);
		pLink->SetSessionId((LLong)krSocket);
		// ���͸���½DB���е�½����, ��ʱ�رյ�ǰ����
		if (m_pShareLoginDBSvr == nullptr) {
			m_NetworkPtr->Send(m_krConnectLoginDB, *pLink);
		}
		else {
			m_pShareLoginDBSvr->OnShareRoutine(PAK_EVENT_LOGIN_LINK, *pLink);
		}
	}
	else {
		CPAKAck LinkAck(PAK_EVENT_LOGIN_LINKACK, (UInt)PAK_TYPE_LOGIN);
		LinkAck.SetAck(LOGIN_ERROR_SESSION);
		LinkAck.AdjustSize();
		m_NetworkPtr->Send(krSocket, LinkAck, SEND_CLOSE);
	}
	return true;
}

bool CLoginServer::OnClientUnlink(CPAKLoginUnlink* pUnlink, KeyRef krSocket)
{
	UInt uAck = LOGIN_ERROR_SESSION;
	{
		CSyncLockWaitScope scope(m_SessionMap.GetLock());
		SESSION_MAP::DATA_MAP_PAIR* pPair = m_SessionMap.Find((DataRef)krSocket);
		if (pPair != nullptr) {
			// session &  UNLINK > status >= LINKACK
			if ( (pUnlink->GetSessionId() == reinterpret_cast<LLong>(pPair))  &&
				 (pPair->m_V.nStatus >= SESSION_DATA::SESSION_STATUS_LINKACK) &&
				 (pPair->m_V.nStatus < SESSION_DATA::SESSION_STATUS_UNLINK) ) {

				pPair->m_V.nStatus   = SESSION_DATA::SESSION_STATUS_UNLINK;
				pPair->m_V.llTimeout = CPlatform::GetRunningTime();

				pUnlink->SetType(PAK_TYPE_LOGIN);
				pUnlink->SetSessionId((LLong)krSocket);
				pUnlink->SetUserId(pPair->m_V.llUserId);
				pUnlink->SetTime(pPair->m_V.llOnline);
				pUnlink->SetGameId(pPair->m_V.nGameId);
				pUnlink->SetCltLast(pPair->m_V.CltLast);

				uAck = DATAD_OKAY;
			}
		}
	}
	if (uAck == DATAD_OKAY) { // ���͸���½DB���е�½����, ��ʱ�رյ�ǰ����
		if (m_pShareLoginDBSvr == nullptr) {
			m_NetworkPtr->Send(m_krConnectLoginDB, *pUnlink);
		}
		else {
			m_pShareLoginDBSvr->OnShareRoutine(PAK_EVENT_LOGIN_UNLINK, *pUnlink);
		}
	}
	else {
		CPAKAck UnlinkAck(PAK_EVENT_LOGIN_UNLINKACK, (UInt)PAK_TYPE_LOGIN);
		UnlinkAck.SetAck(uAck);
		UnlinkAck.AdjustSize();
		m_NetworkPtr->Send(krSocket, UnlinkAck, SEND_CLOSE);
	}
	return true;
}

bool CLoginServer::OnClientSelectGame(CPAKLoginSelectGame* pSelect, KeyRef krSocket)
{
	UInt  uAck    = LOGIN_ERROR_SESSION;
	LLong llIndex = pSelect->GetAuthCode();
	{
		CSyncLockWaitScope scope(m_SessionMap.GetLock());
		SESSION_MAP::DATA_MAP_PAIR* pPair = m_SessionMap.Find((DataRef)krSocket);
		if (pPair != nullptr) {
			// session &  status == LINKACK
			if ((pSelect->GetSessionId() == reinterpret_cast<LLong>(pPair)) &&
				(pPair->m_V.nStatus == SESSION_DATA::SESSION_STATUS_LINKACK)) {

				pPair->m_V.nStatus     = SESSION_DATA::SESSION_STATUS_SELECT;
				pPair->m_V.llTimeout   = CPlatform::GetRunningTime();
				pPair->m_V.llGameIndex = llIndex;

				pSelect->SetType(PAK_TYPE_LOGIN);
				pSelect->SetAck(DATAD_OKAY);
				pSelect->SetSessionId((LLong)krSocket);
				pSelect->SetAuthCode(pPair->m_V.llUserId); // user-id
				pSelect->SetAddr(m_ServerInfo.NetAddr[LOGINI_UDP]);

				uAck = DATAD_OKAY;
			}
		}
	}
	if (uAck == DATAD_OKAY) {
		CSyncLockWaitScope scope(m_GameSvrMap.GetLock());
		SVR_GAME_MAP::SVR_MAP_PAIR* pGameSvr = m_GameSvrMap.Find((DataRef)llIndex);
		if (pGameSvr != nullptr) {
			if ((DataRef)m_pShareGameSvr == pGameSvr->m_K) {
				m_pShareGameSvr->OnShareRoutine(PAK_EVENT_LOGIN_SELECT_GAME, *pSelect, reinterpret_cast<LLong>(this));
			}
			else {
				m_NetworkPtr->SendTo(m_krUDPService, *pSelect, pGameSvr->m_V.NetAddr[GAMEI_UDP]);
			}
			DEV_INFO(TF("[��½������]�ͻ��������½��Ϸ������, ����UserId=%llx�������������Ӧ����Ϸ������"), pSelect->GetAuthCode());
		}
		else {
			DEV_INFO(TF("[��½������]�ͻ��������½��Ϸ������, ����Ϸ��������Ϣ[%llx]����"), llIndex);
			uAck = LOGIN_ERROR_GAME_INDEX;
		}
	}
	if (uAck != DATAD_OKAY) {
		CPAKAck SelectGameAck(PAK_EVENT_LOGIN_SELECT_GAME, (UInt)PAK_TYPE_LOGIN);
		SelectGameAck.SetAck(uAck);
		SelectGameAck.AdjustSize();
		m_NetworkPtr->Send(krSocket, SelectGameAck, SEND_CLOSE);
	}
	return true;
}
//--------------------------------------
bool CLoginServer::OnUdpDispatch(const PacketPtr& PktPtr, PUDP_PARAM pUdp)
{
	// PUDP_PARAM->pCache == nullptr, �ϲ���Ա���ӵײ�����Ļ�������
	// PUDP_PARAM->pCache, PUDP_PARAM->index�ǵײ�����Ļ������ݵ���ʼ��ַ��������
	// ����ϲ㱣�滺��, ����PUDP_PARAM->pDataΪnullptr֪ͨ����ײ�
	// �ͷŻ���ʹ��MObject::sMCFree(index, pCache)
	switch (PktPtr->GetEvent()) {
	case PAK_EVENT_LOGIN_SELECT_GAME:
		{
			assert((PktPtr->GetType() == PAK_TYPE_GAME) || (PktPtr->GetType() == PAK_TYPE_GATE));
			SelectGameAck(static_cast<CPAKSessionAck*>(PktPtr.Get()));
		}
		break;
	case PAK_EVENT_LOGIN_LINK_GAME:
		{
			assert(PktPtr->GetType() == PAK_TYPE_GATE);
			LinkGameAck(static_cast<CPAKLoginLinkGame*>(PktPtr.Get()));
		}
		break;
	case PAK_EVENT_LOGIN_QUEUE_PLAY:
		{
			assert(PktPtr->GetType() == PAK_TYPE_GATE);
			PlayGameAck(static_cast<CPAKSessionAck*>(PktPtr.Get()));
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��½������]%p������Ϣ���������[%d]������Ϸ����������"), pUdp->krSocket, PktPtr->GetType());
		}
	}
	return true;
}
//--------------------------------------
bool CLoginServer::OnTcpAccept(KeyRef krAccept, KeyRef krListen)
{
	UNREFERENCED_PARAMETER( krListen );
	assert(krListen == m_krTCPService);
	if (m_nStatus == STATUSC_RUN) {
		NET_ADDR ClientAddr;
		m_NetworkPtr->GetAddr(krAccept, ClientAddr);
		// �ж�IP�Ƿ��ں�������
		if (CheckAddrBlacklist(ClientAddr) == false) {
			UShort     usPort = 0;
			CStringKey strAddr;
			m_NetworkPtr->GetAddr(krAccept, strAddr, usPort);

			DEV_INFO(TF("[��½������]�ͻ���[%s]:%d�������!"), *strAddr, usPort);
			m_ServerInfo.Incr();
			return true;
		}
	}
	else {
		LOGV_WARN(m_FileLog, TF("[��½������]��½������������ͣ״̬!"));
	}
	return false;
}

bool CLoginServer::CheckAddrBlacklist(NET_ADDR& ClientAddr)
{
	// TODO!!! ������У��δ���
	size_t stHash = 0;
	if (m_pConfig->GetNetworkAttr() & ATTR_IPV6) {
		stHash = CHash::Hash(ClientAddr.Addr.bAddr, NET_ADDR::LEN_BYTE);
	}
	else {
		stHash = (size_t)ClientAddr.Addr.uAddr[0];
	}
	// 1. �Ƿ������ú�����
	// 2. �Ƿ��ڶ�ʱ������(�����Ƿ��ж����������->��������ʱ��)--����Ա�鿴���Խ�����ʱ�䳤�ļ������ú�����
	// 3. ��ʱ��ַ��������Ƿ��ж����������->���ڼ��붨ʱ������
	return (stHash == 0);
}
//--------------------------------------
bool CLoginServer::OnTcpConnect(UInt uError, KeyRef krConnect)
{
	if (krConnect == m_krConnectCenter) {
		m_bCenterCnnted = (uError == 0);
		if (m_bCenterCnnted) {
			LinkCenterServer();
		}
		else {
			DEV_INFO(TF("[��½������]�������ķ�����ʧ��%X!"), uError);
			LOGV_WARN(m_FileLog, TF("[��½������]�������ķ�����ʧ��%X!"), uError);
		}
	}
	else if (krConnect == m_krConnectLoginDB) {
		m_bLoginDBCnnted = (uError == 0);
		if (m_bLoginDBCnnted) {
			LinkLoginDBServer();
		}
		else {
			DEV_INFO(TF("[��½������]���ӵ�½DB������ʧ��%X!"), uError);
			LOGV_WARN(m_FileLog, TF("[��½������]���ӵ�½DB������ʧ��%X!"), uError);
		}
	}
	return true;
}
// �������ķ�����
void CLoginServer::LinkCenterServer(void)
{
	if (m_bCenterCnnted && (m_bCenterLinked == false)) {
		m_ServerInfo.usStatus = STATUSU_LINK;

		CLoginLink Link;
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
			DEV_WARN(TF("[��½������]�������ķ��������������������쳣!"));
			LOG_WARN(m_FileLog, TF("[��½������]�������ķ��������������������쳣!"));
		}
	}
}
// �������ķ�����
void CLoginServer::UnlinkCenterServer(void)
{
	if (m_bCenterCnnted && m_bCenterLinked) {
		m_bCenterLinked = false;
		CPAKSimple<PAK_EVENT_UNLINK, PAK_TYPE_LOGIN> Unlink;
		Unlink.AdjustSize();
		m_NetworkPtr->Send(m_krConnectCenter, Unlink);
	}
}
// ���ӵ�½DB������
void CLoginServer::LinkLoginDBServer(void)
{
	if (m_bLoginDBCnnted && (m_bLoginDBLinked == false)) {
		m_ServerInfo.usStatus = STATUSU_LINK;

		CLoginLink Link;
		Link.SetServerData(m_ServerInfo);
		Link.AdjustSize();

		CStreamScopePtr StreamPtr;
		m_NetworkPtr->AllocBuffer(StreamPtr);
		if (StreamPtr != nullptr) {
			Link.Serialize(*StreamPtr);
			m_ServerInfo.Addr(*StreamPtr);

			m_NetworkPtr->Send(m_krConnectLoginDB, *StreamPtr);
		}
		else {
			DEV_WARN(TF("[��½������]���ӵ�½DB���������������������쳣!"));
			LOG_WARN(m_FileLog, TF("[��½������]���ӵ�½DB���������������������쳣!"));
		}
	}
}
// ������½DB������
void CLoginServer::UnlinkLoginDBServer(void)
{
	if (m_bLoginDBCnnted && m_bLoginDBLinked)
	{
		m_bLoginDBLinked = false;
		CPAKSimple<PAK_EVENT_UNLINK, PAK_TYPE_LOGIN> Unlink;
		Unlink.AdjustSize();
		m_NetworkPtr->Send(m_krConnectLoginDB, Unlink);
	}
}
//--------------------------------------
bool CLoginServer::OnTcpClose(KeyRef krSocket, LLong llLiveData)
{
	if (krSocket == m_krConnectCenter) {
		m_krConnectCenter = nullptr;
		m_bCenterCnnted   = false;
		m_bCenterLinked   = false;
		m_pUIHandler->OnHandle(PAK_EVENT_UNLINK, 0, DATA_INDEX_CENTER);
		DEV_INFO(TF("[��½������]�������ķ��������Ӷ���/�Ͽ�"));
	}
	else if (krSocket == m_krConnectLoginDB) {
		m_krConnectLoginDB = nullptr;
		m_bLoginDBCnnted   = false;
		m_bLoginDBLinked   = false;
		m_pUIHandler->OnHandle(PAK_EVENT_UNLINK, 0, DATA_INDEX_LOGINDB);
		DEV_INFO(TF("[��½������]���ӵ�½DB���������Ӷ���/�Ͽ�"));
	}
	else if (krSocket == m_krTCPService) {
		m_krTCPService = nullptr;
		DEV_INFO(TF("[��½������]�����ͻ������ӹر�"));
	}
	else { // ��������������/�Ͽ�
		switch (llLiveData) {
		case PAK_TYPE_CLIENT_LOGIN:
			{
				DEV_INFO(TF("[��½������]�ͻ���[%p]�Ͻ�"), krSocket);
				CSyncLockWaitScope scope(m_SessionMap.GetLock());
				SESSION_MAP::DATA_MAP_PAIR* pPair = m_SessionMap.Find((DataRef)krSocket);
				if (pPair != nullptr) {
					if ((pPair->m_V.nStatus > SESSION_DATA::SESSION_STATUS_LINK) &&
						(pPair->m_V.nStatus < SESSION_DATA::SESSION_STATUS_UNLINK)) {

						CTRefCountPtr<CPAKLoginUnlink> UnlinkPtr = MNEW CPAKLoginUnlink;
						UnlinkPtr->SetType(PAK_TYPE_LOGIN);
						//UnlinkPtr->SetSessionId(0); // �Ͽ�SessionId����Ϊ0
						UnlinkPtr->SetUserId(pPair->m_V.llUserId);
						UnlinkPtr->SetTime(pPair->m_V.llOnline);
						UnlinkPtr->SetGameId(pPair->m_V.nGameId);
						UnlinkPtr->SetCltLast(pPair->m_V.CltLast);
						UnlinkPtr->AdjustSize();
						if (m_pShareLoginDBSvr == nullptr) {
							m_NetworkPtr->Send(m_krConnectLoginDB, *UnlinkPtr);
						}
						else {
							m_pShareLoginDBSvr->OnShareRoutine(PAK_EVENT_LOGIN_UNLINK, *UnlinkPtr);
						}
					}
					m_SessionMap.RemoveAt(reinterpret_cast<PINDEX>(pPair));
				}
				else {
					DEV_WARN(TF("[��½������]�ͻ��˶Ͽ�ʱ�޷����ҵ�SessionId[%p]����"), krSocket);
					LOGV_WARN(m_FileLog, TF("[��½������]�ͻ��˶Ͽ�ʱ�޷����ҵ�SessionId[%p]����"), krSocket);
				}
			}
			break;
		case PAK_TYPE_CLIENT_READY:
			{
				CSyncLockWaitScope scope(m_SessionMap.GetLock());
				m_SessionMap.Remove((DataRef)krSocket);
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
bool CLoginServer::OnUdpClose(KeyRef krSocket, LLong)
{
	if (krSocket == m_krUDPService){
		m_krUDPService = nullptr;
		DEV_INFO(TF("[��½������]UDP������Ϸ�������ر�"));
	}
	return true;
}
//--------------------------------------
bool CLoginServer::CheckSessionTimeout(void)
{
	TIMEOUT_MAP Timeout;
	{
		CSyncLockWaitScope scope(m_SessionMap.GetLock());
		PINDEX index = m_SessionMap.First();
		while (index != nullptr) {
			SESSION_MAP::DATA_MAP_PAIR* pPair = m_SessionMap.Next(index);
			switch (pPair->m_V.nStatus) {
			case SESSION_DATA::SESSION_STATUS_LINK:
			case SESSION_DATA::SESSION_STATUS_SELECT:
			case SESSION_DATA::SESSION_STATUS_GAME:
			case SESSION_DATA::SESSION_STATUS_UNLINK:
				{
					if (CPlatform::GetRunningTime() > (pPair->m_V.llTimeout + SESSION_DATA::SESSION_TIME_OUT_MIN)) {
						Timeout.Add((KeyRef)pPair->m_K, LOGIN_ERROR_ACK_TIMEOUT);
					}
				}
				break;
			case SESSION_DATA::SESSION_STATUS_SELECTACK:
			case SESSION_DATA::SESSION_STATUS_GAMEACK:
				{
					if (CPlatform::GetRunningTime() > (pPair->m_V.llTimeout + SESSION_DATA::SESSION_TIME_OUT_MAX)) {
						Timeout.Add((KeyRef)pPair->m_K, LOGIN_ERROR_GAME_PLAY);
					}
				}
				break;
			case SESSION_DATA::SESSION_STATUS_NONE:
			case SESSION_DATA::SESSION_STATUS_LINKACK:
			default:
				{
					if (CPlatform::GetRunningTime() > (pPair->m_V.llTimeout + SESSION_DATA::SESSION_TIME_LIMIT)) {
						Timeout.Add((KeyRef)pPair->m_K, LOGIN_ERROR_LINK_IDLE);
					}
				}
			}
		}
	}
	CPAKAck UnlinkAck(PAK_EVENT_LOGIN_UNLINKACK, (UInt)PAK_TYPE_LOGIN);
	UnlinkAck.AdjustSize();
	for (PINDEX index = Timeout.GetFirst(); index != nullptr; ) {
		TIMEOUT_MAP_PAIR* pPair = Timeout.GetNext(index);
		UnlinkAck.SetAck(pPair->m_V);
		m_NetworkPtr->Send(pPair->m_K, UnlinkAck, SEND_CLOSE);
	}
	return true;
}
// ��ʱ����������ķ����������Ӷ����Ƿ���Ч
bool CLoginServer::CheckConnectCenterServer(void)
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
// ��ʱ������ӵ�½DB�����������Ӷ����Ƿ���Ч
bool CLoginServer::CheckConnectLoginDBServer(void)
{
	if (m_bLoginDBCnnted == false) {
		if (StartConnectLoginDBServer() == false) {
			return false;
		}
	}
	if (m_bLoginDBLinked == false) {
		LinkLoginDBServer();
	}
	return true;
}
// ��ʱ���UDP������Ϸ�����������Ӷ����Ƿ���Ч
bool CLoginServer::CheckUDPService(void)
{
	if (m_krUDPService == nullptr) {
		return StartUDPService();
	}
	return true;
}
// ��ʱ�������ͻ��˵����Ӷ����Ƿ���Ч
bool CLoginServer::CheckTCPService(void)
{
	if (m_krTCPService == nullptr) {
		return StartTCPService();
	}
	return true;
}
//--------------------------------------
// ͬ����������Ϣ������
bool CLoginServer::SyncServerInfo(void)
{
	if (m_ServerInfo.usStatus == STATUSU_SYNC) {
		CLoginUpdate Update;
		Update.SetServerData(m_ServerInfo);
		Update.AdjustSize();
		if (m_pShareCenterSvr != nullptr) {
			m_pShareCenterSvr->OnShareRoutine(PAK_EVENT_UPDATE, Update, reinterpret_cast<LLong>(this));
		}
		else {
			m_NetworkPtr->Send(m_krConnectCenter, Update);
		}
		if (m_pShareLoginDBSvr != nullptr) {
			m_pShareLoginDBSvr->OnShareRoutine(PAK_EVENT_UPDATE, Update, reinterpret_cast<LLong>(this));
		}
		else {
			m_NetworkPtr->Send(m_krConnectLoginDB, Update);
		}
		m_ServerInfo.usStatus = STATUSU_OKAY;
		return m_pUIHandler->OnHandle(PAK_EVENT_SYNC, reinterpret_cast<uintptr_t>(&m_ServerInfo), DATA_INDEX_LOGIN);
	}
	else { // ��������
		if (m_pShareCenterSvr == nullptr) { 
			CPAKSimple<PAK_EVENT_LIVE, PAK_TYPE_LOGIN> Live;
			Live.AdjustSize();
			m_NetworkPtr->Send(m_krConnectCenter, Live);
		}
		if (m_pShareLoginDBSvr == nullptr) { 
			CPAKSimple<PAK_EVENT_LIVE, PAK_TYPE_LOGIN> Live;
			Live.AdjustSize();
			m_NetworkPtr->Send(m_krConnectLoginDB, Live);
		}
	}
	return true;
}
// ͬ����Ϸ����������������鲢ͬ��������
bool CLoginServer::SyncGameServerInfo(CStream& Stream)
{
	LLong llPos = (LLong)Stream.Tell();
	m_pUIHandler->OnHandle(PAK_EVENT_SYNC, Stream, DATA_INDEX_GAME);
	m_pUIHandler->OnHandle(PAK_EVENT_SYNC, reinterpret_cast<uintptr_t>(&m_GameInfo), DATA_INDEX_GAME);

	Stream.Seek(llPos); // ��ת�����ݿ�ʼ�ĵط�

	CSyncLockWaitScope scope(m_GameSvrMap.GetLock());
	return m_GameSvrMap.Serialize(Stream);
}
// ͬ����Ϸ��������Ϣ���ͻ���
bool CLoginServer::SyncGameServerInfo(void)
{
	// 3. TODO!!!��ôû�п������ݳ��������󱣴����޵�����, ����ֻ��ʹ��jumbo buffer����, 
	//    �������������ֶ��buffer/jumbo buffer����, Ĭ��jumbo buffer������෢��ͬ����120����������ȫ����Ϣ,
	//    Ŀǰͬ������ֻ�������ķ�����ͬ����������Ϣ��ѡ��͵�½ʱ����, ���е�½������ͬ����Ϸ��������Ϣ���ͻ���
	if (m_GameInfo.usStatus == STATUSU_SYNC) {
		CStreamScopePtr StreamPtr;
		m_NetworkPtr->AllocJumboBuffer(StreamPtr);
		if (StreamPtr != nullptr) {
			CLoginSync Sync;
			Sync.SetServerData(m_GameInfo);
			Sync.Serialize(*StreamPtr);

			bool bRet = false;
			{ CSyncLockWaitScope scope(m_GameSvrMap.GetLock()); bRet = m_GameSvrMap.Serialize(*StreamPtr, STATUSU_SYNCXLINK|STATUSU_PING); }
			if (bRet) {
				if (m_NetworkPtr->Send(nullptr, *StreamPtr, SEND_BROADCAST_AS, PAK_TYPE_CLIENT_LOGIN)) {
					CSyncLockWaitScope scope(m_GameSvrMap.GetLock());
					m_GameSvrMap.Update(); // ͬ�����, ����״̬���������������
					m_GameInfo.usStatus = STATUSU_OKAY;
				}
			}
			else { // û�и�������
				DEV_INFO(TF("[��½������]ͬ����Ϸ��������Ϣ���ͻ��˷���û��ͬ������2"));
			}
		}
		return false;
	}
	else {
		DEV_INFO(TF("[��½������]ͬ����Ϸ��������Ϣ���ͻ��˷���û��ͬ������1"));
	}
	return true;
}
//--------------------------------------
// ���͵�½���������ͻ���
void CLoginServer::GameLinkAck(CPAKSessionAck* pAck)
{
	CSyncLockWaitScope scope(m_SessionMap.GetLock());
	SESSION_MAP::DATA_MAP_PAIR* pPair = m_SessionMap.Find((DataRef)pAck->GetSessionId());
	if ((pPair != nullptr) && (pPair->m_V.nStatus == SESSION_DATA::SESSION_STATUS_LINK)) {
		if (pAck->GetAck() == DATAD_OKAY) {
			pAck->SetType(PAK_TYPE_LOGIN);
			pAck->SetSessionId(reinterpret_cast<LLong>(pPair));
			// ������Ϸ��������Ϣ
			// 3. TODO!!!��ôû�п������ݳ��������󱣴����޵�����, ����ֻ��ʹ��jumbo buffer����, 
			//    �������������ֶ��buffer/jumbo buffer����, Ĭ��jumbo buffer������෢��ͬ����120����������ȫ����Ϣ,
			//    Ŀǰͬ������ֻ�������ķ�����ͬ����������Ϣ��ѡ��͵�½ʱ����, ���е�½������ͬ����Ϸ��������Ϣ���ͻ���
			CStreamScopePtr StreamPtr;
			m_NetworkPtr->AllocJumboBuffer(StreamPtr);
			if (StreamPtr != nullptr) {
				pAck->Serialize(*StreamPtr);
				StreamPtr->Write(&m_GameInfo, sizeof(SERVER_INFO));

				{ CSyncLockWaitScope gamescope(m_GameSvrMap.GetLock());  m_GameSvrMap.Serialize(*StreamPtr, STATUSU_OKAYSYNC|STATUSU_PING); }

				m_NetworkPtr->Send((KeyRef)pPair->m_K, *StreamPtr);
				m_NetworkPtr->SetAttr((KeyRef)pPair->m_K, PAK_TYPE_CLIENT_LOGIN);

				CPAKLoginLinkAck* pLinkAck = static_cast<CPAKLoginLinkAck*>(pAck);
				pPair->m_V.llUserId  = pLinkAck->GetUserId();
				pPair->m_V.llTimeout = CPlatform::GetRunningTime();
				pPair->m_V.llOnline  = CPlatform::GetRunningTime();
				pPair->m_V.nGameId   = pLinkAck->GetGameId();
				pPair->m_V.nStatus   = SESSION_DATA::SESSION_STATUS_LINKACK;
				pPair->m_V.nCount    = 0;
				m_NetworkPtr->GetAddr((KeyRef)pPair->m_K, pPair->m_V.CltLast);
			}
			else {
				DEV_WARN(TF("[��½������]�����������ڴ���PAK_EVENT_LOGIN_LINKACKʱ����ʧ��"));
				LOGV_WARN(m_FileLog, TF("[��½������]�����������ڴ���PAK_EVENT_LOGIN_LINKACKʱ����ʧ��"));
			}
		}
		else {
			CPAKAck LinkAck(PAK_EVENT_LOGIN_LINKACK, (UInt)PAK_TYPE_LOGIN);
			LinkAck.AdjustSize();

			pPair->m_V.nCount += 1;
			if (pPair->m_V.nCount > SESSION_DATA::SESSION_TIME_LINK_MAX) {
				LinkAck.SetAck(LOGIN_ERROR_LINK_LIMIT);
				// ���붨ʱ������
			}
			else {
				LinkAck.SetAck(pAck->GetAck());
			}
			if (LinkAck.GetAck() == LOGIN_ERROR_LINK_LIMIT) {
				m_NetworkPtr->Send((KeyRef)pPair->m_K, LinkAck, SEND_CLOSE);
			}
			else {
				pPair->m_V.llTimeout = CPlatform::GetRunningTime();
				pPair->m_V.nStatus   = SESSION_DATA::SESSION_STATUS_NONE;

				m_NetworkPtr->Send((KeyRef)pPair->m_K, LinkAck);
			}
		}
	}
	else {
		LOGV_WARN(m_FileLog, TF("[��½������]��½DB��������������󷵻�ʱ�޷����ҵ�SessionId[%llx]���ݻ���Session״̬��ƥ��"), pAck->GetSessionId());
	}
}
// ���͵ǳ����������ͻ���
void CLoginServer::GameUnlinkAck(CPAKSessionAck* pAck)
{
	assert(pAck->GetSessionId() > 0); // �Ͽ��ͻ��˵�SessionIdΪ0
	CSyncLockWaitScope scope(m_SessionMap.GetLock());
	SESSION_MAP::DATA_MAP_PAIR* pPair = m_SessionMap.Find((DataRef)pAck->GetSessionId());
	if ((pPair != nullptr) && (pPair->m_V.nStatus == SESSION_DATA::SESSION_STATUS_UNLINK)) {
		Int nFlag = SEND_NORMAL;
		if (pAck->GetAck() == DATAD_OKAY) {
			pPair->m_V.Reset();
			m_NetworkPtr->SetAttr((KeyRef)pPair->m_K, PAK_TYPE_CLIENT_READY);
		}
		else {
			nFlag = SEND_CLOSE;
		}
		CPAKAck UnlinkAck(PAK_EVENT_LOGIN_UNLINKACK, (UInt)PAK_TYPE_LOGIN);
		UnlinkAck.SetAck(pAck->GetAck());
		UnlinkAck.AdjustSize();
		m_NetworkPtr->Send((KeyRef)pPair->m_K, UnlinkAck, nFlag);
	}
	else {
		LOGV_WARN(m_FileLog, TF("[��½������]��½DB��������������󷵻�ʱ�޷����ҵ�SessionId[%llx]���ݻ���Session״̬��ƥ��"), pAck->GetSessionId());
	}
}
// ѡ����Ϸ�������ظ���
void CLoginServer::SelectGameAck(CPAKSessionAck* pAck)
{
	DEV_INFO(TF("[��½������]�յ�%d���ͷ�������ѡ����Ϸ�������ظ���%d[%llx]"), pAck->GetType(), pAck->GetAck(), pAck->GetSessionId());
	CSyncLockWaitScope scope(m_SessionMap.GetLock());
	SESSION_MAP::DATA_MAP_PAIR* pPair = m_SessionMap.Find((DataRef)pAck->GetSessionId());
	if ((pPair != nullptr) && (pPair->m_V.nStatus == SESSION_DATA::SESSION_STATUS_SELECT)) {
		pPair->m_V.llTimeout = CPlatform::GetRunningTime();
		if (pAck->GetAck() == DATAD_OKAY) {
			pAck->SetType(PAK_TYPE_LOGIN);
			pAck->SetSessionId(reinterpret_cast<LLong>(pPair));// TODO!!!���Ը��ͻ���һ��Ψһ��id����У��

			pPair->m_V.nStatus = SESSION_DATA::SESSION_STATUS_SELECTACK;

			LLong llTemp = (LLong)pPair->m_K;
			/////////////////////////////////////////////////////////////////
			// ����AuthCode(��ͻ���Ψһ����Ϣ�й�, �������IP)
			if (pPair->m_V.CltLast.usAttr & ATTR_IPV6) {
				llTemp ^= pPair->m_V.CltLast.Addr.ullAddr[0];
				llTemp ^= pPair->m_V.CltLast.Addr.ullAddr[1];
				DEV_INFO(TF("[��½������]����Keyǰ��Ϊ[%llx-%llx-%llx+%llx]"), pPair->m_K, llTemp, pPair->m_V.CltLast.Addr.ullAddr[0], pPair->m_V.CltLast.Addr.ullAddr[1]);
			}
			else {
				llTemp ^= pPair->m_V.CltLast.Addr.uAddr[0];
				DEV_INFO(TF("[��½������]����Keyǰ��Ϊ[%llx-%llx-%x]"), pPair->m_K, llTemp, pPair->m_V.CltLast.Addr.uAddr[0]);
			}
			/////////////////////////////////////////////////////////////////
			(static_cast<CPAKLoginSelectGame*>(pAck))->SetAuthCode(llTemp);
			m_NetworkPtr->Send((KeyRef)pPair->m_K, (*pAck));
		}
		else {
			pPair->m_V.nStatus = SESSION_DATA::SESSION_STATUS_LINKACK;

			CPAKAck SelectGameAck(PAK_EVENT_LOGIN_SELECT_GAME, (UInt)PAK_TYPE_LOGIN);
			SelectGameAck.SetAck(pAck->GetAck());
			SelectGameAck.AdjustSize();
			m_NetworkPtr->Send((KeyRef)pPair->m_K, SelectGameAck);
		}
	}
	else {
		LOGV_WARN(m_FileLog, TF("[��½������]��Ϸ�����������ѡ�����󷵻�ʱ�޷����ҵ�SessionId[%llx]���ݻ���Session״̬��ƥ��"), pAck->GetSessionId());
	}
}
// ��¼��Ϸ�������ظ���
void CLoginServer::LinkGameAck(CPAKLoginLinkGame* pLink)
{
	DEV_INFO(TF("[��½������]�յ�%d�������ĵ�½1У��ظ���%d[%llx-%llx]"), pLink->GetType(), pLink->GetAck(), pLink->GetSessionId(), pLink->GetAuthCode());
	CSyncLockWaitScope scope(m_SessionMap.GetLock());
	SESSION_MAP::DATA_MAP_PAIR* pPair = m_SessionMap.Find((DataRef)pLink->GetSessionId());
	if ((pPair != nullptr) && (pPair->m_V.nStatus == SESSION_DATA::SESSION_STATUS_SELECTACK)) {

		pPair->m_V.nStatus   = SESSION_DATA::SESSION_STATUS_GAME;
		pPair->m_V.llTimeout = CPlatform::GetRunningTime();

		pLink->SetType(PAK_TYPE_LOGIN);
		pLink->SetSessionId(reinterpret_cast<LLong>(pPair));
		m_NetworkPtr->Send((KeyRef)pPair->m_K, (*pLink));
	}
	else {
		LOGV_WARN(m_FileLog, TF("[��½������]��Ϸ����������ĵ�½���󷵻�ʱ�޷����ҵ�SessionId[%llx]���ݻ���Session״̬��ƥ��"), pLink->GetSessionId());
	}
}
// �û�����/�˳���Ϸ�Ŷӻ�����Ϸ����
void CLoginServer::PlayGameAck(CPAKSessionAck* pAck)
{
	DEV_INFO(TF("[��½������]�յ�%d�������Ľ�����Ϸ�ظ���%d[%llx]"), pAck->GetType(), pAck->GetAck(), pAck->GetSessionId());
	CSyncLockWaitScope scope(m_SessionMap.GetLock());
	SESSION_MAP::DATA_MAP_PAIR* pPair = m_SessionMap.Find((DataRef)pAck->GetSessionId());
	if ((pPair != nullptr) && (pPair->m_V.nStatus == SESSION_DATA::SESSION_STATUS_GAME)) {
		pPair->m_V.llTimeout = CPlatform::GetRunningTime();
		if (pAck->GetAck() == DATAD_OKAY) {
			pPair->m_V.nStatus  = SESSION_DATA::SESSION_STATUS_GAMEACK;
			
		}
		else {
			pPair->m_V.nCount += 1;
			if (pPair->m_V.nCount > SESSION_DATA::SESSION_TIME_LINK_MAX) {
				CPAKAck UnlinkAck(PAK_EVENT_LOGIN_UNLINKACK, (UInt)PAK_TYPE_LOGIN);
				UnlinkAck.SetAck(LOGIN_ERROR_SELECT_LIMIT);
				UnlinkAck.AdjustSize();
				m_NetworkPtr->Send((KeyRef)pPair->m_K, UnlinkAck, SEND_CLOSE);
			}
			else { 
				pPair->m_V.nStatus  = SESSION_DATA::SESSION_STATUS_LINKACK;
			}
		}
	}
	else {
		LOGV_WARN(m_FileLog, TF("[��½������]��Ϸ������������û�����/�˳���Ϸ�Ŷӻ�����Ϸ���緵��ʱ�޷����ҵ�SessionId[%llx]���ݻ���Session״̬��ƥ��"), pAck->GetSessionId());
	}
}

