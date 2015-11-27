//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : SelectServerImp.h                            //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update : 2015-11-25     version 0.0.0.5                    //
//   Detail : ѡ�������ʵ��                                     //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __SELECT_SERVER_IMP_H__
#define __SELECT_SERVER_IMP_H__

#pragma once

#include "selectpacket.h"
#include "servermap.h"
#include "serversort.h"
#include "ServerConfig.h"
#include "CommonServer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///ѡ�������ʵ��
/// - ��Ҫ���� :
/// -# ������ѡȡ��½��������ַͨ��UDP���͸�׼����½�Ŀͻ���
/// -# �����, ȫ��ʹ�������̺߳Ͷ�ʱ�߳�������
/// -# TODO!!! /// @todo ������У��δ���
///    a. ��ѡ���������չ�����ļ��ж�ȡ�������б�
///    b. ���Ӱ��������οͻ��˵���Ϣ��, �����⹥�����߶�����ӵļ������, �����س̶��ۼ�����ʱ��
///    c. ����Ҳ��Ҫ�������οͻ��˵���Ϣ�����ʾ����������
///    d. ��ʱ��(����С��200MS������3��)Ƶ��������, �����������������
///    e. ������ú�����+��ʱ������(����1Сʱ����һ�ε�, ������Сʱ��Ϊ2Сʱ)
///    f. ��ʱ��ַmap����������10000000(1ǧ��, 64λֱ�Ӻ�600M)Ϊ����, ��ʱ���(����2Сʱ����һ�ε�)
class CSelectServer : public ICommonServer, public CPAKHandler
{
public:
	CSelectServer(void);
	virtual ~CSelectServer(void);
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
	virtual bool   OnShareRoutine(Int nEvent, CEventBase& EventRef, LLong llParam = 0, CEventQueue::EVENT_TYPE eType = CEventQueue::EVENT_TYPE_REFCOUNT) OVERRIDE;//
	virtual bool   OnShareRoutine(Int nEvent, CStream& Stream, LLong llParam = 0) OVERRIDE;
	// CPAKHandler
	virtual bool   OnTcpDispatch(const PacketPtr& PktPtr, PTCP_PARAM pTcp) OVERRIDE;
	virtual bool   OnUdpDispatch(const PacketPtr& PktPtr, PUDP_PARAM pUdp) OVERRIDE;
	virtual bool   OnTcpAccept(KeyRef krAccept, KeyRef krListen) OVERRIDE;//
	virtual bool   OnTcpConnect(UInt uError, KeyRef krConnect) OVERRIDE;
	virtual bool   OnTcpClose(KeyRef krSocket, LLong llLiveData) OVERRIDE;
	virtual bool   OnUdpClose(KeyRef krSocket, LLong llLiveData) OVERRIDE;
private:
	CSelectServer(const CSelectServer&);
	CSelectServer& operator=(const CSelectServer&);
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
	/// ����UDP�����ͻ��˶���
	bool  StartUDPService(void);

	/// ֹͣ�������ķ�����
	void  StopConnectCenterServer(void);
	/// ֹͣUDP�����ͻ���
	void  StopUDPService(void);
	/// �����ķ�����ע��
	void  LinkCenterServer(void);
	/// �����ķ�����ע��
	void  UnlinkCenterServer(void);

	/// �������б���
	bool  CheckAddrBlacklist(NET_ADDR& ClientAddr);

	/// ��ʱ����������ķ����������Ƿ���Ч
	bool  CheckConnectCenterServer(void);
	/// ��ʱ���UDP�����ͻ��˶����Ƿ���Ч
	bool  CheckUDPService(void);

	/// ͬ����������Ϣ������
	bool  SyncServerInfo(void);
	/// ͬ����½���������������ͬ��������
	bool  SyncLoginSortInfo(CStream& Stream);
private:
	Int              m_nStatus;         ///< ������״̬
	bool             m_bCenterCnnted;   ///< �Ƿ��������ķ��������
	bool             m_bCenterLinked;   ///< �Ƿ������ķ�����ע�����
	CEventHandler*   m_pUIHandler;      ///< ����ص��ӿ�
	CServerConfig*   m_pConfig;         ///< ���ö���
	KeyRef           m_krConnectCenter; ///< ����, �������ķ�����
	KeyRef           m_krUDPService;    ///< ����, �����ͻ���
	ICommonServer*   m_pShareCenterSvr; ///< ͬ�������ķ�����
	CFileLog         m_FileLog;         ///< ���ı���־
	CNetworkPtr      m_NetworkPtr;      ///< �������
	SELECT_ADDR      m_ServerInfo;      ///< ����������Ϣ
	SERVER_INFO      m_LoginInfo;       ///< ��½��������ͳ������
	RESULT_CHAIN     m_SortChain;       ///< ��½������������
	SVR_LOGIN_MAP    m_LoginSvrMap;     ///< ��½��������Ϣ   
};

INLINE CSelectServer::CSelectServer(void)
: m_nStatus(STATUSC_NONE)
, m_bCenterCnnted(false)
, m_bCenterLinked(false)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krConnectCenter(nullptr)
, m_krUDPService(nullptr)
, m_pShareCenterSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::SelectServer)
{
}

INLINE CSelectServer::~CSelectServer(void)
{
}

INLINE CSelectServer::CSelectServer(const CSelectServer&)
: m_nStatus(STATUSC_NONE)
, m_bCenterCnnted(false)
, m_bCenterLinked(false)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krConnectCenter(nullptr)
, m_krUDPService(nullptr)
, m_pShareCenterSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::SelectServer)
{
}

INLINE CSelectServer& CSelectServer::operator=(const CSelectServer&)
{
	return (*this);
}

INLINE bool CSelectServer::OnShareRoutine(Int, uintptr_t, LLong, CEventQueue::EVENT_TYPE)
{
	return false;
}

INLINE bool CSelectServer::OnShareRoutine(Int, CEventBase&, LLong, CEventQueue::EVENT_TYPE)
{
	return false;
}

INLINE bool CSelectServer::OnTcpAccept(KeyRef, KeyRef)
{
	return false;
}
#endif // __SELECT_SERVER_IMP_H__
