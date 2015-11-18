//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : commonpacket.h                               //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ͨ�������                                         //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __COMMON_PACKET_H__
#define __COMMON_PACKET_H__

#pragma once

#include "packet.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����
enum PAK_EVENT
{
	PAK_EVENT_LIVE          = (CNETTraits::EVENT_RESERVED + 1), // ������, CPAKSimple<PAK_EVENT_LIVE, PAK_TYPE>
	PAK_EVENT_LIVEACK,      // ������Ack, CPAKSimple<PAK_EVENT_LIVEACK, PAK_TYPE>

	PAK_EVENT_LINK,         // ����, CPAKLink
	PAK_EVENT_LINKACK,      // ����Ack, CPAKHead<PAK_EVENT_LINKACK, PAK_TYPE>

	PAK_EVENT_UPDATE,       // ����, CPAKUpdate
	PAK_EVENT_UPDATEACK,    // ����Ack, CPAKHead<PAK_EVENT_UPDATEACK, PAK_TYPE>

	PAK_EVENT_SYNC,         // ͬ��, CPAKSync
	PAK_EVENT_SYNCACK,      // ͬ��Ack, CPAKHead<PAK_EVENT_SYNCACK, PAK_TYPE>

	PAK_EVENT_UNLINK,       // ����, CPAKHead<PAK_EVENT_UNLINK, PAK_TYPE>
	PAK_EVENT_UNLINKACK,    // ����Ack, CPAKHead<PAK_EVENT_UNLINKACK, PAK_TYPE>
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPAKServerData
template <UInt uEvent, UInt uType>
class CPAKServerData : public CPAKSimple<uEvent, uType>
{
public:
	CPAKServerData(void);
	virtual ~CPAKServerData(void);

	CPAKServerData(const CPAKServerData& aSrc);
	CPAKServerData& operator=(const CPAKServerData& aSrc);

	virtual size_t Length(void) OVERRIDE;
	virtual void   Serialize(CStream&) OVERRIDE;

	bool           CheckStream(void);
	CStream&       GetStream(void);
	void           SetStream(CStream& Stream);

	SERVER_DATA&   GetServerData(void);
	void           SetServerData(SERVER_DATA& ServerData);
private:
	CStream*       m_pStream;    // ��������, ���紫�����л�ʱ���Դ�С
	SERVER_DATA    m_ServerData;
};

typedef CPAKServerData<PAK_EVENT_LINK, PAK_TYPE_NONE>       CPAKLink;
typedef CPAKServerData<PAK_EVENT_UPDATE, PAK_TYPE_NONE>     CPAKUpdate;
typedef CPAKServerData<PAK_EVENT_SYNC, PAK_TYPE_NONE>       CPAKSync;

typedef CPAKServerData<PAK_EVENT_SYNC, PAK_TYPE_CENTER>     CCenterSync;
typedef CPAKServerData<PAK_EVENT_SYNC, PAK_TYPE_LOGIN>      CLoginSync;

typedef CPAKServerData<PAK_EVENT_LINK, PAK_TYPE_SELECT>     CSelectLink;
typedef CPAKServerData<PAK_EVENT_UPDATE, PAK_TYPE_SELECT>   CSelectUpdate;

typedef CPAKServerData<PAK_EVENT_LINK, PAK_TYPE_LOGIN>      CLoginLink;
typedef CPAKServerData<PAK_EVENT_UPDATE, PAK_TYPE_LOGIN>    CLoginUpdate;

typedef CPAKServerData<PAK_EVENT_LINK, PAK_TYPE_GAME>       CGameLink;
typedef CPAKServerData<PAK_EVENT_UPDATE, PAK_TYPE_GAME>     CGameUpdate;

typedef CPAKServerData<PAK_EVENT_LINK, PAK_TYPE_ZONE>       CZoneLink;
typedef CPAKServerData<PAK_EVENT_UPDATE, PAK_TYPE_ZONE>     CZoneUpdate;

typedef CPAKServerData<PAK_EVENT_LINK, PAK_TYPE_GATE>       CGateLink;
typedef CPAKServerData<PAK_EVENT_UPDATE, PAK_TYPE_GATE>     CGateUpdate;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPAKSession : Session
class CPAKSession : public CPAKHead
{
public:
	CPAKSession(UInt uEvent, UInt uType = PAK_TYPE_NONE);
	virtual ~CPAKSession(void);

	CPAKSession(const CPAKSession& aSrc);
	CPAKSession& operator=(const CPAKSession& aSrc);

	virtual size_t Length(void) OVERRIDE;
	virtual void   Serialize(CStream&) OVERRIDE;

	LLong          GetSessionId(void);
	void           SetSessionId(LLong llSessionId);
private:
	LLong          m_llSessionId;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPAKSessionAck : SessionAck
class CPAKSessionAck : public CPAKAck
{
public:
	CPAKSessionAck(UInt uEvent, UInt uType = PAK_TYPE_NONE);
	virtual ~CPAKSessionAck(void);

	CPAKSessionAck(const CPAKSessionAck& aSrc);
	CPAKSessionAck& operator=(const CPAKSessionAck& aSrc);

	virtual size_t Length(void) OVERRIDE;
	virtual void   Serialize(CStream&) OVERRIDE;

	LLong          GetSessionId(void);
	void           SetSessionId(LLong llSessionId);
private:
	LLong          m_llSessionId;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "commonpacket.inl"

#endif // __COMMON_PACKET_H__