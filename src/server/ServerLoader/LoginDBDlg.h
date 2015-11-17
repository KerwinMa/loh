//////////////////////////////////////////////////////////////////
//   The Legend of Heros                                        //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : LoginDBDlg.h                                 //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ��½DB�������������Ի���                          //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __LOGINDB_DLG_H__
#define __LOGINDB_DLG_H__

#pragma once

#include "CommonDlg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLoginDBDlg
class CLoginDBDlg : public CCommonDlg
{
public:
	CLoginDBDlg(void);
	virtual ~CLoginDBDlg(void);

	DECLARE_DLG_ID_WND_EXCHANGE( CLoginDBDlg )
protected :
	virtual LRESULT OnInitDialog(void) OVERRIDE;

	LRESULT OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BEGIN_INLINE_MSG_MAP( CLoginDBDlg )
	// {{ --
		MSG_HANDLER(WM_SIZE, OnSize)
	// --}}
	END_CHAIN_MSG_MAP( CXDlg )
private:
	virtual void OnLive(bool bStart) OVERRIDE;
	virtual void OnLink(Int nServerIndex, uintptr_t utParam) OVERRIDE;
	virtual void OnUpdate(Int nServerIndex, uintptr_t utParam) OVERRIDE;
	virtual void OnSync(Int nServerIndex, uintptr_t utParam) OVERRIDE;
	virtual void OnSync(Int nServerIndex, CStream& Stream) OVERRIDE;
	virtual void OnUnlink(Int nServerIndex, uintptr_t utParam) OVERRIDE;

	virtual void UpdateServerInfo(void) OVERRIDE;

	virtual CXListView* GetListView(Int nServerIndex) OVERRIDE;
private:
	UInt                   m_uLoginDBOnline; // ��½DB���������ӵ�½��������ǰ��������
	UInt                   m_uLoginDBAll;    // ��½DB���������ӵ�½������������
	UInt                   m_uLoginDBBusy;   // ��½DB��������æ�̶�
	CXStatic               m_ListenLogin;
	CXStatic               m_ServerInfo;
	CXListView             m_LoginSvr;
};

#endif // __LOGINDB_DLG_H__
