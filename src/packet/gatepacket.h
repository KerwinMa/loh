//////////////////////////////////////////////////////////////////
//   The Legend of Heros                                        //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : gatepacket.h                                 //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ���ط���������                                     //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __GATE_PACKET_H__
#define __GATE_PACKET_H__

#pragma once

#include "commonpacket.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
enum GATE_ERROR
{
	GATE_ERROR_SESSION = (DATAD_FAIL + 1),  // session�������״̬����
	GATE_ERROR_AUTHCODE1,                   // ��֤��1У��ʧ��
	GATE_ERROR_AUTHCODE2,                   // ��֤��2У��ʧ��
	GATE_ERROR_USERID,                      // �û�id����
	GATE_ERROR_QUEUE,                       // �ŶӲ�ѯʧ��
	GATE_ERROR_GAME,                        // ��Ϸ��״̬����, ����������ʶ�Ǻ���״̬
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// gate�����
enum PAK_EVENT_GATE
{
	PAK_EVENT_GATE_QUEUE = (PAK_EVENT_GATE_BEGIN + 1), // �����Ŷ���Ϣ, CPAKSession
	PAK_EVENT_GATE_QUEUEACK,                           // �����Ŷ���ϢAck, CPAKGateQueueAck or CPAKAck(error)

	PAK_EVENT_GATE_SELECT,                             // ����ѡ���ɫ��Ϣ, CPAKGateSelect
	PAK_EVENT_GATE_SELECTACK,                          // ����ѡ���ɫ��ϢAck, CPAKGateSelectAck or CPAKAck(error)

	PAK_EVENT_GATE_PLAY,                               // ����ѡ��Ľ�ɫ������Ϸ, CPAKGatePlay
	PAK_EVENT_GATE_PLAYACK,                            // ����ѡ��Ľ�ɫ������ϷAck, CPAKSessionAck or CPAKAck(error)
	// TODO!!!��ɫ�����ǿͻ�����Ҫ�����Ƿ����Ŷ��л�����Ϸ�е�
	PAK_EVENT_GATE_CREATE,                             // ���봴����ɫ
	PAK_EVENT_GATE_CREATEACK,                          // ���봴����ɫAck or CPAKAck(error)

	PAK_EVENT_GATE_DELETE,                             // ����ɾ����ɫ
	PAK_EVENT_GATE_DELETEACK,                          // ����ɾ����ɫAck or CPAKAck(error)

	PAK_EVENT_GATE_RECOVER,                            // ����ָ���ɫ
	PAK_EVENT_GATE_RECOVERACK,                         // ����ָ���ɫAck or CPAKAck(error)
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPAKGateQueueAck : �����Ŷ���ϢAck
class CPAKGateQueueAck : public CPAKSessionAck
{
public:
	CPAKGateQueueAck(UInt uEvent = PAK_EVENT_GATE_QUEUEACK, UInt uType = PAK_TYPE_GATE);
	virtual ~CPAKGateQueueAck(void);

	CPAKGateQueueAck(const CPAKGateQueueAck& aSrc);
	CPAKGateQueueAck& operator=(const CPAKGateQueueAck& aSrc);

	virtual size_t Length(void) OVERRIDE;
	virtual void   Serialize(CStream&) OVERRIDE;

	Int    GetParam(void);
	void   SetParam(Int nParam);
private:
	Int    m_nParam; // ack=0, m_nParam=0��ʾ���Խ�����Ϸ��
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPAKGateSelect : ����ѡ���ɫ��Ϣ
class CPAKGateSelect : public CPAKSession
{
public:
	CPAKGateSelect(UInt uEvent = PAK_EVENT_GATE_SELECT, UInt uType = PAK_TYPE_GATE);
	virtual ~CPAKGateSelect(void);

	CPAKGateSelect(const CPAKGateSelect& aSrc);
	CPAKGateSelect& operator=(const CPAKGateSelect& aSrc);

	virtual size_t Length(void) OVERRIDE;
	virtual void   Serialize(CStream&) OVERRIDE;

	LLong          GetUserId(void);
	void           SetUserId(LLong llUserId);
private:
	LLong          m_llUserId;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPAKGateSelectAck : ����ѡ���ɫ��ϢAck
class CPAKGateSelectAck : public CPAKSessionAck 
{
public:
	CPAKGateSelectAck(UInt uEvent = PAK_EVENT_GATE_SELECTACK, UInt uType = PAK_TYPE_GATE);
	virtual ~CPAKGateSelectAck(void);

	CPAKGateSelectAck(const CPAKGateSelectAck& aSrc);
	CPAKGateSelectAck& operator=(const CPAKGateSelectAck& aSrc);

	virtual size_t Length(void) OVERRIDE;
	virtual void   Serialize(CStream&) OVERRIDE;

	UInt       GetRoleCount(void);
	void       SetRoleCount(UInt uCount);

	bool       CheckStream(void);
	CStream&   GetStream(void);
	void       SetStream(CStream& Stream);
private:
	UInt       m_uRoleCount; // ��ɫ����
	CStream*   m_pStream;    // ��ɫ��Ϣ
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPAKGatePlay : ���������Ϸ������Ϣ
class CPAKGatePlay : public CPAKSession
{
public:
	CPAKGatePlay(UInt uEvent = PAK_EVENT_GATE_PLAY, UInt uType = PAK_TYPE_GATE);
	virtual ~CPAKGatePlay(void);

	CPAKGatePlay(const CPAKGatePlay& aSrc);
	CPAKGatePlay& operator=(const CPAKGatePlay& aSrc);

	virtual size_t Length(void) OVERRIDE;
	virtual void   Serialize(CStream&) OVERRIDE;

	LLong          GetUserId(void);
	void           SetUserId(LLong llUserId);

	Int            GetRoleId(void);
	void           SetRoleId(Int nRoleId);
private:
	LLong          m_llUserId;
	Int            m_nRoleId;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "gatepacket.inl"

#endif // __GATE_PACKET_H__