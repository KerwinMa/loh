//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : gatepacket.h                                 //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update : 2015-11-25     version 0.0.0.5                    //
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
	GATE_ERROR_SESSION = (DATAD_FAIL + 1),  // �Ự�������״̬����
	GATE_ERROR_AUTHCODE1,                   // ��֤��1У��ʧ��
	GATE_ERROR_AUTHCODE2,                   // ��֤��2У��ʧ��
	GATE_ERROR_USERID,                      // �û�id����
	GATE_ERROR_QUEUE,                       // �ŶӲ�ѯʧ��
	GATE_ERROR_GAME,                        // ��Ϸ��״̬����, ����������ʶ�Ǻ���״̬
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ط����������
enum PAK_EVENT_GATE
{
	PAK_EVENT_GATE_QUEUE = (PAK_EVENT_GATE_BEGIN + 1), // �����Ŷ���Ϣ, CPAKSession
	PAK_EVENT_GATE_QUEUEACK,                           // �����Ŷ���ϢAck, CPAKGateQueueAck or CPAKAck(error)

	PAK_EVENT_GATE_SELECT,                             // ����ѡ���ɫ��Ϣ, CPAKGateSelect
	PAK_EVENT_GATE_SELECTACK,                          // ����ѡ���ɫ��ϢAck, CPAKGateSelectAck or CPAKAck(error)

	PAK_EVENT_GATE_PLAY,                               // ����ѡ��Ľ�ɫ������Ϸ, CPAKGatePlay
	PAK_EVENT_GATE_PLAYACK,                            // ����ѡ��Ľ�ɫ������ϷAck, CPAKSessionAck or CPAKAck(error)

	PAK_EVENT_GATE_CREATE,                             // ���봴����ɫ
	PAK_EVENT_GATE_CREATEACK,                          // ���봴����ɫAck or CPAKAck(error)

	PAK_EVENT_GATE_DELETE,                             // ����ɾ����ɫ
	PAK_EVENT_GATE_DELETEACK,                          // ����ɾ����ɫAck or CPAKAck(error)

	PAK_EVENT_GATE_RECOVER,                            // ����ָ���ɫ
	PAK_EVENT_GATE_RECOVERACK,                         // ����ָ���ɫAck or CPAKAck(error)
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// �����Ŷ���ϢAck
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
	///< ��ѯ�ɹ����ҷ��ص�m_nParamֵΪ0, ��ʾֱ�ӿ��Խ�����Ϸ; ����0��ʾ�Ŷ���
	///< ��ѯ����ACK=GATE_ERROR_GAME, ��ʾ��ǰ�Ѿ�����Ϸ״̬, ����Ҫ�Ŷ�, m_nParamֵΪ�������Ϸ״̬
	Int    m_nParam; 
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ����ѡ���ɫ��Ϣ
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

	Int            GetGameId(void);
	void           SetGameId(Int nGameId);
private:
	LLong          m_llUserId; ///< �û�Id
	Int            m_nGameId;  ///< ��Ϸ������Id, ��ϷDB���������Թ�������Ϸ������, ��ѯʱ��Ҫ�ṩ�ĸ���Ϸ������(��)������
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ����ѡ���ɫ��ϢAck
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
	UInt       m_uRoleCount; ///< ��ɫ����
	CStream*   m_pStream;    ///< ��ɫ��Ϣ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ���������Ϸ������Ϣ
class CPAKGatePlay : public CPAKSession
{
public:
	CPAKGatePlay(UInt uEvent = PAK_EVENT_GATE_PLAY, UInt uType = PAK_TYPE_GATE);
	virtual ~CPAKGatePlay(void);

	CPAKGatePlay(const CPAKGatePlay& aSrc);
	CPAKGatePlay& operator=(const CPAKGatePlay& aSrc);

	virtual size_t Length(void) OVERRIDE;
	virtual void   Serialize(CStream&) OVERRIDE;

	Int            GetRoleId(void);
	void           SetRoleId(Int nRoleId);
private:
	Int            m_nRoleId; ///< ��ɫId
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "gatepacket.inl"

#endif // __GATE_PACKET_H__