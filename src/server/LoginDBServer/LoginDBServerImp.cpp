//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Source File : LoginDBServerImp.cpp                         //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update : 2015-11-25     version 0.0.0.5                    //
//   Detail : ��½DB������ʵ��                                   //
//                                                              //
//////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LoginDBServerImp.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLoginDBServer
UInt CLoginDBServer::Command(PCXStr pszCMD, uintptr_t utParam)
{
	// �������ļ������������Ƹ�����ز���
	UNREFERENCED_PARAMETER(pszCMD);
	UNREFERENCED_PARAMETER(utParam);
	return (UInt)RET_OKAY;
}

UInt CLoginDBServer::Update(void)
{
	CheckListenLoginServer();
	SyncServerInfo();
	return 0;
}
//--------------------------------------
UInt CLoginDBServer::Init(CEventHandler& EventHandlerRef)
{
	if (m_nStatus == STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[��½DB������]��½DB�����ʼ����ʼ!"));
		m_pUIHandler = &EventHandlerRef;

		if ((InitLoadShare() == false) ||
			(InitLoadConfig() == false)) {
			return (UInt)RET_FAIL;
		}

		m_nStatus = STATUSC_INIT;
		LOG_INFO(m_FileLog, TF("[��½DB������]��½DB�����ʼ�����!"));
		return (UInt)RET_OKAY;
	}
	return (UInt)RET_FAIL;
}

bool CLoginDBServer::InitLoadShare(void)
{
	assert(m_pConfig == nullptr);
	m_pUIHandler->OnHandle(CServerConfig::CFG_CONFIG_PTR, reinterpret_cast<uintptr_t>(&m_pConfig), DATA_INDEX_LOGINDB);
	if (m_pConfig == nullptr) {
		LOG_ERROR(m_FileLog, TF("[��½DB������]��ͬ���̹������ݻص��ӿڻ�ȡ���ö���ָ����Ч"));
		return false;
	}
	LOG_INFO(m_FileLog, TF("[��½DB������]�ӽ���ص��ӿڻ�ȡ���ö���ָ�����!"));
	m_pConfig->Dump(m_FileLog); // ������Ϣ�������־�ļ�

	// �Ӷ�̬�������ȡ���������ָ��
	assert(m_NetworkPtr == nullptr);
	CKeyValue::VALUE xValue = { 0 };
	m_pConfig->GetKeyValue()->GetValue(CServerConfig::NetworkPtr, xValue);
	if (xValue.pValue == nullptr) {
		LOG_ERROR(m_FileLog, TF("[��½DB������]�Ӷ�̬�������ȡ���������ָ����Ч"));
		return false;
	}
	LOG_INFO(m_FileLog, TF("[��½DB������]�Ӷ�̬�������ȡ���������ָ�����!"));
	m_NetworkPtr = *(reinterpret_cast<CNetworkPtr*>(xValue.pValue));
	// �����½DB������ָ�뵽��̬������
	LOGV_INFO(m_FileLog, TF("[��½DB������]�����½DB������ָ�뵽��̬���������[key=%s]!"), CServerConfig::LoginDBServer);
	m_pConfig->GetKeyValue()->AddItem(CServerConfig::LoginDBServer, this);
	return true;
}

bool CLoginDBServer::InitLoadConfig(void)
{
	assert(m_krListenLogin == nullptr);
	assert(m_pShareLoginSvr == nullptr);

	assert(m_LoginSvrMap.Size() == 0);
	m_ServerInfo.usId   = m_pConfig->GetServerId(CServerConfig::CFG_DEFAULT_LOGINDB);
	m_ServerInfo.usIncr = m_pConfig->GetServerIncr(CServerConfig::CFG_DEFAULT_LOGINDB);
	LOGV_INFO(m_FileLog, TF("[��½DB������]�����ö����ȡ������Id=%d, Incr=%d!"), m_ServerInfo.usId, m_ServerInfo.usIncr);

	assert(m_LoginDBRoutine == nullptr);
	m_LoginDBRoutine = (ICommonRoutine*)CRTTI::CreateByName(TF("CLoginDBRoutine"));
	if (m_LoginDBRoutine != nullptr) {
		return (m_LoginDBRoutine->Init() == (UInt)RET_OKAY);
	}
	LOG_ERROR(m_FileLog, TF("[��½DB������]�������������ʧ��"));
	return false;
}
//--------------------------------------
void CLoginDBServer::Exit(void)
{
	if (m_nStatus != STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[��½DB������]��½DB�����˳�����ʼ!"));
		m_pUIHandler = nullptr;

		ExitUnloadConfig();
		ExitUnloadShare();

		m_nStatus = STATUSC_NONE;
		LOG_INFO(m_FileLog, TF("[��½DB������]��½DB�����˳��������!"));
	}
}

void CLoginDBServer::ExitUnloadShare(void)
{
	m_pConfig    = nullptr;
	m_NetworkPtr = nullptr;
}

void CLoginDBServer::ExitUnloadConfig(void)
{
	assert(m_krListenLogin == nullptr);
	assert(m_pShareLoginSvr == nullptr);

	assert(m_LoginSvrMap.Size() == 0);
	LOGV_INFO(m_FileLog, TF("[��½DB������]�Ӷ�̬�����������½DB������ָ�����[key=%s]!"), CServerConfig::LoginDBServer);
	m_pConfig->GetKeyValue()->Remove(CServerConfig::LoginDBServer);

	m_LoginDBRoutine->Exit();
	m_LoginDBRoutine = nullptr;
}
//--------------------------------------
bool CLoginDBServer::Start(void)
{
	if (m_nStatus == STATUSC_INIT) {
		LOG_INFO(m_FileLog, TF("[��½DB������]��½DB����������ʼ!"));

		if (StartListenLoginServer() == false){
			return false;
		}
		m_LoginDBRoutine->Start(this);

		m_nStatus = STATUSC_RUN;
		LOG_INFO(m_FileLog, TF("[��½DB������]��½DB�����������!"));
	}
	return true;
}

bool CLoginDBServer::StartListenLoginServer(void)
{
	UShort     usPort = 0;
	CStringKey strAddr;
	m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_LOGINDB, 0, strAddr, usPort);

	bool bRet = true;
	assert(m_krListenLogin == nullptr);
	m_krListenLogin = m_NetworkPtr->Create(*this, usPort, *strAddr);
	if (m_krListenLogin != nullptr) {
		bRet = m_NetworkPtr->Listen(m_krListenLogin);
		LOGV_INFO(m_FileLog, TF("[��½DB������]����������½����������[%s]:%d�ɹ�, %s"), *strAddr, usPort, bRet ? TF("���������ɹ�") : TF("��������ʧ��"));
	}
	else {
		LOGV_ERROR(m_FileLog, TF("[��½DB������]����������½����������[%s]:%dʧ��"), *strAddr, usPort);
		bRet = false;
	}
	return bRet;
}
//--------------------------------------
bool CLoginDBServer::Pause(bool bPause)
{
	if (bPause && (m_nStatus == STATUSC_RUN)) {
		m_nStatus = STATUSC_PAUSE; 
		LOG_INFO(m_FileLog, TF("[��½DB������]��ͣ��½������������"));
		m_LoginDBRoutine->Pause();
		return true;
	}
	else if ((bPause == false) && (m_nStatus == STATUSC_PAUSE)) {
		m_nStatus = STATUSC_RUN;   
		LOG_INFO(m_FileLog, TF("[��½DB������]�����½������������"));
		m_LoginDBRoutine->Pause(false);
		return true;
	}
	return false;
}
//--------------------------------------
void CLoginDBServer::Stop(void)
{
	if (m_nStatus > STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[��½DB������]��½DB����ֹͣ��ʼ!"));

		m_LoginDBRoutine->Stop();

		StopListenLoginServer();

		m_ServerInfo.Zero();
		m_LoginSvrMap.RemoveAll();

		m_nStatus = STATUSC_INIT;
		LOG_INFO(m_FileLog, TF("[��½DB������]��½DB����ֹͣ���!"));
	}
}

void CLoginDBServer::StopListenLoginServer(void)
{
	if (m_krListenLogin != nullptr) {
		m_NetworkPtr->Destroy(m_krListenLogin, false);
		m_krListenLogin = nullptr;
		LOG_INFO(m_FileLog, TF("[��½DB������]���ټ�����½����������ɹ�"));
	}
	m_pShareLoginSvr = nullptr;
}
//--------------------------------------
bool CLoginDBServer::OnShareRoutine(Int nEvent, CEventBase& EventRef, LLong llParam, CEventQueue::EVENT_TYPE eType)
{
	assert((m_pConfig->GetLoadServers() & CServerConfig::CFG_DEFAULT_LOGIN) != 0);
	switch (nEvent) {
	case PAK_EVENT_LINK:
		{
			OnShareLink(EventRef, llParam);
		}
		break;
	case PAK_EVENT_UPDATE:
		{
			OnShareUpdate(EventRef, llParam);
		}
		break;
	//case PAK_EVENT_UNLINK: // ͬ���̷�����ע��, ˵������ֹͣ, ����Ҫ����
	//	break;
	case PAK_EVENT_LOGIN_LINK:
	case PAK_EVENT_LOGIN_UNLINK:
		{
			m_LoginDBRoutine->Add(nEvent, EventRef, llParam, eType);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��½DB������]ͬ���̷������޷�ʶ������������[event=%d]"), nEvent);
			assert(0);
		}
	}
	return true;
}

bool CLoginDBServer::OnShareLink(CEventBase& EventRef, LLong llParam)
{
	CPAKLink* pLink = static_cast<CPAKLink*>(&EventRef);
	assert(pLink->GetType() == PAK_TYPE_LOGIN);
	if (m_pShareLoginSvr == nullptr) {
		// 0.��ù���ָ��
		m_pShareLoginSvr = reinterpret_cast<ICommonServer*>(llParam);
		LOGV_INFO(m_FileLog, TF("[��½DB������]ͬ���̵�½������[%p]ע��"), m_pShareLoginSvr);
		return ServerLink(pLink, (DataRef)llParam);
	}
	LOGV_WARN(m_FileLog, TF("[��½DB������]ͬ���̵�½�����������Ѿ�����[%p-%p]"), m_pShareLoginSvr, llParam);
	return false;
}

bool CLoginDBServer::OnShareUpdate(CEventBase& EventRef, LLong llParam)
{
	CPAKUpdate* pUpdate = static_cast<CPAKUpdate*>(&EventRef);
	assert(pUpdate->GetType() == PAK_TYPE_LOGIN);
	if (m_pShareLoginSvr != nullptr) {
		DEV_INFO(TF("[��½DB������]ͬ���̵�½������[%p]����"), m_pShareLoginSvr);
		assert(m_pShareLoginSvr == reinterpret_cast<ICommonServer*>(llParam));
		return ServerUpdate(pUpdate, (DataRef)llParam);
	}
	LOGV_WARN(m_FileLog, TF("[��½DB������]ͬ���̵�½����������δע��"));
	return false;
}
//--------------------------------------
bool CLoginDBServer::OnTcpDispatch(const PacketPtr& PktPtr, PTCP_PARAM pTcp)
{
	// PTCP_PARAM->pCache == nullptr, �ϲ���Ա���ӵײ�����Ļ�������
	// PTCP_PARAM->pCache, PTCP_PARAM->index�ǵײ�����Ļ������ݵ���ʼ��ַ��������
	// ����ϲ㱣�滺��, ����PTCP_PARAM->pDataΪnullptr֪ͨ����ײ�
	// �ͷŻ���ʹ��MObject::sMCFree(index, pCache)
	bool bRet = false;
	assert(PktPtr->GetType() == PAK_TYPE_LOGIN);
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
	case PAK_EVENT_LOGIN_LINK:
	case PAK_EVENT_LOGIN_UNLINK:
		{
			bRet = m_LoginDBRoutine->Add((Int)PktPtr->GetEvent(), *PktPtr, (LLong)pTcp->krSocket);
		}
		break;
	case PAK_EVENT_LIVE:
		{
			CPAKSimple<PAK_EVENT_LIVEACK, PAK_TYPE_LOGINDB> LiveAck;
			LiveAck.AdjustSize();
			bRet = m_NetworkPtr->Send(pTcp->krSocket, LiveAck);
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[��½DB������]%p�޷�ʶ��ĵ�½���������������[event=%d]"), pTcp->krSocket, PktPtr->GetEvent());
		}
	}
	return bRet;
}

bool CLoginDBServer::OnServerLink(CPAKLink* pLink, KeyRef krSocket)
{
	LOGV_INFO(m_FileLog, TF("[��½DB������]��½������[%p]ע��"), krSocket);
	if (ServerLink(pLink, (DataRef)krSocket)) {
		// 3.����Ϊ��½����������
		m_NetworkPtr->SetAttr(krSocket, PAK_TYPE_LOGIN);
		// 4.����ע���ִ��
		CPAKSimple<PAK_EVENT_LINKACK, PAK_TYPE_LOGINDB> LinkAck;
		LinkAck.AdjustSize();
		return m_NetworkPtr->Send(krSocket, LinkAck);
	}
	return false;
}

bool CLoginDBServer::OnServerUpdate(CPAKUpdate* pUpdate, KeyRef krSocket)
{
	DEV_INFO(TF("[��½DB������]��½������[%p]����"), krSocket);
	if (ServerUpdate(pUpdate, (DataRef)krSocket)) {
		// 3.���͸��»�ִ��
		CPAKSimple<PAK_EVENT_UPDATEACK, PAK_TYPE_LOGINDB> UpdateAck;
		UpdateAck.AdjustSize();
		return m_NetworkPtr->Send(krSocket, UpdateAck);
	}
	return false;
}

bool CLoginDBServer::OnServerUnlink(CPAKHead*, KeyRef krSocket)
{
	LOGV_INFO(m_FileLog, TF("[��½DB������]��½������[%p]ע��"), krSocket);
	if (ServerUnlink(krSocket)) {
		// 2.����Ϊ��Ч����������
		m_NetworkPtr->SetAttr(krSocket, PAK_TYPE_NONE);
		// 3.����ע����ִ��
		CPAKSimple<PAK_EVENT_UNLINKACK, PAK_TYPE_LOGINDB> UnlinkAck;
		UnlinkAck.AdjustSize();
		return m_NetworkPtr->Send(krSocket, UnlinkAck, SEND_CLOSE);
	}
	return false;
}
//--------------------------------------
bool CLoginDBServer::OnTcpAccept(KeyRef krAccept, KeyRef krListen)
{
	UNREFERENCED_PARAMETER( krListen );
	assert(krListen == m_krListenLogin);
	if (m_nStatus == STATUSC_RUN) {
		UShort     usPort = 0;
		CStringKey strAddr;
		m_NetworkPtr->GetAddr(krAccept, strAddr, usPort);

		DEV_INFO(TF("[��½DB������]��½������[%s]:%d�������!"), *strAddr, usPort);
		m_ServerInfo.Incr();
		return true;
	}
	else {
		LOGV_WARN(m_FileLog, TF("[��½DB������]��½DB������������ͣ״̬!"));
	}
	return false;
}
//--------------------------------------
bool CLoginDBServer::OnTcpClose(KeyRef krSocket, LLong llLiveData)
{
	if (krSocket == m_krListenLogin) {
		m_krListenLogin = nullptr;
		DEV_INFO(TF("[��½DB������]������½����������ر�"));
		LOG_INFO(m_FileLog, TF("[��½DB������]������½����������ر�"));
	}
	else { // �����������Ͽ�
		if (llLiveData == PAK_TYPE_LOGIN) {
			ServerUnlink(krSocket);
			LOGV_INFO(m_FileLog, TF("[��½DB������]��½������[%p]�Ͽ�"), krSocket);
		}
		m_ServerInfo.Decr();
	}
	return true;
}
//--------------------------------------
bool CLoginDBServer::CheckListenLoginServer(void)
{
	if (m_krListenLogin == nullptr) {
		return StartListenLoginServer();
	}
	return true;
}

bool CLoginDBServer::SyncServerInfo(void)
{
	if (m_ServerInfo.usStatus == STATUSU_SYNC) {
		m_LoginDBRoutine->Update();

		m_ServerInfo.usStatus = STATUSU_OKAY;
		return m_pUIHandler->OnHandle(PAK_EVENT_SYNC, reinterpret_cast<uintptr_t>(&m_ServerInfo), DATA_INDEX_LOGINDB);
	}
	return true;
}


