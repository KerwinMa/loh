//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : datadef.h                                    //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update : 2015-11-25     version 0.0.0.5                    //
//   Detail : ��Ϣ����                                          //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __DATA_DEF_H__
#define __DATA_DEF_H__

#pragma once

#include "networkevent.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// server uuid : ����������UUID����
DECLARE_UUID( ICenterServer,  {FF2D862B-6BE0-43d7-9450-E76B6116D5B8} ) // ���ķ�����UUID
DECLARE_UUID( ISelectServer,  {038ECA57-8A9B-4dd5-9935-96F118054F66} ) // ѡ�������UUID
DECLARE_UUID( ILoginDBServer, {6E510B2A-112E-4272-ACD8-E2F245DA78D5} ) // ��½DB������UUID
DECLARE_UUID( ILoginServer,   {3FAA0C20-40D7-4515-AEA6-2AA0B3E8C322} ) // ��½������UUID
DECLARE_UUID( IGameDBServer,  {953E6911-4954-4411-9FDE-5483E544EE7D} ) // ��ϷDB������UUID
DECLARE_UUID( IGameServer,    {03ACE2C7-4740-4e5f-91BC-1D1118412D9B} ) // ��Ϸ������UUID
DECLARE_UUID( IZoneServer,    {7F8C1EF8-E551-4845-82FA-AC043BE5163E} ) // ��ͼ������UUID
DECLARE_UUID( IGateServer,    {E4D27215-5944-4C25-906B-147D01D317C7} ) // ���ط�����UUID

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// ͨ�ó�������
enum DATA_DEF
{
	DATAD_OKAY              = 0,      // ���سɹ�
	DATAD_FAIL,                       // �������Id���������Լ�����, ���������DATAD_FAIL��ֵ
	DATAD_PERCENT           = 100,    // �ٷֱ�
	DATAD_TICK_MS           = 1000,   // ��ͺ���ת��
	DATAD_MAX_BUSY          = 10000,  // �������������(��ֱ�)
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����������ֵ
enum DATA_INDEX
{
	DATA_INDEX_CENTER,  // ���ķ�����
	DATA_INDEX_SELECT,	// ѡ�������
	DATA_INDEX_LOGINDB, // ��½DB������
	DATA_INDEX_LOGIN,	// ��½������
	DATA_INDEX_GAMEDB,	// ��ϷDB������
	DATA_INDEX_GAME,	// ��Ϸ������
	DATA_INDEX_ZONE,	// ��ͼ������
	DATA_INDEX_GATE,	// ���ط�����
	DATA_INDEX_MAX,
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// �������Ͷ��� : ���յ����ݰ�ʱ, ���԰����������з��ദ��
//               ����������ķ�����, ѡ��, ��½����Ϸ������ע��ʱ, �Ϳ���ͨ������������
//               �������������Ӧ������, ���Խ�һ�����ݰ��������㲥���ض����͵����Զ���
enum PAK_TYPE
{
	PAK_TYPE_NONE           = 0x00000000, // NONE-������
	PAK_TYPE_CENTER         = 0x67788482, // CNTR-���ķ�����
	PAK_TYPE_SELECT         = 0x83766784, // SLCT-ѡ�������
	PAK_TYPE_LOGINDB        = 0x76786866, // LNDB-��½DB������
	PAK_TYPE_LOGIN          = 0x76717378, // LGIN-��½������
	PAK_TYPE_LOGIN_CENTER   = 0x76718482, // LGTR-��½������(��ֹ���ĺ͵�½DBͬ���̶�ʹ��PAK_TYPE_LOGIN��ʶ��½������, ���Ĺ㲥ʱ��ͬ����½����������2��һ���Ĺ㲥)
	PAK_TYPE_GAMEDB         = 0x71776866, // GMDB-��ϷDB������
	PAK_TYPE_GAME           = 0x71657769, // GAME-��Ϸ������
	PAK_TYPE_ZONE           = 0x90797869, // ZONE-��ͼ������
	PAK_TYPE_GATE           = 0x71658469, // GATE-���ط�����
	PAK_TYPE_CLIENT         = 0x67767884, // CLNT-��Ϸ�����еĿͻ���
	PAK_TYPE_CLIENT_SELECT  = 0x67767883, // CLNS-��ɫѡ���еĿͻ���
	PAK_TYPE_CLIENT_QUEUE   = 0x67767881, // CLNQ-�Ŷ��еĿͻ���
	PAK_TYPE_CLIENT_LOGIN   = 0x67767876, // CLNL-��½�еĿͻ���
	PAK_TYPE_CLIENT_READY   = 0x67767882, // CLNR-׼��״̬�Ŀͻ���
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����������״̬
enum STATUS_CONST
{
	STATUSC_NONE            = 0, // ��״̬
	STATUSC_INIT,                // ��ʼ״̬
	STATUSC_RUN,                 // ����״̬
	STATUSC_PAUSE,               // ��ͣ״̬
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���������ݸ���״̬
enum STATUS_UPDATE
{
	STATUSU_NONE            = 0x00000000, // ��ʼ״̬
	STATUSU_LINK            = 0x00000001, // ע������״̬
	STATUSU_OKAY            = 0x00000002, // ���ݴ������״̬
	STATUSU_SYNC            = 0x00000004, // ��Ҫͬ������״̬
	STATUSU_UNLINK          = 0x00000008, // ע��״̬
	//										 
	STATUSU_PING            = 0x00000010, // ͬ��ʱֻ��ҪPING��ַѡ��
	STATUSU_UPDATE          = 0x00000020, // ���շ�ͬ������ʱֱ�ӱ�����״̬

	STATUSU_SYNCXLINK       = (STATUSU_SYNC|STATUSU_LINK|STATUSU_UNLINK), // ״̬���1
	STATUSU_OKAYSYNC        = (STATUSU_OKAY|STATUSU_SYNC),                // ״̬���2
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������������巶Χ
enum PAK_EVENT_SERVER
{
	// ���ķ��������Χ
	PAK_EVENT_CENTER_BEGIN  = (CNETTraits::EVENT_RESERVED + 100),
	PAK_EVENT_CENTER_END    = (PAK_EVENT_CENTER_BEGIN + 99),
	// ѡ����������Χ
	PAK_EVENT_SELECT_BEGIN,
	PAK_EVENT_SELECT_END    = (PAK_EVENT_SELECT_BEGIN + 99),
	// ��½���ݷ��������Χ
	PAK_EVENT_LOGINDB_BEGIN,
	PAK_EVENT_LOGINDB_END   = (PAK_EVENT_LOGINDB_BEGIN + 99),
	// ��½���������Χ
	PAK_EVENT_LOGIN_BEGIN,
	PAK_EVENT_LOGIN_END     = (PAK_EVENT_LOGIN_BEGIN + 99),
	// ��Ϸ���ݷ��������Χ
	PAK_EVENT_GAMEDB_BEGIN,
	PAK_EVENT_GAMEDB_END    = (PAK_EVENT_GAMEDB_BEGIN + 99),
	// ��Ϸ���������Χ
	PAK_EVENT_GAME_BEGIN,
	PAK_EVENT_GAME_END      = (PAK_EVENT_GAME_BEGIN + 99),
	// ��ͼ���������Χ
	PAK_EVENT_ZONE_BEGIN,
	PAK_EVENT_ZONE_END      = (PAK_EVENT_ZONE_BEGIN + 99),
	// ���ط��������Χ
	PAK_EVENT_GATE_BEGIN,
	PAK_EVENT_GATE_END      = (PAK_EVENT_GATE_BEGIN + 99),
	// ��Ϸ�������Χ
	PAK_EVENT_REALM_BEGIN,
	PAK_EVENT_REALM_END     = (PAK_EVENT_REALM_BEGIN + 9999),
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// DataRef : Ϊͳһ32λ��64λָ��ռ�ÿռ䲻ͬ����, ���ｫͳһ����Ϊ64λ
#if (__INTRIN_TARGET__ == INTRIN_TARGET_X86)
	typedef LLong  DataRef;
#elif ((__INTRIN_TARGET__ == INTRIN_TARGET_X64) || (__INTRIN_TARGET__ == INTRIN_TARGET_IA64))
	typedef KeyRef DataRef;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ��������Ϣ�ṹ���� : ��Ϣͳ��ʹ��
struct tagSERVER_DATA : public MObject
{
public:
	tagSERVER_DATA(void)
	: uAllCount(0)
	, uOnline(0)
	, usBusy(0)
	, usIncr(0)
	, usId(0)
	, usStatus(0)
	{
	}

	~tagSERVER_DATA(void)
	{
	}
public:
	UInt     uAllCount;  ///< �ۼ�����
	UInt     uOnline;    ///< ��������
	UShort   usBusy;     ///< [0 - 10000]�������������(��ֱ�)
	UShort   usIncr;     ///< ÿ����һ�������ĸ���(��ֱ�)
	UShort   usId;       ///< ������Id, ��Ϸ������-�ͻ���������������, �����������������������ַ����
	UShort   usStatus;   ///< ������״̬STATUS_UPDATE
};
typedef struct tagSERVER_DATA   SERVER_DATA, *PSERVER_DATA;

#endif // __DATA_DEF_H__