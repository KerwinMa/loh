//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Source File : ZoneServerImp.cpp                            //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update : 2015-11-25     version 0.0.0.5                    //
//   Detail : ��ͼ������ʵ��                                     //
//                                                              //
//////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZoneServerImp.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoneServer
UInt CZoneServer::Command(PCXStr pszCMD, uintptr_t utParam)
{
	// �������ļ������������Ƹ�����ز���
	UNREFERENCED_PARAMETER(pszCMD);
	UNREFERENCED_PARAMETER(utParam);
	return (UInt)RET_OKAY;
}

UInt CZoneServer::Update(void)
{
	CheckConnectGameServer();
	SyncServerInfo();
	return 0;
}
//--------------------------------------
UInt CZoneServer::Init(CEventHandler& EventHandlerRef)
{
	if (m_nStatus == STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[��ͼ������]��ͼ�����ʼ����ʼ!"));
		m_pUIHandler = &EventHandlerRef;

		if ((InitLoadShare() == false) ||
			(InitLoadConfig() == false)) {
			return (UInt)RET_FAIL;
		}

		m_nStatus = STATUSC_INIT;
		LOG_INFO(m_FileLog, TF("[��ͼ������]��ͼ�����ʼ�����!"));
		return (UInt)RET_OKAY;
	}
	return (UInt)RET_FAIL;
}

bool CZoneServer::InitLoadShare(void)
{
	assert(m_pConfig == nullptr);
	m_pUIHandler->OnHandle(CServerConfig::CFG_CONFIG_PTR, reinterpret_cast<uintptr_t>(&m_pConfig), DATA_INDEX_ZONE);
	if (m_pConfig == nullptr) {
		LOG_ERROR(m_FileLog, TF("[��ͼ������]�ӽ���ص��ӿڻ�ȡ���ö���ָ����Ч"));
		return false;
	}
	LOG_INFO(m_FileLog, TF("[��ͼ������]�ӽ���ص��ӿڻ�ȡ���ö���ָ�����!"));
	m_pConfig->Dump(m_FileLog); // ������Ϣ�������־�ļ�
	// �Ӷ�̬�������ȡ���������ָ��
	assert(m_NetworkPtr == nullptr);
	CKeyValue::VALUE xValue = { 0 };
	m_pConfig->GetKeyValue()->GetValue(CServerConfig::NetworkPtr, xValue);
	if (xValue.pValue == nullptr) {
		LOG_ERROR(m_FileLog, TF("[��ͼ������]�Ӷ�̬�������ȡ���������ָ����Ч"));
		return false;
	}
	LOG_INFO(m_FileLog, TF("[��ͼ������]�Ӷ�̬�������ȡ���������ָ�����!"));
	m_NetworkPtr = *(reinterpret_cast<CNetworkPtr*>(xValue.pValue));
	return true;
}

bool CZoneServer::InitLoadConfig(void)
{
	assert(m_krConnectGame == nullptr);
	assert(m_pShareGameSvr == nullptr);

	m_ServerInfo.usId   = m_pConfig->GetServerId(CServerConfig::CFG_DEFAULT_ZONE);
	m_ServerInfo.usIncr = m_pConfig->GetServerIncr(CServerConfig::CFG_DEFAULT_ZONE);
	LOGV_INFO(m_FileLog, TF("[��ͼ������]�����ö����ȡ������Id=%d, Incr=%d!"), m_ServerInfo.usId, m_ServerInfo.usIncr);

	assert(m_ZoneRoutine == nullptr);
	m_ZoneRoutine = (ICommonRoutine*)CRTTI::CreateByName(TF("CZoneRoutine"));
	if (m_ZoneRoutine != nullptr) {
		return (m_ZoneRoutine->Init() == (UInt)RET_OKAY);
	}
	LOG_ERROR(m_FileLog, TF("[��ͼ������]�������������ʧ��"));
	return false;
}
//--------------------------------------
void CZoneServer::Exit(void)
{
	if (m_nStatus != STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[��ͼ������]��ͼ�����˳�����ʼ!"));
		m_pUIHandler = nullptr;

		ExitUnloadConfig();
		ExitUnloadShare();

		m_nStatus = STATUSC_NONE;
		LOG_INFO(m_FileLog, TF("[��ͼ������]��ͼ�����˳��������!"));
	}
}

void CZoneServer::ExitUnloadShare(void)
{
	m_pConfig    = nullptr;
	m_NetworkPtr = nullptr;
}

void CZoneServer::ExitUnloadConfig(void)
{
	assert(m_krConnectGame == nullptr);
	assert(m_pShareGameSvr == nullptr);

	m_ZoneRoutine->Exit();
	m_ZoneRoutine = nullptr;
}
//--------------------------------------
bool CZoneServer::Start(void)
{
	if (m_nStatus == STATUSC_INIT) {
		LOG_INFO(m_FileLog, TF("[��ͼ������]��ͼ����������ʼ!"));

		if (StartConnectGameServer() == false) {
			return false;
		}
		m_ZoneRoutine->Start(this);

		m_nStatus = STATUSC_RUN;
		LOG_INFO(m_FileLog, TF("[��ͼ������]��ͼ�����������!"));
	}
	return true;
}

bool CZoneServer::StartConnectGameServer(void)
{
	// ��ͼ����Ϸ�ڲ�ͬ����,  ��Ҫ����������Ϸ������
	if ((m_pConfig->GetLoadServers() & CServerConfig::CFG_DEFAULT_GAME) == 0) {
		if (m_krConnectGame == nullptr) {
			UShort     usPort = 0;
			CStringKey strAddr;
			m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_ZONE, 0, strAddr, usPort);
			m_krConnectGame = m_NetworkPtr->Create(*this, usPort, *strAddr);
		}
		if (m_krConnectGame != nullptr) {
			LOG_INFO(m_FileLog, TF("[��ͼ������]��ͼ����������Ϸ�������ڲ�ͬ����, ����������Ϸ����������ɹ�"));
			if (m_bGameCnnted == false) {
				UShort      usPort = 0;
				CStringKey  strAddr;
				m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_GAME, CServerConfig::CFG_DEFAULT_ZONE, strAddr, usPort);
				if (m_NetworkPtr->Connect(m_krConnectGame, usPort, *strAddr) == false) {
					LOGV_ERROR(m_FileLog, TF("[��ͼ������]������Ϸ������[%s]:%d����ʧ��"), *strAddr, usPort);
					return false;
				}
				LOGV_INFO(m_FileLog, TF("[��ͼ������]������Ϸ������[%s]:%d�������"), *strAddr, usPort);
			}
		}
		else {
			LOG_ERROR(m_FileLog, TF("[��ͼ������]��ͼ����������Ϸ�������ڲ�ͬ����, ����������Ϸ����������ʧ��"));
			return false;
		}
	}
	else if (m_bGameCnnted == false) {
		assert(m_pShareGameSvr == nullptr);
		CKeyValue::VALUE xValue = { 0 };
		m_pConfig->GetKeyValue()->GetValue(CServerConfig::GameServer, xValue);
		if (xValue.pValue == nullptr) {
			LOG_ERROR(m_FileLog, TF("[��ͼ������]�Ӷ�̬�������ȡͬ������Ϸ����������ָ����Ч"));
			return false;
		}
		m_pShareGameSvr = reinterpret_cast<ICommonServer*>(xValue.pValue);
		m_bGameCnnted   = true;

		LOG_INFO(m_FileLog, TF("[��ͼ������]ͬ����ֱ������Ϸ������ע��"));
		m_ServerInfo.usStatus = STATUSU_LINK;

		CZoneLink Link;
		Link.SetServerData(m_ServerInfo);
		Link.AdjustSize();

		UInt uIds[2] = { 1, 100 };

		CBufReadStream brs((PByte)uIds, sizeof(uIds)); // ��������
		Link.SetStream(brs);

		m_bGameLinked = m_pShareGameSvr->OnShareRoutine(PAK_EVENT_LINK, Link, reinterpret_cast<uintptr_t>(this));
		m_pUIHandler->OnHandle(PAK_EVENT_LINK, 0, DATA_INDEX_GAME);
		return m_bGameLinked;
	}
	return true;
}
//--------------------------------------
bool CZoneServer::Pause(bool bPause)
{
	if (bPause && (m_nStatus == STATUSC_RUN)) {
		m_nStatus = STATUSC_PAUSE; 
		LOG_INFO(m_FileLog, TF("[��ͼ������]��ͣ����"));
		m_ZoneRoutine->Pause();
		return true;
	}
	else if ((bPause == false) && (m_nStatus == STATUSC_PAUSE)) {
		m_nStatus = STATUSC_RUN;   
		LOG_INFO(m_FileLog, TF("[��ͼ������]�������"));
		m_ZoneRoutine->Pause(false);
		return true;
	}
	return false;
}
//--------------------------------------
void CZoneServer::Stop(void)
{
	if (m_nStatus > STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[��ͼ������]��ͼ����ֹͣ��ʼ!"));

		m_ZoneRoutine->Stop();

		StopConnectGameServer();

		m_ServerInfo.Zero();

		m_nStatus = STATUSC_INIT;
		LOG_INFO(m_FileLog, TF("[��ͼ������]��ͼ����ֹͣ���!"));
	}
}

void CZoneServer::StopConnectGameServer(void)
{
	if (m_krConnectGame != nullptr) {
		m_NetworkPtr->Destroy(m_krConnectGame, false);
		m_krConnectGame = nullptr;
		LOG_INFO(m_FileLog, TF("[��ͼ������]����������Ϸ�������ɹ�"));
	}
	m_bGameCnnted   = false;
	m_bGameLinked   = false;
	m_pShareGameSvr = nullptr;
}
//--------------------------------------
bool CZoneServer::OnShareRoutine(Int nEvent, CEventBase& EventRef, LLong llParam, CEventQueue::EVENT_TYPE eType)
{
	assert((m_pConfig->GetLoadServers() & CServerConfig::CFG_DEFAULT_GAME) != 0);
	switch (nEvent) {
	// game
	case 0:
		{
			m_ZoneRoutine->Add(nEvent, EventRef, llParam, eType);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��ͼ������]ͬ���̷������޷�ʶ������������[event=%d]"), nEvent);
			assert(0);
		}
	}
	return true;
}
//--------------------------------------
bool CZoneServer::OnTcpDispatch(const PacketPtr& PktPtr, PTCP_PARAM pTcp)
{
	// PTCP_PARAM->pCache == nullptr, �ϲ���Ա���ӵײ�����Ļ�������
	// PTCP_PARAM->pCache, PTCP_PARAM->index�ǵײ�����Ļ������ݵ���ʼ��ַ��������
	// ����ϲ㱣�滺��, ����PTCP_PARAM->pDataΪnullptr֪ͨ����ײ�
	// �ͷŻ���ʹ��MObject::sMCFree(index, pCache)
	bool bRet = true;
	if (PktPtr->GetEvent() != PAK_EVENT_LIVEACK) {
		assert(PktPtr->GetType() == PAK_TYPE_GAME);
		switch (PktPtr->GetEvent()) {
		case PAK_EVENT_LINKACK:
			{
				m_bGameLinked = true;

				NET_ADDR NetAddr;
				m_NetworkPtr->GetAddr(pTcp->krSocket, NetAddr, false);
				m_pUIHandler->OnHandle(PAK_EVENT_LINK, reinterpret_cast<uintptr_t>(&NetAddr), DATA_INDEX_GAME);
				LOG_INFO(m_FileLog, TF("[��ͼ������]�յ���Ϸ������ע��ظ���"));
			}
			break;
		case PAK_EVENT_UPDATEACK:
			{
				DEV_INFO(TF("[��ͼ������]�յ���Ϸ���������»ظ���"));
			}
			break;
		case PAK_EVENT_UNLINKACK:
			{
				m_bGameLinked = false;
				LOG_INFO(m_FileLog, TF("[��ͼ������]�յ���Ϸ������ע���ظ���"));
			}
			break;
		default:
			{
				LOGV_WARN(m_FileLog, TF("[��ͼ������]%p�޷�ʶ�����Ϸ���������������[event=%d]"), pTcp->krSocket, PktPtr->GetEvent());
			}
		}
	}
	return bRet;
}
//--------------------------------------
bool CZoneServer::OnTcpConnect(UInt uError, KeyRef krConnect)
{
	assert(krConnect == m_krConnectGame);
	m_bGameCnnted = (uError == 0);
	if (m_bGameCnnted) {
		LinkGameServer();
	}
	else {
		DEV_INFO(TF("[��ͼ������]������Ϸ������ʧ��%X!"), uError);
		LOGV_WARN(m_FileLog, TF("[��ͼ������]������Ϸ������ʧ��%X!"), uError);
	}
	return true;
}

void CZoneServer::LinkGameServer(void)
{
	if (m_bGameCnnted && (m_bGameLinked == false)) {
		m_ServerInfo.usStatus = STATUSU_LINK;

		CZoneLink Link;
		Link.SetServerData(m_ServerInfo);
		Link.AdjustSize();
		m_NetworkPtr->Send(m_krConnectGame, Link);
	}
}

void CZoneServer::UnlinkGameServer(void)
{
	if (m_bGameCnnted && m_bGameLinked) {
		m_bGameLinked = false;
		CPAKSimple<PAK_EVENT_UNLINK, PAK_TYPE_ZONE> Unlink;
		Unlink.AdjustSize();
		m_NetworkPtr->Send(m_krConnectGame, Unlink);
	}
}
//--------------------------------------
bool CZoneServer::OnTcpClose(KeyRef krSocket, LLong)
{
	if (krSocket == m_krConnectGame) {
		m_krConnectGame = nullptr;
		m_bGameCnnted   = false;
		m_bGameLinked   = false;
		m_pUIHandler->OnHandle(PAK_EVENT_UNLINK, 0, DATA_INDEX_GAME);
		DEV_INFO(TF("[��ͼ������]������Ϸ�������Ͽ�"));
		LOG_INFO(m_FileLog, TF("[��ͼ������]������Ϸ�������Ͽ�"));
	}
	return true;
}
//--------------------------------------
bool CZoneServer::CheckConnectGameServer(void)
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
//--------------------------------------
bool CZoneServer::SyncServerInfo(void)
{
	m_ZoneRoutine->Update();
	if (m_ServerInfo.usStatus == STATUSU_SYNC) {
		CZoneUpdate Update;
		Update.SetServerData(m_ServerInfo);
		Update.AdjustSize();
		if (m_pShareGameSvr != nullptr) {
			m_pShareGameSvr->OnShareRoutine(PAK_EVENT_UPDATE, Update, reinterpret_cast<LLong>(this));
		}
		else {
			m_NetworkPtr->Send(m_krConnectGame, Update);
		}
		m_ServerInfo.usStatus = STATUSU_OKAY;
		return m_pUIHandler->OnHandle(PAK_EVENT_SYNC, reinterpret_cast<uintptr_t>(&m_ServerInfo), DATA_INDEX_ZONE);
	}
	else if (m_pShareGameSvr == nullptr) { // ��������
		CPAKSimple<PAK_EVENT_LIVE, PAK_TYPE_ZONE> Live;
		Live.AdjustSize();
		m_NetworkPtr->Send(m_krConnectGame, Live);
	}
	return true;
}




