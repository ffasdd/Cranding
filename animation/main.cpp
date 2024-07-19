#include "stdafx.h"
#include "main.h"
#include "GameFramework.h"
#include "Network.h"
#include "Session.h"

#define MAX_LOADSTRING 100

HINSTANCE						ghAppInstance;
TCHAR							szTitle[MAX_LOADSTRING];
TCHAR							szWindowClass[MAX_LOADSTRING];

CGameFramework					gGameFramework;
Network							gNetwork;

unordered_map<int, Session> g_clients;
unordered_map<int, Session> g_monsters;
unordered_map<int, Session> g_ice_monsters;
unordered_map<int, Session> g_fire_monsters;
unordered_map<int, Session> g_nature_monsters;

Session g_IceBossMonster;
Session g_FireBossMonster;
Session g_NatureBossMonster;


concurrency::concurrent_queue<SENDTYPE> g_sendqueue;
HANDLE loginevent = CreateEvent(NULL, FALSE, FALSE, NULL);
HANDLE startevent = CreateEvent(NULL, FALSE, FALSE, NULL);

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	::LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	::LoadString(hInstance, IDC_CRANDING, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);


	while (!gNetwork.ReadytoConnect());


	//// 정보를 여기서?  send client infO? 로그인 정보를 보낼까 ? 
	gNetwork.StartServer();

	// 로그인 완료 
	if (!InitInstance(hInstance, nCmdShow)) return(FALSE);

	hAccelTable = ::LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CRANDING));


	while (1)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//cout << msg.message << endl;
			if (msg.message == WM_QUIT)
				break;
			if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}

		}
		else
		{

			if (gNetwork.IngameStart == true || gNetwork.SpaceshipScene == true)
			{
				gGameFramework.ReleaseObjects();
				gGameFramework.BuildObjects(SCENEKIND::SPACESHIP);
				gNetwork.IngameStart = false;
				gNetwork.SpaceshipScene = false;
			}

			if (gGameFramework.m_pPlayer != NULL && gGameFramework.m_pScene != NULL)
			{
				if (gNetwork.gamestart)
				{
					for (int i = 0; i < g_clients.size(); ++i)
					{
						if (gNetwork.stage_num != g_clients[i].scene_num)
						{
							gGameFramework.myFunc_SetBlind(i, g_clients[i].getId(), false);
							continue;
						}else 
							gGameFramework.myFunc_SetBlind(i, g_clients[i].getId(), true);
							
						gGameFramework.myFunc_SetPosition(i, g_clients[i].getId(), g_clients[i].getPos());
						gGameFramework.myFunc_SetLookRightUp(i, g_clients[i].getId(), g_clients[i].getLook(), g_clients[i].getUp(), g_clients[i].getRight());
						gGameFramework.myFunc_SetAnimation(i, g_clients[i].getId(), g_clients[i].getprevAnimation(), g_clients[i].getAnimation());
						gGameFramework.myFunc_SetAttack(i, g_clients[i].getId(), g_clients[i].getAttack());
					}

					switch (gGameFramework.sceneManager.GetCurrentScene())
					{
					case SCENEKIND::SPACESHIP:
					{
						// spaceship map // 0  ~ 9 fire  10 ~ 19 ice 20  ~ 29 nature

						for (int i = 0; i < g_monsters.size(); ++i)
						{

							gGameFramework.myFunc_SetMonPosition(i, g_monsters[i].getPos());
							gGameFramework.myFunc_SetMonLookRightUp(i, g_monsters[i].getLook(), g_monsters[i].getUp(), g_monsters[i].getRight());
							gGameFramework.myFunc_SetMonAnimation(i, g_monsters[i].getNpcAttacked(), g_monsters[i].getNpcAttack());
						}
						break;
					}
					case SCENEKIND::ICE:
					{
						// ice map
						for (int i = 0; i < g_ice_monsters.size(); ++i)
						{
							gGameFramework.myFunc_SetMonPosition(i, g_ice_monsters[i].getPos());
							gGameFramework.myFunc_SetMonLookRightUp(i, g_ice_monsters[i].getLook(), g_ice_monsters[i].getUp(), g_ice_monsters[i].getRight());
						}

						gGameFramework.myFunc_SetBossMonPosition(g_IceBossMonster.getPos());
						gGameFramework.myFunc_SetBossMonLookRightUp(g_IceBossMonster.getLook(), g_IceBossMonster.getUp(), g_IceBossMonster.getRight());
						break;
					}
					case SCENEKIND::FIRE:
					{
						// fire map
						for (int i = 0; i < g_fire_monsters.size(); ++i)
						{
							gGameFramework.myFunc_SetMonPosition(i, g_fire_monsters[i].getPos());
							gGameFramework.myFunc_SetMonLookRightUp(i, g_fire_monsters[i].getLook(), g_fire_monsters[i].getUp(), g_fire_monsters[i].getRight());
						}
						gGameFramework.myFunc_SetBossMonPosition(g_FireBossMonster.getPos());
						gGameFramework.myFunc_SetBossMonLookRightUp(g_FireBossMonster.getLook(), g_FireBossMonster.getUp(), g_FireBossMonster.getRight());
						break;
					}
					case SCENEKIND::NATURE:
					{
						// fire map
						for (int i = 0; i < g_nature_monsters.size(); ++i)
						{
							gGameFramework.myFunc_SetMonPosition(i, g_nature_monsters[i].getPos());
							gGameFramework.myFunc_SetMonLookRightUp(i, g_nature_monsters[i].getLook(), g_nature_monsters[i].getUp(), g_nature_monsters[i].getRight());
						}
						gGameFramework.myFunc_SetBossMonPosition(g_NatureBossMonster.getPos());
						gGameFramework.myFunc_SetBossMonLookRightUp(g_NatureBossMonster.getLook(), g_NatureBossMonster.getUp(), g_NatureBossMonster.getRight());
						break;
					}

					default:
						break;
					}


				}
			}

			gGameFramework.FrameAdvance();
		}
	}



	gGameFramework.OnDestroy();

	return((int)msg.wParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CRANDING));
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;//MAKEINTRESOURCE(IDC_CRANDING);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = ::LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return ::RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	ghAppInstance = hInstance;

	RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&rc, dwStyle, FALSE);
	HWND hMainWnd = CreateWindow(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

	if (!hMainWnd) return(FALSE);

	gGameFramework.OnCreate(hInstance, hMainWnd);

	::ShowWindow(hMainWnd, nCmdShow);
	::UpdateWindow(hMainWnd);

	//imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	// 플랫폼/렌더러 바인딩 설정
	ImGui_ImplWin32_Init(hMainWnd);
	ImGui_ImplDX12_Init(gGameFramework.GetDevice(), 3,
		DXGI_FORMAT_R8G8B8A8_UNORM, gGameFramework.m_pd3dImGuiDescriptorHeap,
		gGameFramework.m_pd3dImGuiDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		gGameFramework.m_pd3dImGuiDescriptorHeap->GetGPUDescriptorHandleForHeapStart());


	return(TRUE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
	case WM_KEYUP:
		gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDM_ABOUT:
			::DialogBox(ghAppInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			::DestroyWindow(hWnd);
			break;
		default:
			return(::DefWindowProc(hWnd, message, wParam, lParam));
		}
		break;
	case WM_PAINT:
		hdc = ::BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	default:
		return(::DefWindowProc(hWnd, message, wParam, lParam));
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return((INT_PTR)TRUE);
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			::EndDialog(hDlg, LOWORD(wParam));
			return((INT_PTR)TRUE);
		}
		break;
	}
	return((INT_PTR)FALSE);
}

