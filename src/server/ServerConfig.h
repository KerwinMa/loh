//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : ServerConfig.h                               //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update : 2015-11-25     version 0.0.0.5                    //
//   Detail : �����������ļ�                                     //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __SERVER_CONFIG_H__
#define __SERVER_CONFIG_H__

#pragma once

#include "windows\ini.h"
#include "filelog.h"
#include "filepath.h"
#include "keyvalue.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ͨ�����ö���
class NOVTABLE CServerConfig : public MObject
{
public:
	/// ���ò�������
	enum CFG_DEFAULT
	{
		CFG_DEFAULT_NONE       = 0, ///< ��
		// load server
		CFG_DEFAULT_CENTER     = 0x00000001, ///< �������ķ�����ģ��
		CFG_DEFAULT_SELECT     = 0x00000002, ///< ����ѡ�������ģ��
		CFG_DEFAULT_LOGINDB    = 0x00000004, ///< ���ص�½DB������ģ��
		CFG_DEFAULT_LOGIN      = 0x00000008, ///< ���ص�½������ģ��
		CFG_DEFAULT_GAMEDB     = 0x00000010, ///< ������ϷDB������ģ��
		CFG_DEFAULT_GAME       = 0x00000020, ///< ������Ϸ������ģ��
		CFG_DEFAULT_ZONE       = 0x00000040, ///< ���ص�ͼ������ģ��
		CFG_DEFAULT_GATE       = 0x00000080, ///< �������ط�����ģ��
		CFG_DEFAULT_ALLSERVER  = 0x000000FF, ///< ����ȫ��������ģ��
		//
		CFG_CONFIG_PTR         = 0x10000000, ///< ��ȡ���ö����ַ
		// center
		CFG_CENTER_SELECT_PORT = 11110,   ///< ���ķ�����Ĭ�ϼ����˿�(ѡ��)
		CFG_CENTER_LOGIN_PORT  = 11111,   ///< ���ķ�����Ĭ�ϼ����˿�(��½)
		CFG_CENTER_GAME_PORT   = 11112,   ///< ���ķ�����Ĭ�ϼ����˿�(��Ϸ)
		// select
		CFG_SELECT_PORT        = 10010,   ///< ѡ�������Ĭ��UDP�˿�(�ͻ���)
		// logindb
		CFG_LOGINDB_PORT       = 11120,   ///< ��½DB������Ĭ�ϼ����˿�(��½)
		// login
		CFG_LOGIN_PORT         = 10020,   ///< ��½������Ĭ�ϼ����˿�(�ͻ���)  
		// gamedb
		CFG_GAMEDB_GAME_PORT   = 11130,   ///< ��ϷDB������Ĭ�ϼ����˿�(��Ϸ)
		CFG_GAMEDB_GATE_PORT   = 11131,   ///< ��ϷDB������Ĭ�ϼ����˿�(����)
		// game
		CFG_GAME_ZONE_PORT     = 11140,   ///< ��Ϸ������Ĭ�ϼ����˿�(��ͼ)
		CFG_GAME_GATE_PORT     = 11141,   ///< ��Ϸ������Ĭ�ϼ����˿�(����)
		// gate
		CFG_GATE_PORT          = 10030,   ///< ���ط�����Ĭ�ϼ����˿�(�ͻ���)
		// update
		CFG_UPDATE_MIN_TIME    = 16,      ///< ��Сͬ����������
		CFG_UPDATE_DEF_TIME    = 30,      ///< ͬ����������Ĭ��30��
		CFG_BUSY_DEF_INCREMENT = 40,      ///< ���ӵĸ���Ĭ��ֵ(��ֱ�)

		CFG_LIVE_MIN_TIME      = 60,	  ///< ��С������1����
		CFG_LIVE_DEF_TIME      = 5 * 60,  ///< Ĭ��������5����
		CFG_REPORT_MIN_TIME    = 5 * 60,  ///< �ϱ���������Ϣ�����ݿ����Сʱ����
		CFG_REPORT_DEF_TIME    = 30 * 60, ///< �ϱ���������Ϣ�����ݿ��Ĭ��ʱ����
	};
	/// ��̬������
	static CPCXStr ServerConfig;          ///< ���ö���
	static CPCXStr NetworkPtr;            ///< �������

	static CPCXStr ConfigName;            ///< �����ļ�����
	static CPCXStr CenterServerPath;      ///< ���ķ�����ģ��·��
	static CPCXStr SelectServerPath;      ///< ѡ�������ģ��·��
	static CPCXStr LoginDBServerPath;     ///< ��½DB������ģ��·��
	static CPCXStr LoginServerPath;       ///< ��½������ģ��·��
	static CPCXStr GameDBServerPath;      ///< ��ϷDB������ģ��·��
	static CPCXStr GameServerPath;        ///< ��Ϸ������ģ��·��
	static CPCXStr ZoneServerPath;        ///< ��ͼ������ģ��·��
	static CPCXStr GateServerPath;        ///< ���ط�����ģ��·��

	static CPCXStr NetworkBlock;          ///< NetworkBlock
	static CPCXStr NetworkAttr;           ///< NetworkAttr=0(0��ʾʹ��Ĭ��ֵ, ����Ķ�����)
	static CPCXStr NetworkThread;         ///< NetworkThread=0(cores * 2)
	static CPCXStr NetworkAck;            ///< NetworkAck
	static CPCXStr NetworkTimeout;        ///< NetworkTimeout
	static CPCXStr NetworkBuffer;         ///< NetworkBuffer
	static CPCXStr NetworkJumbo;          ///< NetworkJumbo

	static CPCXStr ServersBlock;          ///< ServersBlock
	static CPCXStr UpdateTick;            ///< UpdateTick=30(��)
	static CPCXStr ReportTick;            ///< ReportTick=30(��)
	static CPCXStr LoadServers;           ///< ���صķ��������� : LoadServers=CenterServer|SelectServer
	static CPCXStr CenterServer;          ///< ���صķ�����DLL·�� : CenterServer=CenterServer.dll
	static CPCXStr SelectServer;          ///< ѡ�������ģ��·��
	static CPCXStr LoginDBServer;         ///< ��½DB������ģ��·��
	static CPCXStr LoginServer;           ///< ��½������ģ��·��
	static CPCXStr GameDBServer;          ///< ��ϷDB������ģ��·��
	static CPCXStr GameServer;            ///< ��Ϸ������ģ��·��
	static CPCXStr ZoneServer;            ///< ��ͼ������ģ��·��
	static CPCXStr GateServer;            ///< ���ط�����ģ��·��

	static CPCXStr Service;
	static CPCXStr Addr;                  ///< Ҫ��NetworkAttr��ipv4 or ipv6ƥ��
	static CPCXStr Port;                  ///< �˿�
	static CPCXStr Id;                    ///< Id
	static CPCXStr Incr;                  ///< ����
	static CPCXStr Queue;                 ///< �����߳�����
	static CPCXStr ExtConfig;             ///< ��������չ�����ļ�·��
	static CPCXStr LocalHost;             ///< ���ص�ַ
public:
	CServerConfig(void);
	~CServerConfig(void);
	/// �����ļ�������
	void SetFilePath(CFilePath* pFilePath);
	/// ��ȡ�ļ�������
	CFilePath* GetFilePath(void);
	/// ����key-value������
	void SetKeyValue(CKeyValue* pKeyValue);
	/// ��ȡkey-value������
	CKeyValue* GetKeyValue(void);
	/// ��ʼ��
	/// @param[in] PCXStr pszName�����ļ�
	bool   Init(PCXStr pszName = nullptr);
	/// ����
	void   Exit(void);
	/// ��ӡ������Ϣ���ļ���־
	void   Dump(CFileLog& Log);
	/// ��ȡ��������ò���-����
	Int    GetNetworkAttr(void);
	/// ��ȡ��������ò���-�߳�����
	Int    GetNetworkThread(void);
	/// ��ȡ��������ò���-ACKʱ��
	Int    GetNetworkAck(void);
	/// ��ȡ��������ò���-��ʱʱ��
	Int    GetNetworkTimeout(void);
	/// ��ȡ��������ò���-��ͨ���ջ��߷��ͻ����С
	Int    GetNetworkBuffer(void);
	/// ��ȡ��������ò���-��֡���ջ��߷��ͻ����С
	Int    GetNetworkJumbo(void);
	/// ��ȡͬ�����������ݼ��
	Int    GetUpdateTick(void);
	/// ��ȡ�ϱ���Ϣʱ����
	Int    GetReportTick(void);
	/// ��ȡ���ط��������
	Int    GetLoadServers(void);
	/// ��ȡ����������ģ���ļ�·��
	/// @param[in] Int nServer ����������, �ο�CFG_DEFAULT_XXX
	/// @param[out] CStringFix& strPath �ļ�·��
	bool   GetServerPath(Int nServer, CStringFix& strPath);
	/// ��ȡ���������ĵ�ַ��Ϣ
	/// @param[in] Int nServer ����������, �ο�CFG_DEFAULT_XXX
	/// @param[in] Int nServerAddr ����������, �ο�CFG_DEFAULT_XXX
	/// @param[out] CStringKey& strAddr ��ַ
	/// @param[out] UShort& usPort �˿�
	bool   GetServerAddr(Int nServer, Int nServerAddr, CStringKey& strAddr, UShort& usPort);
	/// ��ȡ����������Id
	UShort GetServerId(Int nServer);
	/// ��ȡ���������ĸ���������
	UShort GetServerIncr(Int nServer);
	/// ��ȡ��������������������߳���
	Int    GetServerQueue(Int nServer);
	/// ��ȡ������������չ�����ļ�·��
	bool   GetServerExtConfig(Int nServer, CStringFix& strPath);
private:
	CServerConfig(const CServerConfig&);
	CServerConfig& operator=(const CServerConfig&);
	/// ��ini�ļ���ȡ��Ϣ1
	void   CheckLoadAddr(CIni& ini, PCXStr pszSection, PCXStr pszServer, CStringKey& strAddr, Int& nPort, Int nDefault);
	/// ��ini�ļ���ȡ��Ϣ2
	void   LoadAddr(CIni& ini, PCXStr pszSection, PCXStr pszServer, CStringKey& strAddr, Int& nPort, Int nDefault);
public:
	CFilePath*   m_pFilePath; ///< �ļ�������
	CKeyValue*   m_pKeyValue; ///< key-value������
	// network
	Int          m_nNetworkAttr;    ///< ��������ò���-����
	Int          m_nNetworkThread;  ///< ��������ò���-�߳�����
	Int          m_nNetworkAck;     ///< ��������ò���-ACKʱ��
	Int          m_nNetworkTimeout; ///< ��������ò���-��ʱʱ��
	Int          m_nNetworkBuffer;  ///< ��������ò���-��ͨ���ջ��߷��ͻ����С
	Int          m_nNetworkJumbo;   ///< ��������ò���-��֡���ջ��߷��ͻ����С
	// servers
	Int          m_nUpdateTick;  ///< ͬ�����������ݼ��
	Int          m_nReportTick;  ///< �ϱ���Ϣʱ����
	Int          m_nLoadServers; ///< ���ط��������
	// center-server
	Int          m_nCenterServerSelectPort; ///< ��m_strCenterServerSelectAddr��ɼ���ѡ��������ĵ�ַ
	Int          m_nCenterServerLoginPort;  ///< ��m_strCenterServerLoginAddr��ɼ�����½�������ĵ�ַ, �˿�0��ʾ�����ѡ���������ͬ��ַ
	Int          m_nCenterServerGamePort;   ///< ��m_strCenterServerGameAddr��ɼ�����Ϸ�������ĵ�ַ, �˿�0��ʾ�����ѡ����������ߵ�½��������ͬ��ַ
	Int          m_nCenterServerId;         ///< ������Id, �������������ַ����
	Int          m_nCenterServerIncr;       ///< ��������ֵ(��ֱ�)
	// select-server
	Int          m_nSelectServerCenterPort; ///< ��m_strSelectServerCenterAddr����������ķ��������ذ󶨵�ַ, �˿�0��ʾ��ϵͳ�����ַ
	Int          m_nSelectServerClientPort; ///< ��m_strSelectServerClientAddr���UDP�����ַ
	Int          m_nSelectServerId;         ///< ������Id, �������������ַ����
	Int          m_nSelectServerIncr;		///< ��������ֵ(��ֱ�)
	// logindb-server
	Int          m_nLoginDBServerLoginPort; ///< ��m_strLoginDBServerLoginAddr��ɼ�����½�������ĵ�ַ
	Int          m_nLoginDBServerId;        ///< ������Id, �������������ַ����
	Int          m_nLoginDBServerIncr;		///< ��������ֵ(��ֱ�)
	Int          m_nLoginDBServerQueue;     ///< ��������߳�����
	// login-server
	Int          m_nLoginServerCenterPort;  ///< ��m_strLoginServerCenterAddr����������ķ��������ذ󶨵�ַ, �˿�0��ʾ��ϵͳ�����ַ
	Int          m_nLoginServerLoginDBPort; ///< ��m_strLoginServerLoginDBAddr������ӵ�½DB���������ذ󶨵�ַ, �˿�0��ʾ��ϵͳ�����ַ
	Int          m_nLoginServerGamePort;    ///< ��m_strLoginServerGameAddr��ɼ�����Ϸ��������UDP��ַ, �˿�0��ʾ��ϵͳ�����ַ
	Int          m_nLoginServerClientPort;  ///< ��m_strLoginServerClientAddr���TCP�����ַ
	Int          m_nLoginServerId;          ///< ������Id, �������������ַ����
	Int          m_nLoginServerIncr;		///< ��������ֵ(��ֱ�)
	// gamedb-server
	Int          m_nGameDBServerGamePort;   ///< ��m_strGameDBServerGameAddr��ɼ�����Ϸ�������ĵ�ַ
	Int          m_nGameDBServerGatePort;   ///< ��m_strGameDBServerGateAddr��ɼ������ط������ĵ�ַ, �˿�0��ʾ�������Ϸ��������ͬ��ַ
	Int          m_nGameDBServerId;         ///< ������Id, �������������ַ����
	Int          m_nGameDBServerIncr;		///< ��������ֵ(��ֱ�)
	Int          m_nGameDBServerQueue;		///< ��������߳�����
	// game-server
	Int          m_nGameServerCenterPort;   ///< ��m_strGameServerCenterAddr����������ķ��������ذ󶨵�ַ, �˿�0��ʾ��ϵͳ�����ַ
	Int          m_nGameServerGameDBPort;	///< ��m_strGameServerGameDBAddr���������ϷDB���������ذ󶨵�ַ, �˿�0��ʾ��ϵͳ�����ַ
	Int          m_nGameServerLoginPort;	///< ��m_strGameServerLoginAddr��ɼ�����½��������UDP��ַ, �˿�0��ʾ��ϵͳ�����ַ
	Int          m_nGameServerZonePort;     ///< ��m_strGameServerZoneAddr��ɼ�����ͼ�������ĵ�ַ
	Int          m_nGameServerGatePort;     ///< ��m_strGameServerGateAddr��ɼ������ط������ĵ�ַ, �˿�0��ʾ�������ͼ��������ͬ��ַ
	Int          m_nGameServerId;           ///< ������Id, �ͻ���������������
	Int          m_nGameServerIncr;			///< ��������ֵ(��ֱ�)
	// zone-server
	Int          m_nZoneServerGamePort;     ///< ��m_strZoneServerGameAddr���������Ϸ���������ذ󶨵�ַ, �˿�0��ʾ��ϵͳ�����ַ
	Int          m_nZoneServerId;           ///< ������Id, �������������ַ����
	Int          m_nZoneServerIncr;			///< ��������ֵ(��ֱ�)
	// gate-server
	Int          m_nGateServerGameDBPort;   ///< ��m_strGateServerGameDBAddr���������ϷDB���������ذ󶨵�ַ, �˿�0��ʾ��ϵͳ�����ַ
	Int          m_nGateServerGamePort;		///< ��m_strGateServerGameAddr���������Ϸ���������ذ󶨵�ַ, �˿�0��ʾ��ϵͳ�����ַ
	Int          m_nGateServerLoginPort;	///< ��m_strGateServerLoginAddr��ɼ�����½��������UDP��ַ, �˿�0��ʾ��ϵͳ�����ַ
	Int          m_nGateServerClientPort;	///< ��m_strGateServerClientAddr���TCP�����ַ
	Int          m_nGateServerId;           ///< ������Id, �������������ַ����
	Int          m_nGateServerIncr;			///< ��������ֵ(��ֱ�)
	Int          m_nGateServerQueue;		///< ��������߳�����
	//
	CStringFix   m_strCenterServerPath;     ///< �������ķ�����ģ��·��
	CStringFix   m_strSelectServerPath;		///< ����ѡ�������ģ��·��
	CStringFix   m_strLoginDBServerPath;	///< ���ص�½DB������ģ��·��
	CStringFix   m_strLoginServerPath;		///< ���ص�½������ģ��·��
	CStringFix   m_strGameDBServerPath;		///< ������ϷDB������ģ��·��
	CStringFix   m_strGameServerPath;		///< ������Ϸ������ģ��·��
	CStringFix   m_strZoneServerPath;		///< ���ص�ͼ������ģ��·��
	CStringFix   m_strGateServerPath;		///< �������ط�����ģ��·��
	// center-server
	CStringKey   m_strCenterServerSelectAddr; ///< ��m_nCenterServerSelectPort��ɼ���ѡ��������ĵ�ַ
	CStringKey   m_strCenterServerLoginAddr;  ///< ��m_nCenterServerLoginPort��ɼ�����½�������ĵ�ַ, �˿�0��ʾ�����ѡ���������ͬ��ַ
	CStringKey   m_strCenterServerGameAddr;   ///< ��m_nCenterServerGamePort��ɼ�����Ϸ�������ĵ�ַ, �˿�0��ʾ�����ѡ����������ߵ�½��������ͬ��ַ
	CStringFix   m_strCenterServerExtConfig;  ///< ���������ļ�·��
	// select-server
	CStringKey   m_strSelectServerCenterAddr; ///< ��m_nSelectServerCenterPort����������ķ��������ذ󶨵�ַ, �˿�0��ʾ��ϵͳ�����ַ
	CStringKey   m_strSelectServerClientAddr; ///< ��m_nSelectServerClientPort���UDP�����ַ
	CStringFix   m_strSelectServerExtConfig;  ///< ���������ļ�·��
	// logindb-server
	CStringKey   m_strLoginDBServerLoginAddr; ///< ��m_nLoginDBServerLoginPort��ɼ�����½�������ĵ�ַ
	CStringFix   m_strLoginDBServerExtConfig; ///< ���������ļ�·��
	// login-server
	CStringKey   m_strLoginServerCenterAddr;  ///< ��m_strLoginServerCenterPort����������ķ��������ذ󶨵�ַ, �˿�0��ʾ��ϵͳ�����ַ
	CStringKey   m_strLoginServerLoginDBAddr; ///< ��m_strLoginServerLoginDBPort������ӵ�½DB���������ذ󶨵�ַ, �˿�0��ʾ��ϵͳ�����ַ
	CStringKey   m_strLoginServerGameAddr;    ///< ��m_strLoginServerGamePort��ɼ�����Ϸ��������UDP��ַ, �˿�0��ʾ��ϵͳ�����ַ
	CStringKey   m_strLoginServerClientAddr;  ///< ��m_strLoginServerClientPort���TCP�����ַ
	CStringFix   m_strLoginServerExtConfig;   ///< ���������ļ�·��
	// gamedb-server
	CStringKey   m_strGameDBServerGameAddr;   ///< ��m_strGameDBServerGamePort��ɼ�����Ϸ�������ĵ�ַ
	CStringKey   m_strGameDBServerGateAddr;   ///< ��m_strGameDBServerGatePort��ɼ������ط������ĵ�ַ, �˿�0��ʾ�������Ϸ��������ͬ��ַ
	CStringFix   m_strGameDBServerExtConfig;  ///< ���������ļ�·��
	// game-server
	CStringKey   m_strGameServerCenterAddr;   ///< ��m_strGameServerCenterPort����������ķ��������ذ󶨵�ַ, �˿�0��ʾ��ϵͳ�����ַ
	CStringKey   m_strGameServerGameDBAddr;   ///< ��m_strGameServerGameDBPort���������ϷDB���������ذ󶨵�ַ, �˿�0��ʾ��ϵͳ�����ַ
	CStringKey   m_strGameServerLoginAddr;    ///< ��m_strGameServerLoginPort��ɼ�����½��������UDP��ַ, �˿�0��ʾ��ϵͳ�����ַ
	CStringKey   m_strGameServerZoneAddr;     ///< ��m_strGameServerZonePort��ɼ�����ͼ�������ĵ�ַ
	CStringKey   m_strGameServerGateAddr;     ///< ��m_strGameServerGatePort��ɼ������ط������ĵ�ַ, �˿�0��ʾ�������ͼ��������ͬ��ַ
	CStringKey   m_strGameServerPingAddr;     ///< PING��ַ, �ͻ���ͨ��PING�����ַ����������
	CStringFix   m_strGameServerExtConfig;    ///< ���������ļ�·��
	// zone-server
	CStringKey   m_strZoneServerGameAddr;     ///< ��m_strZoneServerGamePort���������Ϸ���������ذ󶨵�ַ, �˿�0��ʾ��ϵͳ�����ַ
	CStringFix   m_strZoneServerExtConfig;    ///< ���������ļ�·��
	// gate-server
	CStringKey   m_strGateServerGameDBAddr;   ///< ��m_strGateServerGameDBPort���������ϷDB���������ذ󶨵�ַ, �˿�0��ʾ��ϵͳ�����ַ
	CStringKey   m_strGateServerGameAddr;     ///< ��m_strGateServerGamePort���������Ϸ���������ذ󶨵�ַ, �˿�0��ʾ��ϵͳ�����ַ
	CStringKey   m_strGateServerLoginAddr;    ///< ��m_strGateServerLoginPort��ɼ�����½��������UDP��ַ, �˿�0��ʾ��ϵͳ�����ַ
	CStringKey   m_strGateServerClientAddr;   ///< ��m_strGateServerClientPort���TCP�����ַ
	CStringFix   m_strGateServerExtConfig;    ///< ���������ļ�·��
	// config file name
	CStringFix   m_strConfigName;             ///< �����ļ�����
};

INLINE CServerConfig::CServerConfig(void)
: m_pFilePath(nullptr)
, m_pKeyValue(nullptr)
, m_nNetworkAttr(CFG_DEFAULT_NONE)
, m_nNetworkThread(0)
, m_nNetworkAck(0)
, m_nNetworkTimeout(0)
, m_nNetworkBuffer(0)
, m_nNetworkJumbo(0)
, m_nUpdateTick(CFG_UPDATE_DEF_TIME)
, m_nReportTick(CFG_REPORT_DEF_TIME)
, m_nLoadServers(CFG_DEFAULT_ALLSERVER)
, m_nCenterServerSelectPort(CFG_CENTER_SELECT_PORT)
, m_nCenterServerLoginPort(0)
, m_nCenterServerGamePort(0)
, m_nCenterServerId(DATA_INDEX_CENTER)
, m_nCenterServerIncr(CFG_BUSY_DEF_INCREMENT)
, m_nSelectServerCenterPort(0)
, m_nSelectServerClientPort(CFG_SELECT_PORT)
, m_nSelectServerId(DATA_INDEX_SELECT)
, m_nSelectServerIncr(CFG_BUSY_DEF_INCREMENT)
, m_nLoginDBServerLoginPort(CFG_LOGINDB_PORT)
, m_nLoginDBServerId(DATA_INDEX_LOGINDB)
, m_nLoginDBServerIncr(CFG_BUSY_DEF_INCREMENT)
, m_nLoginDBServerQueue(1)
, m_nLoginServerCenterPort(0)
, m_nLoginServerLoginDBPort(0)
, m_nLoginServerGamePort(0)
, m_nLoginServerClientPort(CFG_LOGIN_PORT)
, m_nLoginServerId(DATA_INDEX_LOGIN)
, m_nLoginServerIncr(CFG_BUSY_DEF_INCREMENT)
, m_nGameDBServerGamePort(CFG_GAMEDB_GAME_PORT)
, m_nGameDBServerGatePort(0)
, m_nGameDBServerId(DATA_INDEX_GAMEDB)
, m_nGameDBServerIncr(CFG_BUSY_DEF_INCREMENT)
, m_nGameDBServerQueue(1)
, m_nGameServerCenterPort(0)
, m_nGameServerGameDBPort(0)
, m_nGameServerLoginPort(0)
, m_nGameServerZonePort(CFG_GAME_ZONE_PORT)
, m_nGameServerGatePort(0)
, m_nGameServerId(DATA_INDEX_GAME)
, m_nGameServerIncr(CFG_BUSY_DEF_INCREMENT)
, m_nZoneServerGamePort(0)
, m_nZoneServerId(DATA_INDEX_ZONE)
, m_nZoneServerIncr(CFG_BUSY_DEF_INCREMENT)
, m_nGateServerGameDBPort(0)
, m_nGateServerGamePort(0)
, m_nGateServerLoginPort(0)
, m_nGateServerClientPort(CFG_GATE_PORT)
, m_nGateServerId(DATA_INDEX_GATE)
, m_nGateServerIncr(CFG_BUSY_DEF_INCREMENT)
, m_nGateServerQueue(1)
, m_strCenterServerPath(CenterServerPath)
, m_strSelectServerPath(SelectServerPath)
, m_strLoginDBServerPath(LoginDBServerPath)
, m_strLoginServerPath(LoginServerPath)
, m_strGameDBServerPath(GameDBServerPath)
, m_strGameServerPath(GameServerPath)
, m_strZoneServerPath(ZoneServerPath)
, m_strGateServerPath(GateServerPath)
, m_strCenterServerSelectAddr(LocalHost)
, m_strCenterServerLoginAddr(LocalHost)
, m_strCenterServerGameAddr(LocalHost)
, m_strSelectServerCenterAddr(LocalHost)
, m_strSelectServerClientAddr(LocalHost)
, m_strLoginDBServerLoginAddr(LocalHost)
, m_strLoginServerCenterAddr(LocalHost)
, m_strLoginServerLoginDBAddr(LocalHost)
, m_strLoginServerGameAddr(LocalHost)
, m_strLoginServerClientAddr(LocalHost)
, m_strGameServerCenterAddr(LocalHost)
, m_strGameServerGameDBAddr(LocalHost)
, m_strGameServerLoginAddr(LocalHost)
, m_strGameServerZoneAddr(LocalHost)
, m_strGameServerGateAddr(LocalHost)
, m_strGameServerPingAddr(LocalHost)
, m_strZoneServerGameAddr(LocalHost)
, m_strGateServerGameDBAddr(LocalHost)
, m_strGateServerGameAddr(LocalHost)
, m_strGateServerLoginAddr(LocalHost)
, m_strGateServerClientAddr(LocalHost)
, m_strConfigName(ConfigName)
{
}

INLINE CServerConfig::~CServerConfig(void)
{
}

INLINE CServerConfig::CServerConfig(const CServerConfig&)
: m_pFilePath(nullptr)
, m_pKeyValue(nullptr)
, m_nNetworkAttr(CFG_DEFAULT_NONE)
, m_nNetworkThread(0)
, m_nNetworkAck(0)
, m_nNetworkTimeout(0)
, m_nNetworkBuffer(0)
, m_nNetworkJumbo(0)
, m_nUpdateTick(CFG_UPDATE_DEF_TIME)
, m_nReportTick(CFG_REPORT_DEF_TIME)
, m_nLoadServers(CFG_DEFAULT_ALLSERVER)
, m_nCenterServerSelectPort(CFG_CENTER_SELECT_PORT)
, m_nCenterServerLoginPort(0)
, m_nCenterServerGamePort(0)
, m_nCenterServerId(DATA_INDEX_CENTER)
, m_nCenterServerIncr(CFG_BUSY_DEF_INCREMENT)
, m_nSelectServerCenterPort(0)
, m_nSelectServerClientPort(CFG_SELECT_PORT)
, m_nSelectServerId(DATA_INDEX_SELECT)
, m_nSelectServerIncr(CFG_BUSY_DEF_INCREMENT)
, m_nLoginDBServerLoginPort(CFG_LOGINDB_PORT)
, m_nLoginDBServerId(DATA_INDEX_LOGINDB)
, m_nLoginDBServerIncr(CFG_BUSY_DEF_INCREMENT)
, m_nLoginDBServerQueue(1)
, m_nLoginServerCenterPort(0)
, m_nLoginServerLoginDBPort(0)
, m_nLoginServerGamePort(0)
, m_nLoginServerClientPort(CFG_LOGIN_PORT)
, m_nLoginServerId(DATA_INDEX_LOGIN)
, m_nLoginServerIncr(CFG_BUSY_DEF_INCREMENT)
, m_nGameDBServerGamePort(CFG_GAMEDB_GAME_PORT)
, m_nGameDBServerGatePort(0)
, m_nGameDBServerId(DATA_INDEX_GAMEDB)
, m_nGameDBServerIncr(CFG_BUSY_DEF_INCREMENT)
, m_nGameDBServerQueue(1)
, m_nGameServerCenterPort(0)
, m_nGameServerGameDBPort(0)
, m_nGameServerLoginPort(0)
, m_nGameServerZonePort(CFG_GAME_ZONE_PORT)
, m_nGameServerGatePort(0)
, m_nGameServerId(DATA_INDEX_GAME)
, m_nGameServerIncr(CFG_BUSY_DEF_INCREMENT)
, m_nZoneServerGamePort(0)
, m_nZoneServerId(DATA_INDEX_ZONE)
, m_nZoneServerIncr(CFG_BUSY_DEF_INCREMENT)
, m_nGateServerGameDBPort(0)
, m_nGateServerGamePort(0)
, m_nGateServerLoginPort(0)
, m_nGateServerClientPort(CFG_GATE_PORT)
, m_nGateServerId(DATA_INDEX_GATE)
, m_nGateServerIncr(CFG_BUSY_DEF_INCREMENT)
, m_nGateServerQueue(1)
, m_strCenterServerPath(CenterServerPath)
, m_strSelectServerPath(SelectServerPath)
, m_strLoginDBServerPath(LoginDBServerPath)
, m_strLoginServerPath(LoginServerPath)
, m_strGameDBServerPath(GameDBServerPath)
, m_strGameServerPath(GameServerPath)
, m_strZoneServerPath(ZoneServerPath)
, m_strGateServerPath(GateServerPath)
, m_strCenterServerSelectAddr(LocalHost)
, m_strCenterServerLoginAddr(LocalHost)
, m_strCenterServerGameAddr(LocalHost)
, m_strSelectServerCenterAddr(LocalHost)
, m_strSelectServerClientAddr(LocalHost)
, m_strLoginDBServerLoginAddr(LocalHost)
, m_strLoginServerCenterAddr(LocalHost)
, m_strLoginServerLoginDBAddr(LocalHost)
, m_strLoginServerGameAddr(LocalHost)
, m_strLoginServerClientAddr(LocalHost)
, m_strGameServerCenterAddr(LocalHost)
, m_strGameServerGameDBAddr(LocalHost)
, m_strGameServerLoginAddr(LocalHost)
, m_strGameServerZoneAddr(LocalHost)
, m_strGameServerGateAddr(LocalHost)
, m_strGameServerPingAddr(LocalHost)
, m_strZoneServerGameAddr(LocalHost)
, m_strGateServerGameDBAddr(LocalHost)
, m_strGateServerGameAddr(LocalHost)
, m_strGateServerLoginAddr(LocalHost)
, m_strGateServerClientAddr(LocalHost)
, m_strConfigName(ConfigName)
{
}

INLINE CServerConfig& CServerConfig::operator=(const CServerConfig&)
{
	return (*this);
}

INLINE void CServerConfig::SetFilePath(CFilePath* pFilePath)
{
	m_pFilePath = pFilePath;
}

INLINE CFilePath* CServerConfig::GetFilePath(void)
{
	return m_pFilePath;
}

INLINE void CServerConfig::SetKeyValue(CKeyValue* pKeyValue)
{
	m_pKeyValue = pKeyValue;
}

INLINE CKeyValue* CServerConfig::GetKeyValue(void)
{
	return m_pKeyValue;
}

INLINE Int CServerConfig::GetNetworkAttr(void)
{
	return m_nNetworkAttr;
}

INLINE Int CServerConfig::GetNetworkThread(void)
{
	return m_nNetworkThread;
}

INLINE Int CServerConfig::GetNetworkAck(void)
{
	return m_nNetworkAck;
}

INLINE Int CServerConfig::GetNetworkTimeout(void)
{
	return m_nNetworkTimeout;
}

INLINE Int CServerConfig::GetNetworkBuffer(void)
{
	return m_nNetworkBuffer;
}

INLINE Int CServerConfig::GetNetworkJumbo(void)
{
	return m_nNetworkJumbo;
}

INLINE Int CServerConfig::GetUpdateTick(void)
{
	return m_nUpdateTick;
}

INLINE Int CServerConfig::GetReportTick(void)
{
	return m_nReportTick;
}

INLINE Int CServerConfig::GetLoadServers(void)
{
	return m_nLoadServers;
}

INLINE UShort CServerConfig::GetServerId(Int nServer)
{
	Int nRet = 0;
	switch (nServer) {
	case CFG_DEFAULT_CENTER:
		{
			nRet = m_nCenterServerId;
		}
		break;
	case CFG_DEFAULT_SELECT:
		{
			nRet = m_nSelectServerId;
		}
		break;
	case CFG_DEFAULT_LOGINDB:
		{
			nRet = m_nLoginDBServerId;
		}
		break;
	case CFG_DEFAULT_LOGIN:
		{
			nRet = m_nLoginServerId;
		}
		break;
	case CFG_DEFAULT_GAMEDB:
		{
			nRet = m_nGameDBServerId;
		}
		break;
	case CFG_DEFAULT_GAME:
		{
			nRet = m_nGameServerId;
		}
		break;
	case CFG_DEFAULT_ZONE:
		{
			nRet = m_nZoneServerId;
		}
		break;
	case CFG_DEFAULT_GATE:
		{
			nRet = m_nGateServerId;
		}
		break;
	default:
		{
			assert(0);
		}
	}
	return (UShort)nRet;
}

INLINE UShort CServerConfig::GetServerIncr(Int nServer)
{
	Int nRet = 0;
	switch (nServer) {
	case CFG_DEFAULT_CENTER:
		{
			nRet = m_nCenterServerIncr;
		}
		break;
	case CFG_DEFAULT_SELECT:
		{
			nRet = m_nSelectServerIncr;
		}
		break;
	case CFG_DEFAULT_LOGINDB:
		{
			nRet = m_nLoginDBServerIncr;
		}
		break;
	case CFG_DEFAULT_LOGIN:
		{
			nRet = m_nLoginServerIncr;
		}
		break;
	case CFG_DEFAULT_GAMEDB:
		{
			nRet = m_nGameDBServerIncr;
		}
		break;
	case CFG_DEFAULT_GAME:
		{
			nRet = m_nGameServerIncr;
		}
		break;
	case CFG_DEFAULT_ZONE:
		{
			nRet = m_nZoneServerIncr;
		}
		break;
	case CFG_DEFAULT_GATE:
		{
			nRet = m_nGateServerIncr;
		}
		break;
	default:
		{
			assert(0);
		}
	}
	return (UShort)nRet;
}

INLINE Int CServerConfig::GetServerQueue(Int nServer)
{
	Int nRet = 0;
	switch (nServer) {
	case CFG_DEFAULT_LOGINDB:
		{
			nRet = m_nLoginDBServerQueue;
		}
		break;
	case CFG_DEFAULT_GAMEDB:
		{
			nRet = m_nGameDBServerQueue;
		}
		break;
	case CFG_DEFAULT_GATE:
		{
			nRet = m_nGateServerQueue;
		}
		break;
	default:
		{
			assert(0);
		}
	}
	return nRet;
}

INLINE bool CServerConfig::GetServerExtConfig(Int nServer, CStringFix& strPath)
{
	switch (nServer) {
	case CFG_DEFAULT_CENTER:
		{
			strPath = m_strCenterServerExtConfig; // db info
		}
		break;
	case CFG_DEFAULT_SELECT:
		{
			strPath = m_strSelectServerExtConfig; // block ip list
		}
		break;
	case CFG_DEFAULT_LOGINDB:
		{
			strPath = m_strLoginDBServerExtConfig; // db info
		}
		break;
	case CFG_DEFAULT_LOGIN:
		{
			strPath = m_strLoginServerExtConfig; // block ip list
		}
		break;
	case CFG_DEFAULT_GAMEDB:
		{
			strPath = m_strGameDBServerExtConfig; // db info
		}
		break;
	case CFG_DEFAULT_GAME:
		{
			strPath = m_strGameServerExtConfig; // ...
		}
		break;
	case CFG_DEFAULT_ZONE:
		{
			strPath = m_strZoneServerExtConfig; // zone res
		}
		break;
	case CFG_DEFAULT_GATE:
		{
			strPath = m_strGateServerExtConfig; // block ip list
		}
		break;
	default:
		{
			assert(0);
		}
	}
	return (strPath.Length() != 0);
}

#endif // __SERVER_CONFIG_H__
