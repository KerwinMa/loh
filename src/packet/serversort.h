//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : serversort.h                                 //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update : 2015-11-25     version 0.0.0.5                    //
//   Detail : ����������                                         //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __SERVER_SORT_H__
#define __SERVER_SORT_H__

#pragma once

#include "atomics.h"
#include "datadef.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ��������������������
struct tagSERVER_SORT : public MObject
{
public:
	static void QuickSort(tagSERVER_SORT* pSort, Int nCount);
	static void QuickSortRecursion(tagSERVER_SORT* pSort, Int nBegin, Int nEnd);
	static Int  Partition(tagSERVER_SORT* pSort, Int nBegin, Int nEnd);
public:
	tagSERVER_SORT(void);
	~tagSERVER_SORT(void);
public:
	UShort                 usBusy;   ///< �������������(��ֱ�)
	UShort                 usIncr;   ///< ÿ����һ������, �������������������ֵ(��ֱ�)
	DataRef                drServer; ///< ���
	CNETTraits::NET_ADDR   Addr;     ///< ��ַ��Ϣ
};
typedef struct tagSERVER_SORT   SERVER_SORT, *PSERVER_SORT;
///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// �������������������������
struct tagSORT_RESULT : public MObject
{
public:
	/// ��ʱ����������ĸ������, ����С���ص�ǰSORTC_LIMIT������������, ��ѯ��������
	enum SORT_CONST
	{
		SORTC_LIMIT = 256,
	};
public:
	tagSORT_RESULT(void);
	~tagSORT_RESULT(void);
	/// ��������
	void Reset(void);
	/// ��ȡ��������С���صķ�����������
	Long Index(void);
public:
	Long          lIndex; ///< �����µ�������
	Long          lCount; ///< ����������
	SERVER_SORT   Sort[SORTC_LIMIT]; ///< ����������
};
typedef struct tagSORT_RESULT   SORT_RESULT, *PSORT_RESULT;
///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ������������������������齻����
struct tagRESULT_CHAIN : public MObject
{
public:
	/// ������������
	enum CHAIN_CONST
	{
		CHAINC_MAX = 2, 
	};
public:
	tagRESULT_CHAIN(void);
	~tagRESULT_CHAIN(void);
	/// ��������
	void Reset(void);
	/// �л�����
	void Swap(void);
	/// ��ȡ��ǰʹ�õ�����
	PSORT_RESULT GetCur(void);
	/// ��ȡ������
	PSORT_RESULT GetNext(void);
public:
	Long          lIndex; ///< ��ǰʹ�õ�����������
	SORT_RESULT   Sort[CHAINC_MAX]; ///< ��������
};
typedef struct tagRESULT_CHAIN   RESULT_CHAIN, *PRESULT_CHAIN;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "serversort.inl"

#endif // __SERVER_SORT_H__