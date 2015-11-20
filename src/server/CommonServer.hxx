//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Private Header File : CommonServer.hxx                     //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ��������׼�ӿ�                                     //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __COMMON_SERVER_HXX__
#define __COMMON_SERVER_HXX__

#pragma once

#include "CommonServer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CCommonServerSystem : CSubSystem
class CCommonServerSystem : public CSubSystem
{
public:
	// CComponent
	virtual UInt  Command(PCXStr pszCMD, uintptr_t utParam) OVERRIDE;
	virtual UInt  Update(void) OVERRIDE;
	// CSubSystem
	virtual UInt  Init(void) OVERRIDE;
	virtual void  Exit(void) OVERRIDE;

	virtual Int   GetComponentInfo(MAP_COMPONENT_INFO& Components) OVERRIDE;
	virtual bool  FindComponent(const CUUID& uuId) OVERRIDE;
	virtual bool  CreateComponent(const CUUID& uuId, CComponentPtr& CComponentPtrRef) OVERRIDE;
public:
	CCommonServerSystem(void);
	virtual ~CCommonServerSystem(void);
private:
	CCommonServerSystem(const CCommonServerSystem&);
	CCommonServerSystem& operator=(const CCommonServerSystem&);
};

INLINE CCommonServerSystem::CCommonServerSystem(void)
{
}

INLINE CCommonServerSystem::~CCommonServerSystem(void)
{
}

INLINE CCommonServerSystem::CCommonServerSystem(const CCommonServerSystem&)
{
}

INLINE CCommonServerSystem& CCommonServerSystem::operator=(const CCommonServerSystem&)
{
	return (*this);
}
// CComponent
INLINE UInt CCommonServerSystem::Command(PCXStr, uintptr_t)
{
	return 0;
}

INLINE UInt CCommonServerSystem::Update(void)
{
	return 0;
}

INLINE UInt CCommonServerSystem::Init(void)
{
	return (UInt)RET_OKAY;
}

INLINE void CCommonServerSystem::Exit(void)
{
}

#endif // __COMMON_SERVER_HXX__
