//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : GateRoutine.h                                //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update : 2015-11-25     version 0.0.0.5                    //
//   Detail : ���ط���������ʵ��                                 //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __GATE_ROUTINE_H__
#define __GATE_ROUTINE_H__

#pragma once

#include "CommonRoutine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ���ط���������ʵ��
/// - �����ط�������չ�����ļ��ж�ȡ�������б�
/// - ���Ӱ��������οͻ������ӵ���Ϣ��, �����⹥�����߶�����ӵļ������, �����س̶��ۼ�����ʱ��
/// - ����Ҳ��Ҫ�������οͻ��˵���Ϣ�����ʾ����������
/// - ����������ACK(1000MS)��Timeout(30000MS)����
/// - ��ʱ��(����С��200MS������3��)Ƶ��������, �����������������
/// - ������ú�����+��ʱ������(����1Сʱ����һ�ε�, ������Сʱ��Ϊ2Сʱ)
/// - ��ʱ��ַmap����������10000000(1ǧ��, 64λֱ�Ӻ�600M)Ϊ����, ��ʱ���(����2Сʱ����һ�ε�)
class CGateRoutine : public ICommonRoutine
{
	DECLARE_RTTI_CREATE( CGateRoutine )
public:
	enum SESSION_LIMIT
	{
		SESSION_LIMIT_QUEUE  = 128,
		SESSION_LIMIT_GAME   = 128,
	};
public:
	CGateRoutine(void);
	virtual ~CGateRoutine(void);
	// CEventHandler
	virtual bool OnHandle(Int nEvent, uintptr_t utParam, LLong llParam) OVERRIDE;
	virtual bool OnHandle(Int nEvent, CEventBase&, LLong) OVERRIDE;
	virtual bool OnHandle(Int nEvent, CStream& Stream, LLong llParam) OVERRIDE;//
	virtual bool OnHandle(Int nEvent, UInt uCount) OVERRIDE;//
	// ICommonRoutine
	virtual bool Start(class ICommonServer* pServer) OVERRIDE;
	virtual bool Pause(bool bPause = true) OVERRIDE;
	virtual bool Update(void) OVERRIDE;
	virtual void Stop(void) OVERRIDE;

	virtual bool Add(Int nEvent, uintptr_t utParam = 0, LLong llParam = 0, CEventQueue::EVENT_TYPE eType = CEventQueue::EVENT_TYPE_NONE) OVERRIDE;//
	virtual bool Add(Int nEvent, CEventBase& EventRef, LLong llParam = 0, CEventQueue::EVENT_TYPE eType = CEventQueue::EVENT_TYPE_REFCOUNT) OVERRIDE;
	virtual bool Add(Int nEvent, CStream& Stream, LLong llParam = 0) OVERRIDE;//
private:
	// �������б���
	bool  CheckAddrBlacklist(CNETTraits::NET_ADDR& ClientAddr);

	bool  CheckSessionTimeout(void);

	void SelectGame(CPAKLoginSelectGame* pSelect, LLong llParam);
	void LinkGame(CPAKLoginLinkGame* pLink, KeyRef krSocket);
	// ��½��Ϸ��������һ��У��
	void LinkGame1(LLong llDataRef, CPAKLoginLinkGame* pLink, KeyRef krSocket);
	// ��½��Ϸ�������ڶ���У��
	void LinkGame2(LLong llDataRef, CPAKLoginLinkGame* pLink, KeyRef krSocket);

	void QueueRank(CPAKSession* pQueue, KeyRef krSocket);
	void SelectRole(CPAKGateSelect* pSelect, KeyRef krSocket);
	void PlayRole(CPAKGatePlay* pPlay, KeyRef krSocket);

	void SelectRoleAck(CPAKSessionAck* pAck);
	void PlayRoleAck(CPAKSessionAck* pAck);
private:
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// �Ự��������
	struct tagSESSION_DATA : public MObject
	{
	public:
		/// �Ự����״̬
		enum SESSION_STATUS
		{
			SESSION_STATUS_NONE,      ///< ��״̬
			//
			SESSION_STATUS_READY,     ///< ׼��״̬
			SESSION_STATUS_QUEUE,     ///< �Ŷ�״̬
			SESSION_STATUS_GAME,      ///< ��Ϸ״̬
			// ��ɫѡ��
			SESSION_STATUS_SELECT,    ///< �ͻ��������ɫѡ��״̬
			SESSION_STATUS_SELECTACK, ///< ��ʾ�������Ѿ���Ӧ����ɫ���ݷ����˿ͻ���
			// ������Ϸ
			SESSION_STATUS_PLAY,      ///< �ͻ������������Ϸ״̬
			/// TODO!!! /// @todo ע��:����ͬ���û�������Ϸ��, ��Ϸ��������Ҫ֪ͨԭ�ȵ����Ӳ��ر�����
			SESSION_STATUS_PLAYACK,   ///< ��ʾ�������Ѿ���Ӧ��������Ϸ�����˿ͻ���(��ϷDB�Ѿ������ݶ��뻺��ȴ���Ϸ��������ȡ)
			// 
			SESSION_STATUS_REALM,     ///< ������Ϸ״̬
		};
		/// �Ự����ʱ���
		enum SESSION_TIME
		{
			SESSION_TIME_SELECT_GAME = 1000,           ///< �����½���е����ȴ�ʱ��, ��ʱ��Ҫ�ӵ�½���������¿�ʼ��½��Ϸ����
			SESSION_TIME_SELECT_ROLE = 5 * 60 * 1000,  ///< ��ɫ����, û�в������ȴ�5����
			SESSION_TIME_REALM_IDLE  = 30 * 60 * 1000, ///< ������Ϸ, û�в������ȴ�30����
		};
	public:
		tagSESSION_DATA(void) : llUserId(0), llTimeout(0), llOnline(0), index(nullptr), nParam(0), nStatus(SESSION_STATUS_NONE), llLoginIndex(0), pLoginServer(nullptr) { }
		~tagSESSION_DATA(void) { }
	public:
		LLong                  llUserId;      ///< �û�Id
		LLong                  llTimeout;     ///< ��ʱʱ����
		LLong                  llOnline;      ///< ��½�ɹ���ʱ��(����ʱ��)
		PINDEX                 index;         ///< ������Ϸ�Ŷ�������
		Int                    nParam;        ///< �Ŷ���� or ʹ�ý�ɫId
		Int                    nStatus;       ///< �Ự����״̬
		LLong                  llLoginIndex;  ///< ��½����������ֵ
		ICommonServer*         pLoginServer;  ///< ͬ���̵�½������
		CNETTraits::NET_ADDR   AddrLogin;     ///< ��½��������ַ[ipv4 or ipv6]
	};
	typedef struct tagSESSION_DATA   SESSION_DATA, *PSESSION_DATA;
	typedef tagDATA_MAP<DataRef, SESSION_DATA>    SESSION_DATA_MAP, *PSESSION_DATA_MAP;
	typedef CTList<PINDEX>                        SESSION_RANK_LST, *PSESSION_RANK_LST;
	typedef CTMap<KeyRef, UInt>                   TIMEOUT_MAP,      *PTIMEOUT_MAP;
	typedef CTMap<KeyRef, UInt>::PAIR             TIMEOUT_MAP_PAIR;
private:
	class CGateServer*        m_pServer;          ///< �������Ӧ�ķ�����
	Int                       m_nQueueCur;        ///< �ȴ���Ϸ��ǰ������к�
	Int                       m_nQueueEnd;        ///< �ȴ���Ϸ���������к�
	Int                       m_nQueueLimit;      ///< �ȴ���������:��½����+�Ŷ�����
	Int                       m_nGameCur;         ///< ��ǰ��Ϸ����
	Int                       m_nGameSelect;      ///< ��Ϸ����ͣ���ڽ�ɫ����������
	Int                       m_nGameLimit;       ///< ��Ϸ��������
	SESSION_RANK_LST          m_QueueRank;        ///< �ŶӶ������ȼ�
	SESSION_DATA_MAP          m_SessionQueue;     ///< �Ŷ�
	SESSION_DATA_MAP          m_SessionLogin;     ///< ��½
	SESSION_DATA_MAP          m_SessionGame;      ///< ��Ϸ

	CEventQueuePtr            m_EventQueuePtr;    ///< �¼����� 
};

INLINE CGateRoutine::CGateRoutine(void)
: m_pServer(nullptr)
, m_nQueueCur(0)
, m_nQueueEnd(0)
, m_nQueueLimit(SESSION_LIMIT_QUEUE)
, m_nGameCur(0)
, m_nGameSelect(0)
, m_nGameLimit(SESSION_LIMIT_GAME)
{
}

INLINE CGateRoutine::~CGateRoutine(void)
{
}

INLINE bool CGateRoutine::OnHandle(Int, CStream&, LLong)
{ 
	return false; 
}

INLINE bool CGateRoutine::OnHandle(Int, UInt)
{ 
	return false;
}

INLINE bool CGateRoutine::Add(Int, uintptr_t, LLong, CEventQueue::EVENT_TYPE)
{
	return false;
}

INLINE bool CGateRoutine::Add(Int, CStream&, LLong)
{
	return false;
}

#endif // __GATE_ROUTINE_H__
