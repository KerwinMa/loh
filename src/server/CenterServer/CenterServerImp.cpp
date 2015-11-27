//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Source File : CenterServerImp.cpp                          //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update : 2015-11-25     version 0.0.0.5                    //
//   Detail : ���ķ�����ʵ��                                     //
//                                                              //
//////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CenterServerImp.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CCenterServer
UInt CCenterServer::Command(PCXStr pszCMD, uintptr_t utParam)
{
	// �������ļ������������Ƹ�����ز���
	UNREFERENCED_PARAMETER( pszCMD );
	UNREFERENCED_PARAMETER( utParam );
	return (UInt)RET_OKAY;
}

UInt CCenterServer::Update(void)
{
	CheckListenServers();
	SyncServersInfo();
	return 0;
}
//--------------------------------------
UInt CCenterServer::Init(CEventHandler& EventHandlerRef)
{
	if (m_nStatus == STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[���ķ�����]���ķ����ʼ����ʼ!"));
		m_pUIHandler = &EventHandlerRef;

		if ( (InitLoadShare()  == false) ||
			 (InitLoadConfig() == false) ) {
			return (UInt)RET_FAIL;
		}

		m_nStatus = STATUSC_INIT;
		LOG_INFO(m_FileLog, TF("[���ķ�����]���ķ����ʼ�����!"));
		return (UInt)RET_OKAY;
	}
	return (UInt)RET_FAIL;
}

bool CCenterServer::InitLoadShare(void)
{
	assert(m_pConfig == nullptr);
	m_pUIHandler->OnHandle(CServerConfig::CFG_CONFIG_PTR, reinterpret_cast<uintptr_t>(&m_pConfig), DATA_INDEX_CENTER);
	if (m_pConfig == nullptr) {
		LOG_ERROR(m_FileLog, TF("[���ķ�����]�ӽ���ص��ӿڻ�ȡ���ö���ָ����Ч"));
		return false;
	}
	LOG_INFO(m_FileLog, TF("[���ķ�����]�ӽ���ص��ӿڻ�ȡ���ö���ָ�����!"));
	m_pConfig->Dump(m_FileLog); // ������Ϣ�������־�ļ�

	// �Ӷ�̬�������ȡ���������ָ��
	assert(m_NetworkPtr == nullptr);
	CKeyValue::VALUE xValue = { 0 };
	m_pConfig->GetKeyValue()->GetValue(CServerConfig::NetworkPtr, xValue);
	if (xValue.pValue == nullptr) {
		LOG_ERROR(m_FileLog, TF("[���ķ�����]�Ӷ�̬�������ȡ���������ָ����Ч"));
		return false;
	}
	LOG_INFO(m_FileLog, TF("[���ķ�����]�Ӷ�̬�������ȡ���������ָ�����!"));
	m_NetworkPtr = *(reinterpret_cast<CNetworkPtr*>(xValue.pValue));
	// �������ķ�����ָ�뵽��̬������
	LOGV_INFO(m_FileLog, TF("[���ķ�����]�������ķ�����ָ�뵽��̬���������[key=%s]!"), CServerConfig::CenterServer);
	m_pConfig->GetKeyValue()->AddItem(CServerConfig::CenterServer, this);
	return true;
}

bool CCenterServer::InitLoadConfig(void)
{
	assert(m_krListenSelect == nullptr);
	assert(m_krListenLogin  == nullptr);
	assert(m_krListenGame   == nullptr);

	assert(m_pShareSelectSvr == nullptr);
	assert(m_pShareLoginSvr  == nullptr);
	assert(m_pShareGameSvr   == nullptr);

	assert(m_SelectSvrMap.Size() == 0);
	assert(m_LoginSvrMap.Size()  == 0);
	assert(m_GameSvrMap.Size()   == 0);

	m_ServerInfo[INFOI_CENTER].usId   = m_pConfig->GetServerId(CServerConfig::CFG_DEFAULT_CENTER);
	m_ServerInfo[INFOI_CENTER].usIncr = m_pConfig->GetServerIncr(CServerConfig::CFG_DEFAULT_CENTER);
	LOGV_INFO(m_FileLog, TF("[���ķ�����]�����ö����ȡ������Id=%d, Incr=%d!"), m_ServerInfo[INFOI_CENTER].usId, m_ServerInfo[INFOI_CENTER].usIncr);
	return true;
}
//--------------------------------------
void CCenterServer::Exit(void)
{
	if (m_nStatus != STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[���ķ�����]���ķ����˳�����ʼ!"));
		m_pUIHandler = nullptr;

		ExitUnloadConfig();
		ExitUnloadShare();

		m_nStatus = STATUSC_NONE;
		LOG_INFO(m_FileLog, TF("[���ķ�����]���ķ����˳��������!"));
	}
}

void CCenterServer::ExitUnloadShare(void)
{
	m_pConfig    = nullptr;
	m_NetworkPtr = nullptr;
}
//
void CCenterServer::ExitUnloadConfig(void)
{
	assert(m_krListenSelect == nullptr);
	assert(m_krListenLogin  == nullptr);
	assert(m_krListenGame   == nullptr);

	assert(m_pShareSelectSvr == nullptr);
	assert(m_pShareLoginSvr  == nullptr);
	assert(m_pShareGameSvr   == nullptr);

	assert(m_SelectSvrMap.Size() == 0);
	assert(m_LoginSvrMap.Size()  == 0);
	assert(m_GameSvrMap.Size()   == 0);
	LOGV_INFO(m_FileLog, TF("[���ķ�����]�Ӷ�̬������������ķ�����ָ�����[key=%s]!"), CServerConfig::CenterServer);
	m_pConfig->GetKeyValue()->Remove(CServerConfig::CenterServer);
}
//--------------------------------------
bool CCenterServer::Start(void)
{
	if (m_nStatus == STATUSC_INIT) {
		LOG_INFO(m_FileLog, TF("[���ķ�����]���ķ���������ʼ!"));

		if (StartListenServers() == false) {
			return false;
		}

		m_nStatus = STATUSC_RUN;
		LOG_INFO(m_FileLog, TF("[���ķ�����]���ķ����������!"));
	}
	return true;
}

bool CCenterServer::StartListenServers(void)
{
	UShort     usPort = 0;
	CStringKey strAddr;
	// select
	if ((m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_CENTER, CServerConfig::CFG_DEFAULT_SELECT, strAddr, usPort) == false) || 
		(StartListenServer(m_krListenSelect, strAddr, usPort) == false)) {
		return false;
	}
	// login
	if (m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_CENTER, CServerConfig::CFG_DEFAULT_LOGIN, strAddr, usPort) == false) {
		return false;
	}
	if (usPort == 0) {
		m_krListenLogin = (KeyRef)this;
		LOG_INFO(m_FileLog, TF("[���ķ�����]������½��������ַ�ͼ���ѡ���������ַһ��"));
	}
	else if (StartListenServer(m_krListenLogin, strAddr, usPort) == false) {
		return false;
	}
	// game
	if (m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_CENTER, CServerConfig::CFG_DEFAULT_GAME, strAddr, usPort) == false) {
		return false;
	}
	if (usPort == 0) {
		m_krListenGame = (KeyRef)this;
		LOG_INFO(m_FileLog, TF("[���ķ�����]������Ϸ��������ַ�ͼ���ѡ����������ߵ�½��������ַһ��"));
	}
	else if (StartListenServer(m_krListenGame, strAddr, usPort) == false) {
		return false;
	}
	return true;
}

bool CCenterServer::StartListenServer(KeyRef& krListen, const CStringKey& strAddr, UShort usPort)
{
	if (krListen != nullptr) {
		LOGV_INFO(m_FileLog, TF("[���ķ�����]������������������[%s]:%d�Ѿ�����"), *strAddr, usPort);
		return true;
	}
	bool bRet = true;
	krListen = m_NetworkPtr->Create(*this, usPort, *strAddr);
	if (krListen != nullptr) {
		bRet = m_NetworkPtr->Listen(krListen);
		LOGV_INFO(m_FileLog, TF("[���ķ�����]������������������[%s]:%d�ɹ�, %s"), *strAddr, usPort, bRet ? TF("���������ɹ�") : TF("��������ʧ��"));
	}
	else {
		LOGV_ERROR(m_FileLog, TF("[���ķ�����]������������������[%s]:%dʧ��"), *strAddr, usPort);
		bRet = false;
	}
	return bRet;
}
//--------------------------------------
bool CCenterServer::Pause(bool bPause)
{
	if (bPause && (m_nStatus == STATUSC_RUN)) {
		m_nStatus = STATUSC_PAUSE;
		LOG_INFO(m_FileLog, TF("[���ķ�����]��ͣ������������"));
		return true;
	}
	else if ((bPause == false) && (m_nStatus == STATUSC_PAUSE)) {
		m_nStatus = STATUSC_RUN;
		LOG_INFO(m_FileLog, TF("[���ķ�����]���������������"));
		return true;
	}
	return false;
}
//--------------------------------------
void CCenterServer::Stop(void)
{
	if (m_nStatus > STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[���ķ�����]���ķ���ֹͣ��ʼ!"));

		StopListenServers();

		m_pShareSelectSvr = nullptr;
		m_pShareLoginSvr  = nullptr;
		m_pShareGameSvr   = nullptr;

		m_ServerInfo[INFOI_CENTER].Zero();
		m_ServerInfo[INFOI_SELECT].Reset();
		m_ServerInfo[INFOI_LOGIN].Reset();
		m_ServerInfo[INFOI_GAME].Reset();
		m_SelectSvrMap.RemoveAll();
		m_LoginSvrMap.RemoveAll();
		m_GameSvrMap.RemoveAll();

		m_nStatus = STATUSC_INIT;
		LOG_INFO(m_FileLog, TF("[���ķ�����]���ķ���ֹͣ���!"));
	}
}

void CCenterServer::StopListenServers(void)
{
	if (m_krListenSelect != nullptr) {
		m_NetworkPtr->Destroy(m_krListenSelect, false);
		m_krListenSelect = nullptr;
		LOG_INFO(m_FileLog, TF("[���ķ�����]���ټ���ѡ�����������ɹ�"));
	}
	if (m_krListenLogin != nullptr) {
		if (m_krListenLogin != (KeyRef)this) {
			m_NetworkPtr->Destroy(m_krListenLogin, false);
		}
		m_krListenLogin = nullptr;
		LOG_INFO(m_FileLog, TF("[���ķ�����]���ټ�����½����������ɹ�"));
	}
	if (m_krListenGame != nullptr) {
		if (m_krListenGame != (KeyRef)this) {
			m_NetworkPtr->Destroy(m_krListenGame, false);
		}
		m_krListenGame = nullptr;
		LOG_INFO(m_FileLog, TF("[���ķ�����]���ټ�����Ϸ����������ɹ�"));
	}
}
//--------------------------------------
bool CCenterServer::OnShareRoutine(Int nEvent, CEventBase& EventRef, LLong llParam, CEventQueue::EVENT_TYPE)
{
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
	//case PAK_EVENT_UNLINK: // ͬ���̷�����ע��, ˵������ֹͣ, ����Ҫ����
	//	break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[���ķ�����]ͬ���̷������޷�ʶ������������[event=%d]"), nEvent);
			assert(0);
		}
	}
	return false;
}

bool CCenterServer::OnShareLink(CEventBase& EventRef, LLong llParam)
{
	CPAKLink* pLink = static_cast<CPAKLink*>(&EventRef);
	switch (pLink->GetType()) {
	case PAK_TYPE_GAME:
		if (m_pShareGameSvr == nullptr) {
			// 0.��ù���ָ��
			m_pShareGameSvr = reinterpret_cast<ICommonServer*>(llParam);
			LOGV_INFO(m_FileLog, TF("[���ķ�����]ͬ������Ϸ������[%p]ע��"), m_pShareGameSvr);
			return ServerLink<SVR_GAME_MAP, DATA_INDEX_GAME, INFOI_GAME>(pLink, (DataRef)llParam, m_GameSvrMap);
		}
		else {
			LOGV_WARN(m_FileLog, TF("[���ķ�����]ͬ������Ϸ�����������Ѿ�����[%p-%p]"), m_pShareGameSvr, llParam);
		}
		break;
	case PAK_TYPE_LOGIN:
		if (m_pShareLoginSvr == nullptr) {
			// 0.��ù���ָ��
			m_pShareLoginSvr = reinterpret_cast<ICommonServer*>(llParam);
			LOGV_INFO(m_FileLog, TF("[���ķ�����]ͬ���̵�½������[%p]ע��"), m_pShareLoginSvr);
			if (ServerLink<SVR_LOGIN_MAP, DATA_INDEX_LOGIN, INFOI_LOGIN>(pLink, (DataRef)llParam, m_LoginSvrMap)) {
				// 3.��ע��ĵ�½����������okay��sync״̬����Ϸ��������Ϣ
				return SendGameServerInfo();
			}		
		}
		else {
			LOGV_WARN(m_FileLog, TF("[���ķ�����]ͬ���̵�½�����������Ѿ�����[%p-%p]"), m_pShareLoginSvr, llParam);
		}
		break;
	case PAK_TYPE_SELECT:
		if (m_pShareSelectSvr == nullptr) {
			// 0.��ù���ָ��
			m_pShareSelectSvr = reinterpret_cast<ICommonServer*>(llParam);
			LOGV_INFO(m_FileLog, TF("[���ķ�����]ͬ����ѡ�������[%p]ע��"), m_pShareSelectSvr);
			if (ServerLink<SVR_SELECT_MAP, DATA_INDEX_SELECT, INFOI_SELECT>(pLink, (DataRef)llParam, m_SelectSvrMap)) {
				// 3.��ע���ѡ�����������okay��sync״̬�ĵ�½��������Ϣ
				return SendLoginServerInfo();
			}
		}
		else {
			LOGV_WARN(m_FileLog, TF("[���ķ�����]ͬ����ѡ������������Ѿ�����[%p-%p]"), m_pShareSelectSvr, llParam);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[���ķ�����]ͬ���̷���������δ֪%X"), pLink->GetType());
		}
	}
	return false;
}

bool CCenterServer::OnShareUpdate(CEventBase& EventRef, LLong llParam)
{
	CPAKUpdate* pUpdate = static_cast<CPAKUpdate*>(&EventRef);
	switch (pUpdate->GetType()) {
	case PAK_TYPE_GAME:
		if (m_pShareGameSvr != nullptr) {
			DEV_INFO(TF("[���ķ�����]ͬ������Ϸ������[%p]����"), m_pShareGameSvr);
			assert(m_pShareGameSvr == reinterpret_cast<ICommonServer*>(llParam));
			return ServerUpdate<SVR_GAME_MAP, DATA_INDEX_GAME, INFOI_GAME>(pUpdate, (DataRef)llParam, m_GameSvrMap);
		}
		else {
			LOGV_WARN(m_FileLog, TF("[���ķ�����]ͬ������Ϸ����������δע��"));
		}
		break;
	case PAK_TYPE_LOGIN:
		if (m_pShareLoginSvr != nullptr) {
			DEV_INFO(TF("[���ķ�����]ͬ���̵�½������[%p]����"), m_pShareLoginSvr);
			assert(m_pShareLoginSvr == reinterpret_cast<ICommonServer*>(llParam));
			return ServerUpdate<SVR_LOGIN_MAP, DATA_INDEX_LOGIN, INFOI_LOGIN>(pUpdate, (DataRef)llParam, m_LoginSvrMap);
		}
		else {
			LOGV_WARN(m_FileLog, TF("[���ķ�����]ͬ���̵�½����������δע��"));
		}
		break;
	case PAK_TYPE_SELECT:
		if (m_pShareSelectSvr != nullptr) {
			DEV_INFO(TF("[���ķ�����]ͬ����ѡ�������[%p]����"), m_pShareSelectSvr);
			assert(m_pShareSelectSvr == reinterpret_cast<ICommonServer*>(llParam));
			return ServerUpdate<SVR_SELECT_MAP, DATA_INDEX_SELECT, INFOI_SELECT>(pUpdate, (DataRef)llParam, m_SelectSvrMap);
		}
		else {
			LOGV_WARN(m_FileLog, TF("[���ķ�����]ͬ����ѡ�����������δע��"));
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[���ķ�����]ͬ���̷�������������δ֪%X"), pUpdate->GetType());
		}
	}
	return false;
}
//--------------------------------------
bool CCenterServer::OnTcpDispatch(const PacketPtr& PktPtr, PTCP_PARAM pTcp)
{
	// PTCP_PARAM->pCache == nullptr, �ϲ���Ա���ӵײ�����Ļ�������
	// PTCP_PARAM->pCache, PTCP_PARAM->index�ǵײ�����Ļ������ݵ���ʼ��ַ��������
	// ����ϲ㱣�滺��, ����PTCP_PARAM->pDataΪnullptr֪ͨ����ײ�
	// �ͷŻ���ʹ��MObject::sMCFree(index, pCache)
	bool bRet = false;
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
	case PAK_EVENT_SYNCACK:
		{
			bRet = true;
			DEV_INFO(TF("[���ķ�����]��%X������[%p]ͬ����������Ϣ���"), PktPtr->GetType(), pTcp->krSocket);
		}
		break;
	case PAK_EVENT_LIVE:
		{
			CPAKSimple<PAK_EVENT_LIVEACK, PAK_TYPE_CENTER> LiveAck;
			LiveAck.AdjustSize();
			bRet = m_NetworkPtr->Send(pTcp->krSocket, LiveAck);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[���ķ�����]%p�޷�ʶ���%X���������������[event=%d]"), pTcp->krSocket, PktPtr->GetType(), PktPtr->GetEvent());
		}
	}
	return bRet;
}

bool CCenterServer::OnServerLink(CPAKLink* pLink, KeyRef krSocket)
{
	switch (pLink->GetType()) {
	case PAK_TYPE_GAME:
		{
			LOGV_INFO(m_FileLog, TF("[���ķ�����]��Ϸ������[%p]ע��"), krSocket);
			if (ServerLink<SVR_GAME_MAP, DATA_INDEX_GAME, INFOI_GAME>(pLink, (DataRef)krSocket, m_GameSvrMap)) {
				// 3.����Ϊ��Ϸ����������
				m_NetworkPtr->SetAttr(krSocket, PAK_TYPE_GAME);
				// 4.����ע���ִ��
				CPAKSimple<PAK_EVENT_LINKACK, PAK_TYPE_CENTER> LinkAck;
				LinkAck.AdjustSize();
				return m_NetworkPtr->Send(krSocket, LinkAck);
			}
		}
		break;
	case PAK_TYPE_LOGIN:
		{
			LOGV_INFO(m_FileLog, TF("[���ķ�����]��½������[%p]ע��"), krSocket);
			if (ServerLink<SVR_LOGIN_MAP, DATA_INDEX_LOGIN, INFOI_LOGIN>(pLink, (DataRef)krSocket, m_LoginSvrMap)) {
				// 3.����Ϊ��½����������
				m_NetworkPtr->SetAttr(krSocket, PAK_TYPE_LOGIN_CENTER);
				// 4.����ע���ִ��
				CPAKSimple<PAK_EVENT_LINKACK, PAK_TYPE_CENTER> LinkAck;
				LinkAck.AdjustSize();
				m_NetworkPtr->Send(krSocket, LinkAck);
				// 5.��ע��ĵ�½����������okay��sync״̬����Ϸ��������Ϣ
				return SendGameServerInfo(krSocket);
			}
		}
		break;
	case PAK_TYPE_SELECT:
		{
			LOGV_INFO(m_FileLog, TF("[���ķ�����]ѡ�������[%p]ע��"), krSocket);
			if (ServerLink<SVR_SELECT_MAP, DATA_INDEX_SELECT, INFOI_SELECT>(pLink, (DataRef)krSocket, m_SelectSvrMap)) {
				// 3.����Ϊѡ�����������
				m_NetworkPtr->SetAttr(krSocket, PAK_TYPE_SELECT);
				// 4.����ע���ִ��
				CPAKSimple<PAK_EVENT_LINKACK, PAK_TYPE_CENTER> LinkAck;
				LinkAck.AdjustSize();
				m_NetworkPtr->Send(krSocket, LinkAck);
				// 5.��ע���ѡ�����������okay��sync״̬�ĵ�½��������Ϣ
				return SendLoginServerInfo(krSocket);
			}
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[���ķ�����]%pע����Ϣ�޷�ʶ���%X���������������"), krSocket, pLink->GetType());
		}
	}
	return false;
}

bool CCenterServer::OnServerUpdate(CPAKUpdate* pUpdate, KeyRef krSocket)
{
	bool bRet = false;
	switch (pUpdate->GetType()) {
	case PAK_TYPE_GAME:
		{
			DEV_INFO(TF("[���ķ�����]��Ϸ������[%p]����"), krSocket);
			bRet = ServerUpdate<SVR_GAME_MAP, DATA_INDEX_GAME, INFOI_GAME>(pUpdate, (DataRef)krSocket, m_GameSvrMap);
		}
		break;
	case PAK_TYPE_LOGIN:
		{
			DEV_INFO(TF("[���ķ�����]��½������[%p]����"), krSocket);
			bRet = ServerUpdate<SVR_LOGIN_MAP, DATA_INDEX_LOGIN, INFOI_LOGIN>(pUpdate, (DataRef)krSocket, m_LoginSvrMap);
		}
		break;
	case PAK_TYPE_SELECT:
		{
			DEV_INFO(TF("[���ķ�����]ѡ�������[%p]����"), krSocket);
			bRet = ServerUpdate<SVR_SELECT_MAP, DATA_INDEX_SELECT, INFOI_SELECT>(pUpdate, (DataRef)krSocket, m_SelectSvrMap);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[���ķ�����]%p������Ϣ�޷�ʶ���%X���������������"), krSocket, pUpdate->GetType());
		}
	}
	if (bRet) {
		// ���͸��»�ִ��
		CPAKSimple<PAK_EVENT_UPDATEACK, PAK_TYPE_CENTER> UpdateAck;
		UpdateAck.AdjustSize();
		m_NetworkPtr->Send(krSocket, UpdateAck);
	}
	return bRet;
}

bool CCenterServer::OnServerUnlink(CPAKHead* pUnlink, KeyRef krSocket)
{
	bool bRet = false;
	switch (pUnlink->GetType()) {
	case PAK_TYPE_GAME:
		{
			LOGV_INFO(m_FileLog, TF("[���ķ�����]��Ϸ������[%p]ע��"), krSocket);
			bRet = ServerUnlink<SVR_GAME_MAP, DATA_INDEX_GAME, INFOI_GAME>(krSocket, m_GameSvrMap);
		}
		break;
	case PAK_TYPE_LOGIN:
		{
			LOGV_INFO(m_FileLog, TF("[���ķ�����]��½������[%p]ע��"), krSocket);
			bRet = ServerUnlink<SVR_LOGIN_MAP, DATA_INDEX_LOGIN, INFOI_LOGIN>(krSocket, m_LoginSvrMap);
		}
		break;
	case PAK_TYPE_SELECT:
		{
			LOGV_INFO(m_FileLog, TF("[���ķ�����]ѡ�������[%p]ע��"), krSocket);
			bRet = ServerUnlink<SVR_SELECT_MAP, DATA_INDEX_SELECT, INFOI_SELECT>(krSocket, m_SelectSvrMap);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[���ķ�����]%pע����Ϣ�޷�ʶ���%X���������������"), krSocket, pUnlink->GetType());
		}
	}
	if (bRet) {
		// 3.����Ϊ��Ч����������
		m_NetworkPtr->SetAttr(krSocket, PAK_TYPE_NONE);
		// 4.����ע����ִ��
		CPAKSimple<PAK_EVENT_UNLINKACK, PAK_TYPE_CENTER> UnlinkAck;
		UnlinkAck.AdjustSize();
		m_NetworkPtr->Send(krSocket, UnlinkAck);
	}
	return bRet;
}
//--------------------------------------
bool CCenterServer::OnTcpAccept(KeyRef krAccept, KeyRef)
{
	if (m_nStatus == STATUSC_RUN) {
		UShort     usPort = 0;
		CStringKey strAddr;
		m_NetworkPtr->GetAddr(krAccept, strAddr, usPort);
		DEV_INFO(TF("[���ķ�����]������[%s]:%d�������!"), *strAddr, usPort);

		m_ServerInfo[INFOI_CENTER].Incr();
		return true;
	}
	else {
		LOGV_WARN(m_FileLog, TF("[���ķ�����]���ķ�����������ͣ״̬!"));
	}
	return false;
}
//--------------------------------------
bool CCenterServer::OnTcpClose(KeyRef krSocket, LLong llLiveData)
{
	if (krSocket == m_krListenGame) {
		m_krListenGame = nullptr;
		DEV_INFO(TF("[���ķ�����]������Ϸ����������ر�"));
		LOG_INFO(m_FileLog, TF("[���ķ�����]������Ϸ����������ر�"));
	}
	else if (krSocket == m_krListenLogin) {
		m_krListenLogin = nullptr;
		DEV_INFO(TF("[���ķ�����]������½����������ر�"));
		LOG_INFO(m_FileLog, TF("[���ķ�����]������½����������ر�"));
	}
	else if (krSocket == m_krListenSelect) {
		m_krListenSelect = nullptr;
		DEV_INFO(TF("[���ķ�����]����ѡ�����������ر�"));
		LOG_INFO(m_FileLog, TF("[���ķ�����]����ѡ�����������ر�"));
	}
	else { // �����������Ͽ�
		switch (llLiveData) {
		case PAK_TYPE_GAME:
			{
				ServerUnlink<SVR_GAME_MAP, DATA_INDEX_GAME, INFOI_GAME>(krSocket, m_GameSvrMap);
				LOGV_INFO(m_FileLog, TF("[���ķ�����]��Ϸ������[%p]�Ͽ�"), krSocket);
			}
			break;
		case PAK_TYPE_LOGIN_CENTER:
			{
				ServerUnlink<SVR_LOGIN_MAP, DATA_INDEX_LOGIN, INFOI_LOGIN>(krSocket, m_LoginSvrMap);
				LOGV_INFO(m_FileLog, TF("[���ķ�����]��½������[%p]�Ͽ�"), krSocket);
			}
			break;
		case PAK_TYPE_SELECT:
			{
				ServerUnlink<SVR_SELECT_MAP, DATA_INDEX_SELECT, INFOI_SELECT>(krSocket, m_SelectSvrMap);
				LOGV_INFO(m_FileLog, TF("[���ķ�����]ѡ�������[%p]�Ͽ�"), krSocket);
			}
			break;
		default:
			{
			}
		}
		m_ServerInfo[INFOI_CENTER].Decr();
	}
	return true;
}
//--------------------------------------
bool CCenterServer::CheckListenServers(void)
{
	return (CheckListenSelectServer() && CheckListenLoginServer() && CheckListenGameServer());
}

bool CCenterServer::CheckListenSelectServer(void)
{
	if (m_krListenSelect == nullptr){
		UShort     usPort = 0;
		CStringKey strAddr;
		if ((m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_CENTER, CServerConfig::CFG_DEFAULT_SELECT, strAddr, usPort) == false) ||
			(StartListenServer(m_krListenSelect, strAddr, usPort) == false)){
			return false;
		}
	}
	return true;
}

bool CCenterServer::CheckListenLoginServer(void)
{
	if (m_krListenLogin == nullptr){
		UShort     usPort = 0;
		CStringKey strAddr;
		if ((m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_CENTER, CServerConfig::CFG_DEFAULT_LOGIN, strAddr, usPort) == false) ||
			(StartListenServer(m_krListenLogin, strAddr, usPort) == false)){
			return false;
		}
	}
	return true;
}

bool CCenterServer::CheckListenGameServer(void)
{
	if (m_krListenGame == nullptr){
		UShort     usPort = 0;
		CStringKey strAddr;
		if ((m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_CENTER, CServerConfig::CFG_DEFAULT_GAME, strAddr, usPort) == false) ||
		    (StartListenServer(m_krListenGame, strAddr, usPort) == false)){
			return false;
		}
	}
	return true;
}

bool CCenterServer::SyncServersInfo(void)
{
	SyncCenterServerInfo();
	SyncSelectServerInfo();
	SyncLoginServerInfo();
	SyncGameServerInfo();
	return true;
}

bool CCenterServer::SyncCenterServerInfo(void)
{
	if (m_ServerInfo[INFOI_CENTER].usStatus == STATUSU_SYNC) {
		m_ServerInfo[INFOI_CENTER].usStatus = STATUSU_OKAY;
		return m_pUIHandler->OnHandle(PAK_EVENT_SYNC, reinterpret_cast<uintptr_t>(m_ServerInfo + INFOI_CENTER), DATA_INDEX_CENTER);
	}
	return true;
}

bool CCenterServer::SyncSelectServerInfo(void)
{
	if (m_ServerInfo[INFOI_SELECT].usStatus == STATUSU_SYNC) {
		m_ServerInfo[INFOI_SELECT].usStatus = STATUSU_OKAY;
		return m_pUIHandler->OnHandle(PAK_EVENT_SYNC, reinterpret_cast<uintptr_t>(m_ServerInfo + INFOI_SELECT), DATA_INDEX_SELECT);
	}
	return true;
}

bool CCenterServer::SyncLoginServerInfo(void)
{
	// 3. TODO!!!��ôû�п������ݳ��������󱣴����޵�����, ����ֻ��ʹ��jumbo buffer����, 
	//    �������������ֶ��buffer/jumbo buffer����, Ĭ��jumbo buffer������෢��ͬ����120����������ȫ����Ϣ,
	//    Ŀǰͬ������ֻ�������ķ�����ͬ����������Ϣ��ѡ��͵�½ʱ����, ���е�½������ͬ����Ϸ��������Ϣ���ͻ���
	if (m_ServerInfo[INFOI_LOGIN].usStatus == STATUSU_SYNC) {
		DEV_INFO(TF("[���ķ�����]ͬ����½��������Ϣ������ѡ�������"));
		CStreamScopePtr StreamPtr;
		if (m_NetworkPtr->AllocJumboBuffer(StreamPtr)) {
			CCenterSync Sync;
			Sync.SetServerData(m_ServerInfo[INFOI_LOGIN]);
			Sync.Serialize(*StreamPtr);

			bool bRet = false;
			{ CSyncLockWaitScope scope(m_LoginSvrMap.GetLock()); bRet = m_LoginSvrMap.Serialize(*StreamPtr); }
			if (bRet) {
				if (m_pShareSelectSvr != nullptr) {
					m_pShareSelectSvr->OnShareRoutine(PAK_EVENT_SYNC, *StreamPtr);
				}
				if (m_NetworkPtr->Send(nullptr, *StreamPtr, SEND_BROADCAST_AS, PAK_TYPE_SELECT)) { // �㲥������ѡ�������
					CSyncLockWaitScope scope(m_LoginSvrMap.GetLock());
					m_LoginSvrMap.Update();
					m_ServerInfo[INFOI_LOGIN].usStatus = STATUSU_OKAY;
				}
			}
			return m_pUIHandler->OnHandle(PAK_EVENT_SYNC, reinterpret_cast<uintptr_t>(m_ServerInfo + INFOI_LOGIN), DATA_INDEX_LOGIN);
		}
		return false;
	}
	else {
		DEV_INFO(TF("[���ķ�����]ͬ����½��������Ϣ������ѡ�����������û��ͬ������"));
	}
	return true;
}

bool CCenterServer::SyncGameServerInfo(void)
{
	// 3. TODO!!!��ôû�п������ݳ��������󱣴����޵�����, ����ֻ��ʹ��jumbo buffer����, 
	//    �������������ֶ��buffer/jumbo buffer����, Ĭ��jumbo buffer������෢��ͬ����120����������ȫ����Ϣ,
	//    Ŀǰͬ������ֻ�������ķ�����ͬ����������Ϣ��ѡ��͵�½ʱ����, ���е�½������ͬ����Ϸ��������Ϣ���ͻ���
	if (m_ServerInfo[INFOI_GAME].usStatus == STATUSU_SYNC) {
		DEV_INFO(TF("[���ķ�����]ͬ����Ϸ��������Ϣ����½������"));
		CStreamScopePtr StreamPtr;
		if (m_NetworkPtr->AllocJumboBuffer(StreamPtr)) {
			CCenterSync Sync;
			Sync.SetServerData(m_ServerInfo[INFOI_GAME]);
			Sync.Serialize(*StreamPtr);

			bool bRet = false;
			{ CSyncLockWaitScope scope(m_GameSvrMap.GetLock()); bRet = m_GameSvrMap.Serialize(*StreamPtr); }
			if (bRet) {
				if (m_pShareLoginSvr != nullptr) {
					m_pShareLoginSvr->OnShareRoutine(PAK_EVENT_SYNC, *StreamPtr);
				}
				if (m_NetworkPtr->Send(nullptr, *StreamPtr, SEND_BROADCAST_AS, PAK_TYPE_LOGIN_CENTER)) { // �㲥�����е�½������
					CSyncLockWaitScope scope(m_GameSvrMap.GetLock());
					m_GameSvrMap.Update();
					m_ServerInfo[INFOI_GAME].usStatus = STATUSU_OKAY;
				}
			}
			return m_pUIHandler->OnHandle(PAK_EVENT_SYNC, reinterpret_cast<uintptr_t>(m_ServerInfo + INFOI_GAME), DATA_INDEX_GAME);
		}
		return false;
	}
	else {
		DEV_INFO(TF("[���ķ�����]ͬ����Ϸ��������Ϣ����½����������û��ͬ������"));
	}
	return true;
}

bool CCenterServer::SendLoginServerInfo(KeyRef krSocket)
{
	// 3. TODO!!!��ôû�п������ݳ��������󱣴����޵�����, ����ֻ��ʹ��jumbo buffer����, 
	//    �������������ֶ��buffer/jumbo buffer����, Ĭ��jumbo buffer������෢��ͬ����120����������ȫ����Ϣ,
	//    Ŀǰͬ������ֻ�������ķ�����ͬ����������Ϣ��ѡ��͵�½ʱ����, ���е�½������ͬ����Ϸ��������Ϣ���ͻ���
	CStreamScopePtr StreamPtr;
	if (m_NetworkPtr->AllocJumboBuffer(StreamPtr)) {
		CCenterSync Sync;
		Sync.SetServerData(m_ServerInfo[INFOI_LOGIN]);
		Sync.Serialize(*StreamPtr);

		bool bRet = false;
		{ CSyncLockWaitScope scope(m_LoginSvrMap.GetLock()); bRet = m_LoginSvrMap.Serialize(*StreamPtr, STATUSU_OKAYSYNC); }
		if (bRet){
			if (krSocket != nullptr) {
				m_NetworkPtr->Send(krSocket, *StreamPtr);
			}
			else {
				assert(m_pShareSelectSvr != nullptr);
				m_pShareSelectSvr->OnShareRoutine(PAK_EVENT_SYNC, *StreamPtr);
			}
		}
		return true;
	}
	return false;
}

bool CCenterServer::SendGameServerInfo(KeyRef krSocket)
{
	// 3. TODO!!!��ôû�п������ݳ��������󱣴����޵�����, ����ֻ��ʹ��jumbo buffer����, 
	//    �������������ֶ��buffer/jumbo buffer����, Ĭ��jumbo buffer������෢��ͬ����120����������ȫ����Ϣ,
	//    Ŀǰͬ������ֻ�������ķ�����ͬ����������Ϣ��ѡ��͵�½ʱ����, ���е�½������ͬ����Ϸ��������Ϣ���ͻ���
	CStreamScopePtr StreamPtr;
	if (m_NetworkPtr->AllocJumboBuffer(StreamPtr)) {
		CCenterSync Sync;
		Sync.SetServerData(m_ServerInfo[INFOI_GAME]);
		Sync.Serialize(*StreamPtr);

		bool bRet = false;
		{ CSyncLockWaitScope scope(m_GameSvrMap.GetLock()); bRet = m_GameSvrMap.Serialize(*StreamPtr, STATUSU_OKAYSYNC); }
		if (bRet){
			if (krSocket != nullptr) {
				m_NetworkPtr->Send(krSocket, *StreamPtr);
			}
			else {
				assert(m_pShareLoginSvr != nullptr);
				m_pShareLoginSvr->OnShareRoutine(PAK_EVENT_SYNC, *StreamPtr);
			}
		}
		return true;
	}
	return false;
}




