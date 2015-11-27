//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : LoginServerImp.h                             //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update : 2015-11-25     version 0.0.0.5                    //
//   Detail : ��½������ʵ��                                     //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __LOGIN_SERVER_IMP_H__
#define __LOGIN_SERVER_IMP_H__

#pragma once

#include "logindbpacket.h"
#include "loginpacket.h"
#include "servermap.h"
#include "ServerConfig.h"
#include "CommonServer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ��½������ʵ��
/// - ��Ҫ���� :
/// -# ά��������Ϸ�ͻ��˵�½����
/// -# �����, ȫ��ʹ�������̺߳Ͷ�ʱ�߳�������
/// -# TODO!!! /// @todo ������У��δ���
///    a. �ӵ�½��������չ�����ļ��ж�ȡ�������б�
///    b. ���Ӱ��������οͻ������ӵ���Ϣ��, �����⹥�����߶�����ӵļ������, �����س̶��ۼ�����ʱ��
///    c. ����Ҳ��Ҫ�������οͻ��˵���Ϣ�����ʾ����������
///    d. ����������ACK(1000MS)��Timeout(30000MS)����
///    e. ��ʱ��(����С��200MS������3��)Ƶ��������, �����������������
///    f. ������ú�����+��ʱ������(����1Сʱ����һ�ε�, ������Сʱ��Ϊ2Сʱ)
///    g. ��ʱ��ַmap����������10000000(1ǧ��, 64λֱ�Ӻ�600M)Ϊ����, ��ʱ���(����2Сʱ����һ�ε�)
class CLoginServer : public ICommonServer, public CPAKHandler
{
public:
	CLoginServer(void);
	virtual ~CLoginServer(void);
	// CComponent
	// Command to set param value
	virtual UInt   Command(PCXStr pszCMD, uintptr_t utParam) OVERRIDE;
	virtual UInt   Update(void) OVERRIDE;

	virtual UInt   Init(CEventHandler& EventHandlerRef) OVERRIDE;
	virtual void   Exit(void) OVERRIDE;

	virtual bool   Start(void) OVERRIDE;
	virtual bool   Pause(bool bPause = true) OVERRIDE;
	virtual void   Stop(void) OVERRIDE;

	virtual bool   OnShareRoutine(Int nEvent, uintptr_t utParam = 0, LLong llParam = 0, CEventQueue::EVENT_TYPE eType = CEventQueue::EVENT_TYPE_NONE) OVERRIDE;//
	virtual bool   OnShareRoutine(Int nEvent, CEventBase& EventRef, LLong llParam = 0, CEventQueue::EVENT_TYPE eType = CEventQueue::EVENT_TYPE_REFCOUNT) OVERRIDE;
	virtual bool   OnShareRoutine(Int nEvent, CStream& Stream, LLong llParam = 0) OVERRIDE;//
	// CPAKHandler
	virtual bool   OnTcpDispatch(const PacketPtr& PktPtr, PTCP_PARAM pTcp) OVERRIDE;
	virtual bool   OnUdpDispatch(const PacketPtr& PktPtr, PUDP_PARAM pUdp) OVERRIDE;
	virtual bool   OnTcpAccept(KeyRef krAccept, KeyRef krListen) OVERRIDE;
	virtual bool   OnTcpConnect(UInt uError, KeyRef krConnect) OVERRIDE;
	virtual bool   OnTcpClose(KeyRef krSocket, LLong llLiveData) OVERRIDE;
	virtual bool   OnUdpClose(KeyRef krSocket, LLong llLiveData) OVERRIDE;
private:
	CLoginServer(const CLoginServer&);
	CLoginServer& operator=(const CLoginServer&);
	/// ��ȡ��������ö�����������
	bool  InitLoadShare(void);
	/// ��ʼ������
	bool  InitLoadConfig(void);

	/// �����������ö�����������
	void  ExitUnloadShare(void);
	/// �������
	void  ExitUnloadConfig(void);

	/// �����������ķ���������
	bool  StartConnectCenterServer(void);
	/// �������ӵ�½DB����������
	bool  StartConnectLoginDBServer(void);
	/// ����UDP������Ϸ����������
	bool  StartUDPService(void);
	/// ���������ͻ��˶���
	bool  StartTCPService(void);

	/// ֹͣ�������ķ�����
	void  StopConnectCenterServer(void);
	/// ֹͣ���ӵ�½DB������
	void  StopConnectLoginDBServer(void);
	/// ֹͣUDP������Ϸ������
	void  StopUDPService(void);
	/// ֹͣ�����ͻ���
	void  StopTCPService(void);

	/// �������ķ������������
	bool  DispatchCenterServer(const PacketPtr& PktPtr, KeyRef krSocket);
	/// �����½DB�������������
	bool  DispatchLoginDBServer(const PacketPtr& PktPtr, KeyRef krSocket);
	/// ������Ϸ�ͻ��˵������
	bool  DispatchGameService(const PacketPtr& PktPtr, KeyRef krSocket);

	/// �����ķ�����ע��
	void  LinkCenterServer(void);
	/// �����ķ�����ע��
	void  UnlinkCenterServer(void);
	/// ���½DB������ע��
	void  LinkLoginDBServer(void);
	/// ���½DB������ע��
	void  UnlinkLoginDBServer(void);

	/// �������б���
	bool  CheckAddrBlacklist(NET_ADDR& ClientAddr);
	/// ��ʱ���ͻ�����Ч��
	bool  CheckSessionTimeout(void);

	/// ��ʱ����������ķ����������Ƿ���Ч
	bool  CheckConnectCenterServer(void);
	/// ��ʱ������ӵ�½DB�����������Ƿ���Ч
	bool  CheckConnectLoginDBServer(void);
	/// ��ʱ���UDP������Ϸ�����������Ƿ���Ч
	bool  CheckUDPService(void);
	/// ��ʱ�������ͻ��˶����Ƿ���Ч
	bool  CheckTCPService(void);

	/// ͬ����������Ϣ������
	bool  SyncServerInfo(void);
	/// ͬ����Ϸ�������������鲢ͬ��������
	bool  SyncGameServerInfo(CStream& Stream);
	/// ͬ����Ϸ��������Ϣ���ͻ���
	bool  SyncGameServerInfo(void);

	/// �ͻ��˴���-��½
	bool  OnClientLink(CPAKLoginLink* pLink, KeyRef krSocket);
	/// �ͻ��˴���-�ǳ�
	bool  OnClientUnlink(CPAKLoginUnlink* pUnlink, KeyRef krSocket);
	/// �ͻ��˴���-ѡ����Ϸ������
	bool  OnClientSelectGame(CPAKLoginSelectGame* pSelect, KeyRef krSocket);

	/// ���͵�½���������ͻ���
	void  GameLinkAck(CPAKSessionAck* pAck);
	/// ���͵ǳ����������ͻ���
	void  GameUnlinkAck(CPAKSessionAck* pAck);
	/// ѡ����Ϸ�������ظ���
	void  SelectGameAck(CPAKSessionAck* pAck);
	/// ��֤��Ϸ�������ظ���
	void  LinkGameAck(CPAKLoginLinkGame* pLink);
	/// �û�����/�˳���Ϸ�Ŷӻ�����Ϸ����
	void  PlayGameAck(CPAKSessionAck* pAck);
private:
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// �Ự��������
	struct tagSESSION_DATA : public MObject
	{
	public:
		/// �Ự����״̬
		enum SESSION_STATUS
		{
			SESSION_STATUS_NONE,          ///< ��״̬
			SESSION_STATUS_LINK,          ///< �յ���½����
			SESSION_STATUS_LINKACK,       ///< DB�ظ���½�ɹ�
			SESSION_STATUS_SELECT,        ///< �յ�ѡ����Ϸ����������
			SESSION_STATUS_SELECTACK,     ///< ѡ�����Ϸ�������ظ���������
			SESSION_STATUS_GAME,          ///< ѡ�����Ϸ������ת����֤��2
			SESSION_STATUS_GAMEACK,       ///< ��Ϸ�������ظ���֤���, �ͻ����Ŷӻ�����Ϸ��
			SESSION_STATUS_UNLINK,        ///< �յ��ǳ�����
			SESSION_STATUS_UNLINKACK,     ///< �ǳ�״̬
		};
		/// �Ự����ʱ���
		enum SESSION_TIME
		{
			SESSION_TIME_LINK_MAX    = 5,             ///< ���������֤�������ѡ����Ϸ����, ���а����س̶��ۼ�����ʱ��
			SESSION_TIME_OUT_MIN     = 30000,         ///< ��½���ߵǳ��ύDB������, ѡ����Ϸ�������ȴ��ظ�ʱ��, ��ʱ��Ҫ�Ͽ��ͻ���
			SESSION_TIME_OUT_MAX     = 60 * 1000,     ///< ��ɫ�Ŷӻ�����Ϸ���ȴ�ʱ��
			SESSION_TIME_LIMIT       = 5 * 60 * 1000, ///< ��½����Ժ�, û�в������ȴ�ʱ��
		};
	public:
		tagSESSION_DATA(void) { Reset(); }
		~tagSESSION_DATA(void) { }

		void Reset(void) { memset(this, 0, sizeof(tagSESSION_DATA)); }
	public:
		LLong      llUserId;    ///< �û�Id
		LLong      llTimeout;   ///< ��ʱʱ����
		LLong      llOnline;    ///< ��½�ɹ���ʱ��(����ʱ��)
		Int        nGameId;     ///< ѡ�����Ϸ������Id
		Int        nStatus;     ///< �Ự����״̬
		Int        nCount;      ///< ���������֤�������ѡ����Ϸ���� <= SESSION_TIME_LINK_MAX
		NET_ADDR   CltAddr;     ///< ���ε�½�ͻ��˵�ַ
	};
	typedef struct tagSESSION_DATA   SESSION_DATA, *PSESSION_DATA;

	typedef tagDATA_MAP<DataRef, SESSION_DATA>   SESSION_MAP, *PSESSION_MAP;
	typedef CTMap<KeyRef, UInt>                  TIMEOUT_MAP, *PTIMEOUT_MAP;
	typedef CTMap<KeyRef, UInt>::PAIR            TIMEOUT_MAP_PAIR;
private:
	Int              m_nStatus;          ///< ������״̬
	bool             m_bCenterCnnted;    ///< �Ƿ��������ķ��������
	bool             m_bCenterLinked;    ///< �Ƿ������ķ�����ע�����
	bool             m_bLoginDBCnnted;   ///< �Ƿ����ӵ�½DB���������
	bool             m_bLoginDBLinked;   ///< �Ƿ����½DB������ע�����
	CEventHandler*   m_pUIHandler;       ///< ����ص��ӿ�
	CServerConfig*   m_pConfig;          ///< ���ö���
	KeyRef           m_krConnectCenter;  ///< ����, �������ķ�����
	KeyRef           m_krConnectLoginDB; ///< ����, ���ӵ�½DB������
	KeyRef           m_krUDPService;     ///< ����, ������Ϸ������
	KeyRef           m_krTCPService;     ///< ����, �����ͻ���
	ICommonServer*   m_pShareCenterSvr;  ///< ͬ�������ķ�����
	ICommonServer*   m_pShareLoginDBSvr; ///< ͬ���̵�½DB������
	ICommonServer*   m_pShareGameSvr;    ///< ͬ������Ϸ������
	CFileLog         m_FileLog;          ///< ���ı���־
	CNetworkPtr      m_NetworkPtr;       ///< �������
	LOGIN_ADDR       m_ServerInfo;       ///< ����������Ϣ
	SERVER_INFO      m_GameSvrInfo;      ///< ��Ϸ��������Ϣ
	SVR_GAME_MAP     m_GameSvrMap;       ///< ��Ϸ��������Ϣ  
	SESSION_MAP      m_SessionMap;       ///< �Ự�ͻ�����Ϣ
};

INLINE CLoginServer::CLoginServer(void)
: m_nStatus(STATUSC_NONE)
, m_bCenterCnnted(false)
, m_bCenterLinked(false)
, m_bLoginDBCnnted(false)
, m_bLoginDBLinked(false)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krConnectCenter(nullptr)
, m_krConnectLoginDB(nullptr)
, m_krUDPService(nullptr)
, m_krTCPService(nullptr)
, m_pShareCenterSvr(nullptr)
, m_pShareLoginDBSvr(nullptr)
, m_pShareGameSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::LoginServer)
{
}

INLINE CLoginServer::~CLoginServer(void)
{
}

INLINE CLoginServer::CLoginServer(const CLoginServer&)
: m_nStatus(STATUSC_NONE)
, m_bCenterCnnted(false)
, m_bCenterLinked(false)
, m_bLoginDBCnnted(false)
, m_bLoginDBLinked(false)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krConnectCenter(nullptr)
, m_krConnectLoginDB(nullptr)
, m_krUDPService(nullptr)
, m_krTCPService(nullptr)
, m_pShareCenterSvr(nullptr)
, m_pShareLoginDBSvr(nullptr)
, m_pShareGameSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::LoginServer)
{
}

INLINE CLoginServer& CLoginServer::operator=(const CLoginServer&)
{
	return (*this);
}

INLINE bool CLoginServer::OnShareRoutine(Int, uintptr_t, LLong, CEventQueue::EVENT_TYPE)
{
	return false;
}

#endif // __LOGIN_SERVER_IMP_H__
