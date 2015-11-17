//////////////////////////////////////////////////////////////////
//   The Legend of Heros                                        //
//   Copyright(C) LH Studio. All Rights Reserved                //
//                                                              //
//   Source File : ServerLoader.cpp                             //
//   Author : jaredz@outlook.com                                //
//   Create : 2012-12-01     version 0.0.0.1                    //
//   Update :                                                   //
//   Detail : ���������                                         //
//                                                              //
//////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainFrame.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CServerLoader

IMPLEMENT_CLASS_LOADER( CServerLoader, XUI)
IMPLEMENT_CLASS_LOADER( CServerLoader, NET)
IMPLEMENT_CLASS_LOADER( CServerLoader, Center)
IMPLEMENT_CLASS_LOADER( CServerLoader, Select)
IMPLEMENT_CLASS_LOADER( CServerLoader, LoginDB)
IMPLEMENT_CLASS_LOADER( CServerLoader, Login)
IMPLEMENT_CLASS_LOADER( CServerLoader, GameDB)
IMPLEMENT_CLASS_LOADER( CServerLoader, Game)
IMPLEMENT_CLASS_LOADER( CServerLoader, Zone)
IMPLEMENT_CLASS_LOADER( CServerLoader, Gate)

CServerLoader::CServerLoader(void)
{
}

CServerLoader::~CServerLoader(void)
{
}

bool CServerLoader::Init(PCXStr pszName)
{
	m_Config.SetFilePath(&m_FilePath);
	m_Config.SetKeyValue(&m_KeyValue);
	if (m_Config.Init(pszName)) {
		return InitSystems();
	}
	return false;
}

void CServerLoader::Exit(void)
{
	StopServers();
	UnloadServers();

	ExitSystems();
	m_Config.Exit();
}

bool CServerLoader::LoadServer(Int nServer, CEventHandler* pUIHandler)
{
	if (m_Config.GetLoadServers() & nServer) {
		assert(pUIHandler != nullptr);
		CStringFix strPath;
		if (m_Config.GetServerPath(nServer, strPath) == false) {
			DEV_ERROR(TF("����������%X�������ļ��в������ļ�·��"), nServer);
			return false;
		}
		switch (nServer) {
		// TODO!!!����˳���ܸĶ��� ����ͬ���̹�����޷���ȷ������
		case CServerConfig::CFG_DEFAULT_CENTER:
			{
				if (m_CenterServer != nullptr) {
					DEV_ERROR(TF("�������ķ�����ģ��ʧ��, ���ķ�����ģ���Ѿ�����"));
					return false;
				}
				INIT_CLASS_LOADER(CServerLoader, Center, UUID_OF(CCommonServerSystem), *strPath);
				LOADER_CLASS_CREEATE(CServerLoader, Center, UUID_OF(ICenterServer), m_CenterServer.Cast<CComponent>());
				if ((m_CenterServer == nullptr) || (m_CenterServer->Init(*pUIHandler) != RET_OKAY)) {
					DEV_ERROR(TF("�������ķ�����ģ��ʧ��"));
					return false;
				}
				DEV_INFO(TF("�������ķ�����ģ�����"));
				pUIHandler->OnHandle(DATA_INDEX_CENTER, reinterpret_cast<uintptr_t>(m_CenterServer.Get()), 0);
			}
			break;
		case CServerConfig::CFG_DEFAULT_SELECT:
			{
				if (m_SelectServer != nullptr) {
					DEV_ERROR(TF("����ѡ�������ģ��ʧ��, ѡ�������ģ���Ѿ�����"));
					return false;
				}
				INIT_CLASS_LOADER(CServerLoader, Select, UUID_OF(CCommonServerSystem), *strPath);
				LOADER_CLASS_CREEATE(CServerLoader, Select, UUID_OF(ISelectServer), m_SelectServer.Cast<CComponent>());
				if ((m_SelectServer == nullptr) || (m_SelectServer->Init(*pUIHandler) != RET_OKAY)) {
					DEV_ERROR(TF("����ѡ�������ģ��ʧ��"));
					return false;
				}
				DEV_INFO(TF("����ѡ�������ģ�����"));
				pUIHandler->OnHandle(DATA_INDEX_SELECT, reinterpret_cast<uintptr_t>(m_SelectServer.Get()), 0);
			}
			break;
		case CServerConfig::CFG_DEFAULT_LOGINDB:
			{
				if (m_LoginDBServer != nullptr) {
					DEV_ERROR(TF("���ص�½DB������ģ��ʧ��, ��½DB������ģ���Ѿ�����"));
					return false;
				}
				INIT_CLASS_LOADER(CServerLoader, LoginDB, UUID_OF(CCommonServerSystem), *strPath);
				LOADER_CLASS_CREEATE(CServerLoader, LoginDB, UUID_OF(ILoginDBServer), m_LoginDBServer.Cast<CComponent>());
				if ((m_LoginDBServer == nullptr) || (m_LoginDBServer->Init(*pUIHandler) != RET_OKAY)) {
					DEV_ERROR(TF("���ص�½DB������ģ��ʧ��"));
					return false;
				}
				DEV_INFO(TF("���ص�½DB������ģ�����"));
				pUIHandler->OnHandle(DATA_INDEX_LOGINDB, reinterpret_cast<uintptr_t>(m_LoginDBServer.Get()), 0);
			}
			break;
		case CServerConfig::CFG_DEFAULT_LOGIN:
			{
				if (m_LoginServer != nullptr) {
					DEV_ERROR(TF("���ص�½������ģ��ʧ��, ��½������ģ���Ѿ�����"));
					return false;
				}
				INIT_CLASS_LOADER(CServerLoader, Login, UUID_OF(CCommonServerSystem), *strPath);
				LOADER_CLASS_CREEATE(CServerLoader, Login, UUID_OF(ILoginServer), m_LoginServer.Cast<CComponent>());
				if ((m_LoginServer == nullptr) || (m_LoginServer->Init(*pUIHandler) != RET_OKAY)) {
					DEV_ERROR(TF("���ص�½������ģ��ʧ��"));
					return false;
				}
				DEV_INFO(TF("���ص�½������ģ�����"));
				pUIHandler->OnHandle(DATA_INDEX_LOGIN, reinterpret_cast<uintptr_t>(m_LoginServer.Get()), 0);
			}
			break;
		case CServerConfig::CFG_DEFAULT_GAMEDB:
			{
				if (m_GameDBServer != nullptr) {
					DEV_ERROR(TF("������ϷDB������ģ��ʧ��, ��ϷDB������ģ���Ѿ�����"));
					return false;
				}
				INIT_CLASS_LOADER(CServerLoader, GameDB, UUID_OF(CCommonServerSystem), *strPath);
				LOADER_CLASS_CREEATE(CServerLoader, GameDB, UUID_OF(IGameDBServer), m_GameDBServer.Cast<CComponent>());
				if ((m_GameDBServer == nullptr) || (m_GameDBServer->Init(*pUIHandler) != RET_OKAY)) {
					DEV_ERROR(TF("������ϷDB������ģ��ʧ��"));
					return false;
				}
				DEV_INFO(TF("������ϷDB������ģ�����"));
				pUIHandler->OnHandle(DATA_INDEX_GAMEDB, reinterpret_cast<uintptr_t>(m_GameDBServer.Get()), 0);
			}
			break;
		case CServerConfig::CFG_DEFAULT_GAME:
			{
				if (m_GameServer != nullptr) {
					DEV_ERROR(TF("������Ϸ������ģ��ʧ��, ��Ϸ������ģ���Ѿ�����"));
					return false;
				}
				INIT_CLASS_LOADER(CServerLoader, Game, UUID_OF(CCommonServerSystem), *strPath);
				LOADER_CLASS_CREEATE(CServerLoader, Game, UUID_OF(IGameServer), m_GameServer.Cast<CComponent>());
				if ((m_GameServer == nullptr) || (m_GameServer->Init(*pUIHandler) != RET_OKAY)) {
					DEV_ERROR(TF("������Ϸ������ģ��ʧ��"));
					return false;
				}
				DEV_INFO(TF("������Ϸ������ģ�����"));
				pUIHandler->OnHandle(DATA_INDEX_GAME, reinterpret_cast<uintptr_t>(m_GameServer.Get()), 0);
			}
			break;
		case CServerConfig::CFG_DEFAULT_ZONE:
			{
				if (m_ZoneServer != nullptr) {
					DEV_ERROR(TF("���ص�ͼ������ģ��ʧ��, ��ͼ������ģ���Ѿ�����"));
					return false;
				}
				INIT_CLASS_LOADER(CServerLoader, Zone, UUID_OF(CCommonServerSystem), *strPath);
				LOADER_CLASS_CREEATE(CServerLoader, Zone, UUID_OF(IZoneServer), m_ZoneServer.Cast<CComponent>());
				if ((m_ZoneServer == nullptr) || (m_ZoneServer->Init(*pUIHandler) != RET_OKAY)) {
					DEV_ERROR(TF("���ص�ͼ������ģ��ʧ��"));
					return false;
				}
				DEV_INFO(TF("���ص�ͼ������ģ�����"));
				pUIHandler->OnHandle(DATA_INDEX_ZONE, reinterpret_cast<uintptr_t>(m_ZoneServer.Get()), 0);
			}
			break;
		case CServerConfig::CFG_DEFAULT_GATE:
			{
				if (m_GateServer != nullptr) {
					DEV_ERROR(TF("�������ط�����ģ��ʧ��, ���ط�����ģ���Ѿ�����"));
					return false;
				}
				INIT_CLASS_LOADER(CServerLoader, Gate, UUID_OF(CCommonServerSystem), *strPath);
				LOADER_CLASS_CREEATE(CServerLoader, Gate, UUID_OF(IGateServer), m_GateServer.Cast<CComponent>());
				if ((m_GateServer == nullptr) || (m_GateServer->Init(*pUIHandler) != RET_OKAY)) {
					DEV_ERROR(TF("�������ط�����ģ��ʧ��"));
					return false;
				}
				DEV_INFO(TF("�������ط�����ģ�����"));
				pUIHandler->OnHandle(DATA_INDEX_GATE, reinterpret_cast<uintptr_t>(m_GateServer.Get()), 0);
			}
			break;
		default:
			{
				DEV_WARN(TF("δ֪�ķ���������%X"), nServer);
				return false;
			}
		}
		return true;
	}
	else {
		DEV_WARN(TF("����������%X�������ļ��в����ڼ�������"), nServer);
	}
	return false;
}

void CServerLoader::UnloadServers(void)
{
	if (m_GateServer != nullptr) {
		m_GateServer->Exit();
		m_GateServer = nullptr;
		DEV_INFO(TF("ж�����ط�����ģ�����"));
	}
	if (m_ZoneServer != nullptr) {
		m_ZoneServer->Exit();
		m_ZoneServer = nullptr;
		DEV_INFO(TF("ж�ص�ͼ������ģ�����"));
	}
	if (m_GameServer != nullptr) {
		m_GameServer->Exit();
		m_GameServer = nullptr;
		DEV_INFO(TF("ж����Ϸ������ģ�����"));
	}
	if (m_GameDBServer != nullptr) {
		m_GameDBServer->Exit();
		m_GameDBServer = nullptr;
		DEV_INFO(TF("ж����ϷDB������ģ�����"));
	}
	if (m_LoginServer != nullptr) {
		m_LoginServer->Exit();
		m_LoginServer = nullptr;
		DEV_INFO(TF("ж�ص�½������ģ�����"));
	}
	if (m_LoginDBServer != nullptr) {
		m_LoginDBServer->Exit();
		m_LoginDBServer = nullptr;
		DEV_INFO(TF("ж�ص�½DB������ģ�����"));
	}
	if (m_SelectServer != nullptr) {
		m_SelectServer->Exit();
		m_SelectServer = nullptr;
		DEV_INFO(TF("ж��ѡ�������ģ�����"));
	}
	if (m_CenterServer != nullptr) {
		m_CenterServer->Exit();
		m_CenterServer = nullptr;
		DEV_INFO(TF("ж�����ķ�����ģ�����"));
	}
	EXIT_CLASS_LOADER( CServerLoader, Center );
	EXIT_CLASS_LOADER( CServerLoader, Select );
	EXIT_CLASS_LOADER( CServerLoader, LoginDB );
	EXIT_CLASS_LOADER( CServerLoader, Login );
	EXIT_CLASS_LOADER( CServerLoader, GameDB );
	EXIT_CLASS_LOADER( CServerLoader, Game );
	EXIT_CLASS_LOADER( CServerLoader, Zone );
	EXIT_CLASS_LOADER( CServerLoader, Gate );
}

bool CServerLoader::StartServers(void)
{
	if (m_NetworkPtr != nullptr) {
		CNETTraits::NET_ATTR attr;
		attr.nAttrs     = m_Config.GetNetworkAttr();
		attr.nThread    = m_Config.GetNetworkThread();
		attr.nAckTime   = m_Config.GetNetworkAck();
		attr.nTimeout   = m_Config.GetNetworkTimeout();
		attr.nMaxBuffer = m_Config.GetNetworkBuffer();
		attr.nMaxJumbo  = m_Config.GetNetworkJumbo();
		if (m_NetworkPtr->Init(attr) != RET_OKAY) {
			DEV_ERROR(TF("��ʼ������ģ��ʧ��!"));
			return false;
		}
	}
	else {
		DEV_ERROR(TF("��������ģ��ʧ��!"));
		return false;
	}

	if (m_CenterServer != nullptr) {
		if (m_CenterServer->Start() == false) {
			DEV_ERROR(TF("�������ķ�����������ʧ��!"));
			return false;
		}
	}
	if (m_SelectServer != nullptr) {
		if (m_SelectServer->Start() == false) {
			DEV_ERROR(TF("����ѡ�������������ʧ��!"));
			return false;
		}
	}
	if (m_LoginDBServer != nullptr) {
		if (m_LoginDBServer->Start() == false) {
			DEV_ERROR(TF("������½DB������������ʧ��!"));
			return false;
		}
	}
	if (m_LoginServer != nullptr) {
		if (m_LoginServer->Start() == false) {
			DEV_ERROR(TF("������½������������ʧ��!"));
			return false;
		}
	}
	if (m_GameDBServer != nullptr) {
		if (m_GameDBServer->Start() == false) {
			DEV_ERROR(TF("������ϷDB������������ʧ��!"));
			return false;
		}
	}
	if (m_GameServer != nullptr) {
		if (m_GameServer->Start() == false) {
			DEV_ERROR(TF("������Ϸ������������ʧ��!"));
			return false;
		}
	}
	if (m_ZoneServer != nullptr) {
		if (m_ZoneServer->Start() == false) {
			DEV_ERROR(TF("������ͼ������������ʧ��!"));
			return false;
		}
	}
	if (m_GateServer != nullptr) {
		if (m_GateServer->Start() == false) {
			DEV_ERROR(TF("�������ط�����������ʧ��!"));
			return false;
		}
	}
	return true;
}

void CServerLoader::StopServers(void)
{
	if (m_GateServer != nullptr) {
		m_GateServer->Stop();
	}
	if (m_ZoneServer != nullptr) {
		m_ZoneServer->Stop();
	}
	if (m_GameServer != nullptr) {
		m_GameServer->Stop();
	}
	if (m_GameDBServer != nullptr) {
		m_GameDBServer->Stop();
	}
	if (m_LoginServer != nullptr) {
		m_LoginServer->Stop();
	}
	if (m_LoginDBServer != nullptr) {
		m_LoginDBServer->Stop();
	}
	if (m_SelectServer != nullptr) {
		m_SelectServer->Stop();
	}
	if (m_CenterServer != nullptr) {
		m_CenterServer->Stop();
	}
	if (m_NetworkPtr != nullptr) {
		m_NetworkPtr->Exit();
	}
}

bool CServerLoader::InitSystems(void)
{
	INIT_CLASS_LOADER( CServerLoader, XUI, UUID_OF( CXUISystem ),     XUI_MODULE_NAME);
	INIT_CLASS_LOADER( CServerLoader, NET, UUID_OF( CNetworkSystem ), NETWORK_MODULE_NAME);

	LOADER_CLASS_CREEATE(CServerLoader, XUI, UUID_OF(CXUIManager), m_UIManagerPtr.Cast<CComponent>());
	if ((m_UIManagerPtr == nullptr) || (m_UIManagerPtr->Init() != RET_OKAY)) {
		DEV_ERROR(TF("��ʼ������ģ��ʧ��!"));
		return false;
	}
	IXWnd::InitCommonCtrls((ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES));

	LOADER_CLASS_CREEATE(CServerLoader, NET, UUID_OF(CNetwork), m_NetworkPtr.Cast<CComponent>());
	if (m_NetworkPtr == nullptr) {
		DEV_ERROR(TF("��������ģ��ʧ��!"));
		return false;
	}
	m_KeyValue.AddItem(CServerConfig::NetworkPtr, (void*)&m_NetworkPtr);

	DEV_INFO(TF("��ʼ������ģ�������ģ�����"));
	return true;
}

void CServerLoader::ExitSystems(void)
{
	if (m_NetworkPtr != nullptr) {
		//m_NetworkPtr->Exit();
		m_NetworkPtr = nullptr;
		m_KeyValue.Remove(CServerConfig::NetworkPtr);
	}
	if (m_UIManagerPtr != nullptr) {
		m_UIManagerPtr->Exit();
		m_UIManagerPtr = nullptr;
	}

	EXIT_CLASS_LOADER( CServerLoader, NET );
	EXIT_CLASS_LOADER( CServerLoader, XUI );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// _tWinMain
Int WINAPI _tWinMain(HINSTANCE,
                     HINSTANCE,
                     LPTSTR,
                     Int)
{
	CoreInit();

	CString strCfgFile;
	PCXStr pszCMD = ::GetCommandLine();
	PCXStr pszFile = CXChar::FindString(pszCMD, TF("-CfgFile:"));
	if (pszFile != nullptr) {
		pszFile += 9;
		size_t stRet = CXChar::FindOneIndexOf(pszFile, TF(" -"));
		strCfgFile.AppendBuffer(pszFile, stRet);

		pszFile = *strCfgFile;
	}

	if (GServerLoaderInst->Init(pszFile)) {
		CMainFrame MainFrame;
		CXUITraits::CREATE_PARAM cpm;

		CStringFix strTemp;
		strTemp.Load(IDS_CENTER_ID);
		cpm.pszName = *strTemp;
		if (GServerLoaderInst->m_UIManagerPtr->Create(MainFrame, cpm) != -1) {
			MainFrame.UpdateWindow();
			GServerLoaderInst->m_UIManagerPtr->MsgLoop();
		}
	}
	GServerLoaderInst->Exit();
	CoreExit();
	return 0;
}




