//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : serversort.h                                 //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ����������                                         //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __SERVER_SORT_H__
#define __SERVER_SORT_H__

#pragma once

#include "atomics.h"
#include "datadef.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// tagSERVER_SORT : ������������Ϣ�ṹ����
typedef struct tagSERVER_SORT
{
public:
	// ��������������������
	static void QuickSort(tagSERVER_SORT* pSort, Int nCount);
	static void QuickSortRecursion(tagSERVER_SORT* pSort, Int nBegin, Int nEnd);
	static Int  Partition(tagSERVER_SORT* pSort, Int nBegin, Int nEnd);
public:
	tagSERVER_SORT(void);
	~tagSERVER_SORT(void);
public:
	UShort                 usBusy; // ��������æ(��ֱ�)
	UShort                 usIncr; // ÿ����һ������, ��������æ������ֵ
	DataRef                drServer;
	CNETTraits::NET_ADDR   Addr;
}SERVER_SORT, *PSERVER_SORT;
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// tagSORT_RESULT : ����������æ������
typedef struct tagSORT_RESULT
{
public:
	enum SORT_CONST
	{
		SORTC_LIMIT = 256,    // ��ʱ����������ĸ������, �������ǰSORTC_LIMIT����������, ��ѯ��������
	};
public:
	tagSORT_RESULT(void);
	~tagSORT_RESULT(void);

	void Reset(void);
	Long Index(void);
public:
	Long          lIndex;
	Long          lCount;
	SERVER_SORT   Sort[SORTC_LIMIT];
}SORT_RESULT, *PSORT_RESULT;
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// tagRESULT_CHAIN : ����������æ��������ʱ������
typedef struct tagRESULT_CHAIN
{
public:
	enum CHAIN_CONST
	{
		CHAINC_MAX = 2, // ������������
	};
public:
	tagRESULT_CHAIN(void);
	~tagRESULT_CHAIN(void);

	void Reset(void);
	void Swap(void); // 

	PSORT_RESULT GetCur(void);
	PSORT_RESULT GetNext(void);
public:
	Long          lIndex;
	SORT_RESULT   Sort[CHAINC_MAX];
}RESULT_CHAIN, *PRESULT_CHAIN;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "serversort.inl"

#endif // __SERVER_SORT_H__