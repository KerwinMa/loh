//////////////////////////////////////////////////////////////////
//   The Legend of Heros                                        //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Source File : GameDBServerImp.cpp                          //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ��ϷDB����������ʵ��                               //
//                                                              //
//////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameDBServerImp.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGameDBServer
UInt CGameDBServer::Command(PCXStr pszCMD, uintptr_t utParam)
{
	// �������ļ������������Ƹ�����ز���
	UNREFERENCED_PARAMETER(pszCMD);
	UNREFERENCED_PARAMETER(utParam);
	return (UInt)RET_OKAY;
}

UInt CGameDBServer::Update(void)
{
	CheckListenGameServer();
	CheckListenGateServer();
	SyncServerInfo();
	return 0;
}
//--------------------------------------
UInt CGameDBServer::Init(CEventHandler& EventHandlerRef)
{
	if (m_nStatus == STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[��ϷDB������]��ϷDB�����ʼ����ʼ!"));
		m_pUIHandler = &EventHandlerRef;

		if ((InitLoadShare() == false) ||
			(InitLoadConfig() == false)) {
			return (UInt)RET_FAIL;
		}

		m_nStatus = STATUSC_INIT;
		LOG_INFO(m_FileLog, TF("[��ϷDB������]��ϷDB�����ʼ�����!"));
		return (UInt)RET_OKAY;
	}
	return (UInt)RET_FAIL;
}
// ��ȡ��������ö�����������
bool CGameDBServer::InitLoadShare(void)
{
	assert(m_pConfig == nullptr);
	m_pUIHandler->OnHandle(CServerConfig::CFG_DEFAULT_CONFIG, reinterpret_cast<uintptr_t>(&m_pConfig), DATA_INDEX_GAMEDB);
	if (m_pConfig == nullptr) {
		LOG_ERROR(m_FileLog, TF("[��ϷDB������]�ӽ���ص��ӿڻ�ȡ���ö���ָ����Ч"));
		return false;
	}
	LOG_INFO(m_FileLog, TF("[��ϷDB������]�ӽ���ص��ӿڻ�ȡ���ö���ָ�����!"));
	m_pConfig->Dump(m_FileLog); // ������Ϣ�������־�ļ�
	// �Ӷ�̬�������ȡ���������ָ��
	assert(m_NetworkPtr == nullptr);
	CKeyValue::VALUE xValue = { 0 };
	m_pConfig->GetKeyValue()->GetValue(CServerConfig::NetworkPtr, xValue);
	if (xValue.pValue == nullptr) {
		LOG_ERROR(m_FileLog, TF("[��ϷDB������]�Ӷ�̬�������ȡ���������ָ����Ч"));
		return false;
	}
	LOG_INFO(m_FileLog, TF("[��ϷDB������]�Ӷ�̬�������ȡ���������ָ�����!"));
	m_NetworkPtr = *(reinterpret_cast<CNetworkPtr*>(xValue.pValue));
	// ������ϷDB������ָ�뵽��̬������
	LOGV_INFO(m_FileLog, TF("[��ϷDB������]������ϷDB������ָ�뵽��̬���������[key=%s]!"), CServerConfig::GameDBServer);
	m_pConfig->GetKeyValue()->AddItem(CServerConfig::GameDBServer, this);
	return true;
}
// ��ʼ������
bool CGameDBServer::InitLoadConfig(void)
{
	assert(m_krListenGame == nullptr);
	assert(m_krListenGate == nullptr);
	assert(m_pShareGameSvr == nullptr);
	assert(m_pShareGateSvr == nullptr);

	assert(m_GameSvrMap.Size() == 0);
	assert(m_GateSvrMap.Size() == 0);

	m_ServerInfo[INFOI_GAMEDB].usId   = m_pConfig->GetServerId(CServerConfig::CFG_DEFAULT_GAMEDB);
	m_ServerInfo[INFOI_GAMEDB].usIncr = m_pConfig->GetServerIncr(CServerConfig::CFG_DEFAULT_GAMEDB);
	LOGV_INFO(m_FileLog, TF("[��ϷDB������]�����ö����ȡ������Id=%d, Incr=%d!"), m_ServerInfo[INFOI_GAMEDB].usId, m_ServerInfo[INFOI_GAMEDB].usIncr);
	assert(m_GameDBRoutine == nullptr);
	m_GameDBRoutine = (ICommonRoutine*)CRTTI::CreateByName(TF("CGameDBRoutine"));
	if (m_GameDBRoutine != nullptr) {
		return (m_GameDBRoutine->Init() == (UInt)RET_OKAY);
	}
	LOG_ERROR(m_FileLog, TF("[��ϷDB������]�������������ʧ��"));
	return false;
}
//--------------------------------------
void CGameDBServer::Exit(void)
{
	if (m_nStatus != STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[��ϷDB������]��ϷDB�����˳�����ʼ!"));
		m_pUIHandler = nullptr;

		ExitUnloadConfig();
		ExitUnloadShare();

		m_nStatus = STATUSC_NONE;
		LOG_INFO(m_FileLog, TF("[��ϷDB������]��ϷDB�����˳��������!"));
	}
}
// �����������ö�����������
void CGameDBServer::ExitUnloadShare(void)
{
	m_pConfig    = nullptr;
	m_NetworkPtr = nullptr;
}
//
void CGameDBServer::ExitUnloadConfig(void)
{
	assert(m_krListenGame == nullptr);
	assert(m_krListenGate == nullptr);
	assert(m_pShareGameSvr == nullptr);
	assert(m_pShareGateSvr == nullptr);

	assert(m_GameSvrMap.Size() == 0);
	assert(m_GateSvrMap.Size() == 0);
	LOGV_INFO(m_FileLog, TF("[��ϷDB������]�Ӷ�̬�����������ϷDB������ָ�����[key=%s]!"), CServerConfig::GameDBServer);
	m_pConfig->GetKeyValue()->Remove(CServerConfig::GameDBServer);

	m_GameDBRoutine->Exit();
	m_GameDBRoutine = nullptr;
}
//--------------------------------------
bool CGameDBServer::Start(void)
{
	if (m_nStatus == STATUSC_INIT) {
		LOG_INFO(m_FileLog, TF("[��ϷDB������]��ϷDB����������ʼ!"));

		if ((StartListenGameServer() == false) ||
			(StartListenGateServer() == false)){
			return false;
		}
		m_GameDBRoutine->Start(this);

		m_nStatus = STATUSC_RUN;
		LOG_INFO(m_FileLog, TF("[��ϷDB������]��ϷDB�����������!"));
	}
	return true;
}
// ���д���������Ϸ���������ӵ����Ӷ���
bool CGameDBServer::StartListenGameServer(void)
{
	Int        nPort = 0;
	CStringKey strAddr;
	m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_GAMEDB, 0, strAddr, nPort);

	assert(m_krListenGame == nullptr);
	bool bRet = true;
	m_krListenGame = m_NetworkPtr->Create(*this, (UShort)nPort, *strAddr);
	if (m_krListenGame != nullptr) {
		bRet = m_NetworkPtr->Listen(m_krListenGame);
		LOGV_INFO(m_FileLog, TF("[��ϷDB������]����������Ϸ������������[%s]:%d�ɹ�, %s"), *strAddr, nPort, bRet ? TF("�������ӳɹ�") : TF("��������ʧ��"));
	}
	else {
		LOGV_ERROR(m_FileLog, TF("[��ϷDB������]����������Ϸ������������[%s]:%dʧ��"), *strAddr, nPort);
		bRet = false;
	}
	return bRet;
}
// ���д����������ط��������ӵ����Ӷ���
bool CGameDBServer::StartListenGateServer(void)
{
	Int        nPort = 0;
	CStringKey strAddr;
	m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_GAMEDB, CServerConfig::CFG_DEFAULT_GATE, strAddr, nPort);

	assert(m_krListenGate == nullptr);
	bool bRet = true;
	m_krListenGate = m_NetworkPtr->Create(*this, (UShort)nPort, *strAddr);
	if (m_krListenGate != nullptr) {
		bRet = m_NetworkPtr->Listen(m_krListenGate);
		LOGV_INFO(m_FileLog, TF("[��ϷDB������]�����������ط�����������[%s]:%d�ɹ�, %s"), *strAddr, nPort, bRet ? TF("�������ӳɹ�") : TF("��������ʧ��"));
	}
	else {
		LOGV_ERROR(m_FileLog, TF("[��ϷDB������]�����������ط�����������[%s]:%dʧ��"), *strAddr, nPort);
		bRet = false;
	}
	return bRet;
}
//--------------------------------------
bool CGameDBServer::Pause(bool bPause)
{
	if (bPause && (m_nStatus == STATUSC_RUN)) {
		m_nStatus = STATUSC_PAUSE; 
		LOG_INFO(m_FileLog, TF("[��ϷDB������]��ͣ����"));
		m_GameDBRoutine->Pause();
		return true;
	}
	else if ((bPause == false) && (m_nStatus == STATUSC_PAUSE)) {
		m_nStatus = STATUSC_RUN;   
		LOG_INFO(m_FileLog, TF("[��ϷDB������]�������"));
		m_GameDBRoutine->Pause(false);
		return true;
	}
	return false;
}
//--------------------------------------
void CGameDBServer::Stop(void)
{
	if (m_nStatus > STATUSC_INIT) {
		LOG_INFO(m_FileLog, TF("[��ϷDB������]��ϷDB����ֹͣ��ʼ!"));

		m_GameDBRoutine->Stop();

		StopListenGateServer();
		StopListenGameServer();

		m_ServerInfo[INFOI_GAMEDB].Zero();
		m_ServerInfo[INFOI_GAME].Reset();
		m_ServerInfo[INFOI_GATE].Reset();
		m_GameSvrMap.RemoveAll();
		m_GateSvrMap.RemoveAll();

		m_nStatus = STATUSC_INIT;
		LOG_INFO(m_FileLog, TF("[��ϷDB������]��ϷDB����ֹͣ���!"));
	}
}
// ֹͣ������Ϸ����������
void CGameDBServer::StopListenGameServer(void)
{
	if (m_krListenGame != nullptr) {
		m_NetworkPtr->Destroy(m_krListenGame, false);
		m_krListenGame = nullptr;
		LOG_INFO(m_FileLog, TF("[��ϷDB������]���ټ�����Ϸ�����������ӳɹ�"));
	}
	m_pShareGameSvr = nullptr;
}
// ֹͣ�������ط���������
void CGameDBServer::StopListenGateServer(void)
{
	if (m_krListenGate != nullptr) {
		m_NetworkPtr->Destroy(m_krListenGate, false);
		m_krListenGate = nullptr;
		LOG_INFO(m_FileLog, TF("[��ϷDB������]���ټ������ط����������ӳɹ�"));
	}
	m_pShareGateSvr = nullptr;
}
//--------------------------------------
bool CGameDBServer::OnShareRoutine(Int nEvent, CEventBase& EventRef, LLong llParam, CEventQueue::EVENT_TYPE eType)
{
	assert((m_pConfig->GetLoadServers() & (CServerConfig::CFG_DEFAULT_GAME| CServerConfig::CFG_DEFAULT_GATE)) != 0);
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
	case PAK_EVENT_GATE_SELECT:
	case PAK_EVENT_GATE_PLAY:
	case PAK_EVENT_GATE_CREATE:
	case PAK_EVENT_GATE_DELETE:
	case PAK_EVENT_GATE_RECOVER:
		{
			m_GameDBRoutine->Add(nEvent, EventRef, llParam, eType);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��ϷDB������]ͬ���̷������޷�ʶ������������[event=%d]"), nEvent);
			assert(0);
		}
	}
	return true;
}
// ͬ���̷���������
bool CGameDBServer::OnShareLink(CEventBase& EventRef, LLong llParam)
{
	CPAKLink* pLink = static_cast<CPAKLink*>(&EventRef);
	if (pLink->GetType() == PAK_TYPE_GAME) {
		if (m_pShareGameSvr == nullptr) {
			// 0.��ù���ָ��
			m_pShareGameSvr = reinterpret_cast<ICommonServer*>(llParam);
			LOGV_INFO(m_FileLog, TF("[��ϷDB������]ͬ������Ϸ������[%p]����"), m_pShareGameSvr);
			return ServerLink<SVR_GAME_MAP, DATA_INDEX_GAME, INFOI_GAME>(pLink, (DataRef)llParam, m_GameSvrMap);
		}
		else {
			LOGV_WARN(m_FileLog, TF("[��ϷDB������]ͬ������Ϸ�����������Ѿ�����[%p-%p]"), m_pShareGameSvr, llParam);
		}
	}
	else if (pLink->GetType() == PAK_TYPE_GATE) {
		if (m_pShareGateSvr == nullptr) {
			// 0.��ù���ָ��
			m_pShareGateSvr = reinterpret_cast<ICommonServer*>(llParam);
			LOGV_INFO(m_FileLog, TF("[��ϷDB������]ͬ�������ط�����[%p]����"), m_pShareGateSvr);
			return ServerLink<SVR_GATE_MAP, DATA_INDEX_GATE, INFOI_GATE>(pLink, (DataRef)llParam, m_GateSvrMap);
		}
		else {
			LOGV_WARN(m_FileLog, TF("[��ϷDB������]ͬ�������ط����������Ѿ�����[%p-%p]"), m_pShareGateSvr, llParam);
		}
	}
	return false;
}

bool CGameDBServer::OnShareUpdate(CEventBase& EventRef, LLong llParam)
{
	CPAKUpdate* pUpdate = static_cast<CPAKUpdate*>(&EventRef);
	if (pUpdate->GetType() == PAK_TYPE_GAME) {
		if (m_pShareGameSvr != nullptr) {
			DEV_INFO(TF("[��ϷDB������]ͬ������Ϸ������[%p]����"), m_pShareGameSvr);
			assert(m_pShareGameSvr == reinterpret_cast<ICommonServer*>(llParam));
			return ServerUpdate<SVR_GAME_MAP, DATA_INDEX_GAME, INFOI_GAME>(pUpdate, (DataRef)llParam, m_GameSvrMap);
		}
		else {
			LOGV_WARN(m_FileLog, TF("[��ϷDB������]ͬ������Ϸ���������󲻴���"));
		}
	}
	else if (pUpdate->GetType() == PAK_TYPE_GATE) {
		if (m_pShareGateSvr != nullptr) {
			DEV_INFO(TF("[��ϷDB������]ͬ�������ط�����[%p]����"), m_pShareGateSvr);
			assert(m_pShareGateSvr == reinterpret_cast<ICommonServer*>(llParam));
			return ServerUpdate<SVR_GATE_MAP, DATA_INDEX_GATE, INFOI_GATE>(pUpdate, (DataRef)llParam, m_GateSvrMap);
		}
		else {
			LOGV_WARN(m_FileLog, TF("[��ϷDB������]ͬ�������ط��������󲻴���"));
		}
	}
	return false;
}
//--------------------------------------
bool CGameDBServer::OnTcpDispatch(const PacketPtr& PktPtr, PTCP_PARAM pTcp)
{
	// PTCP_PARAM->pCache == nullptr, �ϲ���Ա���ӵײ�����Ļ�������
	// PTCP_PARAM->pCache, PTCP_PARAM->index�ǵײ�����Ļ������ݵ���ʼ��ַ��������
	// ����ϲ㱣�滺��, ����PTCP_PARAM->pDataΪnullptr֪ͨ����ײ�
	// �ͷŻ���ʹ��MObject::sMCFree(index, pCache)
	bool bRet = false;
	if (PktPtr->GetEvent() != PAK_EVENT_LIVE) {
		switch (PktPtr->GetEvent()) {
		case PAK_EVENT_LINK:
			{
				bRet = OnServerLink(static_cast<CPAKLink*>(PktPtr.Get()), pTcp->krSocket);
			}
			break;
		case PAK_EVENT_UPDATE:
			{
				bRet = OnServerUpdate(static_cast<CPAKUpdate*>(PktPtr.Get()), pTcp->krSocket);
			}
			break;
		case PAK_EVENT_UNLINK:
			{
				bRet = OnServerUnlink(PktPtr.Get(), pTcp->krSocket);
			}
			break;
		case PAK_EVENT_GATE_SELECT:
		case PAK_EVENT_GATE_PLAY:
		case PAK_EVENT_GATE_CREATE:
		case PAK_EVENT_GATE_DELETE:
		case PAK_EVENT_GATE_RECOVER:
			{
				m_GameDBRoutine->Add((Int)PktPtr->GetEvent(), *PktPtr, (LLong)pTcp->krSocket);
			}
			break;
		default:
			{
				LOGV_WARN(m_FileLog, TF("[��ϷDB������]%p������Ϣ�޷�ʶ���%X���������������[event=%d]"), pTcp->krSocket, PktPtr->GetType(), PktPtr->GetEvent());
			}
		}
	}
	else {
		CPAKSimple<PAK_EVENT_LIVEACK, PAK_TYPE_GAMEDB> LiveAck;
		LiveAck.AdjustSize();
		bRet = m_NetworkPtr->Send(pTcp->krSocket, LiveAck);
	}
	return bRet;
}
// ����������
bool CGameDBServer::OnServerLink(CPAKLink* pLink, KeyRef krSocket)
{
	bool bRet = false;
	switch (pLink->GetType()) {
	case PAK_TYPE_GAME:
		{
			LOGV_INFO(m_FileLog, TF("[��ϷDB������]��Ϸ������[%p]����"), krSocket);
			bRet = ServerLink<SVR_GAME_MAP, DATA_INDEX_GAME, INFOI_GAME>(pLink, (DataRef)krSocket, m_GameSvrMap);
		}
		break;
	case PAK_TYPE_GATE:
		{
			LOGV_INFO(m_FileLog, TF("[��ϷDB������]���ط�����[%p]����"), krSocket);
			bRet = ServerLink<SVR_GATE_MAP, DATA_INDEX_GATE, INFOI_GATE>(pLink, (DataRef)krSocket, m_GateSvrMap);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��ϷDB������]%p������Ϣ�޷�ʶ���%X���������������"), krSocket, pLink->GetType());
		}
	}
	if (bRet) {
		// 3.��������Ϊѡ�����������
		m_NetworkPtr->SetAttr(krSocket, pLink->GetType());
		// 4.�������ӻ�ִ��
		CPAKSimple<PAK_EVENT_LINKACK, PAK_TYPE_GAMEDB> LinkAck;
		LinkAck.AdjustSize();
		m_NetworkPtr->Send(krSocket, LinkAck);
	}
	return bRet;
}

bool CGameDBServer::OnServerUpdate(CPAKUpdate* pUpdate, KeyRef krSocket)
{
	bool bRet = false;
	switch (pUpdate->GetType()) {
	case PAK_TYPE_GAME:
		{
			DEV_INFO(TF("[��ϷDB������]��Ϸ������[%p]����"), krSocket);
			bRet = ServerUpdate<SVR_GAME_MAP, DATA_INDEX_GAME, INFOI_GAME>(pUpdate, (DataRef)krSocket, m_GameSvrMap);
		}
		break;
	case PAK_TYPE_GATE:
		{
			DEV_INFO(TF("[��ϷDB������]���ط�����[%p]����"), krSocket);
			bRet = ServerUpdate<SVR_GATE_MAP, DATA_INDEX_GATE, INFOI_GATE>(pUpdate, (DataRef)krSocket, m_GateSvrMap);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��ϷDB������]%p������Ϣ�޷�ʶ���%X���������������"), krSocket, pUpdate->GetType());
		}
	}
	if (bRet) {
		// ���͸��»�ִ��
		CPAKSimple<PAK_EVENT_UPDATEACK, PAK_TYPE_GAMEDB> UpdateAck;
		UpdateAck.AdjustSize();
		m_NetworkPtr->Send(krSocket, UpdateAck);
	}
	return bRet;
}

bool CGameDBServer::OnServerUnlink(CPAKHead* pUnlink, KeyRef krSocket)
{
	bool bRet = false;
	switch (pUnlink->GetType()) {
	case PAK_TYPE_GAME:
		{
			LOGV_INFO(m_FileLog, TF("[��ϷDB������]��Ϸ������[%p]�Ͻ�"), krSocket);
			bRet = ServerUnlink<SVR_GAME_MAP, DATA_INDEX_GAME, INFOI_GAME>(krSocket, m_GameSvrMap);
		}
		break;
	case PAK_TYPE_GATE:
		{
			LOGV_INFO(m_FileLog, TF("[��ϷDB������]���ط�����[%p]�Ͻ�"), krSocket);
			bRet = ServerUnlink<SVR_GATE_MAP, DATA_INDEX_GATE, INFOI_GATE>(krSocket, m_GateSvrMap);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��ϷDB������]%p������Ϣ�޷�ʶ���%X���������������"), krSocket, pUnlink->GetType());
		}
	}
	if (bRet) {
		// 3.��������Ϊ��Ч����������
		m_NetworkPtr->SetAttr(krSocket, PAK_TYPE_NONE);
		// 4.���Ͷ�����ִ��
		CPAKSimple<PAK_EVENT_UNLINKACK, PAK_TYPE_GAMEDB> UnlinkAck;
		UnlinkAck.AdjustSize();
		m_NetworkPtr->Send(krSocket, UnlinkAck);
	}
	return bRet;
}
//--------------------------------------
bool CGameDBServer::OnTcpAccept(KeyRef krAccept, KeyRef krListen)
{
	if (m_nStatus == STATUSC_RUN) {
		UShort     usPort = 0;
		CStringKey strAddr;
		m_NetworkPtr->GetAddr(krAccept, strAddr, usPort);

		if (krListen == m_krListenGame) {
			DEV_INFO(TF("[��ϷDB������]��Ϸ������[%s]:%d�������!"), *strAddr, usPort);
		}
		else if (krListen == m_krListenGate) {
			DEV_INFO(TF("[��ϷDB������]���ط�����[%s]:%d�������!"), *strAddr, usPort);
		}
		m_ServerInfo[INFOI_GAMEDB].Incr();
		return true;
	}
	else {
		LOGV_WARN(m_FileLog, TF("[��ϷDB������]��ϷDB������������ͣ״̬!"));
	}
	return false;
}
//--------------------------------------
bool CGameDBServer::OnTcpClose(KeyRef krSocket, LLong llLiveData)
{
	if (krSocket == m_krListenGame) {
		m_krListenGame = nullptr;
		DEV_INFO(TF("[��ϷDB������]������Ϸ���������ӹر�"));
	}
	else if (krSocket == m_krListenGate) {
		m_krListenGate = nullptr;
		DEV_INFO(TF("[��ϷDB������]�������ط��������ӹر�"));
	}
	else { // ��������������/�Ͽ�
		switch (llLiveData) {
		case PAK_TYPE_GAME:
			{
				ServerUnlink<SVR_GAME_MAP, DATA_INDEX_GAME, INFOI_GAME>(krSocket, m_GameSvrMap);
			}
			break;
		case PAK_TYPE_GATE:
			{
				ServerUnlink<SVR_GATE_MAP, DATA_INDEX_GATE, INFOI_GATE>(krSocket, m_GateSvrMap);
			}
			break;
		default:
			{
			}
		}
		m_ServerInfo[INFOI_GAMEDB].Decr();
	}
	return true;
}
//--------------------------------------
// ��ʱ��������Ϸ���������ӵ����Ӷ����Ƿ���Ч
bool CGameDBServer::CheckListenGameServer(void)
{
	if (m_krListenGame == nullptr) {
		return StartListenGameServer();
	}
	return true;
}
// ��ʱ���������ط��������ӵ����Ӷ����Ƿ���Ч
bool CGameDBServer::CheckListenGateServer(void)
{
	if (m_krListenGate == nullptr) {
		return StartListenGateServer();
	}
	return true;
}
// ͬ����������Ϣ������
bool CGameDBServer::SyncServerInfo(void)
{
	if (m_ServerInfo[INFOI_GAMEDB].usStatus == STATUSU_SYNC) {
		m_GameDBRoutine->Update();
		m_ServerInfo[INFOI_GAMEDB].usStatus = STATUSU_OKAY;
		return m_pUIHandler->OnHandle(PAK_EVENT_SYNC, reinterpret_cast<uintptr_t>(m_ServerInfo + INFOI_GAMEDB), DATA_INDEX_GAMEDB);
	}
	if (m_ServerInfo[INFOI_GAME].usStatus == STATUSU_SYNC) {
		m_ServerInfo[INFOI_GAME].usStatus = STATUSU_OKAY;
		return m_pUIHandler->OnHandle(PAK_EVENT_SYNC, reinterpret_cast<uintptr_t>(m_ServerInfo + INFOI_GAME), DATA_INDEX_GAME);
	}
	if (m_ServerInfo[INFOI_GATE].usStatus == STATUSU_SYNC) {
		m_ServerInfo[INFOI_GATE].usStatus = STATUSU_OKAY;
		return m_pUIHandler->OnHandle(PAK_EVENT_SYNC, reinterpret_cast<uintptr_t>(m_ServerInfo + INFOI_GATE), DATA_INDEX_GATE);
	}
	return true;
}


