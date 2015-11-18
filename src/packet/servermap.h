//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : servermap.h                                  //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ��Ϣ���ݴ洢                                      //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __SERVER_MAP_H__
#define __SERVER_MAP_H__

#pragma once

#include "datadef.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// tagSERVER_INFO : ��������Ϣ�ṹ����
struct tagSERVER_INFO : public tagSERVER_DATA
{
public:
	tagSERVER_INFO(void);
	~tagSERVER_INFO(void);

	void     Info(CStream& Stream, Int nStatus = STATUSU_SYNCXLINK);
	void     Copy(const tagSERVER_DATA& sd);
	// ͳ��ʱ���õ���
	void     Incr(const tagSERVER_DATA& sd);
	void     Diff(const tagSERVER_DATA& sdIncr, const tagSERVER_DATA& sdDecr);
	void     Decr(const tagSERVER_DATA& sd);
	void     Drop(const tagSERVER_DATA& sd);

	void     Incr(void);    // ����������, ���Ӹ���
	void     Decr(void);    // ����������, ���ٸ���
	void     Zero(void);    // ����
	void     Reset(void);   //
};
typedef tagSERVER_INFO SERVER_INFO, *PSERVER_INFO;
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// tagSERVER_ADDR : ������ַ�ķ�������Ϣ�ṹ����
template <size_t stLen>
struct tagSERVER_ADDR : public tagSERVER_INFO
{
public:
	tagSERVER_ADDR(void);
	~tagSERVER_ADDR(void);

	void Addr(CStream& Stream, Int nStatus = STATUSU_SYNCXLINK);
public:
//	Long                   lRefCount;
	CNETTraits::NET_ADDR   NetAddr[stLen];
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������������ݵĲ��ֻ�������ַ�����Ķ���, ����ʹ�ü򵥵�ģ��ṹ������������������Ϣ�ṹ
// ����ض��ķ�������Ҫ�������������(����������������Ƶ�), ����������������Ϣ�ṹ
// TEST_INDEX
enum TEST_INDEX
{
	TESTI_UNKNOWN,
	TESTI_COUNT,
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEST_ADDR : ��������Ϣ�ṹ����
typedef tagSERVER_ADDR<TESTI_COUNT>   TEST_ADDR, *PTEST_ADDR;   //
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// SELECT_INDEX
enum SELECT_INDEX 
{
	SELECTI_UDP,    // UDP�����ͻ��˵�ַ
	SELECTI_CENTER, // �������ĵ��ض���ַ(����������ָ���ض����������ض��˿�)
	SELECTI_COUNT,
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// SELECT_ADDR : ѡ���������Ϣ�ṹ����
typedef tagSERVER_ADDR<SELECTI_COUNT> SELECT_ADDR, *PSELECT_ADDR; // 0-�ⲿUDP��ַ; 1-�ڲ��������ķ�������ַ
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// LOGIN_INDEX
enum LOGIN_INDEX 
{
	LOGINI_TCP,          // TCP�����ͻ��˵�ַ
	LOGINI_UDP,          // UDP����Ϸ������ͨ��
	LOGINI_CENTER,       // �������ĵ��ض���ַ(����������ָ���ض����������ض��˿�)
	LOGINI_LOGINDB,      // ���ӵ�½DB���ض���ַ(����������ָ���ض����������ض��˿�)
	LOGINI_LOGINDB_ADDR, // ���ӵĵ�½DB��ַ
	LOGINI_COUNT,
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// LOGIN_ADDR : ��½��������Ϣ�ṹ����
typedef tagSERVER_ADDR<LOGINI_COUNT>  LOGIN_ADDR,  *PLOGIN_ADDR;  // 0-�ⲿ��ַ; 1-�ڲ���game��UDP��ַ; 2-logindb��ַ; 3-�ڲ��������ķ�������ַ; 
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// GAME_INDEX
enum GAME_INDEX 
{
	GAMEI_GATE,          // TCP�������ط�������ַ
	GAMEI_ZONE,          // TCP������ͼ��������ַ
	GAMEI_UDP,           // UDP���½������ͨ��
	GAMEI_CENTER,        // �������ĵ��ض���ַ(����������ָ���ض����������ض��˿�)
	GAMEI_GAMEDB,        // ������ϷDB���ض���ַ(����������ָ���ض����������ض��˿�)
	GAMEI_GAMEDB_ADDR,   // ���ӵ���ϷDB��ַ
	GAMEI_COUNT,
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// GAME_ADDR : ��Ϸ��������Ϣ�ṹ����
typedef tagSERVER_ADDR<GAMEI_COUNT>   GAME_ADDR,   *PGAME_ADDR;   // 0-��gate�ڲ���ַ; 1-��zone�ڲ���ַ; 2-�ڲ���login��UDP��ַ; 3-gamedb��ַ; 4-�ڲ��������ķ�������ַ; 
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// ZONE_INDEX
enum ZONE_INDEX 
{
	ZONEI_GAME, // ������Ϸ���ض���ַ(����������ָ���ض����������ض��˿�)
	ZONEI_COUNT,
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// ZONE_ADDR : ��Ϸ��ͼ��������Ϣ�ṹ����
typedef tagSERVER_ADDR<ZONEI_COUNT>   ZONE_ADDR,   *PZONE_ADDR;   // 0-�ڲ�������Ϸ��������ַ

struct tagZONE_ADDR_INDEX : public ZONE_ADDR {
public:
	tagZONE_ADDR_INDEX(void);
	~tagZONE_ADDR_INDEX(void);
public:
	typedef CTArray<PINDEX>   ARY_INDEX;
public:
	ARY_INDEX   Index;
};

typedef tagZONE_ADDR_INDEX            ZONE_ADDR_INDEX, *PZONE_ADDR_INDEX;
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// GATE_INDEX
enum GATE_INDEX 
{
	GATEI_TCP,           // TCP�����ͻ��˵�ַ
	GATEI_UDP,           // UDP���½������ͨ��
	GATEI_GAME,          // ������Ϸ���ض���ַ(����������ָ���ض����������ض��˿�)
	GATEI_GAMEDB,        // ������ϷDB���ض���ַ(����������ָ���ض����������ض��˿�)
	GATEI_GAMEDB_ADDR,   // ���ӵ���ϷDB��ַ
	GATEI_COUNT,
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// GATE_ADDR : ��Ϸ���ط�������Ϣ�ṹ����
typedef tagSERVER_ADDR<GATEI_COUNT>   GATE_ADDR,   *PGATE_ADDR;   // 0-�ⲿ��ַ; 1-�ڲ�������Ϸ��������ַ; 2-�ڲ�������ϷDB��������ַ; 3-�ڲ���login��UDP��ַ
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// tagSERVER_NOADDR : ��������ַ�ķ�������Ϣ�ṹ����
struct tagSERVER_NOADDR : public tagSERVER_INFO
{
public:
	tagSERVER_NOADDR(void);
	~tagSERVER_NOADDR(void);

	void Addr(CStream& Stream, Int nStatus = STATUSU_SYNCXLINK);
//public:
//	Long                   lRefCount;
};
typedef tagSERVER_NOADDR SERVER_NOADDR, *PSERVER_NOADDR;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// tagSERVER_MAP : ��������Ϣӳ���ṹ����
template <typename V>
struct tagSERVER_MAP : public MObject
{
public:
	tagSERVER_MAP(void);
	~tagSERVER_MAP(void);

	CSyncLock& GetLock(void);

	void  Update(void);

	// 1. �ͻ��˴�������STATUSU_NOADDR
	// 2. ����ͬ��+��ɵķ�������ϢSTATUSU_OKAYSYNC, Link���ʱ����
	// 3. TODO!!!��ôû�п������ݳ��������󱣴����޵�����, ����ֻ��ʹ��jumbo buffer����, 
	//    �������������ֶ��buffer/jumbo buffer����, Ĭ��jumbo buffer������෢��ͬ����120����������ȫ����Ϣ,
	//    Ŀǰͬ������ֻ�������ķ�����ͬ����������Ϣ��ѡ��͵�½ʱ����, ���е�½������ͬ����Ϸ��������Ϣ���ͻ���
	bool  Serialize(CStream& Stream, Int nStatus = STATUSU_SYNCXLINK);
public:
	bool  Read(CStream& Stream, Int nStatus);
	bool  Write(CStream& Stream, Int nStatus);
public:
	typedef struct tagSVR_PAIR
	{
	public:
		void Serialize(CStream& Stream);
	public:
		DataRef   drKey;
		V         Value;
	}SVR_PAIR, *PSVR_PAIR;
	typedef CTMap<DataRef, V>                  SVR_MAP;
	typedef typename CTMap<DataRef, V>::PAIR   SVR_MAP_PAIR;
public:
	Int           Size(void);

	PINDEX        Add(DataRef drKey);
	PINDEX        Add(DataRef drKey, V& v);

	SVR_MAP_PAIR* Find(DataRef drKey);
	SVR_MAP_PAIR* At(PINDEX index);

	PINDEX        First(void);
	SVR_MAP_PAIR* Next(PINDEX& index);

	bool          Remove(DataRef drKey);
	bool          RemoveAt(PINDEX index);
	void          RemoveAll(void);
private:
	SVR_MAP       m_SvrMap;
	CSyncLock     m_SyncLock;
};
typedef tagSERVER_MAP<SELECT_ADDR>       SVR_SELECT_MAP,     *PSVR_SELECT_MAP;
typedef tagSERVER_MAP<LOGIN_ADDR>        SVR_LOGIN_MAP,      *PSVR_LOGIN_MAP;
typedef tagSERVER_MAP<GAME_ADDR>         SVR_GAME_MAP,       *PSVR_GAME_MAP;
typedef tagSERVER_MAP<ZONE_ADDR>         SVR_ZONE_MAP,       *PSVR_ZONE_MAP;
typedef tagSERVER_MAP<ZONE_ADDR_INDEX>   SVR_ZONE_INDEX_MAP, *PSVR_ZONE_INDEX_MAP;
typedef tagSERVER_MAP<GATE_ADDR>         SVR_GATE_MAP,       *PSVR_GATE_MAP;
typedef tagSERVER_MAP<TEST_ADDR>         SVR_TEST_MAP,       *PSVR_TEST_MAP;
typedef tagSERVER_MAP<SERVER_NOADDR>     SVR_SERVER_MAP,     *PSVR_SERVER_MAP;
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// DATA_MAP : ������Ϣӳ���
template <typename K, typename V, class KTraits = CTElementTraits<K>, class VTraits = CTElementTraits<V>>
struct tagDATA_MAP : public MObject
{
public:
	tagDATA_MAP(void);
	~tagDATA_MAP(void);

	CSyncLock& GetLock(void);

	bool Serialize(CStream& Stream);
public:
	typedef CTMap<K, V, KTraits, VTraits>                   DATA_MAP;
	typedef typename CTMap<K, V, KTraits, VTraits>::PAIR    DATA_MAP_PAIR;
public:
	Int            Size(void);
	void           Cache(Int nCache);

	PINDEX         Add(K key);
	PINDEX         Add(K key, V& val);

	DATA_MAP_PAIR* Find(K key);

	PINDEX         First(void);
	DATA_MAP_PAIR* Next(PINDEX& index);

	DATA_MAP_PAIR* Get(PINDEX index);

	bool           Remove(K key);
	bool           RemoveAt(PINDEX index);
	void           RemoveAll(void);
private:
	DATA_MAP       m_DataMap;
	CSyncLock      m_SyncLock;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "servermap.inl"

#endif // __SERVER_MAP_H__