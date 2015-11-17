//////////////////////////////////////////////////////////////////
//   The Legend of Heros                                        //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : GameDBDlg.h                                  //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ��ϷDB�������������Ի���                          //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __GAMEDB_DLG_H__
#define __GAMEDB_DLG_H__

#pragma once

#include "CommonDlg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGameDBDlg
class CGameDBDlg : public CCommonDlg
{
public:
	CGameDBDlg(void);
	virtual ~CGameDBDlg(void);

	DECLARE_DLG_ID_WND_EXCHANGE( CGameDBDlg )
protected :
	virtual LRESULT OnInitDialog(void) OVERRIDE;

	LRESULT OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BEGIN_INLINE_MSG_MAP( CGameDBDlg )
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
	UInt                   m_uGameDBOnline; // ���߷���������(��Ϸ+����)
	UInt                   m_uGameDBAll;    // �ܹ����ӹ��ķ���������
	UInt                   m_uGameDBBusy;   // ��ϷDB��������æ�̶�
	UInt                   m_uGameCount;    // ���ӵ���Ϸ����������
	UInt                   m_uGameOnline;   // ��Ϸ���������ӵ�ͼ&���ط�������ǰ��������
	UInt                   m_uGameAll;      // ��Ϸ���������ӵ�ͼ&���ط�����������
	UInt                   m_uGateCount;    // ���ӵ����ط���������
	UInt                   m_uGateOnline;	// ���ط��������ӿͻ��˵�ǰ��������
	UInt                   m_uGateAll;      // ���ط��������ӿͻ���������
	CXStatic               m_ListenGame;
	CXStatic               m_ListenGate;
	CXStatic               m_ServerInfo;
	CXListView             m_GameSvr;
	CXListView             m_GateSvr;
};

#endif // __GAMEDB_DLG_H__
