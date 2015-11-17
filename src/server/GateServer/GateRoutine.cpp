//////////////////////////////////////////////////////////////////
//   The Legend of Heros                                        //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Source File : GateRoutine.cpp                              //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ���ط���������ʵ��                                 //
//                                                              //
//////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GateServerImp.h"
#include "GateRoutine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGateRoutine
IMPLEMENT_RTTI_BEGIN_DUMP_CREATE( CGateRoutine, CObject, CObjectTraits::OBJECTT_TRANSIENT, 12 )
DUMP_RTTI_PTR( m_pServer,       VART_NONE, void )
DUMP_RTTI_STD( m_nQueueCur,     VART_INT,  Int )
DUMP_RTTI_STD( m_nQueueEnd,     VART_INT,  Int )
DUMP_RTTI_STD( m_nQueueLimit,   VART_INT,  Int )
DUMP_RTTI_STD( m_nGameCur,      VART_INT,  Int )
DUMP_RTTI_STD( m_nGameSelect,   VART_INT,  Int )
DUMP_RTTI_STD( m_nGameLimit,    VART_INT,  Int )
DUMP_RTTI_DAT( m_QueueRank,     VART_LIST, SESSION_RANK_LST, sizeof(SESSION_RANK_LST) )
DUMP_RTTI_DAT( m_SessionQueue,  VART_MAP,  SESSION_DATA_MAP, sizeof(SESSION_DATA_MAP) )
DUMP_RTTI_DAT( m_SessionLogin,  VART_MAP,  SESSION_DATA_MAP, sizeof(SESSION_DATA_MAP) )
DUMP_RTTI_DAT( m_SessionGame,   VART_MAP,  SESSION_DATA_MAP, sizeof(SESSION_DATA_MAP) )
DUMP_RTTI_DAT( m_EventQueuePtr, VART_REFCOUNT, CEventQueuePtr, sizeof(CEventQueuePtr) )
END_DUMP_RTTI( CGateRoutine, CObject, 12 )

bool CGateRoutine::OnHandle(Int nEvent, uintptr_t utParam, LLong llParam)
{
	if (nEvent == PAK_EVENT_LIVE) {// �ͻ�������, ���������
		CNETTraits::NET_ADDR* pAddr = reinterpret_cast<CNETTraits::NET_ADDR*>(utParam);
		return (CheckAddrBlacklist(*pAddr) == false);
	}
	else if (nEvent == PAK_EVENT_UNLINK) {
		// �ͻ��˶���, ֪ͨ��Ϸ�����������ɫ����
		utParam; // session-id
		llParam; // queue & select just remove user info
	}
	return true;
}

bool CGateRoutine::OnHandle(Int nEvent, CEventBase& EventRef, LLong llParam)
{
	switch (nEvent) {
	// gamedb
	case PAK_EVENT_GATE_SELECTACK:
		{
			SelectRoleAck(static_cast<CPAKSessionAck*>(&EventRef));
		}
		break;
	case PAK_EVENT_GATE_PLAYACK:
		{
			PlayRoleAck(static_cast<CPAKSessionAck*>(&EventRef));
		}
		break;
	//case PAK_EVENT_GATE_CREATEACK:
	//case PAK_EVENT_GATE_DELETEACK:
	//case PAK_EVENT_GATE_RECOVERACK:
	//	{
	//	}
	//	break;
	// game
	case PAK_EVENT_LOGIN_SELECT_GAME: // ѡ����Ϸ������
		{
			SelectGame(static_cast<CPAKLoginSelectGame*>(&EventRef), llParam);
		}
		break;
	// client
	case PAK_EVENT_LOGIN_LINK_GAME: // ������Ϸ������
		{
			LinkGame(static_cast<CPAKLoginLinkGame*>(&EventRef), (KeyRef)llParam);
		}
		break;
	case PAK_EVENT_GATE_QUEUE: // ��ǰ�Ŷ���Ų�ѯ
		{
			QueueRank(static_cast<CPAKSession*>(&EventRef), (KeyRef)llParam);
		}
		break;
	case PAK_EVENT_GATE_SELECT: // ��ɫѡ��
		{
			SelectRole(static_cast<CPAKGateSelect*>(&EventRef), (KeyRef)llParam);
		}
		break;
	case PAK_EVENT_GATE_PLAY: // ������Ϸ����
		{
			PlayRole(static_cast<CPAKGatePlay*>(&EventRef), (KeyRef)llParam);
		}
		break;
	default:
		{
			if ((nEvent > PAK_EVENT_REALM_BEGIN) && (nEvent < PAK_EVENT_REALM_END)) {
				// ����Ƿ���Ϸ����״̬, �ǵ�ת������Ϸ������
			}
		}
	}
	return true;
}

bool CGateRoutine::Start(class ICommonServer* pServer)
{
	m_pServer = static_cast<CGateServer*>(pServer);
	if (m_pServer != nullptr) {

		CStringFix strExtConfig;
		if (m_pServer->GetConfig()->GetServerExtConfig(CServerConfig::CFG_DEFAULT_GATE, strExtConfig)) {
			m_pServer->GetConfig()->GetFilePath()->GetFullPath(CFilePath::ModulePath, strExtConfig);
			CIni ini(*strExtConfig); // ����ʹ��xml, ������ݸ���
			if (ini.IsExist()) {
				m_nQueueLimit = (Int)ini.GetKeyIntValue(TF("GateRoutine"), TF("QueueLimit"), SESSION_LIMIT_QUEUE);
				m_nGameLimit  = (Int)ini.GetKeyIntValue(TF("GateRoutine"), TF("GameLimit"),  SESSION_LIMIT_GAME);
			}
		}

		// Ĭ�϶��д����߳�Ϊ1
		if (CEventQueue::EventQueue(m_EventQueuePtr, *this, 0, m_pServer->GetConfig()->GetServerQueue(CServerConfig::CFG_DEFAULT_GATE))) {
			return m_EventQueuePtr->Init();
		}
	}
	return false;
}

bool CGateRoutine::Pause(bool)
{
	return false;
}

bool CGateRoutine::Update(void)
{
	CheckSessionTimeout();
	return true;
}

void CGateRoutine::Stop(void)
{
	if (m_EventQueuePtr != nullptr) {
		m_EventQueuePtr->Exit();
		m_EventQueuePtr = nullptr;
	}
	m_pServer = nullptr;
}

bool CGateRoutine::Add(Int nEvent, CEventBase& EventRef, LLong llParam, CEventQueue::EVENT_TYPE eType)
{
	return m_EventQueuePtr->Add(nEvent, EventRef, llParam, eType);
}
//--------------------------------------
bool CGateRoutine::CheckAddrBlacklist(CNETTraits::NET_ADDR& ClientAddr)
{
	// TODO!!! ������У��δ���
	size_t stHash = 0;
	if (m_pServer->GetConfig()->GetNetworkAttr() & CNETTraits::ATTR_IPV6) {
		stHash = CHash::Hash(ClientAddr.Addr.bAddr, CNETTraits::NET_ADDR::LEN_BYTE);
	}
	else {
		stHash = (size_t)ClientAddr.Addr.uAddr[0];
	}
	// 1. �Ƿ������ú�����
	// 2. �Ƿ��ڶ�ʱ������(�����Ƿ��ж����������->��������ʱ��)--����Ա�鿴���Խ�����ʱ�䳤�ļ������ú�����
	// 3. ��ʱ��ַ��������Ƿ��ж����������->���ڼ��붨ʱ������
	return (stHash == 0);
}

bool CGateRoutine::CheckSessionTimeout(void)
{
	//TIMEOUT_MAP Timeout;
	//{
	//	CSyncLockWaitScope scope(m_SessionMap.GetLock());
	//	PINDEX index = m_SessionMap.First();
	//	while (index != nullptr) {
	//		SESSION_MAP::DATA_MAP_PAIR* pPair = m_SessionMap.Next(index);
	//		switch (pPair->m_V.nStatus) {
	//		case SESSION_DATA::SESSION_STATUS_LINK:
	//		case SESSION_DATA::SESSION_STATUS_SELECT:
	//		case SESSION_DATA::SESSION_STATUS_GAME:
	//		case SESSION_DATA::SESSION_STATUS_UNLINK:
	//			{
	//				if (CPlatform::GetRunningTime() > (pPair->m_V.llTimeout + SESSION_DATA::SESSION_TIME_OUT_MIN)) {
	//					Timeout.Add((KeyRef)pPair->m_K, LOGIN_ERROR_ACK_TIMEOUT);
	//				}
	//			}
	//			break;
	//		case SESSION_DATA::SESSION_STATUS_SELECTACK:
	//		case SESSION_DATA::SESSION_STATUS_GAMEACK:
	//			{
	//				if (CPlatform::GetRunningTime() > (pPair->m_V.llTimeout + SESSION_DATA::SESSION_TIME_OUT_MAX)) {
	//					Timeout.Add((KeyRef)pPair->m_K, LOGIN_ERROR_GAME_PLAY);
	//				}
	//			}
	//			break;
	//		case SESSION_DATA::SESSION_STATUS_NONE:
	//		case SESSION_DATA::SESSION_STATUS_LINKACK:
	//		default:
	//			{
	//				if (CPlatform::GetRunningTime() > (pPair->m_V.llTimeout + SESSION_DATA::SESSION_TIME_LIMIT)) {
	//					Timeout.Add((KeyRef)pPair->m_K, LOGIN_ERROR_LINK_IDLE);
	//				}
	//			}
	//		}
	//	}
	//}
	//CPAKAck UnlinkAck(PAK_EVENT_LOGIN_UNLINKACK, (UInt)PAK_TYPE_LOGIN);
	//UnlinkAck.AdjustSize();
	//for (PINDEX index = Timeout.GetFirst(); index != nullptr; ) {
	//	TIMEOUT_MAP_PAIR* pPair = Timeout.GetNext(index);
	//	UnlinkAck.SetAck(pPair->m_V);
	//	m_NetworkPtr->Send(pPair->m_K, UnlinkAck, SEND_CLOSE);
	//}
	return true;
}

void CGateRoutine::SelectGame(CPAKLoginSelectGame* pSelect, LLong llParam)
{
	pSelect->SetType(PAK_TYPE_GATE);
	DEV_INFO(TF("[���ط�����]�յ���½������ת���Ŀͻ��˵�½��Ϸ����, SessionId=%llx, UserId=%llx"), pSelect->GetSessionId(), pSelect->GetAuthCode());

	ICommonServer* pLoginServer    = reinterpret_cast<ICommonServer*>(llParam);
	CNETTraits::NET_ADDR AddrLogin = pSelect->GetAddr();

	if ((m_SessionLogin.Size() + m_SessionQueue.Size()) < m_nQueueLimit) { // û���߳���, ������������һ�������, Ӧ�ò���Ӱ����Ϸ
		SESSION_DATA LoginData;
		LoginData.llUserId     = pSelect->GetAuthCode();
		LoginData.llTimeout    = CPlatform::GetRunningTime(); // ������е�ʱ��, ��ʱSESSION_TIME_SELECT_GAME����ɾ��
		LoginData.llLoginIndex = pSelect->GetSessionId();
		LoginData.pLoginServer = pLoginServer;
		LoginData.AddrLogin    = pSelect->GetAddr();
		{
			CSyncLockWaitScope scope(m_SessionLogin.GetLock());
			assert(m_SessionLogin.Find((DataRef)pSelect->GetSessionId()) == nullptr);
			m_SessionLogin.Add((DataRef)pSelect->GetSessionId(), LoginData);
		}
		pSelect->SetAddr(m_pServer->GetGateAddr().NetAddr[GATEI_TCP]);
		pSelect->SetAck(DATAD_OKAY);
	}
	else {
		pSelect->SetAck(LOGIN_ERROR_QUEUE_LIMIT);
	}
	if (pLoginServer != nullptr) {
		pLoginServer->OnShareRoutine(PAK_EVENT_LOGIN_SELECT_GAME, *pSelect);
	}
	else {
		m_pServer->GetNetworkPtr()->SendTo(m_pServer->GetUDPService(), *pSelect, AddrLogin);
	}
}

void CGateRoutine::LinkGame(CPAKLoginLinkGame* pLink, KeyRef krSocket)
{
	pLink->SetType(PAK_TYPE_GATE);
	DEV_INFO(TF("[���ط�����]�յ��ͻ��˵�½��Ϸ����, SessionId=%llx, UserId=%llx"), pLink->GetSessionId(), pLink->GetAuthCode());
	LLong llTemp = pLink->GetSessionId();
	if (llTemp != 0) {
		llTemp = pLink->GetAuthCode();
		llTemp ^= (LLong)krSocket;
		LinkGame2(llTemp, pLink, krSocket);
	}
	else { // ��֤��1У��
		/////////////////////////////////////////////////////////////////
		// ����AuthCode(��ͻ���Ψһ����Ϣ�й�, �������IP), ����Ҫ��loginserver���͸��ͻ��˵ļ��ܷ�ʽ��Ӧ
		CNETTraits::NET_ADDR AddrClient;
		m_pServer->GetNetworkPtr()->GetAddr(krSocket, AddrClient);

		llTemp = pLink->GetAuthCode();
		if (AddrClient.usAttr & CNETTraits::ATTR_IPV6) {
			llTemp ^= AddrClient.Addr.ullAddr[0];
			llTemp ^= AddrClient.Addr.ullAddr[1];
			DEV_INFO(TF("[���ط�����]����Keyǰ��Ϊ[%llx-%llx-%llx+%llx]"), pLink->GetAuthCode(), llTemp, AddrClient.Addr.ullAddr[0], AddrClient.Addr.ullAddr[1]);
		}
		else {
			llTemp ^= AddrClient.Addr.uAddr[0];
			DEV_INFO(TF("[���ط�����]����Keyǰ��Ϊ[%p-%p-%x]"), pLink->GetAuthCode(), llTemp, AddrClient.Addr.uAddr[0]);
		}
		/////////////////////////////////////////////////////////////////
		LinkGame1(llTemp, pLink, krSocket);
	}
}

void CGateRoutine::LinkGame1(LLong llDataRef, CPAKLoginLinkGame* pLink, KeyRef krSocket)
{
	SESSION_DATA LoginData;
	{
		CSyncLockWaitScope scope(m_SessionLogin.GetLock());
		SESSION_DATA_MAP::DATA_MAP_PAIR* pPair = m_SessionLogin.Find((DataRef)llDataRef);
		if (pPair != nullptr) {
			pPair->m_V.llTimeout = CPlatform::GetRunningTime();
			if (pPair->m_V.nStatus == SESSION_DATA::SESSION_STATUS_NONE) {
				pPair->m_V.nStatus = SESSION_DATA::SESSION_STATUS_READY;
				LoginData = pPair->m_V;
			}
		}
	}
	if (LoginData.llUserId != 0) {
		DEV_INFO(TF("[���ط�����]�ͻ�����֤��1У�����"));
		pLink->SetAck(DATAD_OKAY);
		pLink->SetSessionId(llDataRef);
		pLink->SetAuthCode(llDataRef ^ (LLong)krSocket); // ��֤��2 : login-sessionid ^ gate-socket id
		if (LoginData.pLoginServer != nullptr) {
			LoginData.pLoginServer->OnShareRoutine(PAK_EVENT_LOGIN_LINK_GAME, *pLink);
		}
		else {
			m_pServer->GetNetworkPtr()->SendTo(m_pServer->GetUDPService(), *pLink, LoginData.AddrLogin);
		}
	}
	else {
		DEV_INFO(TF("[���ط�����]�ͻ�����֤��1У����Ч"));
		CPAKAck LinkGameAck(PAK_EVENT_LOGIN_LINK_GAME, (UInt)PAK_TYPE_GATE);
		LinkGameAck.SetAck(GATE_ERROR_AUTHCODE1);
		LinkGameAck.AdjustSize();
		m_pServer->GetNetworkPtr()->Send(krSocket, LinkGameAck, CNETTraits::SEND_CLOSE);
	}
}

void CGateRoutine::LinkGame2(LLong llDataRef, CPAKLoginLinkGame* pLink, KeyRef krSocket)
{
	Int nQueue = -1;
	SESSION_DATA Data;
	{
		CSyncLockWaitScope scope(m_SessionLogin.GetLock());
		SESSION_DATA_MAP::DATA_MAP_PAIR* pPair = m_SessionLogin.Find((DataRef)llDataRef);
		if (pPair != nullptr) {
			Data = pPair->m_V;
			m_SessionLogin.RemoveAt(reinterpret_cast<PINDEX>(pPair));
		}
	}
	if (Data.llUserId == pLink->GetSessionId()) {
		Data.llTimeout = CPlatform::GetRunningTime();
		Data.llOnline  = Data.llTimeout;

		if (m_nGameCur < m_nGameLimit) { // ֱ�Ӽ�����Ϸ����
			CAtomics::Increment<UInt>((PUInt)&m_nGameCur);
			CAtomics::Increment<UInt>((PUInt)&m_nGameSelect);

			Data.nStatus   = SESSION_DATA::SESSION_STATUS_GAME;
			CSyncLockWaitScope scope(m_SessionGame.GetLock());
			m_SessionGame.Add((DataRef)krSocket, Data);

			nQueue = 0; // ֱ�ӽ�����Ϸ, ����Ҫ�Ŷ�
			m_pServer->GetNetworkPtr()->SetAttr(krSocket, PAK_TYPE_CLIENT_SELECT);
		}
		else {	// �����Ŷ�
			if ((m_nQueueEnd > 0) && (m_nQueueCur == m_nQueueEnd)) { // �ŶӶ��п�ʱ, ����
				m_nQueueEnd = 0;
				m_nQueueCur = 0;
			}
			++m_nQueueEnd;
			nQueue       = m_nQueueEnd;
			Data.nData   = m_nQueueEnd;
			Data.nStatus = SESSION_DATA::SESSION_STATUS_QUEUE;
			{
				CSyncLockWaitScope scope(m_SessionQueue.GetLock());
				PINDEX inxQueue = m_SessionQueue.Add((DataRef)krSocket, Data);
				PINDEX inxRank = m_QueueRank.AddTail(inxQueue);
				m_SessionQueue.Get(inxQueue)->m_V.index = inxRank;
			}
			m_pServer->GetNetworkPtr()->SetAttr(krSocket, PAK_TYPE_CLIENT_QUEUE);
		}
		DEV_INFO(TF("[���ط�����]�ͻ�����֤��2У�����"));
		pLink->SetAck(DATAD_OKAY);
		pLink->SetAuthCode(nQueue); // nQueue = 0; // ֱ�ӽ�����Ϸ, ����Ҫ�Ŷ�
		m_pServer->GetNetworkPtr()->Send(krSocket, *pLink);
		// ֪ͨ��Ӧ�ĵ�¼�������û��Ѿ������Ϸ�������ĵ�½����
		CPAKSessionAck PlayAck(PAK_EVENT_LOGIN_QUEUE_PLAY, (UInt)PAK_TYPE_GATE);
		PlayAck.SetSessionId(llDataRef);
		PlayAck.AdjustSize();
		if (Data.pLoginServer != nullptr) {
			Data.pLoginServer->OnShareRoutine(PAK_EVENT_LOGIN_QUEUE_PLAY, PlayAck);
		}
		else {
			m_pServer->GetNetworkPtr()->SendTo(m_pServer->GetUDPService(), PlayAck, Data.AddrLogin);
		}
	}
	else {
		DEV_INFO(TF("[���ط�����]�ͻ�����֤��2У����Ч����session-id(user-id)����"));
		CPAKAck LinkGameAck(PAK_EVENT_LOGIN_LINK_GAME, (UInt)PAK_TYPE_GATE);
		LinkGameAck.SetAck((Data.llUserId == pLink->GetSessionId()) ? GATE_ERROR_AUTHCODE2 : GATE_ERROR_SESSION);
		LinkGameAck.AdjustSize();
		m_pServer->GetNetworkPtr()->Send(krSocket, LinkGameAck, CNETTraits::SEND_CLOSE);
	}
}

void CGateRoutine::QueueRank(CPAKSession* pQueue, KeyRef krSocket)
{
	UInt  uAck   = GATE_ERROR_QUEUE;
	Int   nParam = -1;
	{
		CSyncLockWaitScope scope(m_SessionQueue.GetLock());
		SESSION_DATA_MAP::DATA_MAP_PAIR* pPair = m_SessionQueue.Find((DataRef)krSocket);
		if (pPair != nullptr) {
			pPair->m_V.llTimeout = CPlatform::GetRunningTime();
			if (pQueue->GetSessionId() == pPair->m_V.llUserId) {
				uAck   = DATAD_OKAY;
				nParam = pPair->m_V.nData - m_nQueueCur;
			}
			else {
				uAck = GATE_ERROR_SESSION;
			}
		}
	}
	if (uAck == GATE_ERROR_QUEUE) {
		CSyncLockWaitScope scope(m_SessionGame.GetLock());
		SESSION_DATA_MAP::DATA_MAP_PAIR* pPair = m_SessionGame.Find((DataRef)krSocket);
		if (pPair != nullptr) {
			//pPair->m_V.llTimeout = CPlatform::GetRunningTime(); // ������Ŷ���Ϣ��ѯ, ������ʱ��
			uAck   = GATE_ERROR_GAME;
			nParam = pPair->m_V.nStatus; // ����Ϸ�����е�, ���ص�ǰ״̬
		}
	}
	if (nParam >= 0) {
		DEV_INFO(TF("[���ط�����]�ͻ��˲�ѯ�Ŷ���Ϣ�ɹ�����%d"), nParam);
		CPAKGateQueueAck QueueAck;
		QueueAck.SetAck(uAck);
		QueueAck.SetParam(nParam);
		QueueAck.AdjustSize();
		m_pServer->GetNetworkPtr()->Send(krSocket, QueueAck);
	}
	else { // 
		DEV_INFO(TF("[���ط�����]�ͻ��˲�ѯ�Ŷ���Ϣʧ��"));
		CPAKAck QueueAck(PAK_EVENT_GATE_QUEUEACK, (UInt)PAK_TYPE_GATE);
		QueueAck.SetAck(uAck);
		QueueAck.AdjustSize();
		m_pServer->GetNetworkPtr()->Send(krSocket, QueueAck, CNETTraits::SEND_CLOSE);
	}
}

void CGateRoutine::SelectRole(CPAKGateSelect* pSelect, KeyRef krSocket)
{
	UInt uAck = GATE_ERROR_SESSION;
	{
		CSyncLockWaitScope scope(m_SessionGame.GetLock());
		SESSION_DATA_MAP::DATA_MAP_PAIR* pPair = m_SessionGame.Find((DataRef)krSocket);
		if (pPair != nullptr) {
			pPair->m_V.llTimeout = CPlatform::GetRunningTime();
			if (pPair->m_V.llUserId != pSelect->GetSessionId()) {
				uAck  = GATE_ERROR_SESSION;
			}
			else if (pPair->m_V.llUserId != pSelect->GetUserId()) {
				uAck  = GATE_ERROR_USERID;
			}
			else if (pPair->m_V.nStatus == SESSION_DATA::SESSION_STATUS_GAME) {
				pPair->m_V.nStatus = SESSION_DATA::SESSION_STATUS_SELECT;
				uAck   = DATAD_OKAY;
			}
		}
	}
	if (uAck == DATAD_OKAY) { // ����ϷDB���������������ɫ��Ϣ
		pSelect->SetSessionId((LLong)krSocket);
		ICommonServer* pGameDBServer = m_pServer->GetShareGameDBServer();
		if (pGameDBServer != nullptr) {
			pGameDBServer->OnShareRoutine(PAK_EVENT_GATE_SELECT, *pSelect);
		}
		else {
			m_pServer->GetNetworkPtr()->Send(m_pServer->GetConnectGameDB(), *pSelect);
		}
	}
	else { // 
		DEV_INFO(TF("[���ط�����]�ͻ��������ɫ��Ϣ����"));
		CPAKAck SelectAck(PAK_EVENT_GATE_SELECTACK, (UInt)PAK_TYPE_GATE);
		SelectAck.SetAck(uAck);
		SelectAck.AdjustSize();
		m_pServer->GetNetworkPtr()->Send(krSocket, SelectAck, CNETTraits::SEND_CLOSE);
	}
}

void CGateRoutine::PlayRole(CPAKGatePlay* pPlay, KeyRef krSocket)
{
	UInt uAck = GATE_ERROR_SESSION;
	{
		CSyncLockWaitScope scope(m_SessionGame.GetLock());
		SESSION_DATA_MAP::DATA_MAP_PAIR* pPair = m_SessionGame.Find((DataRef)krSocket);
		if (pPair != nullptr) {
			pPair->m_V.llTimeout = CPlatform::GetRunningTime();
			if (pPair->m_V.llUserId != pPlay->GetSessionId()) {
				uAck  = GATE_ERROR_SESSION;
			}
			else if (pPair->m_V.llUserId != pPlay->GetUserId()) {
				uAck  = GATE_ERROR_USERID;
			}
			else if (pPair->m_V.nStatus == SESSION_DATA::SESSION_STATUS_SELECTACK) {
				pPair->m_V.nStatus = SESSION_DATA::SESSION_STATUS_PLAY;
				pPair->m_V.nData   = pPlay->GetRoleId();
				uAck   = DATAD_OKAY;
			}
		}
	}
	if (uAck == DATAD_OKAY) { // ����Ϸ��������������
		pPlay->SetSessionId((LLong)krSocket);
		ICommonServer* pGameServer = m_pServer->GetShareGameServer();
		if (pGameServer != nullptr) {
			pGameServer->OnShareRoutine(PAK_EVENT_GATE_PLAY, *pPlay);
		}
		else {
			m_pServer->GetNetworkPtr()->Send(m_pServer->GetConnectGame(), *pPlay);
		}
	}
	else { // 
		DEV_INFO(TF("[���ط�����]�ͻ������������Ϸ�������"));
		CPAKAck PlayAck(PAK_EVENT_GATE_PLAYACK, (UInt)PAK_TYPE_GATE);
		PlayAck.SetAck(uAck);
		PlayAck.AdjustSize();
		m_pServer->GetNetworkPtr()->Send(krSocket, PlayAck, CNETTraits::SEND_CLOSE);
	}
}

void CGateRoutine::SelectRoleAck(CPAKSessionAck* pAck)
{
	UInt uAck = pAck->GetAck();
	if (uAck == DATAD_OKAY) {
		pAck->SetType(PAK_TYPE_GATE);

		CSyncLockWaitScope scope(m_SessionGame.GetLock());
		SESSION_DATA_MAP::DATA_MAP_PAIR* pPair = m_SessionGame.Find((DataRef)pAck->GetSessionId());
		if ((pPair != nullptr) && (pPair->m_V.nStatus == SESSION_DATA::SESSION_STATUS_SELECT)) {
			pPair->m_V.llTimeout = CPlatform::GetRunningTime();
			pPair->m_V.nStatus   = SESSION_DATA::SESSION_STATUS_SELECTACK;

			pAck->SetSessionId(pPair->m_V.llUserId);
			m_pServer->GetNetworkPtr()->Send((KeyRef)pPair->m_K, *pAck);
		}
		else {
			uAck = GATE_ERROR_SESSION;
		}
	}
	if (uAck != DATAD_OKAY) {
		CPAKAck SelectAck(PAK_EVENT_GATE_SELECTACK, (UInt)PAK_TYPE_GATE);
		SelectAck.SetAck(uAck);
		SelectAck.AdjustSize();
		m_pServer->GetNetworkPtr()->Send((KeyRef)pAck->GetSessionId(), SelectAck, CNETTraits::SEND_CLOSE);
	}
}

void CGateRoutine::PlayRoleAck(CPAKSessionAck* pAck)
{
	UInt uAck = pAck->GetAck();
	if (uAck == DATAD_OKAY) {
		pAck->SetType(PAK_TYPE_GATE);

		CSyncLockWaitScope scope(m_SessionGame.GetLock());
		SESSION_DATA_MAP::DATA_MAP_PAIR* pPair = m_SessionGame.Find((DataRef)pAck->GetSessionId());
		if ((pPair != nullptr) && (pPair->m_V.nStatus == SESSION_DATA::SESSION_STATUS_PLAY)) {
			pPair->m_V.llTimeout = CPlatform::GetRunningTime();
			pPair->m_V.nStatus   = SESSION_DATA::SESSION_STATUS_PLAYACK;

			pAck->SetSessionId(pPair->m_V.llUserId);
			m_pServer->GetNetworkPtr()->Send((KeyRef)pPair->m_K, *pAck);
		}
		else {
			uAck = GATE_ERROR_SESSION;
		}
	}
	if (uAck != DATAD_OKAY) {
		CPAKAck SelectAck(PAK_EVENT_GATE_PLAYACK, (UInt)PAK_TYPE_GATE);
		SelectAck.SetAck(uAck);
		SelectAck.AdjustSize();
		m_pServer->GetNetworkPtr()->Send((KeyRef)pAck->GetSessionId(), SelectAck, CNETTraits::SEND_CLOSE);
	}
}



