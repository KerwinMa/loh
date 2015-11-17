//////////////////////////////////////////////////////////////////
//   The Legend of Heros                                        //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : packet.h                                     //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : �����                                           //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __PACKET_H__
#define __PACKET_H__

#pragma once

#include "datadef.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPAKHead : ���������ͷ
class CPAKHead : public CEventBase
{
public:
	static CPAKHead* Create(CStream&);
public:
	CPAKHead(UInt uEvent = 0, UInt uType = PAK_TYPE_NONE);
	virtual ~CPAKHead(void);

	CPAKHead(const CPAKHead& aSrc);
	CPAKHead& operator=(const CPAKHead& aSrc);

	virtual size_t Length(void) OVERRIDE;
	virtual void   Serialize(CStream&) OVERRIDE;

	UInt   GetEvent(void);
	void   SetEvent(UInt uEvent);

	UInt   GetType(void);
	void   SetType(UInt uType);

	UInt   GetSize(void);
	void   AdjustSize(void);
private:
	UInt   m_uEvent; // �����
	UInt   m_uType;  // �������ݰ�����
	UInt   m_uSize;  // �������С
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPAKSimple : �����ģ��
template <UInt uEvent, UInt uType>
class CPAKSimple : public CPAKHead
{
public:
	CPAKSimple(void);
	virtual ~CPAKSimple(void);

	CPAKSimple(const CPAKSimple& aSrc);
	CPAKSimple& operator=(const CPAKSimple& aSrc);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPAKAck : ��Ӧ���
class CPAKAck : public CPAKHead
{
public:
	CPAKAck(UInt uEvent = 0, UInt uType = PAK_TYPE_NONE);
	virtual ~CPAKAck(void);

	CPAKAck(const CPAKAck& aSrc);
	CPAKAck& operator=(const CPAKAck& aSrc);

	virtual size_t Length(void) OVERRIDE;
	virtual void   Serialize(CStream&) OVERRIDE;

	UInt   GetAck(void);
	void   SetAck(UInt uAck);
private:
	UInt   m_uAck;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CNetworkEventHandler<CNETDispatch<CPAKHead>> CPAKHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "packet.inl"

#endif // __PACKET_H__