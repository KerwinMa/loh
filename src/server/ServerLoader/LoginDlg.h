//////////////////////////////////////////////////////////////////
//   The Legend of Heros                                        //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : LoginDlg.h                                   //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ��½�������������Ի���                            //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __LOGIN_DLG_H__
#define __LOGIN_DLG_H__

#pragma once

#include "CommonDlg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLoginDlg
class CLoginDlg : public CCommonDlg
{
public:
	CLoginDlg(void);
	virtual ~CLoginDlg(void);

	DECLARE_DLG_ID_WND_EXCHANGE( CLoginDlg )
protected :
	virtual LRESULT OnInitDialog(void);

	LRESULT OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BEGIN_INLINE_MSG_MAP( CLoginDlg )
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
	UInt                   m_uLoginOnline; // ��½���������ӿͻ��˵�ǰ��������
	UInt                   m_uLoginAll;    // ��½���������ӿͻ���������
	UInt                   m_uLoginBusy;   // ��½��������æ�̶�
	UInt                   m_uGameCount;   // ���ӵ���Ϸ����������
	UInt                   m_uGameOnline;  // ��Ϸ���������ӵ�ͼ&���ط�������ǰ��������
	UInt                   m_uGameAll;     // ��Ϸ���������ӵ�ͼ&���ط�����������
	CXStatic               m_ConnectCenter;
	CXStatic               m_ConnectLoginDB;
	CXStatic               m_TCPService;
	CXStatic               m_UDPService;
	CXStatic               m_ServerInfo;
	CXListView             m_GameSvr;
};

#endif // __LOGIN_DLG_H__
