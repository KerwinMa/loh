//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Source File : TestLaunch.cpp                               //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ���Կͻ���                                         //
//                                                              //
//////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "TestLaunch.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTestSystem
IMPLEMENT_CLASS_LOADER( CTestSystem, XUI )
IMPLEMENT_CLASS_LOADER( CTestSystem, NET )

CTestSystem::CTestSystem(void)
{
}

CTestSystem::~CTestSystem(void)
{
}

bool CTestSystem::Init(void)
{
	INIT_CLASS_LOADER( CTestSystem, XUI, UUID_OF( CXUISystem ),     XUI_MODULE_NAME);
	INIT_CLASS_LOADER( CTestSystem, NET, UUID_OF( CNetworkSystem ), NETWORK_MODULE_NAME);

	LOADER_CLASS_CREEATE( CTestSystem, XUI, UUID_OF( CXUIManager ), m_UIManagerPtr.Cast<CComponent>());
	if ((m_UIManagerPtr != nullptr) && (m_UIManagerPtr->Init() == RET_OKAY)) {
		IXWnd::InitCommonCtrls((ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES));

		CNETTraits::NET_ATTR attr;
		attr.nAttrs  = CNETTraits::ATTR_THREAD;
		attr.nThread = 1;
		LOADER_CLASS_CREEATE( CTestSystem, NET, UUID_OF( CNetwork ), m_NetworkPtr.Cast<CComponent>());
		if ((m_NetworkPtr != nullptr) && (m_NetworkPtr->Init(attr) == RET_OKAY)) {
			DEV_DEBUG(TF("���ؽ���ģ�������ģ�����"));
			return true;
		}
	}
	return false;
}

void CTestSystem::Exit(void)
{
	if (m_NetworkPtr != nullptr) {
		m_NetworkPtr->Exit();
		m_NetworkPtr = nullptr;
	}
	if (m_UIManagerPtr != nullptr) {
		m_UIManagerPtr->Exit();
		m_UIManagerPtr = nullptr;
	}
	EXIT_CLASS_LOADER( CTestSystem, NET );
	EXIT_CLASS_LOADER( CTestSystem, XUI );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTestLoginDlg
BEGIN_DLG_ID_WND_EXCHANGE( CTestLoginDlg )
	DLG_ID_WND( IDC_USERID,   m_xuiId )
	DLG_ID_WND( IDC_PASSWORD, m_xuiPw )
	DLG_ID_WND( IDC_STEPINFO, m_xuiInfo )
END_DLG_ID_WND_EXCHANGE( CXDlg )

PCXStr CTestLoginDlg::GetId(void)
{
	return m_szId;
}

PCXStr CTestLoginDlg::GetPw(void)
{
	return m_szPw;
}

LRESULT CTestLoginDlg::OnOk(WORD, WORD, HWND, BOOL& bHandled)
{
	m_xuiId.GetWindowText(m_szId, LMT_KEY);
	m_xuiPw.GetWindowText(m_szPw, LMT_KEY);
	if ((CXChar::Length(m_szId, LMT_KEY) > 0) &&
		(CXChar::Length(m_szPw, LMT_KEY) > 0)) {
		bHandled = FALSE; // �õײ㴦����Ϣ
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTestSelGameDlg
BEGIN_DLG_ID_WND_EXCHANGE( CTestSelGameDlg )
	DLG_ID_WND( IDC_GAMESVR, m_xuiGameSvr )
END_DLG_ID_WND_EXCHANGE( CXDlg )

CTestSelGameDlg::CTestSelGameDlg(void)
: m_nGameId(0)
, m_llGameKey(0)
{
}

CTestSelGameDlg::~CTestSelGameDlg(void)
{
}

void CTestSelGameDlg::LinkAck(CPAKLoginLinkAck* pLinkAck)
{
	m_nGameId = pLinkAck->GetGameId();

	pLinkAck->GetStream().Read(&m_ServerData, sizeof(SERVER_DATA));

	//CSyncLockWaitScope scope(m_ServerMap.GetLock());
	m_ServerMap.Serialize(pLinkAck->GetStream());
}

void CTestSelGameDlg::Sync(CLoginSync* pSync)
{
	m_ServerData = pSync->GetServerData();

	CSyncLockWaitScope scope(m_ServerMap.GetLock());
	m_ServerMap.Serialize(pSync->GetStream());
	Update();
}

LRESULT CTestSelGameDlg::OnInitDialog(void)
{
	m_xuiGameSvr.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_xuiGameSvr.AddColumn(TF("��������ʶ"), 0, 120, -1, LVCF_WIDTH|LVCF_TEXT); // ��Id����ת��������
	m_xuiGameSvr.AddColumn(TF("�ܹ�������"), 1, 90,  -1, LVCF_WIDTH|LVCF_TEXT);
	m_xuiGameSvr.AddColumn(TF("����������"), 2, 90,  -1, LVCF_WIDTH|LVCF_TEXT);
	m_xuiGameSvr.AddColumn(TF("���ذٷֱ�"), 3, 90,  -1, LVCF_WIDTH|LVCF_TEXT);
	m_xuiGameSvr.AddColumn(TF("��ַ"),       4, 120,  -1, LVCF_WIDTH|LVCF_TEXT);
	m_xuiGameSvr.AddColumn(TF("PING"),      5, 60,  -1, LVCF_WIDTH|LVCF_TEXT);
	{
		CSyncLockWaitScope scope(m_ServerMap.GetLock());
		Update();
	}
	// ����PING�̲߳���PINGֵ, ���½���
	return TRUE;
}

LRESULT CTestSelGameDlg::OnOk(WORD, WORD, HWND, BOOL& bHandled)
{
	Int nIndex = m_xuiGameSvr.GetSelectedIndex();
	if (nIndex != -1) {
		m_llGameKey = (LLong)m_xuiGameSvr.GetItemData(nIndex);
		bHandled = FALSE;
	}
	else {
		MessageBox(TF("��ѡ��һ����Ϸ������"), TF("TestLaunch"));
	}
	return 0;
}

void CTestSelGameDlg::Update(void)
{
	for (PINDEX index = m_ServerMap.First(); index != nullptr; ) {
		SVR_TEST_MAP::SVR_MAP_PAIR* pPair = m_ServerMap.Next(index);

		LVFINDINFO find = { 0 };
		find.flags  = LVFI_PARAM;
		find.lParam = (LPARAM)pPair->m_K;
		Int nIndex  = m_xuiGameSvr.FindItem(&find);

		if (pPair->m_V.usStatus == STATUSU_UNLINK) {
			if (nIndex > -1) {
				m_xuiGameSvr.DeleteItem(nIndex);
				m_ServerMap.RemoveAt(reinterpret_cast<PINDEX>(pPair));
			}
		}
		else if ((pPair->m_V.usStatus == STATUSU_SYNC) || (pPair->m_V.usStatus == STATUSU_LINK)) {
			if (nIndex > -1) {
				Update(nIndex, pPair->m_V);
			}
			else {
				Link(pPair->m_K, pPair->m_V, pPair->m_V.NetAddr);
			}
			pPair->m_V.usStatus = STATUSU_OKAY;
		}
	}
}

void CTestSelGameDlg::Link(DataRef drKey, SERVER_DATA& sd, CNETTraits::NET_ADDR* pAddr)
{
	CStringFix strTemp;
	strTemp.ToString((ULong)sd.usId); // ��Id����ת��������
	Int nIndex = m_xuiGameSvr.InsertItem(LMT_MAX, *strTemp);
	if (nIndex >= 0) {
		Update(nIndex, sd);

		CStringKey strAddr;
		UShort     usPort = 0;
		GTestSystemInst->m_NetworkPtr->TranslateAddr(strAddr, usPort, *pAddr, false);
		m_xuiGameSvr.SetItemText(nIndex, 4, *strAddr);

		m_xuiGameSvr.SetItemData(nIndex, (uintptr_t)drKey);
		if ((Int)sd.usId == m_nGameId) {
			m_xuiGameSvr.SelectItem(nIndex);
			m_nGameId = 0;
		}
	}
}

void CTestSelGameDlg::Update(Int nIndex, SERVER_DATA& sd)
{
	CStringFix strTemp;
	strTemp.ToString((ULong)sd.uAllCount);
	m_xuiGameSvr.SetItemText(nIndex, 1, *strTemp);

	strTemp.ToString((ULong)sd.uOnline);
	m_xuiGameSvr.SetItemText(nIndex, 2, *strTemp);

	strTemp.ToString((ULong)(sd.usBusy / DATAD_PERCENT));
	m_xuiGameSvr.SetItemText(nIndex, 3, *strTemp);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTestMainFrame
CPCXStr CTestMainFrame::SELECT_SERVER_ADDR = TF("127.0.0.1");

CTestMainFrame::CTestMainFrame(void)
: m_nState(FRAME_STATE_NONE)
, m_nRetryCount(0)
, m_krUDPSelect(nullptr)
, m_krTCPLogin(nullptr)
, m_krTCPGame(nullptr)
, m_llLoginSession(0)
, m_llGameSession(0)
{
	SetFrameMode();
}

CTestMainFrame::~CTestMainFrame(void)
{
	if (m_krUDPSelect != nullptr) {
		GTestSystemInst->m_NetworkPtr->Destroy(m_krUDPSelect);
		m_krUDPSelect = nullptr;
	}
	if (m_krTCPLogin != nullptr) {
		GTestSystemInst->m_NetworkPtr->Destroy(m_krTCPLogin);
		m_krTCPLogin = nullptr;
	}
	if (m_krTCPGame != nullptr) {
		GTestSystemInst->m_NetworkPtr->Destroy(m_krTCPGame);
		m_krTCPGame = nullptr;
	}
}

bool CTestMainFrame::OnTcpDispatch(const PacketPtr& PktPtr, PTCP_PARAM pTcp)
{
	if (pTcp->krSocket == m_krTCPGame) {
		assert(PktPtr->GetType() == PAK_TYPE_GATE);
		switch (PktPtr->GetEvent()) {
		case PAK_EVENT_LOGIN_LINK_GAME:
			{
				CPAKAck* pAck = static_cast<CPAKAck*>(PktPtr.Get());
				if (pAck->GetAck() == DATAD_OKAY) {
					assert(m_nState == FRAME_STATE_LINKGAME2);
					OnLinkGame2Ack(static_cast<CPAKLoginLinkGame*>(pAck));
				}
				else {
					CStringFix strErr;
					strErr.Format(TF("����Ϸ����������֤���ش�����:%d"), pAck->GetAck());
					m_xuiList.AddString(*strErr);

					m_nState = FRAME_STATE_SELGAME;
					PostMessage(FRAME_MSG_SELGAME);
				}
			}
			break;
		default:
			{
			}
		}
	}
	else if (pTcp->krSocket == m_krTCPLogin) {
		assert(PktPtr->GetType() == PAK_TYPE_LOGIN);
		switch (PktPtr->GetEvent()) {
		case PAK_EVENT_LINKACK:
			{
				assert(m_nState == FRAME_STATE_LOGIN);
				CEventQueue::DestroyTickEvent(FRAMEC_TICK_ID);
				m_xuiList.AddString(TF("��½������ȷ�����ְ�, ��ʾ��½����"));
				PostMessage(FRAME_MSG_LOGIN);
			}
			break;
		case PAK_EVENT_LOGIN_LINKACK:
			{
				assert(m_nState == FRAME_STATE_LOGIN);
				CPAKAck* pAck = static_cast<CPAKAck*>(PktPtr.Get());
				if (pAck->GetAck() == DATAD_OKAY) {
					OnLoginAck(static_cast<CPAKLoginLinkAck*>(pAck));
				}
				else {
					PostMessage(FRAME_MSG_LOGIN, pAck->GetAck());
				}
			}
			break;
		case PAK_EVENT_SYNC:
			{
				assert(m_nState >= FRAME_STATE_LOGIN);
				m_xuiSelGameDlg.Sync(static_cast<CLoginSync*>(PktPtr.Get()));
			}
			break;
		case PAK_EVENT_LOGIN_SELECT_GAME:
			{
				assert(m_nState == FRAME_STATE_SELGAME);
				CPAKAck* pAck = static_cast<CPAKAck*>(PktPtr.Get());
				if (pAck->GetAck() == DATAD_OKAY) {
					OnSelectGameAck(static_cast<CPAKLoginSelectGame*>(pAck));
				}
				else {
					PostMessage(FRAME_MSG_SELGAME, pAck->GetAck());
				}
			}
			break;
		case PAK_EVENT_LOGIN_LINK_GAME:
			{
				assert(m_nState == FRAME_STATE_LINKGAME1);
				OnLinkGame1Ack(static_cast<CPAKLoginLinkGame*>(PktPtr.Get()));
			}
			break;
		default:
			{
			}
		}
	}
	return true;
}

bool CTestMainFrame::OnUdpDispatch(const PacketPtr& PktPtr, PUDP_PARAM pUdp)
{
	if ((PktPtr->GetType()  == PAK_TYPE_SELECT) && (PktPtr->GetEvent() == PAK_EVENT_SELECT_LOGINACK)) {
		// �κ�ʱ�������������CPAKSelectLoginAck
		CPAKSelectLoginAck* pSelectAck = static_cast<CPAKSelectLoginAck*>(PktPtr.Get());
		if (pSelectAck->GetAck() == DATAD_OKAY) {
			OnSelectAck(pSelectAck, pUdp->NetAddr);
		}
		else {
			PostMessage(FRAME_MSG_SELECT, pSelectAck->GetAck());
		}
	}
	else {
		PostMessage(WM_CLOSE);
	}
	return true;
}

bool CTestMainFrame::OnTcpAccept(KeyRef, KeyRef)
{
	return false;
}

bool CTestMainFrame::OnTcpConnect(UInt uError, KeyRef krConnect)
{
	if (krConnect == m_krTCPLogin) {
		if ((uError == 0) && (m_nState == FRAME_STATE_NONE)) {
			m_nState = FRAME_STATE_LOGIN;
		}
		if (m_nState == FRAME_STATE_LOGIN) {
			GTestSystemInst->m_NetworkPtr->Destroy(m_krUDPSelect);
			m_krUDPSelect = nullptr;
	
			m_nRetryCount = 0;

			CPAKSimple<PAK_EVENT_LINK, (UInt)PAK_TYPE_CLIENT_LOGIN> Link;
			Link.AdjustSize();
			GTestSystemInst->m_NetworkPtr->Send(m_krTCPLogin, Link);
	
			m_xuiList.AddString(TF("���ӵ�½�������ɹ�, �������ְ�"));
		}
		else {
			PostMessage(FRAME_MSG_SELECT, uError);
		}
	}
	else if (krConnect == m_krTCPGame) {
		if ((uError == 0) && (m_nState == FRAME_STATE_CNTGAME)) {
			m_nState = FRAME_STATE_LINKGAME1;
		}
		if (m_nState == FRAME_STATE_LINKGAME1) {
			m_xuiList.AddString(TF("������Ϸ�������ɹ�, ��ʼ��֤У��"));
			PostMessage(FRAME_MSG_LINKGAME1);
		}
		else {
			m_nState = FRAME_STATE_SELGAME;
			PostMessage(FRAME_MSG_SELGAME);
		}
	}
	return true;
}

bool CTestMainFrame::OnTcpClose(KeyRef krSocket, LLong)
{
	if (krSocket == m_krTCPLogin) {
		m_krTCPLogin = nullptr;
		//m_xuiList.AddString(TF("��½�������Ͽ�"));
	}
	else if (krSocket == m_krTCPGame) {
		m_krTCPGame = nullptr;
		//m_xuiList.AddString(TF("��Ϸ�������Ͽ�"));

		//if (m_nState != FRAME_STATE_EXIT) {
		//	if (m_krTCPLogin != nullptr) {
		//		//m_nState = 
		//	}
		//	else {

		//	}
		//}
	}
	return true;
}

bool CTestMainFrame::OnUdpClose(KeyRef, LLong)
{
	m_krUDPSelect = nullptr;
	return true;
}

bool CTestMainFrame::OnHandle(Int, UInt)
{
	if (CPlatform::GetRunningTime() > m_llTimeout + FRAMEC_TICK_TIME) {
		PostMessage(WM_CLOSE);
	}
	return true;
}

LRESULT CTestMainFrame::OnCreate(LPCREATESTRUCT)
{
	CXUITraits::CREATE_PARAM cpm;
	// cmdbar
	cpm.pParent  = this;
	cpm.itMenuID = IDC_VIEW;
	cpm.ulStyle  = XUIS_CHILD_SIBLING;
	if (GTestSystemInst->m_UIManagerPtr->Create(m_xuiView, cpm) == -1) {
		return -1;
	}

	cpm.itMenuID = IDC_LOG;
	cpm.bFlag    = FALSE;
	cpm.ulStyle  = XUIS_CHILD_SIBLING | WS_VSCROLL | LBS_NOSEL;
	if (GTestSystemInst->m_UIManagerPtr->Create(m_xuiList, cpm) == -1) {
		return -1;
	}
	///////////////////////////////
	RECT rt = { 0, 0, FRAMEC_WIDTH, FRAMEC_HEIGHT };
	///////////////////////////////
	AdjustWindowRect(&rt, FRAMEC_TRAITS, false);

	long x = GetSystemMetrics(SM_CXSCREEN);
	long y = GetSystemMetrics(SM_CYSCREEN);
	x = (x - (rt.right - rt.left)) / 2;
	y = (y - (rt.bottom - rt.top)) / 2;

	SetWindowPos(nullptr, x, y, rt.right - rt.left, rt.bottom - rt.top, SWP_NOZORDER|SWP_SHOWWINDOW);
	// ���ӷ�����+������ʱ
	CEventQueue::CreateTickEvent(FRAMEC_TICK_ID, FRAMEC_TICK_TIME, *this);
	PostMessage(FRAME_MSG_SELECT);
	return 0;
}

void CTestMainFrame::OnDestroy(void)
{
	CXFrameWnd::OnDestroy();
}

void CTestMainFrame::FrameLayout(void)
{
	RECT rc;
	GetClientRect(&rc);

	m_xuiView.MoveWindow(0, 0, rc.right, (rc.bottom - FRAMEC_LIST_HEIGHT));
	m_xuiList.MoveWindow(0, (rc.bottom - FRAMEC_LIST_HEIGHT), rc.right, FRAMEC_LIST_HEIGHT);
}

LRESULT CTestMainFrame::OnSelect(WPARAM wParam, LPARAM lParam, BOOL&)
{
	if (m_nState == FRAME_STATE_NONE) {
		if (wParam != 0) {
			CStringFix strErr;
			strErr.Format(TF("��%d��(���3��)��ѡ�������%s:%d���������½��������ַUDP���ݰ����ش�����:%d"), m_nRetryCount, SELECT_SERVER_ADDR, FRAMEC_SELECTPORT, wParam);
			m_xuiList.AddString(*strErr);
		}
		else if (lParam != 0) {
			CStringFix strErr;
			strErr.Format(TF("��%d��(���3��)��ѡ�������%s:%d���������½��������ַUDP���ݰ�����, ERROR id:%d"), m_nRetryCount, SELECT_SERVER_ADDR, FRAMEC_SELECTPORT, lParam);
			m_xuiList.AddString(*strErr);
		}
		if (m_nRetryCount <= FRAMEC_RETRYCOUNT) {
			++m_nRetryCount;

			if (m_krUDPSelect == nullptr) {
				m_krUDPSelect = GTestSystemInst->m_NetworkPtr->Create(*this, 0, nullptr, SOCKET_UDP);
			}
			if (m_krUDPSelect != nullptr) {
				CStringFix strLog;
				strLog.Format(TF("��%d��(���3��)��ѡ�������%s:%d���͵�%d�������½��������ַUDP���ݰ�"), m_nRetryCount, SELECT_SERVER_ADDR, FRAMEC_SELECTPORT, m_nRetryCount);
				m_xuiList.AddString(*strLog);

				m_llTimeout = CPlatform::GetRunningTime();

				CPAKSimple<PAK_EVENT_SELECT_LOGIN, (UInt)PAK_TYPE_CLIENT_READY> Select;
				Select.AdjustSize();
				if (GTestSystemInst->m_NetworkPtr->SendTo(m_krUDPSelect, Select, SELECT_SERVER_ADDR, FRAMEC_SELECTPORT)) {
					return 0;
				}
			}
			CStringFix strErr;
			strErr.Format(TF("�����������Ӵ���, ����ID:%d"), ::GetLastError());
			m_xuiList.AddString(*strErr);
			MessageBox(*strErr, TF("TestLaunch"));
		}
	}
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CTestMainFrame::OnLogin(WPARAM wParam, LPARAM, BOOL&)
{
	if (m_nState == FRAME_STATE_LOGIN) {
		if (wParam != 0) {
			CStringFix strErr;
			strErr.Format(TF("��%d��(���3��)���½���������͵�½�û��������뷵�ش�����:%d"), m_nRetryCount, wParam);
			m_xuiList.AddString(*strErr);
		}
		if (m_nRetryCount <= FRAMEC_RETRYCOUNT) {
			++m_nRetryCount;

			CXUITraits::CREATE_PARAM cpm;
			cpm.pParent  = this;
			cpm.itMenuID = IDD_LOGIN;

			if (GTestSystemInst->m_UIManagerPtr->Create(m_xuiLoginDlg, cpm) == IDOK) {
				CStringFix strLog;
				strLog.Format(TF("��%d��(���3��)���½���������͵�½�û���(%s)������(%s)"), m_nRetryCount, m_xuiLoginDlg.GetId(), m_xuiLoginDlg.GetPw());
				DEV_DEBUG(*strLog);
				m_xuiList.AddString(*strLog);

				CPAKLoginLink Link;
				Link.SetUserId(m_xuiLoginDlg.GetId());
				Link.SetPassword(m_xuiLoginDlg.GetPw());
				Link.AdjustSize();
				GTestSystemInst->m_NetworkPtr->Send(m_krTCPLogin, Link);
				return 0;
			}
		}
	}
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CTestMainFrame::OnSelGame(WPARAM wParam, LPARAM, BOOL&)
{
	if (m_nState == FRAME_STATE_SELGAME) {
		if (wParam != 0) {
			CStringFix strErr;
			strErr.Format(TF("��%d��(���3��)ѡȡ��Ϸ���������ش�����:%d"), m_nRetryCount, wParam);
			m_xuiList.AddString(*strErr);
		}
		if (m_nRetryCount <= FRAMEC_RETRYCOUNT) {
			++m_nRetryCount;

			CXUITraits::CREATE_PARAM cpm;
			cpm.pParent  = this;
			cpm.itMenuID = IDD_SELGAME;

			if (GTestSystemInst->m_UIManagerPtr->Create(m_xuiSelGameDlg, cpm) == IDOK) {
				CStringFix strLog;
				strLog.Format(TF("��%d��(���3��)ѡ����Ϸ������ : %llx"), m_nRetryCount, m_xuiSelGameDlg.GetGameKey());
				m_xuiList.AddString(*strLog);

				CPAKLoginSelectGame SelectGame;
				SelectGame.SetSessionId(m_llLoginSession);
				SelectGame.SetAuthCode(m_xuiSelGameDlg.GetGameKey());
				SelectGame.AdjustSize();
				GTestSystemInst->m_NetworkPtr->Send(m_krTCPLogin, SelectGame);
				return 0;
			}
		}
	}
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CTestMainFrame::OnCntGame(WPARAM, LPARAM, BOOL&)
{
	if (m_nState == FRAME_STATE_CNTGAME) { // ����ʧ����Ҫ�˻ᵽ��Ϸ������ѡ��״̬, û�ж�γ������ӵĻ���
		UShort usPort = 0;
		CStringKey strAddr;
		GTestSystemInst->m_NetworkPtr->TranslateAddr(strAddr, usPort, m_adrSelGate, false);

		CStringFix strLog;
		strLog.Format(TF("��Ϸ��������ַ%s:%d, ����������Ϸ������"), *strAddr, usPort);
		m_xuiList.AddString(*strLog);

		if (m_krTCPGame == nullptr) {
			m_krTCPGame = GTestSystemInst->m_NetworkPtr->Create(*this);
		}
		if (m_krTCPGame != nullptr) {
			if (GTestSystemInst->m_NetworkPtr->Connect(m_krTCPGame, m_adrSelGate)) {
				strLog.Format(TF("������Ϸ������[%s:%d]�ȴ����ӷ���"), *strAddr, usPort);
				m_xuiList.AddString(*strLog);
			}
			else {
				strLog.Format(TF("������Ϸ������[%s:%d]���ش���ID:%d"), *strAddr, usPort, ::GetLastError());
				m_xuiList.AddString(*strLog);

				m_nState = FRAME_STATE_SELGAME;
				PostMessage(FRAME_MSG_SELGAME);
			}
			return 0;
		}
	}
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CTestMainFrame::OnLinkGame1(WPARAM, LPARAM, BOOL&)
{
	if (m_nState == FRAME_STATE_LINKGAME1) {
		CStringFix strLog;
		strLog.Format(TF("����Ϸ������������֤1����session=0, authcode1=%llX"), m_llGameSession);
		m_xuiList.AddString(*strLog);

		CPAKLoginLinkGame LinkGame1;
		LinkGame1.SetAuthCode(m_llGameSession);
		LinkGame1.AdjustSize();
		GTestSystemInst->m_NetworkPtr->Send(m_krTCPGame, LinkGame1);
	}
	else {
		PostMessage(WM_CLOSE);
	}
	return 0;
}


void CTestMainFrame::OnSelectAck(CPAKSelectLoginAck* pSelectAck, NET_ADDR& ServerAddr)
{
	CStringFix strLog;
	CStringKey strAddr;
	UShort     usPort = 0;

	if (m_nState == FRAME_STATE_NONE) {
		GTestSystemInst->m_NetworkPtr->TranslateAddr(strAddr, usPort, pSelectAck->GetAddr(), false);

		strLog.Format(TF("��õ�½��������ַ%s:%d, �������ӵ�½������"), *strAddr, usPort);
		m_xuiList.AddString(*strLog);

		if (m_krTCPLogin == nullptr) {
			m_krTCPLogin = GTestSystemInst->m_NetworkPtr->Create(*this);
		}
		if (GTestSystemInst->m_NetworkPtr->Connect(m_krTCPLogin, pSelectAck->GetAddr())) {
			m_llTimeout = CPlatform::GetRunningTime();

			strLog.Format(TF("���ӵ�½������[%s:%d]�ȴ����ؽ��"), *strAddr, usPort);
			m_xuiList.AddString(*strLog);
		}
		else {
			strLog.Format(TF("���ӵ�½������[%s:%d]���ش���ID:%d"), *strAddr, usPort, ::GetLastError());
			m_xuiList.AddString(*strLog);

			PostMessage(FRAME_MSG_SELECT, 0, ::GetLastError());
		}
	}
	else {
		GTestSystemInst->m_NetworkPtr->TranslateAddr(strAddr, usPort, ServerAddr, false);
		strLog.Format(TF("�յ�ѡ�������[%s:%d]�Ļ�ȡ��¼��������ַ�ظ���, ���ǵ�ǰ״̬����:%d"), *strAddr, usPort, m_nState);
		m_xuiList.AddString(*strLog);
	}
}

void CTestMainFrame::OnLoginAck(CPAKLoginLinkAck* pLoginAck)
{
	CStringFix strLog;
	if (m_nState == FRAME_STATE_LOGIN) {

		m_nState         = FRAME_STATE_SELGAME;
		m_nRetryCount    = 0;
		m_llLoginSession = pLoginAck->GetSessionId();
		m_llUserId       = pLoginAck->GetUserId();

		DEV_DEBUG(TF("��½�ɹ�, ����SessionId=%llx, UserId=%llx, last login time=%llx"), pLoginAck->GetSessionId(), pLoginAck->GetUserId(), pLoginAck->GetTime());
		strLog.Format(TF("��½�ɹ�, ����SessionId=%llx, UserId=%llx, last login time=%llx"), pLoginAck->GetSessionId(), pLoginAck->GetUserId(), pLoginAck->GetTime());
		m_xuiList.AddString(*strLog);

		m_xuiSelGameDlg.LinkAck(pLoginAck);
		PostMessage(FRAME_MSG_SELGAME);
	}
	else {
		strLog.Format(TF("�յ���½�������ĵ�½�ظ���, ���ǵ�ǰ״̬����:%d"), m_nState);
		m_xuiList.AddString(*strLog);
		PostMessage(WM_CLOSE);
	}
}

void CTestMainFrame::OnSelectGameAck(CPAKLoginSelectGame* pSelectGameAck)
{
	CStringFix strLog;
	if (m_nState == FRAME_STATE_SELGAME) {

		m_nState = FRAME_STATE_CNTGAME;
		m_nRetryCount = 0;

		DEV_DEBUG(TF("ѡ����Ϸ�������ɹ�, ����SessionId=%llx, AuthCode1=%llx"), pSelectGameAck->GetSessionId(), pSelectGameAck->GetAuthCode());
		strLog.Format(TF("ѡ����Ϸ�������ɹ�, ����SessionId=%llx, AuthCode1=%llx"), pSelectGameAck->GetSessionId(), pSelectGameAck->GetAuthCode());
		m_xuiList.AddString(*strLog);

		m_llGameSession = pSelectGameAck->GetAuthCode();
		m_adrSelGate    = pSelectGameAck->GetAddr();

		PostMessage(FRAME_MSG_CNTGAME);
	}
	else {
		strLog.Format(TF("�յ���½��������ѡ����Ϸ�������ظ���, ���ǵ�ǰ״̬����:%d"), m_nState);
		m_xuiList.AddString(*strLog);
		PostMessage(WM_CLOSE);
	}
}

void CTestMainFrame::OnLinkGame1Ack(CPAKLoginLinkGame* pLinkGame1Ack)
{
	CStringFix strLog;
	if (m_nState == FRAME_STATE_LINKGAME1) {
		m_nState = FRAME_STATE_LINKGAME2;

		DEV_DEBUG(TF("��֤1����SessionId=%llx, AuthCode2=%llx"), pLinkGame1Ack->GetSessionId(), pLinkGame1Ack->GetAuthCode());
		strLog.Format(TF("��֤1����SessionId=%llx, AuthCode2=%llx"), pLinkGame1Ack->GetSessionId(), pLinkGame1Ack->GetAuthCode());
		m_xuiList.AddString(*strLog);

		CPAKLoginLinkGame LinkGame2;
		LinkGame2.SetSessionId(m_llUserId);
		LinkGame2.SetAuthCode(pLinkGame1Ack->GetAuthCode());
		LinkGame2.AdjustSize();
		GTestSystemInst->m_NetworkPtr->Send(m_krTCPGame, LinkGame2);
	}
	else {
		strLog.Format(TF("�յ���½����������֤1�ظ���, ���ǵ�ǰ״̬����:%d"), m_nState);
		m_xuiList.AddString(*strLog);
		PostMessage(WM_CLOSE);
	}
}

void CTestMainFrame::OnLinkGame2Ack(CPAKLoginLinkGame* pLinkGame2Ack)
{
	CStringFix strLog;
	if (pLinkGame2Ack->GetAuthCode() == 0) {
		m_nState = FRAME_STATE_SELECTROLE;
		strLog.Format(TF("��½��Ϸ��������֤���, ��ʼ��Ϸ�߼�����-->��ɫ����"));
	}
	else {
		m_nState = FRAME_STATE_SELECTROLE;
		strLog.Format(TF("��½��Ϸ��������֤���, ������Ϸ�Ŷ���(%d)"), pLinkGame2Ack->GetAuthCode());
	}
	m_xuiList.AddString(*strLog);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// _tWinMain
Int WINAPI _tWinMain(HINSTANCE,
                     HINSTANCE,
                     LPTSTR,
                     Int)
{
	CoreInit();
	if (GTestSystemInst->Init())
	{
		CTestMainFrame MainFrame;
		CXUITraits::CREATE_PARAM cpm;
		cpm.ulStyle = CTestMainFrame::FRAMEC_TRAITS;
		cpm.pszName = TF("TestLaunch");
		if (GTestSystemInst->m_UIManagerPtr->Create(MainFrame, cpm) != -1)
		{
			MainFrame.UpdateWindow();
			GTestSystemInst->m_UIManagerPtr->MsgLoop();
		}
	}
	GTestSystemInst->Exit();
	CoreExit();
	return 0;
}




