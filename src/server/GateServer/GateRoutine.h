//////////////////////////////////////////////////////////////////
//   The Legend of Heros                                        //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : GateRoutine.h                                //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ���ط���������ʵ��                                 //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __GATE_ROUTINE_H__
#define __GATE_ROUTINE_H__

#pragma once

#include "CommonRoutine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGateRoutine
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
	// �ͻ��˵�½����
	// 1. ���ܵ���½������UDPת���Ŀͻ���(user-id)��½����
	// 2. �����½����+�ŶӶ���û�е�����������, UDP�ظ���½���������Ե�¼������(��½����ά��GATE_TIME_SELECT_GAME, û����ʱ���ڵ�½����ɾ��)
	// 3. �ͻ����յ���½�������ĵ�½���ص�ַ�Լ���½��֤��1, �������ط�����������֤��1У��
	// 4. ���ط�������֤��1У��ͨ��, ������֤��2ͨ���ͻ��˵�½������ת�����ͻ���
	// 5. �ͻ�����user-id+��֤��2�ٴν���У��, ͨ��У���, �����ŶӶ���(�����Ϸ������������)������Ϸ����
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// SESSION_DATA : ���ؿͻ��˵�½���߽�ɫѡȡ�Ŷ���Ϣ�ṹ����
	typedef struct tagSESSION_DATA
	{
	public:
		enum SESSION_STATUS
		{
			SESSION_STATUS_NONE,
			//
			SESSION_STATUS_READY,
			SESSION_STATUS_QUEUE,
			SESSION_STATUS_GAME,
			// ��ɫѡ��
			SESSION_STATUS_SELECT,    // �ͻ��������ɫѡ��״̬
			SESSION_STATUS_SELECTACK, // ��ʾ�������Ѿ���Ӧ����ɫ���ݷ����˿ͻ���
			// ������Ϸ
			SESSION_STATUS_PLAY,      // �ͻ������������Ϸ״̬
			// TODO!!!ע��:����ͬ���û�������Ϸ��, ��Ϸ��������Ҫ֪ͨԭ�ȵ����Ӳ��ر�����, Game DB��Ҫ��¼user-id��½��gate-keyref
			SESSION_STATUS_PLAYACK,   // ��ʾ�������Ѿ���Ӧ��������Ϸ�����˿ͻ���(��ϷDB�Ѿ������ݷ��͸�����Ϸ������)
			// 
			SESSION_STATUS_REALM,     // ������Ϸģʽ
		};

		enum SESSION_TIME
		{
			SESSION_TIME_SELECT_GAME = 1000,           // �����½���е����ȴ�ʱ��, ��ʱ��Ҫ�ӵ�½���������¿�ʼ��½��Ϸ����
			SESSION_TIME_SELECT_ROLE = 5 * 60 * 1000,  // ��ɫ����, û�в������ȴ�5����
			SESSION_TIME_REALM_IDLE  = 30 * 60 * 1000, // ������Ϸ, û�в������ȴ�30����
		};
	public:
		tagSESSION_DATA(void) : llUserId(0), llTimeout(0), llOnline(0), index(nullptr), nData(0), nStatus(SESSION_STATUS_NONE), llLoginIndex(0), pLoginServer(nullptr) { }
		~tagSESSION_DATA(void) { }
	public:
		LLong                  llUserId;      // 
		LLong                  llTimeout;     //
		LLong                  llOnline;      // online time[_time_t]
		PINDEX                 index;         // ������Ϸ�Ŷ�������
		Int                    nData;         // �Ŷ���� or ��ɫId
		Int                    nStatus;       // SESSION_STATUS, ��ʾ��ɫ��Ϣ�Ƿ��Ѿ����͸��ͻ���
		LLong                  llLoginIndex;  // login index
		ICommonServer*         pLoginServer;  // 
		CNETTraits::NET_ADDR   AddrLogin;     // [ipv4 or ipv6]
	}SESSION_DATA, *PSESSION_DATA;
	typedef tagDATA_MAP<DataRef, SESSION_DATA>    SESSION_DATA_MAP, *PSESSION_DATA_MAP;
	typedef CTList<PINDEX>                        SESSION_RANK_LST, *PSESSION_RANK_LST;
	typedef CTMap<KeyRef, UInt>                   TIMEOUT_MAP,      *PTIMEOUT_MAP;
	typedef CTMap<KeyRef, UInt>::PAIR             TIMEOUT_MAP_PAIR;
private:
	class CGateServer*        m_pServer;
	// 1. �����ط�������չ�����ļ��ж�ȡ�������б�
	// 2. ���Ӱ��������οͻ������ӵ���Ϣ��, �����⹥�����߶�����ӵļ������, �����س̶��ۼ�����ʱ��
	// 3. ����Ҳ��Ҫ�������οͻ��˵���Ϣ�����ʾ����������
	// 4. ����������ACK(1000MS)��Timeout(30000MS)����
	// 5. ��ʱ��(����С��200MS������3��)Ƶ��������, �����������������
	// 6. ������ú�����+��ʱ������(����1Сʱ����һ�ε�, ������Сʱ��Ϊ2Сʱ)
	// 7. ��ʱ��ַmap����������10000000(1ǧ��, 64λֱ�Ӻ�600M)Ϊ����, ��ʱ���(����2Сʱ����һ�ε�)
	Int                       m_nQueueCur;        // �ȴ���Ϸ��ǰ������к�
	Int                       m_nQueueEnd;        // �ȴ���Ϸ���������к�
	Int                       m_nQueueLimit;      // �ȴ���������:��½����+�Ŷ�����
	Int                       m_nGameCur;         // ��ǰ��Ϸ����
	Int                       m_nGameSelect;      // ��Ϸ����ͣ���ڽ�ɫ����������
	Int                       m_nGameLimit;       // ��Ϸ��������
	SESSION_RANK_LST          m_QueueRank;        // �Ŷ����ȼ�
	SESSION_DATA_MAP          m_SessionQueue;     // �Ŷ�
	SESSION_DATA_MAP          m_SessionLogin;     // ��½�Ŷ�
	SESSION_DATA_MAP          m_SessionGame;      // ��Ϸ

	CEventQueuePtr            m_EventQueuePtr;    // �¼����� 
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
