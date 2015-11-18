//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : CenterDlg.h                                  //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ���ķ������������Ի���                            //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __CENTER_DLG_H__
#define __CENTER_DLG_H__

#pragma once

#include "CommonDlg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CCenterDlg
class CCenterDlg : public CCommonDlg
{
public:
	CCenterDlg(void);
	virtual ~CCenterDlg(void);

	DECLARE_DLG_ID_WND_EXCHANGE( CCenterDlg )
protected :
	virtual LRESULT OnInitDialog(void) OVERRIDE;

	LRESULT OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BEGIN_INLINE_MSG_MAP( CCenterDlg )
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
	UInt         m_uCenterOnline; // ���߷���������(ѡ��+��½+��Ϸ)
	UInt         m_uCenterAll;    // �ܹ����ӹ��ķ���������
	UInt         m_uCenterBusy;   // ���ķ�������æ�̶�
	UInt         m_uSelectCount;  // ���ӵ�ѡ�����������
	UInt         m_uSelectOnline; // ѡ����������ӿͻ��˵�ǰ��������
	UInt         m_uSelectAll;    // ѡ����������ӿͻ���������
	UInt         m_uLoginCount;   // ���ӵĵ�½����������
	UInt         m_uLoginOnline;  // ��½���������ӿͻ��˵�ǰ��������
	UInt         m_uLoginAll;     // ��½���������ӿͻ���������
	UInt         m_uGameCount;    // ���ӵ���Ϸ����������
	UInt         m_uGameOnline;   // ��Ϸ���������ӵ�ͼ&���ط�������ǰ��������
	UInt         m_uGameAll;      // ��Ϸ���������ӵ�ͼ&���ط�����������
	CXStatic     m_ListenSelect;
	CXStatic     m_ListenLogin;
	CXStatic     m_ListenGame;
	CXStatic     m_ServerInfo;
	CXListView   m_SelectSvr;
	CXListView   m_LoginSvr;
	CXListView   m_GameSvr;
};

#endif // __CENTER_DLG_H__
