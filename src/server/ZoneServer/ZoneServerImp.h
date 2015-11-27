//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : ZoneServerImp.h                              //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update : 2015-11-25     version 0.0.0.5                    //
//   Detail : ��ͼ������ʵ��                                     //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __ZONE_SERVER_IMP_H__
#define __ZONE_SERVER_IMP_H__

#pragma once

#include "zonepacket.h"
#include "servermap.h"
#include "ServerConfig.h"
#include "CommonServer.h"
#include "CommonRoutine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ��ͼ������ʵ��
/// - ��Ҫ���� :
/// -# ����ͻ��˵���Ϸ����
class CZoneServer : public ICommonServer, public CPAKHandler
{
public:
	CZoneServer(void);
	virtual ~CZoneServer(void);
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
	virtual bool   OnUdpDispatch(const PacketPtr& PktPtr, PUDP_PARAM pUdp) OVERRIDE;//
	virtual bool   OnTcpAccept(KeyRef krAccept, KeyRef krListen) OVERRIDE;//
	virtual bool   OnTcpConnect(UInt uError, KeyRef krConnect) OVERRIDE;
	virtual bool   OnTcpClose(KeyRef krSocket, LLong llLiveData) OVERRIDE;
	virtual bool   OnUdpClose(KeyRef krSocket, LLong llLiveData) OVERRIDE;//
	// ...
	CEventHandler* GetUIHandler(void);
	CServerConfig* GetConfig(void);
	ICommonServer* GetShareGameServer(void);
	CFileLog&      GetFileLog(void);
	CNetworkPtr&   GetNetworkPtr(void);
private:
	CZoneServer(const CZoneServer&);
	CZoneServer& operator=(const CZoneServer&);
	/// ��ȡ��������ö�����������
	bool  InitLoadShare(void);
	/// ��ʼ������
	bool  InitLoadConfig(void);

	/// �����������ö�����������
	void  ExitUnloadShare(void);
	/// �������
	void  ExitUnloadConfig(void);

	/// ����������Ϸ����������
	bool  StartConnectGameServer(void);

	/// ֹͣ������Ϸ������
	void  StopConnectGameServer(void);

	/// ����Ϸ������ע��
	void  LinkGameServer(void);
	/// ����Ϸ������ע��
	void  UnlinkGameServer(void);

	/// ��ʱ���������Ϸ�����������Ƿ���Ч
	bool  CheckConnectGameServer(void);

	/// ͬ����������Ϣ������
	bool  SyncServerInfo(void);
private:
	Int                    m_nStatus;         ///< ������״̬
	bool                   m_bGameCnnted;     ///< �Ƿ�������Ϸ���������
	bool                   m_bGameLinked;     ///< �Ƿ�����Ϸ������ע�����
	CEventHandler*         m_pUIHandler;      ///< ����ص��ӿ�
	CServerConfig*         m_pConfig;         ///< ���ö���
	KeyRef                 m_krConnectGame;   ///< ����, ������Ϸ������
	ICommonServer*         m_pShareGameSvr;   ///< ͬ������Ϸ������
	CFileLog               m_FileLog;         ///< ���ı���־
	CNetworkPtr            m_NetworkPtr;      ///< �������
	CCommonRoutinePtr      m_ZoneRoutine;     ///< ��Ϸ������ 
	ZONE_ADDR              m_ServerInfo;      ///< ����������Ϣ, �ܽ����ͼ����-��ǰ��ͼ��������-��ͼά���ĵ�ͼ����-��ͼά�����߳�����
};

INLINE CZoneServer::CZoneServer(void)
: m_nStatus(STATUSC_NONE)
, m_bGameCnnted(false)
, m_bGameLinked(false)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krConnectGame(nullptr)
, m_pShareGameSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::ZoneServer)
{
}

INLINE CZoneServer::~CZoneServer(void)
{
}

INLINE CZoneServer::CZoneServer(const CZoneServer&)
: m_nStatus(STATUSC_NONE)
, m_bGameCnnted(false)
, m_bGameLinked(false)
, m_pUIHandler(nullptr)
, m_pConfig(nullptr)
, m_krConnectGame(nullptr)
, m_pShareGameSvr(nullptr)
, m_FileLog(true, LOGL_ALL, nullptr, CServerConfig::ZoneServer)
{
}

INLINE CZoneServer& CZoneServer::operator=(const CZoneServer&)
{
	return (*this);
}

INLINE bool CZoneServer::OnShareRoutine(Int, uintptr_t, LLong, CEventQueue::EVENT_TYPE)
{
	return false;
}

INLINE bool CZoneServer::OnShareRoutine(Int, CStream&, LLong)
{
	return false;
}

INLINE bool CZoneServer::OnUdpDispatch(const PacketPtr&, PUDP_PARAM)
{
	// PUDP_PARAM->pCache == nullptr, �ϲ���Ա���ӵײ�����Ļ�������
	// PUDP_PARAM->pCache, PUDP_PARAM->index�ǵײ�����Ļ������ݵ���ʼ��ַ��������
	// ����ϲ㱣�滺��, ����PUDP_PARAM->pDataΪnullptr֪ͨ����ײ�
	// �ͷŻ���ʹ��MObject::sMCFree(index, pCache)
	return false;
}

INLINE bool CZoneServer::OnTcpAccept(KeyRef, KeyRef)
{
	return false;
}

INLINE bool CZoneServer::OnUdpClose(KeyRef, LLong)
{
	return false;
}

INLINE CEventHandler* CZoneServer::GetUIHandler(void)
{
	return m_pUIHandler;
}

INLINE CServerConfig* CZoneServer::GetConfig(void)
{
	return m_pConfig;
}

INLINE ICommonServer* CZoneServer::GetShareGameServer(void)
{
	return m_pShareGameSvr;
}

INLINE CFileLog& CZoneServer::GetFileLog(void)
{
	return m_FileLog;
}

INLINE CNetworkPtr& CZoneServer::GetNetworkPtr(void)
{
	return m_NetworkPtr;
}

#endif // __ZONE_SERVER_IMP_H__
