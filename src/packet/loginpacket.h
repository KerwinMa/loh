//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : loginpacket.h                                //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update : 2015-11-25     version 0.0.0.5                    //
//   Detail : ��½����������                                     //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __LOGIN_PACKET_H__
#define __LOGIN_PACKET_H__

#pragma once

#include "commonpacket.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
enum LOGIN_ERROR
{
	LOGIN_ERROR_SESSION = (DATAD_FAIL + 1),   // �Ự�������״̬����
	LOGIN_ERROR_ACK_TIMEOUT,                  // ����ʱ

	LOGIN_ERROR_LINK_ID,
	LOGIN_ERROR_LINK_PW,

	LOGIN_ERROR_LINK_LIMIT,                   // ��½��֤����������, һ��ʱ���ڲ����ٵ�½
	LOGIN_ERROR_SELECT_LIMIT,                 // �л���Ϸ����������������, һ��ʱ���ڲ����ٵ�½
	LOGIN_ERROR_LINK_IDLE,                    // ��½�Ժ�δ�����κβ���, ��½�������Զ��Ͽ��ͻ���
	LOGIN_ERROR_GAME_PLAY,                    // �Ŷӻ�����Ϸ���ض�ʱ��û���˳�, ��½�������Զ��Ͽ��ͻ���
	LOGIN_ERROR_GAME_INDEX,                   // ��Ϸ�����������Ŵ���
	LOGIN_ERROR_NONE_GATE,                    // û�����ط�����
	LOGIN_ERROR_QUEUE_LIMIT,                  // ��Ϸ�������ŶӶ����Ѿ���, ������������
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��½�����������
enum PAK_EVENT_LOGIN
{
	PAK_EVENT_LOGIN_LINK = (PAK_EVENT_LOGIN_BEGIN + 1), // CPAKLoginLink
	PAK_EVENT_LOGIN_LINKACK, // CPAKLoginLinkAck or CPAKAck(error)

	PAK_EVENT_LOGIN_UNLINK,      // CPAKLoginUnlink
	PAK_EVENT_LOGIN_UNLINKACK,   // CPAKAck

	PAK_EVENT_LOGIN_SELECT_GAME, // ѡ����Ϸ������, CPAKLoginSelectGame or CPAKAck(error)

	PAK_EVENT_LOGIN_LINK_GAME,   // ��Ϸ��������֤�ͻ�����Ч��, CPAKLoginLinkGame or CPAKAck(error)

	PAK_EVENT_LOGIN_QUEUE_PLAY,  // �Ŷӻ��߽�����Ϸ, CPAKSessionAck<PAK_EVENT_LOGIN_QUEUE_PLAY, PAK_TYPE_GATE>, ack=0����, 1-�˳�
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// �˺ŵ�½, session-id=0
class CPAKLoginLink : public CPAKSession
{
public:
	CPAKLoginLink(UInt uEvent = PAK_EVENT_LOGIN_LINK, UInt uType = PAK_TYPE_CLIENT_LOGIN);
	virtual ~CPAKLoginLink(void);

	CPAKLoginLink(const CPAKLoginLink& aSrc);
	CPAKLoginLink& operator=(const CPAKLoginLink& aSrc);

	virtual size_t Length(void) OVERRIDE;
	virtual void   Serialize(CStream&) OVERRIDE;

	CString&  GetUserId(void);
	void      SetUserId(CString& strUserId);
	void      SetUserId(PCXStr pszUserId);

	CString&  GetPassword(void);
	void      SetPassword(CString& strPassword);
	void      SetPassword(PCXStr pszPassword);
private:
	CString   m_strUserId;
	CString   m_strPassword;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// �˺ŵ�½Ack
class CPAKLoginLinkAck : public CPAKSessionAck
{
public:
	CPAKLoginLinkAck(UInt uEvent = PAK_EVENT_LOGIN_LINKACK, UInt uType = PAK_TYPE_LOGIN);
	virtual ~CPAKLoginLinkAck(void);

	CPAKLoginLinkAck(const CPAKLoginLinkAck& aSrc);
	CPAKLoginLinkAck& operator=(const CPAKLoginLinkAck& aSrc);

	virtual size_t Length(void) OVERRIDE;
	virtual void   Serialize(CStream&) OVERRIDE;

	LLong      GetUserId(void);
	void       SetUserId(LLong llUserId);

	LLong      GetTime(void);
	void       SetTime(LLong llTime);

	Int        GetGameId(void);
	void       SetGameId(Int nGameId);

	bool       CheckStream(void);
	CStream&   GetStream(void);
	void       SetStream(CStream& Stream);

	CNETTraits::NET_ADDR&  GetCltLast(void);
	void                   SetCltLast(CNETTraits::NET_ADDR& CltLast);
private:
	LLong                  m_llUserId;   ///< �û�Id
	LLong                  m_llTime;     ///< ���һ�ε�½��ʱ��
	CStream*               m_pStream;    ///< ����������, ���紫�����л�ʱ���Դ�С
	Int                    m_nGameId;    ///< ���һ��������Ϸ������Id
	CNETTraits::NET_ADDR   m_Addr;       ///< ���һ�ε�½�Ŀͻ��˵�ַ
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// �˺ŵǳ�
class CPAKLoginUnlink : public CPAKSession
{
public:
	CPAKLoginUnlink(UInt uEvent = PAK_EVENT_LOGIN_UNLINK, UInt uType = PAK_TYPE_CLIENT_LOGIN);
	virtual ~CPAKLoginUnlink(void);

	CPAKLoginUnlink(const CPAKLoginUnlink& aSrc);
	CPAKLoginUnlink& operator=(const CPAKLoginUnlink& aSrc);

	virtual size_t Length(void) OVERRIDE;
	virtual void   Serialize(CStream&) OVERRIDE;

	LLong                  GetUserId(void);
	void                   SetUserId(LLong llUserId);

	LLong                  GetTime(void);
	void                   SetTime(LLong llTime);

	Int                    GetGameId(void);
	void                   SetGameId(Int nGameId);

	CNETTraits::NET_ADDR&  GetCltLast(void);
	void                   SetCltLast(CNETTraits::NET_ADDR& CltLast);
private:
	LLong                  m_llUserId; ///< �û�Id
	LLong                  m_llTime;   ///< ���ε�½�ɹ���ʱ��(�ɵ�½��������д)
	Int                    m_nGameId;  ///< ���ε�½�Ժ����ӵ���Ϸ������Id
	CNETTraits::NET_ADDR   m_Addr;     ///< ���ε�½�ͻ��˵�ַ
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ѡ����Ϸ������
class CPAKLoginSelectGame : public CPAKSessionAck
{
public:
	CPAKLoginSelectGame(UInt uEvent = PAK_EVENT_LOGIN_SELECT_GAME, UInt uType = PAK_TYPE_CLIENT_LOGIN);
	virtual ~CPAKLoginSelectGame(void);

	CPAKLoginSelectGame(const CPAKLoginSelectGame& aSrc);
	CPAKLoginSelectGame& operator=(const CPAKLoginSelectGame& aSrc);

	virtual size_t Length(void) OVERRIDE;
	virtual void   Serialize(CStream&) OVERRIDE;

	LLong                  GetAuthCode(void);
	void                   SetAuthCode(LLong llAuthCode);

	CNETTraits::NET_ADDR&  GetAddr(void);
	void                   SetAddr(CNETTraits::NET_ADDR& Addr);
private:
	LLong                  m_llAuthCode;  ///< �ͻ���ʹ����Ϸ��������ֵ����������Ϸ������, �ɹ�������֤��1
	CNETTraits::NET_ADDR   m_Addr;        ///< ���Է�������ַ : ��¼��������д�Լ���UDP��ַ, Ӧ������ط�������д�Լ���UDP��ַ
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ��Ϸ��������֤�ͻ����Ƿ�Ϸ�����
class CPAKLoginLinkGame : public CPAKSessionAck
{
public:
	CPAKLoginLinkGame(UInt uEvent = PAK_EVENT_LOGIN_LINK_GAME, UInt uType = PAK_TYPE_CLIENT);
	virtual ~CPAKLoginLinkGame(void);

	CPAKLoginLinkGame(const CPAKLoginLinkGame& aSrc);
	CPAKLoginLinkGame& operator=(const CPAKLoginLinkGame& aSrc);

	virtual size_t Length(void) OVERRIDE;
	virtual void   Serialize(CStream&) OVERRIDE;

	LLong          GetAuthCode(void);
	void           SetAuthCode(LLong llAuthCode);
private:
	///< 1. ��һ��SessionΪ��, ��֤��Ϊǰ���½���������ص���֤��1
	///< 2. ��һ����֤ͨ��, ��Ϸ����������֤��2ͨ���ͻ������ӵĵ�½����������, �ڶ�����֤Session=�û�Id, ��֤��Ϊ���ص���֤��2
	///< 3. �ڶ�����֤ͨ��  ��Ϸ���������ص�AuthCode�ǿͻ����ڴ���Ϸ���������Ŷ����, ���Ϊ0��ʾֱ�ӿ��Խ�����Ϸ(����Ϸ������ͨ�ŵ�Session��ʱΪ�û�Id)
	LLong         m_llAuthCode;  
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "loginpacket.inl"

#endif // __LOGIN_PACKET_H__