//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Inline File : serversort.inl                               //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update : 2015-11-25     version 0.0.0.5                    //
//   Detail : ��Ϣ����                                           //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __SERVER_SORT_INL__
#define __SERVER_SORT_INL__

#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// tagSERVER_SORT : ��������������������
INLINE void tagSERVER_SORT::QuickSort(tagSERVER_SORT* pSort, Int nCount)
{
	QuickSortRecursion(pSort, 0, nCount);
}
// ����ĵݹ����
INLINE void tagSERVER_SORT::QuickSortRecursion(tagSERVER_SORT* pSort, Int nBegin, Int nEnd)
{
    if (nBegin < (nEnd - 1)){// ������򲻴��ڻ�ֻ��һ�������򲻵ݹ�����
		// ������Ϊ�ָ��ʱ��, �ָ�㴦�����ݾ�������������λ��
		// Ҳ����˵������ߵ����ݶ�С�ڵ�����, ���ұߵ����ݶ�������
		// ���������ڵݹ���õ�������
		Int nPart = Partition(pSort, nBegin, nEnd);  // �ҵ��ָ��
		QuickSortRecursion(pSort, nBegin, nPart);    // �ݹ���ߵ�����
		QuickSortRecursion(pSort, nPart + 1, nEnd);  // �ݹ��ұߵ�����
	}
}
// ��������, �ҵ����Ԫ�ص�����λ��, �����طָ��ĵ�(�����һ���������λ��)
// ���ֵ�������[nBegin, nEnd), pSort�Ǳ������ݵ�ָ��
INLINE Int tagSERVER_SORT::Partition(tagSERVER_SORT* pSort, Int nBegin, Int nEnd)
{
	Int nPart = nBegin - 1;    // ���Comp����������
    --nEnd;

	tagSERVER_SORT Temp;       // �����õ���ʱ����
    tagSERVER_SORT Comp = pSort[nEnd]; // �Ƚϵ�����

    // �������ݱȽ�, �ҵ�Comp��λ��, ����ע��, �ȽϽ����:
    // ���i�������С�ڵ���Comp��, i���ұ��Ǵ���Comp��
    for (Int i = nBegin; i < nEnd; ++i){
        if (pSort[i].usBusy <= Comp.usBusy){ // ������ݱ�Ҫ�Ƚϵ�С, ���ڸ����ݵ����, �� nPart + 1 ����
            ++nPart;                        // С��Comp�����ݶ�һ��, ����Ҫ��1, i��������ݶ���CompС
            Temp         = pSort[nPart];    // ��������
			pSort[nPart] = pSort[i];
			pSort[i]     = Temp;
        }
    }
    // ���Ҫ���˰�Comp�� nPart + 1 ����, ��Ϊ�������Comp��λ��
    ++nPart;
    pSort[nEnd]  = pSort[nPart];
    pSort[nPart] = Comp;

    return nPart;   // ����nPart��λ��, ���Ƿָ��λ��
}

INLINE tagSERVER_SORT::tagSERVER_SORT(void)
: usBusy(0)
, usIncr(0)
#if (__INTRIN_TARGET__ == INTRIN_TARGET_X86)
, drServer(0)
#elif ((__INTRIN_TARGET__ == INTRIN_TARGET_X64) || (__INTRIN_TARGET__ == INTRIN_TARGET_IA64))
, drServer(nullptr)
#endif
{
}

INLINE tagSERVER_SORT::~tagSERVER_SORT(void)
{
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// tagSORT_RESULT : �������������������������
INLINE tagSORT_RESULT::tagSORT_RESULT(void)
: lIndex(0)
, lCount(0)
{
}

INLINE tagSORT_RESULT::~tagSORT_RESULT(void)
{
}

INLINE void tagSORT_RESULT::Reset(void)
{ 
	lIndex = 0; 
	lCount = 0; 
}

INLINE Long tagSORT_RESULT::Index(void)
{
	if (lIndex > lCount){
		lIndex = 0;
	}
	Long lRet = lIndex;

	Sort[lRet].usBusy += Sort[lRet].usIncr;
	Long lNext = 0;
	if (lRet < (lCount - 1)) {
		lNext = lRet + 1;
	}
	if (Sort[lRet].usBusy > Sort[lNext].usBusy)
	{
		lIndex = lNext;
	}
	return lRet;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// tagRESULT_CHAIN : ������������������������齻����
INLINE tagRESULT_CHAIN::tagRESULT_CHAIN(void)
: lIndex(0)
{
}

INLINE tagRESULT_CHAIN::~tagRESULT_CHAIN(void)
{
}

INLINE void tagRESULT_CHAIN::Reset(void)
{
	lIndex = 0;
	Sort[0].Reset();
	Sort[1].Reset();
}

INLINE void tagRESULT_CHAIN::Swap(void)
{
	if (lIndex > 0){
		CAtomics::Decrement<Long>(&lIndex);
	}
	else{
		CAtomics::Increment<Long>(&lIndex);
	}
}

INLINE PSORT_RESULT tagRESULT_CHAIN::GetCur(void)
{
	return (Sort + lIndex);
}

INLINE PSORT_RESULT tagRESULT_CHAIN::GetNext(void)
{
	if (lIndex > 0){
		Sort[0].Reset();
		return (Sort + 0);
	}
	else{
		Sort[1].Reset();
		return (Sort + 1);
	}
}

#endif // __SERVER_SORT_INL__