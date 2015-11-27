//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : packet.h                                     //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update : 2015-11-25     version 0.0.0.5                    //
//   Detail : �����                                           //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __PACKET_H__
#define __PACKET_H__

#pragma once

#include "datadef.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ���������ͷ
class CPAKHead : public CEventBase
{
public:
	/// ����CPAKHead�����ݰ��������
	/// @param[in] CStream& ������
	/// @retval CPAKHead* �����ɹ�����ָ��CPAKHead�����ָ��, ʧ�ܷ���nullptr
	static CPAKHead* Create(CStream&);
public:
	/// ����
	/// @param[in] �����
	/// @param[in] ���ݰ�����
	CPAKHead(UInt uEvent = 0, UInt uType = PAK_TYPE_NONE);
	virtual ~CPAKHead(void);

	CPAKHead(const CPAKHead& aSrc);
	CPAKHead& operator=(const CPAKHead& aSrc);
	/// �������ݰ���С
	/// @remarks AdjustSize����Length����ֵ���m_uSize
	virtual size_t Length(void) OVERRIDE;
	/// �����ݰ����л�
	/// @param[in] CStream& ������
	virtual void   Serialize(CStream&) OVERRIDE;
	/// ��ȡ�����
	UInt   GetEvent(void);
	/// ���������
	void   SetEvent(UInt uEvent);
	/// ��ȡ����
	UInt   GetType(void);
	/// ��������
	void   SetType(UInt uType);
	/// ��ȡ���ݰ���С
	UInt   GetSize(void);
	/// У�����ݰ���С
	void   AdjustSize(void);
private:
	UInt   m_uEvent; ///< �����
	UInt   m_uType;  ///< �������ݰ�����
	UInt   m_uSize;  ///< ���ݰ���С, �������������ͷ
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// �����ģ��
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
/// ��Ӧ���
class CPAKAck : public CPAKHead
{
public:
	CPAKAck(UInt uEvent = 0, UInt uType = PAK_TYPE_NONE);
	virtual ~CPAKAck(void);

	CPAKAck(const CPAKAck& aSrc);
	CPAKAck& operator=(const CPAKAck& aSrc);

	virtual size_t Length(void) OVERRIDE;
	virtual void   Serialize(CStream&) OVERRIDE;
	/// ��ȡACK
	UInt   GetAck(void);
	/// ����ACK
	void   SetAck(UInt uAck);
private:
	UInt   m_uAck; ///< 0-�ɹ�, ����ֵ��ʾ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// �������������ɷ�����
typedef CNetworkEventHandler<CNETDispatch<CPAKHead>> CPAKHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "packet.inl"

#endif // __PACKET_H__