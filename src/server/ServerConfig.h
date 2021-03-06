//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : ServerConfig.h                               //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update : 2015-11-25     version 0.0.0.5                    //
//   Detail : 服务器配置文件                                     //
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
/// 通用配置对象
class NOVTABLE CServerConfig : public MObject
{
public:
	/// 配置参数定义
	enum CFG_DEFAULT
	{
		CFG_DEFAULT_NONE       = 0, ///< 无
		// load server
		CFG_DEFAULT_CENTER     = 0x00000001, ///< 加载中心服务器模块
		CFG_DEFAULT_SELECT     = 0x00000002, ///< 加载选择服务器模块
		CFG_DEFAULT_LOGINDB    = 0x00000004, ///< 加载登陆DB服务器模块
		CFG_DEFAULT_LOGIN      = 0x00000008, ///< 加载登陆服务器模块
		CFG_DEFAULT_GAMEDB     = 0x00000010, ///< 加载游戏DB服务器模块
		CFG_DEFAULT_GAME       = 0x00000020, ///< 加载游戏服务器模块
		CFG_DEFAULT_ZONE       = 0x00000040, ///< 加载地图服务器模块
		CFG_DEFAULT_GATE       = 0x00000080, ///< 加载网关服务器模块
		CFG_DEFAULT_ALLSERVER  = 0x000000FF, ///< 加载全部服务器模块
		//
		CFG_CONFIG_PTR         = 0x10000000, ///< 获取配置对象地址
		// center
		CFG_CENTER_SELECT_PORT = 11110,   ///< 中心服务器默认监听端口(选择)
		CFG_CENTER_LOGIN_PORT  = 11111,   ///< 中心服务器默认监听端口(登陆)
		CFG_CENTER_GAME_PORT   = 11112,   ///< 中心服务器默认监听端口(游戏)
		// select
		CFG_SELECT_PORT        = 10010,   ///< 选择服务器默认UDP端口(客户端)
		// logindb
		CFG_LOGINDB_PORT       = 11120,   ///< 登陆DB服务器默认监听端口(登陆)
		// login
		CFG_LOGIN_PORT         = 10020,   ///< 登陆服务器默认监听端口(客户端)  
		// gamedb
		CFG_GAMEDB_GAME_PORT   = 11130,   ///< 游戏DB服务器默认监听端口(游戏)
		CFG_GAMEDB_GATE_PORT   = 11131,   ///< 游戏DB服务器默认监听端口(网关)
		// game
		CFG_GAME_ZONE_PORT     = 11140,   ///< 游戏服务器默认监听端口(地图)
		CFG_GAME_GATE_PORT     = 11141,   ///< 游戏服务器默认监听端口(网关)
		// gate
		CFG_GATE_PORT          = 10030,   ///< 网关服务器默认监听端口(客户端)
		// update
		CFG_UPDATE_MIN_TIME    = 16,      ///< 最小同步更新周期
		CFG_UPDATE_DEF_TIME    = 30,      ///< 同步更新周期默认30秒
		CFG_BUSY_DEF_INCREMENT = 40,      ///< 增加的负载默认值(万分比)

		CFG_LIVE_MIN_TIME      = 60,	  ///< 最小心跳包1分钟
		CFG_LIVE_DEF_TIME      = 5 * 60,  ///< 默认心跳包5分钟
		CFG_REPORT_MIN_TIME    = 5 * 60,  ///< 上报服务器信息到数据库的最小时间间隔
		CFG_REPORT_DEF_TIME    = 30 * 60, ///< 上报服务器信息到数据库的默认时间间隔
	};
	/// 动态变量表
	static CPCXStr ServerConfig;          ///< 配置对象
	static CPCXStr NetworkPtr;            ///< 网络对象

	static CPCXStr ConfigName;            ///< 配置文件名称
	static CPCXStr CenterServerPath;      ///< 中心服务器模块路径
	static CPCXStr SelectServerPath;      ///< 选择服务器模块路径
	static CPCXStr LoginDBServerPath;     ///< 登陆DB服务器模块路径
	static CPCXStr LoginServerPath;       ///< 登陆服务器模块路径
	static CPCXStr GameDBServerPath;      ///< 游戏DB服务器模块路径
	static CPCXStr GameServerPath;        ///< 游戏服务器模块路径
	static CPCXStr ZoneServerPath;        ///< 地图服务器模块路径
	static CPCXStr GateServerPath;        ///< 网关服务器模块路径

	static CPCXStr NetworkBlock;          ///< NetworkBlock
	static CPCXStr NetworkAttr;           ///< NetworkAttr=0(0表示使用默认值, 后面的都忽略)
	static CPCXStr NetworkThread;         ///< NetworkThread=0(cores * 2)
	static CPCXStr NetworkAck;            ///< NetworkAck
	static CPCXStr NetworkTimeout;        ///< NetworkTimeout
	static CPCXStr NetworkBuffer;         ///< NetworkBuffer
	static CPCXStr NetworkJumbo;          ///< NetworkJumbo

	static CPCXStr ServersBlock;          ///< ServersBlock
	static CPCXStr UpdateTick;            ///< UpdateTick=30(秒)
	static CPCXStr ReportTick;            ///< ReportTick=30(分)
	static CPCXStr LoadServers;           ///< 加载的服务器种类 : LoadServers=CenterServer|SelectServer
	static CPCXStr CenterServer;          ///< 加载的服务器DLL路径 : CenterServer=CenterServer.dll
	static CPCXStr SelectServer;          ///< 选择服务器模块路径
	static CPCXStr LoginDBServer;         ///< 登陆DB服务器模块路径
	static CPCXStr LoginServer;           ///< 登陆服务器模块路径
	static CPCXStr GameDBServer;          ///< 游戏DB服务器模块路径
	static CPCXStr GameServer;            ///< 游戏服务器模块路径
	static CPCXStr ZoneServer;            ///< 地图服务器模块路径
	static CPCXStr GateServer;            ///< 网关服务器模块路径

	static CPCXStr Service;
	static CPCXStr Addr;                  ///< 要和NetworkAttr中ipv4 or ipv6匹配
	static CPCXStr Port;                  ///< 端口
	static CPCXStr Id;                    ///< Id
	static CPCXStr Incr;                  ///< 增长
	static CPCXStr Queue;                 ///< 队列线程数量
	static CPCXStr ExtConfig;             ///< 服务器扩展配置文件路径
	static CPCXStr LocalHost;             ///< 本地地址
public:
	CServerConfig(void);
	~CServerConfig(void);
	/// 设置文件管理器
	void SetFilePath(CFilePath* pFilePath);
	/// 获取文件管理器
	CFilePath* GetFilePath(void);
	/// 设置key-value管理器
	void SetKeyValue(CKeyValue* pKeyValue);
	/// 获取key-value管理器
	CKeyValue* GetKeyValue(void);
	/// 初始化
	/// @param[in] PCXStr pszName配置文件
	bool   Init(PCXStr pszName = nullptr);
	/// 结束
	void   Exit(void);
	/// 打印配置信息到文件日志
	void   Dump(CFileLog& Log);
	/// 获取网络层配置参数-属性
	Int    GetNetworkAttr(void);
	/// 获取网络层配置参数-线程数量
	Int    GetNetworkThread(void);
	/// 获取网络层配置参数-ACK时间
	Int    GetNetworkAck(void);
	/// 获取网络层配置参数-超时时间
	Int    GetNetworkTimeout(void);
	/// 获取网络层配置参数-普通接收或者发送缓存大小
	Int    GetNetworkBuffer(void);
	/// 获取网络层配置参数-巨帧接收或者发送缓存大小
	Int    GetNetworkJumbo(void);
	/// 获取同步服务器数据间隔
	Int    GetUpdateTick(void);
	/// 获取上报信息时间间隔
	Int    GetReportTick(void);
	/// 获取加载服务器情况
	Int    GetLoadServers(void);
	/// 获取各服务器的模块文件路径
	/// @param[in] Int nServer 服务器类型, 参考CFG_DEFAULT_XXX
	/// @param[out] CStringFix& strPath 文件路径
	bool   GetServerPath(Int nServer, CStringFix& strPath);
	/// 获取各服务器的地址信息
	/// @param[in] Int nServer 服务器类型, 参考CFG_DEFAULT_XXX
	/// @param[in] Int nServerAddr 服务器类型, 参考CFG_DEFAULT_XXX
	/// @param[out] CStringKey& strAddr 地址
	/// @param[out] UShort& usPort 端口
	bool   GetServerAddr(Int nServer, Int nServerAddr, CStringKey& strAddr, UShort& usPort);
	/// 获取各服务器的Id
	UShort GetServerId(Int nServer);
	/// 获取各服务器的负载增长率
	UShort GetServerIncr(Int nServer);
	/// 获取各服务器的事务处理队列线程数
	Int    GetServerQueue(Int nServer);
	/// 获取各服务器的扩展配置文件路径
	bool   GetServerExtConfig(Int nServer, CStringFix& strPath);
private:
	CServerConfig(const CServerConfig&);
	CServerConfig& operator=(const CServerConfig&);
	/// 从ini文件读取信息1
	void   CheckLoadAddr(CIni& ini, PCXStr pszSection, PCXStr pszServer, CStringKey& strAddr, Int& nPort, Int nDefault);
	/// 从ini文件读取信息2
	void   LoadAddr(CIni& ini, PCXStr pszSection, PCXStr pszServer, CStringKey& strAddr, Int& nPort, Int nDefault);
public:
	CFilePath*   m_pFilePath; ///< 文件管理器
	CKeyValue*   m_pKeyValue; ///< key-value管理器
	// network
	Int          m_nNetworkAttr;    ///< 网络层配置参数-属性
	Int          m_nNetworkThread;  ///< 网络层配置参数-线程数量
	Int          m_nNetworkAck;     ///< 网络层配置参数-ACK时间
	Int          m_nNetworkTimeout; ///< 网络层配置参数-超时时间
	Int          m_nNetworkBuffer;  ///< 网络层配置参数-普通接收或者发送缓存大小
	Int          m_nNetworkJumbo;   ///< 网络层配置参数-巨帧接收或者发送缓存大小
	// servers
	Int          m_nUpdateTick;  ///< 同步服务器数据间隔
	Int          m_nReportTick;  ///< 上报信息时间间隔
	Int          m_nLoadServers; ///< 加载服务器情况
	// center-server
	Int          m_nCenterServerSelectPort; ///< 和m_strCenterServerSelectAddr组成监听选择服务器的地址
	Int          m_nCenterServerLoginPort;  ///< 和m_strCenterServerLoginAddr组成监听登陆服务器的地址, 端口0表示与监听选择服务器相同地址
	Int          m_nCenterServerGamePort;   ///< 和m_strCenterServerGameAddr组成监听游戏服务器的地址, 端口0表示与监听选择服务器或者登陆服务器相同地址
	Int          m_nCenterServerId;         ///< 服务器Id, 可以用来物理地址划分
	Int          m_nCenterServerIncr;       ///< 负载增长值(万分比)
	// select-server
	Int          m_nSelectServerCenterPort; ///< 和m_strSelectServerCenterAddr组成连接中心服务器本地绑定地址, 端口0表示由系统分配地址
	Int          m_nSelectServerClientPort; ///< 和m_strSelectServerClientAddr组成UDP服务地址
	Int          m_nSelectServerId;         ///< 服务器Id, 可以用来物理地址划分
	Int          m_nSelectServerIncr;		///< 负载增长值(万分比)
	// logindb-server
	Int          m_nLoginDBServerLoginPort; ///< 和m_strLoginDBServerLoginAddr组成监听登陆服务器的地址
	Int          m_nLoginDBServerId;        ///< 服务器Id, 可以用来物理地址划分
	Int          m_nLoginDBServerIncr;		///< 负载增长值(万分比)
	Int          m_nLoginDBServerQueue;     ///< 事务队列线程数量
	// login-server
	Int          m_nLoginServerCenterPort;  ///< 和m_strLoginServerCenterAddr组成连接中心服务器本地绑定地址, 端口0表示由系统分配地址
	Int          m_nLoginServerLoginDBPort; ///< 和m_strLoginServerLoginDBAddr组成连接登陆DB服务器本地绑定地址, 端口0表示由系统分配地址
	Int          m_nLoginServerGamePort;    ///< 和m_strLoginServerGameAddr组成监听游戏服务器的UDP地址, 端口0表示由系统分配地址
	Int          m_nLoginServerClientPort;  ///< 和m_strLoginServerClientAddr组成TCP服务地址
	Int          m_nLoginServerId;          ///< 服务器Id, 可以用来物理地址划分
	Int          m_nLoginServerIncr;		///< 负载增长值(万分比)
	// gamedb-server
	Int          m_nGameDBServerGamePort;   ///< 和m_strGameDBServerGameAddr组成监听游戏服务器的地址
	Int          m_nGameDBServerGatePort;   ///< 和m_strGameDBServerGateAddr组成监听网关服务器的地址, 端口0表示与监听游戏服务器相同地址
	Int          m_nGameDBServerId;         ///< 服务器Id, 可以用来物理地址划分
	Int          m_nGameDBServerIncr;		///< 负载增长值(万分比)
	Int          m_nGameDBServerQueue;		///< 事务队列线程数量
	// game-server
	Int          m_nGameServerCenterPort;   ///< 和m_strGameServerCenterAddr组成连接中心服务器本地绑定地址, 端口0表示由系统分配地址
	Int          m_nGameServerGameDBPort;	///< 和m_strGameServerGameDBAddr组成连接游戏DB服务器本地绑定地址, 端口0表示由系统分配地址
	Int          m_nGameServerLoginPort;	///< 和m_strGameServerLoginAddr组成监听登陆服务器的UDP地址, 端口0表示由系统分配地址
	Int          m_nGameServerZonePort;     ///< 和m_strGameServerZoneAddr组成监听地图服务器的地址
	Int          m_nGameServerGatePort;     ///< 和m_strGameServerGateAddr组成监听网关服务器的地址, 端口0表示与监听地图服务器相同地址
	Int          m_nGameServerId;           ///< 服务器Id, 客户端用来索引名称
	Int          m_nGameServerIncr;			///< 负载增长值(万分比)
	// zone-server
	Int          m_nZoneServerGamePort;     ///< 和m_strZoneServerGameAddr组成连接游戏服务器本地绑定地址, 端口0表示由系统分配地址
	Int          m_nZoneServerId;           ///< 服务器Id, 可以用来物理地址划分
	Int          m_nZoneServerIncr;			///< 负载增长值(万分比)
	// gate-server
	Int          m_nGateServerGameDBPort;   ///< 和m_strGateServerGameDBAddr组成连接游戏DB服务器本地绑定地址, 端口0表示由系统分配地址
	Int          m_nGateServerGamePort;		///< 和m_strGateServerGameAddr组成连接游戏服务器本地绑定地址, 端口0表示由系统分配地址
	Int          m_nGateServerLoginPort;	///< 和m_strGateServerLoginAddr组成监听登陆服务器的UDP地址, 端口0表示由系统分配地址
	Int          m_nGateServerClientPort;	///< 和m_strGateServerClientAddr组成TCP服务地址
	Int          m_nGateServerId;           ///< 服务器Id, 可以用来物理地址划分
	Int          m_nGateServerIncr;			///< 负载增长值(万分比)
	Int          m_nGateServerQueue;		///< 事务队列线程数量
	//
	CStringFix   m_strCenterServerPath;     ///< 加载中心服务器模块路径
	CStringFix   m_strSelectServerPath;		///< 加载选择服务器模块路径
	CStringFix   m_strLoginDBServerPath;	///< 加载登陆DB服务器模块路径
	CStringFix   m_strLoginServerPath;		///< 加载登陆服务器模块路径
	CStringFix   m_strGameDBServerPath;		///< 加载游戏DB服务器模块路径
	CStringFix   m_strGameServerPath;		///< 加载游戏服务器模块路径
	CStringFix   m_strZoneServerPath;		///< 加载地图服务器模块路径
	CStringFix   m_strGateServerPath;		///< 加载网关服务器模块路径
	// center-server
	CStringKey   m_strCenterServerSelectAddr; ///< 和m_nCenterServerSelectPort组成监听选择服务器的地址
	CStringKey   m_strCenterServerLoginAddr;  ///< 和m_nCenterServerLoginPort组成监听登陆服务器的地址, 端口0表示与监听选择服务器相同地址
	CStringKey   m_strCenterServerGameAddr;   ///< 和m_nCenterServerGamePort组成监听游戏服务器的地址, 端口0表示与监听选择服务器或者登陆服务器相同地址
	CStringFix   m_strCenterServerExtConfig;  ///< 额外配置文件路径
	// select-server
	CStringKey   m_strSelectServerCenterAddr; ///< 和m_nSelectServerCenterPort组成连接中心服务器本地绑定地址, 端口0表示由系统分配地址
	CStringKey   m_strSelectServerClientAddr; ///< 和m_nSelectServerClientPort组成UDP服务地址
	CStringFix   m_strSelectServerExtConfig;  ///< 额外配置文件路径
	// logindb-server
	CStringKey   m_strLoginDBServerLoginAddr; ///< 和m_nLoginDBServerLoginPort组成监听登陆服务器的地址
	CStringFix   m_strLoginDBServerExtConfig; ///< 额外配置文件路径
	// login-server
	CStringKey   m_strLoginServerCenterAddr;  ///< 和m_strLoginServerCenterPort组成连接中心服务器本地绑定地址, 端口0表示由系统分配地址
	CStringKey   m_strLoginServerLoginDBAddr; ///< 和m_strLoginServerLoginDBPort组成连接登陆DB服务器本地绑定地址, 端口0表示由系统分配地址
	CStringKey   m_strLoginServerGameAddr;    ///< 和m_strLoginServerGamePort组成监听游戏服务器的UDP地址, 端口0表示由系统分配地址
	CStringKey   m_strLoginServerClientAddr;  ///< 和m_strLoginServerClientPort组成TCP服务地址
	CStringFix   m_strLoginServerExtConfig;   ///< 额外配置文件路径
	// gamedb-server
	CStringKey   m_strGameDBServerGameAddr;   ///< 和m_strGameDBServerGamePort组成监听游戏服务器的地址
	CStringKey   m_strGameDBServerGateAddr;   ///< 和m_strGameDBServerGatePort组成监听网关服务器的地址, 端口0表示与监听游戏服务器相同地址
	CStringFix   m_strGameDBServerExtConfig;  ///< 额外配置文件路径
	// game-server
	CStringKey   m_strGameServerCenterAddr;   ///< 和m_strGameServerCenterPort组成连接中心服务器本地绑定地址, 端口0表示由系统分配地址
	CStringKey   m_strGameServerGameDBAddr;   ///< 和m_strGameServerGameDBPort组成连接游戏DB服务器本地绑定地址, 端口0表示由系统分配地址
	CStringKey   m_strGameServerLoginAddr;    ///< 和m_strGameServerLoginPort组成监听登陆服务器的UDP地址, 端口0表示由系统分配地址
	CStringKey   m_strGameServerZoneAddr;     ///< 和m_strGameServerZonePort组成监听地图服务器的地址
	CStringKey   m_strGameServerGateAddr;     ///< 和m_strGameServerGatePort组成监听网关服务器的地址, 端口0表示与监听地图服务器相同地址
	CStringKey   m_strGameServerPingAddr;     ///< PING地址, 客户端通过PING这个地址检测网络快慢
	CStringFix   m_strGameServerExtConfig;    ///< 额外配置文件路径
	// zone-server
	CStringKey   m_strZoneServerGameAddr;     ///< 和m_strZoneServerGamePort组成连接游戏服务器本地绑定地址, 端口0表示由系统分配地址
	CStringFix   m_strZoneServerExtConfig;    ///< 额外配置文件路径
	// gate-server
	CStringKey   m_strGateServerGameDBAddr;   ///< 和m_strGateServerGameDBPort组成连接游戏DB服务器本地绑定地址, 端口0表示由系统分配地址
	CStringKey   m_strGateServerGameAddr;     ///< 和m_strGateServerGamePort组成连接游戏服务器本地绑定地址, 端口0表示由系统分配地址
	CStringKey   m_strGateServerLoginAddr;    ///< 和m_strGateServerLoginPort组成监听登陆服务器的UDP地址, 端口0表示由系统分配地址
	CStringKey   m_strGateServerClientAddr;   ///< 和m_strGateServerClientPort组成TCP服务地址
	CStringFix   m_strGateServerExtConfig;    ///< 额外配置文件路径
	// config file name
	CStringFix   m_strConfigName;             ///< 配置文件名称
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
