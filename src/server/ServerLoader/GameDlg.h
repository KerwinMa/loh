//////////////////////////////////////////////////////////////////
//   The Legend of Heroes                                       //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Header File : GameDlg.h                                    //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ��Ϸ�������������Ի���                            //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef __GAME_DLG_H__
#define __GAME_DLG_H__

#pragma once

#include "CommonDlg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGameDlg
class CGameDlg : public CCommonDlg
{
public:
	CGameDlg(void);
	virtual ~CGameDlg(void);

	DECLARE_DLG_ID_WND_EXCHANGE( CGameDlg )
protected :
	virtual LRESULT OnInitDialog(void) OVERRIDE;

	LRESULT OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BEGIN_INLINE_MSG_MAP( CGameDlg )
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
	UInt                   m_uGameOnline; // ��Ϸ���������ӵ�ͼ&���ط�������ǰ��������
	UInt                   m_uGameAll;    // ��Ϸ���������ӵ�ͼ&���ط�����������
	UInt                   m_uGameBusy;   // ��Ϸ��������æ�̶�
	UInt                   m_uZoneCount;  // ���ӵĵ�ͼ����������
	UInt                   m_uZoneOnline; // ��ͼ���������ӿͻ��˵�ǰ��������
	UInt                   m_uZoneAll;    // ��ͼ���������ӿͻ���������
	UInt                   m_uGateCount;  // ���ӵ����ط���������
	UInt                   m_uGateOnline; // ���ط��������ӿͻ��˵�ǰ��������
	UInt                   m_uGateAll;    // ���ط��������ӿͻ���������
	CXStatic               m_ConnectCenter;
	CXStatic               m_ConnectGameDB;
	CXStatic               m_PingService;
	CXStatic               m_ListenZone;
	CXStatic               m_ListenGate;
	CXStatic               m_UDPService;
	CXStatic               m_ServerInfo;
	CXListView             m_ZoneSvr;
	CXListView             m_GateSvr;
};

#endif // __GAME_DLG_H__
