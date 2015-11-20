//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Source File : SelectServerImp.cpp                          //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ѡ�����������ʵ��                                 //
//                                                              //
//////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SelectServerImp.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSelectServer
UInt CSelectServer::Command(PCXStr pszCMD, uintptr_t utParam)
{
	// �������ļ������������Ƹ�����ز���
	UNREFERENCED_PARAMETER(pszCMD);
	UNREFERENCED_PARAMETER(utParam);
	return (UInt)RET_OKAY;
}

UInt CSelectServer::Update(void)
{
	CheckConnectCenterServer();
	CheckUDPService();
	SyncServerInfo();
	return 0;
}
//--------------------------------------
UInt CSelectServer::Init(CEventHandler& EventHandlerRef)
{
	if (m_nStatus == STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[ѡ�������]ѡ������ʼ����ʼ!"));
		m_pUIHandler = &EventHandlerRef;

		if ((InitLoadShare() == false) ||
			(InitLoadConfig() == false)) {
			return (UInt)RET_FAIL;
		}

		m_nStatus = STATUSC_INIT;
		LOG_INFO(m_FileLog, TF("[ѡ�������]ѡ������ʼ�����!"));
		return (UInt)RET_OKAY;
	}
	return (UInt)RET_FAIL;
}
// ��ȡ��������ö�����������
bool CSelectServer::InitLoadShare(void)
{
	assert(m_pConfig == nullptr);
	m_pUIHandler->OnHandle(CServerConfig::CFG_DEFAULT_CONFIG, reinterpret_cast<uintptr_t>(&m_pConfig), DATA_INDEX_SELECT);
	if (m_pConfig == nullptr) {
		LOG_ERROR(m_FileLog, TF("[ѡ�������]��ͬ���̹������ݻص��ӿڻ�ȡ���ö���ָ����Ч"));
		return false;
	}
	LOG_INFO(m_FileLog, TF("[ѡ�������]�ӽ���ص��ӿڻ�ȡ���ö���ָ�����!"));
	m_pConfig->Dump(m_FileLog); // ������Ϣ�������־�ļ�

	// �Ӷ�̬�������ȡ���������ָ��
	assert(m_NetworkPtr == nullptr);
	CKeyValue::VALUE xValue = { 0 };
	m_pConfig->GetKeyValue()->GetValue(CServerConfig::NetworkPtr, xValue);
	if (xValue.pValue == nullptr) {
		LOG_ERROR(m_FileLog, TF("[ѡ�������]�Ӷ�̬�������ȡ���������ָ����Ч"));
		return false;
	}
	LOG_INFO(m_FileLog, TF("[ѡ�������]�Ӷ�̬�������ȡ���������ָ�����!"));
	m_NetworkPtr = *(reinterpret_cast<CNetworkPtr*>(xValue.pValue));
	return true;
}
// ��ʼ������
bool CSelectServer::InitLoadConfig(void)
{
	assert(m_krConnectCenter == nullptr);
	assert(m_krUDPService == nullptr);

	assert(m_pShareCenterSvr == nullptr);

	assert(m_LoginSvrMap.Size() == 0);
	m_ServerInfo.usId   = m_pConfig->GetServerId(CServerConfig::CFG_DEFAULT_SELECT);
	m_ServerInfo.usIncr = m_pConfig->GetServerIncr(CServerConfig::CFG_DEFAULT_SELECT);
	LOGV_INFO(m_FileLog, TF("[ѡ�������]�����ö����ȡ������Id=%d, Incr=%d!"), m_ServerInfo.usId, m_ServerInfo.usIncr);
	if (m_pConfig->GetNetworkAttr() & ATTR_IPV6) {
		m_ServerInfo.NetAddr[SELECTI_UDP].usAttr = ATTR_IPV6;
	}

	UShort     usPort = 0;
	CStringKey strAddr;
	m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_SELECT, 0, strAddr, usPort);
	m_NetworkPtr->TranslateAddr(strAddr, usPort, m_ServerInfo.NetAddr[SELECTI_UDP]); // client udp
	return true;
}
//--------------------------------------
void CSelectServer::Exit(void)
{
	if (m_nStatus != STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[ѡ�������]ѡ������˳�����ʼ!"));
		m_pUIHandler = nullptr;

		ExitUnloadConfig();
		ExitUnloadShare();

		m_nStatus = STATUSC_NONE;
		LOG_INFO(m_FileLog, TF("[ѡ�������]ѡ������˳��������!"));
	}
}
// �����������ö�����������
void CSelectServer::ExitUnloadShare(void)
{
	m_pConfig    = nullptr;
	m_NetworkPtr = nullptr;
}
//
void CSelectServer::ExitUnloadConfig(void)
{
	assert(m_krConnectCenter == nullptr);
	assert(m_krUDPService == nullptr);

	assert(m_pShareCenterSvr == nullptr);

	assert(m_LoginSvrMap.Size() == 0);
}
//--------------------------------------
bool CSelectServer::Start(void)
{
	if (m_nStatus == STATUSC_INIT) {
		LOG_INFO(m_FileLog, TF("[ѡ�������]ѡ�����������ʼ!"));

		if ((StartConnectCenterServer() == false) ||
			(StartUDPService() == false)) {
			return false;
		}

		m_nStatus = STATUSC_RUN;
		LOG_INFO(m_FileLog, TF("[ѡ�������]ѡ������������!"));
	}
	return true;
}
// ���д����������ķ����������Ӷ���
bool CSelectServer::StartConnectCenterServer(void)
{
	// ѡ��������ڲ�ͬ����,  ��Ҫ�����������ķ�����
	if ((m_pConfig->GetLoadServers() & CServerConfig::CFG_DEFAULT_CENTER) == 0) {
		if (m_krConnectCenter == nullptr) {
			UShort     usPort = 0;
			CStringKey strAddr;
			m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_SELECT, CServerConfig::CFG_DEFAULT_CENTER, strAddr, usPort);
			m_krConnectCenter = m_NetworkPtr->Create(*this, usPort, *strAddr);
		}
		if (m_krConnectCenter != nullptr) {
			if (m_bCenterCnnted == false) {
				LOG_INFO(m_FileLog, TF("[ѡ�������]ѡ������������ķ������ڲ�ͬ����, �����������ķ�����Socket�ɹ�"));
				UShort     usPort = 0;
				CStringKey strAddr;
				m_pConfig->GetServerAddr(CServerConfig::CFG_DEFAULT_CENTER, CServerConfig::CFG_DEFAULT_SELECT, strAddr, usPort);
				if (m_NetworkPtr->Connect(m_krConnectCenter, usPort, *strAddr) == false) {
					LOGV_ERROR(m_FileLog, TF("[ѡ�������]�������ķ�����[%s]:%d����ʧ��"), *strAddr, usPort);
					return false;
				}
				LOGV_INFO(m_FileLog, TF("[ѡ�������]�������ķ�����[%s]:%d�������"), *strAddr, usPort);
			}
		}
		else {
			LOG_ERROR(m_FileLog, TF("[ѡ�������]ѡ������������ķ������ڲ�ͬ����, �����������ķ�����Socketʧ��"));
			return false;
		}
	}
	else if (m_bCenterCnnted == false) {
		assert(m_pShareCenterSvr == nullptr);
		CKeyValue::VALUE xValue = { 0 };
		m_pConfig->GetKeyValue()->GetValue(CServerConfig::CenterServer, xValue);
		if (xValue.pValue == nullptr) {
			LOG_ERROR(m_FileLog, TF("[ѡ�������]�Ӷ�̬�������ȡͬ�������ķ���������ָ����Ч"));
			return false;
		}
		m_pShareCenterSvr = reinterpret_cast<ICommonServer*>(xValue.pValue);
		m_bCenterCnnted   = true;

		LOG_INFO(m_FileLog, TF("[ѡ�������]ͬ����ֱ���������ķ�����"));
		m_ServerInfo.usStatus = STATUSU_LINK;

		CSelectLink Link;
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
// ���д���UDP�����ͻ���ѡ����е�½�����������Ӷ���
bool CSelectServer::StartUDPService(void)
{
	// ������UDP, ���ڿͻ���UDP�����½��������ַ
	if (m_krUDPService == nullptr) {
		m_krUDPService = m_NetworkPtr->Create(*this, m_ServerInfo.NetAddr[SELECTI_UDP], SOCKET_UDP);
		if (m_krUDPService != nullptr) {
			LOGV_INFO(m_FileLog, TF("[ѡ�������]��������UDP����ɹ�"));
		}
		else {
			LOGV_ERROR(m_FileLog, TF("[ѡ�������]��������UDP����ʧ��"));
			return false;
		}
	}
	return (m_krUDPService != nullptr);
}
//--------------------------------------
bool CSelectServer::Pause(bool bPause)
{
	if (bPause && (m_nStatus == STATUSC_RUN)) {
		m_nStatus = STATUSC_PAUSE; 
		LOG_INFO(m_FileLog, TF("[ѡ�������]��ͣUDP����"));
		return true;
	}
	else if ((bPause == false) && (m_nStatus == STATUSC_PAUSE)) {
		m_nStatus = STATUSC_RUN;   
		LOG_INFO(m_FileLog, TF("[ѡ�������]����UDP����"));
		return true;
	}
	return false;
}
//--------------------------------------
void CSelectServer::Stop(void)
{
	if (m_nStatus > STATUSC_NONE) {
		LOG_INFO(m_FileLog, TF("[ѡ�������]ѡ�����ֹͣ��ʼ!"));

		StopUDPService();
		StopConnectCenterServer();

		m_ServerInfo.Zero();
		m_LoginInfo.Reset();
		m_SortChain.Reset();
		m_LoginSvrMap.RemoveAll();

		m_nStatus = STATUSC_INIT;
		LOG_INFO(m_FileLog, TF("[ѡ�������]ѡ�����ֹͣ���!"));
	}
}
// ֹͣ�������ķ�����
void CSelectServer::StopConnectCenterServer(void)
{
	if (m_krConnectCenter != nullptr) {
		m_NetworkPtr->Destroy(m_krConnectCenter, false);
		m_krConnectCenter = nullptr;
		LOG_INFO(m_FileLog, TF("[ѡ�������]�����������ķ����������ӳɹ�"));
	}
	m_bCenterCnnted   = false;
	m_bCenterLinked   = false;
	m_pShareCenterSvr = nullptr;
}
// ֹͣUDP�����ͻ�������
void CSelectServer::StopUDPService(void)
{
	if (m_krUDPService != nullptr) {
		m_NetworkPtr->Destroy(m_krUDPService, false);
		m_krUDPService = nullptr;
		LOG_INFO(m_FileLog, TF("[ѡ�������]����UDP��������ӳɹ�"));
	}
}
//--------------------------------------
bool CSelectServer::OnShareRoutine(Int nEvent, CStream& Stream, LLong)
{
	assert((m_pConfig->GetLoadServers() & CServerConfig::CFG_DEFAULT_CENTER) != 0);
	if (nEvent == PAK_EVENT_SYNC) {
		LOGV_INFO(m_FileLog, TF("[ѡ�������]ͬ����ͬ����¼��������Ϣ"));
		CStreamScopePtr StreamPtr;
		if (m_NetworkPtr->ReferBuffer(StreamPtr, Stream)) { // ͬ���̵�Stream��дģʽ, ����һ����ģʽ������
			CPAKSync Sync;
			Sync.Serialize(*StreamPtr);

			m_LoginInfo.Copy(Sync.GetServerData());
			SyncLoginSortInfo(*StreamPtr);
		}
		else {
			LOGV_WARN(m_FileLog, TF("[ѡ�������]ͬ����ͬ����¼��������Ϣ����"));
		}
	}
	else {
		LOGV_WARN(m_FileLog, TF("[ѡ�������]ͬ���̷������޷�ʶ������������[event=%d]"), nEvent);
		assert(0);
	}
	return true;
}
//--------------------------------------
bool CSelectServer::OnTcpDispatch(const PacketPtr& PktPtr, PTCP_PARAM pTcp)
{
	// PTCP_PARAM->pCache == nullptr, �ϲ���Ա���ӵײ�����Ļ�������
	// PTCP_PARAM->pCache, PTCP_PARAM->index�ǵײ�����Ļ������ݵ���ʼ��ַ��������
	// ����ϲ㱣�滺��, ����PTCP_PARAM->pDataΪnullptr֪ͨ����ײ�
	// �ͷŻ���ʹ��MObject::sMCFree(index, pCache)
	assert(pTcp->krSocket == m_krConnectCenter);
	assert(PktPtr->GetType() == PAK_TYPE_CENTER);
	switch (PktPtr->GetEvent()) {
	case PAK_EVENT_LINKACK:
		{
			m_bCenterLinked = true;

			NET_ADDR NetAddr;
			m_NetworkPtr->GetAddr(pTcp->krSocket, NetAddr, false);
			m_pUIHandler->OnHandle(PAK_EVENT_LINK, reinterpret_cast<uintptr_t>(&NetAddr), DATA_INDEX_CENTER);
			LOG_INFO(m_FileLog, TF("[ѡ�������]�յ����ķ��������ӻظ���"));
		}
		break;
	case PAK_EVENT_UPDATEACK:
		{
			DEV_INFO(TF("[ѡ�������]�յ����ķ��������»ظ���"));
		}
		break;
	case PAK_EVENT_SYNC:
		{
			DEV_INFO(TF("[ѡ�������]���ķ�����ͬ����½��������Ϣ"));
			CPAKSync* pSync = static_cast<CPAKSync*>(PktPtr.Get());
			m_LoginInfo.Copy(pSync->GetServerData());

			if (pSync->CheckStream()) {
				SyncLoginSortInfo(pSync->GetStream());
			}
			// ����ͬ����ִ��
			CPAKSimple<PAK_EVENT_SYNCACK, PAK_TYPE_SELECT> SyncAck;
			SyncAck.AdjustSize();
			m_NetworkPtr->Send(pTcp->krSocket, SyncAck);
		}
		break;
	case PAK_EVENT_UNLINKACK:
		{
			m_bCenterLinked = false;
			LOG_INFO(m_FileLog, TF("[ѡ�������]�յ����ķ������Ͻӻظ���"));
		}
		break;
	case PAK_EVENT_LIVEACK:
		{
		}
		break;
	default:
		{
			LOGV_WARN(m_FileLog, TF("[ѡ�������]%p������Ϣ�޷�ʶ������ķ��������������[event=%d]"), pTcp->krSocket, PktPtr->GetEvent());
		}
	}
	return true;
}
//--------------------------------------
bool CSelectServer::OnUdpDispatch(const PacketPtr& PktPtr, PUDP_PARAM pUdp)
{
	// PUDP_PARAM->pCache == nullptr, �ϲ���Ա���ӵײ�����Ļ�������
	// PUDP_PARAM->pCache, PUDP_PARAM->index�ǵײ�����Ļ������ݵ���ʼ��ַ��������
	// ����ϲ㱣�滺��, ����PUDP_PARAM->pDataΪnullptr֪ͨ����ײ�
	// �ͷŻ���ʹ��MObject::sMCFree(index, pCache)
	bool bRet = false;
	if ((PktPtr->GetType() == PAK_TYPE_CLIENT_READY) && (PktPtr->GetEvent() == PAK_EVENT_SELECT_LOGIN)) {
		m_ServerInfo.Incr();
		// 1. �ж�IP�Ƿ��ں�����
		if (CheckAddrBlacklist(pUdp->NetAddr) == false) {
			// TODO!!! �ͻ��˿���ͨ��DNS���ܽ�������ͬ�����ѡ�������
			CPAKSelectLoginAck SelectLoginAck;
			PSORT_RESULT pSortResult = m_SortChain.GetCur();
			if (pSortResult->lCount > 0) {
				Long lIndex = pSortResult->Index();
				DEV_INFO(TF("[ѡ�������]ѡȡ��%d����½������"), lIndex);
				SelectLoginAck.SetAddr(pSortResult->Sort[lIndex].Addr);
				SelectLoginAck.SetAck(DATAD_OKAY);
			}
			else {
				SelectLoginAck.SetAck(SELECT_ERROR_NONE_LOGIN);
				DEV_INFO(TF("[ѡ�������]û�е�¼��������Ϣ����"));
			}
			SelectLoginAck.AdjustSize();
			bRet = m_NetworkPtr->SendTo(pUdp->krSocket, SelectLoginAck, pUdp->NetAddr);
		}
		//
		m_ServerInfo.Decr();
	}
	else {
		LOGV_WARN(m_FileLog, TF("[ѡ�������]�޷�ʶ���UDP���������[type=%d, event=%d]"), PktPtr->GetType(), PktPtr->GetEvent());
	}
	return bRet;
}

bool CSelectServer::CheckAddrBlacklist(NET_ADDR& ClientAddr)
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
bool CSelectServer::OnTcpConnect(UInt uError, KeyRef krConnect)
{
	UNREFERENCED_PARAMETER(krConnect);
	assert(krConnect == m_krConnectCenter);
	m_bCenterCnnted = (uError == 0);
	if (m_bCenterCnnted) {
		LinkCenterServer();
	}
	else {
		DEV_INFO(TF("[ѡ�������]�������ķ�����ʧ��%X!"), uError);
		LOGV_WARN(m_FileLog, TF("[ѡ�������]�������ķ�����ʧ��%X!"), uError);
	}
	return true;
}
// �������ķ�����
void CSelectServer::LinkCenterServer(void)
{
	if (m_bCenterCnnted && (m_bCenterLinked == false)) {
		m_ServerInfo.usStatus = STATUSU_LINK;

		CSelectLink Link;
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
			DEV_WARN(TF("[ѡ�������]�������ķ��������������������쳣!"));
			LOG_WARN(m_FileLog, TF("[ѡ�������]�������ķ��������������������쳣!"));
		}
	}
}
// �������ķ�����
void CSelectServer::UnlinkCenterServer(void)
{
	if (m_bCenterCnnted && m_bCenterLinked) {
		m_bCenterLinked = false;
		CPAKSimple<PAK_EVENT_UNLINK, PAK_TYPE_SELECT> Unlink;
		Unlink.AdjustSize();
		m_NetworkPtr->Send(m_krConnectCenter, Unlink);
	}
}
//--------------------------------------
bool CSelectServer::OnTcpClose(KeyRef krSocket, LLong)
{
	if (krSocket == m_krConnectCenter) {
		m_krConnectCenter = nullptr;
		m_bCenterCnnted   = false;
		m_bCenterLinked   = false;
		DEV_INFO(TF("[ѡ�������]�������ķ��������Ӷ���/�Ͽ�"));

		m_SortChain.Reset();
		{ CSyncLockWaitScope scope(m_LoginSvrMap.GetLock()); m_LoginSvrMap.RemoveAll(); }
		m_pUIHandler->OnHandle(PAK_EVENT_UNLINK, 0, DATA_INDEX_CENTER);
	}
	return true;
}
//--------------------------------------
bool CSelectServer::OnUdpClose(KeyRef krSocket, LLong)
{
	if (krSocket == m_krUDPService) {
		m_krUDPService = nullptr;
		DEV_INFO(TF("[ѡ�������]ѡ��UDP�ر�"));
	}
	return true;
}
//--------------------------------------
// ��ʱ���������ķ��������ӵ����Ӷ����Ƿ���Ч
bool CSelectServer::CheckConnectCenterServer(void)
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
// ��ʱ���UDP�����ͻ��˵����Ӷ����Ƿ���Ч
bool CSelectServer::CheckUDPService(void)
{
	if (m_krUDPService == nullptr) {
		return StartUDPService();
	}
	return true;
}
// ͬ����������Ϣ������
bool CSelectServer::SyncServerInfo(void)
{
	if (m_ServerInfo.usStatus == STATUSU_SYNC) {
		CSelectUpdate Update;
		Update.SetServerData(m_ServerInfo);
		Update.AdjustSize();
		if (m_pShareCenterSvr != nullptr) {
			m_pShareCenterSvr->OnShareRoutine(PAK_EVENT_UPDATE, Update, reinterpret_cast<LLong>(this));
		}
		else {
			m_NetworkPtr->Send(m_krConnectCenter, Update);
		}
		m_ServerInfo.usStatus = STATUSU_OKAY;
		return m_pUIHandler->OnHandle(PAK_EVENT_SYNC, reinterpret_cast<uintptr_t>(&m_ServerInfo), DATA_INDEX_SELECT);
	}
	else if (m_pShareCenterSvr == nullptr) { // ��������
		CPAKSimple<PAK_EVENT_LIVE, PAK_TYPE_SELECT> Live;
		Live.AdjustSize();
		m_NetworkPtr->Send(m_krConnectCenter, Live);
	}
	return true;
}
// ͬ����½�����������������
bool CSelectServer::SyncLoginSortInfo(CStream& Stream)
{
	LLong llPos = (LLong)Stream.Tell();
	m_pUIHandler->OnHandle(PAK_EVENT_SYNC, Stream, DATA_INDEX_LOGIN);
	m_pUIHandler->OnHandle(PAK_EVENT_SYNC, reinterpret_cast<uintptr_t>(&m_LoginInfo), DATA_INDEX_LOGIN);

	Stream.Seek(llPos); // ��ת�����ݿ�ʼ�ĵط�

	Int nCount = 0;
	PSORT_RESULT pSortResult = m_SortChain.GetNext();
	{
		CSyncLockWaitScope scope(m_LoginSvrMap.GetLock());
		m_LoginSvrMap.Serialize(Stream, STATUSU_UPDATE);

		PINDEX index  = m_LoginSvrMap.First();
		while ((index != nullptr) && (nCount < SORT_RESULT::SORTC_LIMIT)) {
			SVR_LOGIN_MAP::SVR_MAP_PAIR* pPair = m_LoginSvrMap.Next(index);
			pSortResult->Sort[nCount].usBusy   = pPair->m_V.usBusy;
			pSortResult->Sort[nCount].usIncr   = pPair->m_V.usIncr;
			pSortResult->Sort[nCount].drServer = pPair->m_K;
			pSortResult->Sort[nCount].Addr     = pPair->m_V.NetAddr[LOGINI_TCP];
			++nCount;
		}
	}
	if (nCount > 1) {
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
	m_SortChain.Swap(); // ������½���������, ����ǰ���µ���Ϊ��Ч����
	LOGV_INFO(m_FileLog, TF("[ѡ�������]���µ�½�������������������%d����½��������Ϣ"), nCount);
	return (nCount > 0);
}




