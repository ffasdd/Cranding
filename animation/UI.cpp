#include "stdafx.h"
#include "UI.h"
#include "GameFramework.h"
#include "Scenemanager.h"

using namespace std;

UILayer* UILayer::s_instance = nullptr;

extern CGameFramework					gGameFramework;

UILayer::UILayer(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight)
{
    m_fWidth = static_cast<float>(nWidth);
    m_fHeight = static_cast<float>(nHeight);
    m_nRenderTargets = nFrames;
    m_ppd3d11WrappedRenderTargets = new ID3D11Resource * [nFrames];
    m_ppd2dRenderTargets = new ID2D1Bitmap1 * [nFrames];

    // m_nTextBlocks = nTextBlocks;
    // m_pTextBlocks = new TextBlock[nTextBlocks];

    InitializeDevice(pd3dDevice, pd3dCommandQueue, ppd3dRenderTargets);
}

UILayer::UILayer()
{
}

UILayer::~UILayer()
{
}

UILayer* UILayer::Create(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight)
{
    UILayer* pInstance = new UILayer();
    s_instance = pInstance;
    cout << "ui start" << endl;
    if (FAILED(pInstance->Initialize(nFrames, nTextBlocks, pd3dDevice, pd3dCommandQueue, ppd3dRenderTargets, nWidth, nHeight)))
    {
        SafeDelete(pInstance);
        return nullptr;
    }
    cout << "ui end" << endl;

    return pInstance;
}

HRESULT UILayer::Initialize(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight)
{
    // 클릭 처리 필요한 ui
    // game start
    UILayer::GetInstance()->AddUIRect(SCENEKIND::LOGIN, m_GameStart, [this]()-> bool {
        gGameFramework.SceneNum = 1;
        gGameFramework.isSceneChange = true;
        gGameFramework.isready = false;
       
        cout << "게임 시작" << endl;
        return true;
        });
    UILayer::GetInstance()->AddUIRect(SCENEKIND::LOGIN, m_usernameRect, [this]()-> bool {
       gGameFramework.m_pScene->m_isUsernameInput = true;  // 입력 모드 (true: 사용자 이름 입력, false: 비밀번호 입력)
       cout << "m_usernameRect" << endl;
        
        return true;
        });
    UILayer::GetInstance()->AddUIRect(SCENEKIND::LOGIN, m_passwordRect, [this]()-> bool {
        gGameFramework.m_pScene->m_isUsernameInput = false;  // 입력 모드 (true: 사용자 이름 입력, false: 비밀번호 입력)
        cout << "m_usernameRect" << endl;

        return true;
        });
  
    m_fWidth = static_cast<float>(nWidth);
    m_fHeight = static_cast<float>(nHeight);
    m_nRenderTargets = nFrames;
    m_ppd3d11WrappedRenderTargets = new ID3D11Resource * [nFrames];
    m_ppd2dRenderTargets = new ID2D1Bitmap1 * [nFrames];

    m_nTextBlocks = nTextBlocks;
    m_pTextBlocks = new TextBlock[nTextBlocks];

    //  login scene
    int loginUI_num = 2;
    WCHAR** pMent = new WCHAR * [loginUI_num];
    for (int i = 0; i < loginUI_num; ++i)
    {
        pMent[i] = new WCHAR[256];
    }

    wcscpy_s(pMent[0], 256, L"Cranding");
    wcscpy_s(pMent[1], 256, L"게임 시작");
    for (int i = 0; i < loginUI_num; ++i) {
        m_vecLoginSceneMenu.push_back(pMent[i]);
    }

    // Lobby Scene
    int lobbyUI_num = 2;
    WCHAR** plobbytext = new WCHAR * [lobbyUI_num];
    for (int i = 0; i < lobbyUI_num; ++i)
    {
        plobbytext[i] = new WCHAR[256];
    }

    wcscpy_s(plobbytext[0], 256, L"게임 시작을 위해 '2'를 눌러주세요");
    wcscpy_s(plobbytext[1], 256, L"ready");
    for (int i = 0; i < lobbyUI_num; ++i) {
        m_vecLobbyScene.push_back(plobbytext[i]);
    }

    // Result Scene
    int Result_num = 2;
    WCHAR** presulttext = new WCHAR * [Result_num];
    for (int i = 0; i < Result_num; ++i)
    {
        presulttext[i] = new WCHAR[256];
    }

    wcscpy_s(presulttext[0], 256, L"VICTORY");
    wcscpy_s(presulttext[1], 256, L"DEFEAT");
    for (int i = 0; i < Result_num; ++i) {
        m_vecResultScene.push_back(presulttext[i]);
    }

    //  ingame scene
    int ingameUI_num = 4;
    WCHAR** pMap = new WCHAR * [ingameUI_num];
    for (int i = 0; i < ingameUI_num; ++i)
    {
        pMap[i] = new WCHAR[256];
    }

    wcscpy_s(pMap[0], 256, L"F키로 ice map 이동 !");
    wcscpy_s(pMap[1], 256, L"F키로 fire map 이동 !");
    wcscpy_s(pMap[2], 256, L"F키로 grass map 이동 !");
    wcscpy_s(pMap[3], 256, L"F키로 spaceship map 이동 !");
    for (int i = 0; i < ingameUI_num; ++i) {
        m_vecIngameScene.push_back(pMap[i]);
    }

    //  ingame item
    int Item_num = 4;
    WCHAR** pItem = new WCHAR * [Item_num];
    for (int i = 0; i < Item_num; ++i)
    {
        pItem[i] = new WCHAR[256];
    }

    wcscpy_s(pItem[0], 256, L"Ice");
    wcscpy_s(pItem[1], 256, L"Fire");
    wcscpy_s(pItem[2], 256, L"Nature");
    wcscpy_s(pItem[3], 256, L"SpaceShip HP");
    for (int i = 0; i < Item_num; ++i) {
        m_vecItem.push_back(pItem[i]);
    }

    InitializeDevice(pd3dDevice, pd3dCommandQueue, ppd3dRenderTargets);

    // brush
    m_brushes[BRUSH_COLOR::WHITE] = CreateBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f));
    m_brushes[BRUSH_COLOR::PINK] = CreateBrush(D2D1::ColorF(1.0f, 0.8f, 0.8f, 1.0f));
    m_brushes[BRUSH_COLOR::BLACK] = CreateBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
    m_brushes[BRUSH_COLOR::LIGHTBLACK] = CreateBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.8f));
    m_brushes[BRUSH_COLOR::ABLACK] = CreateBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.3f));
    m_brushes[BRUSH_COLOR::RED] = CreateBrush(D2D1::ColorF(1.0f, 0.14f, 0.14f, 1.0f));

    // text format
    m_textFormats[TEXT_SIZE::SIZE_15] = CreateTextFormat(L"맑은 고딕", 15.0f * 3.35f);
    m_textFormats[TEXT_SIZE::SIZE_18] = CreateTextFormat(L"맑은 고딕", 18.0f * 3.35f);
    m_textFormats[TEXT_SIZE::SIZE_25] = CreateTextFormat(L"맑은 고딕", 25.0f * 3.35f);
    m_textFormats[TEXT_SIZE::SIZE_35] = CreateTextFormat(L"맑은 고딕", 35.0f * 3.35f);
    m_textFormats[TEXT_SIZE::SIZE_40] = CreateTextFormat(L"맑은 고딕", 40.0f * 3.35f);
    m_textFormats[TEXT_SIZE::SIZE_50] = CreateTextFormat(L"맑은 고딕", 50.0f * 3.35f);
    m_textFormats[TEXT_SIZE::SIZE_60] = CreateTextFormat(L"맑은 고딕", 80.0f * 3.35f);

    return NOERROR;
}

void UILayer::ProcessMouseClick(SCENEKIND scenekind, POINT clickPos)
{
    int index = static_cast<int>(scenekind);

    for (UIRect& rect : m_uiRects[index]) {
        if (rect.ClickCollide(clickPos))
            break;
    }
}

void UILayer::InitializeDevice(ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets)
{
    UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D2D1_FACTORY_OPTIONS d2dFactoryOptions = { };

#if defined(_DEBUG) || defined(DBG)
    d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
    d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    ID3D11Device* pd3d11Device = NULL;
    ID3D12CommandQueue* ppd3dCommandQueues[] = { pd3dCommandQueue };
    ::D3D11On12CreateDevice(pd3dDevice, d3d11DeviceFlags, nullptr, 0, reinterpret_cast<IUnknown**>(ppd3dCommandQueues), _countof(ppd3dCommandQueues), 0, (ID3D11Device**)&pd3d11Device, (ID3D11DeviceContext**)&m_pd3d11DeviceContext, nullptr);

    pd3d11Device->QueryInterface(__uuidof(ID3D11On12Device), (void**)&m_pd3d11On12Device);
    pd3d11Device->Release();

#if defined(_DEBUG) || defined(DBG)
    ID3D12InfoQueue* pd3dInfoQueue;
    if (SUCCEEDED(pd3dDevice->QueryInterface(IID_PPV_ARGS(&pd3dInfoQueue))))
    {
        D3D12_MESSAGE_SEVERITY pd3dSeverities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_MESSAGE_ID pd3dDenyIds[] = { D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE };

        D3D12_INFO_QUEUE_FILTER d3dInforQueueFilter = { };
        d3dInforQueueFilter.DenyList.NumSeverities = _countof(pd3dSeverities);
        d3dInforQueueFilter.DenyList.pSeverityList = pd3dSeverities;
        d3dInforQueueFilter.DenyList.NumIDs = _countof(pd3dDenyIds);
        d3dInforQueueFilter.DenyList.pIDList = pd3dDenyIds;

        pd3dInfoQueue->PushStorageFilter(&d3dInforQueueFilter);
    }
    pd3dInfoQueue->Release();
#endif

    IDXGIDevice* pdxgiDevice = NULL;
    m_pd3d11On12Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pdxgiDevice);

    ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, (void**)&m_pd2dFactory);
    HRESULT hResult = m_pd2dFactory->CreateDevice(pdxgiDevice, (ID2D1Device2**)&m_pd2dDevice);
    m_pd2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, (ID2D1DeviceContext2**)&m_pd2dDeviceContext);

    m_pd2dDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

    ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_pd2dWriteFactory);
    pdxgiDevice->Release();

    D2D1_BITMAP_PROPERTIES1 d2dBitmapProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));

    for (UINT i = 0; i < m_nRenderTargets; i++)
    {
        D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
        m_pd3d11On12Device->CreateWrappedResource(ppd3dRenderTargets[i], &d3d11Flags, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(&m_ppd3d11WrappedRenderTargets[i]));
        IDXGISurface* pdxgiSurface = NULL;
        m_ppd3d11WrappedRenderTargets[i]->QueryInterface(__uuidof(IDXGISurface), (void**)&pdxgiSurface);
        m_pd2dDeviceContext->CreateBitmapFromDxgiSurface(pdxgiSurface, &d2dBitmapProperties, &m_ppd2dRenderTargets[i]);
        pdxgiSurface->Release();
    }
}
float ConvertHPToPercentage(float hp) {
    // 원래 값의 범위
    float min_in = 0.0f;
    float max_in = 2000.0f;

    // 변환할 범위
    float min_out = 0.0f;
    float max_out = 100.0f;

    // 입력값 범위 체크
    hp = max(min_in, min(hp, max_in));

    // 선형 보간 공식 적용
    float normalizedValue = ((hp - min_in) * (max_out - min_out)) / (max_in - min_in) + min_out;

    return normalizedValue;
}

void UILayer::SetHP()
{
    if (gNetwork.my_id != -1) {
    m_playerHP = g_clients[gNetwork.my_id].getHp();
    }

    float spaceshipHP = gGameFramework.spaceshipHP; // 0 ~ 2000 사이의 값
    m_spaceshipHP = ConvertHPToPercentage(spaceshipHP); // 0 ~ 100 사이의 값

    UpdateHPBar();
}
void UILayer::UpdateHPBar()
{
    m_HPBar = D2D1::RectF(1300.0f, 930.0f, 1300 + m_playerHP * 6.0f, FRAME_BUFFER_HEIGHT - 50.0f);
    m_spaceshipHPBar = D2D1::RectF(10.0f, 0.0f, 10 + m_spaceshipHP * 6.0f, 40.0f);
}
ID2D1SolidColorBrush* UILayer::CreateBrush(D2D1::ColorF d2dColor)
{
    ID2D1SolidColorBrush* pd2dDefaultTextBrush = NULL;
    m_pd2dDeviceContext->CreateSolidColorBrush(d2dColor, &pd2dDefaultTextBrush);

    return(pd2dDefaultTextBrush);
}

IDWriteTextFormat* UILayer::CreateTextFormat(WCHAR* pszFontName, float fFontSize)
{
    IDWriteTextFormat* pdwDefaultTextFormat = NULL;
    m_pd2dWriteFactory->CreateTextFormat(L"맑은 고딕", nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_EXPANDED, fFontSize, L"en-us", &pdwDefaultTextFormat);

    pdwDefaultTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING); // 왼쪽 정렬
    pdwDefaultTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    //m_pd2dWriteFactory->CreateTextFormat(L"Arial", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fSmallFontSize, L"en-us", &m_pdwDefaultTextFormat);

    return(pdwDefaultTextFormat);
}

void UILayer::UpdateTextOutputs(UINT nIndex, WCHAR* pstrUIText, D2D1_RECT_F* pd2dLayoutRect, IDWriteTextFormat* pdwFormat, ID2D1SolidColorBrush* pd2dTextBrush)
{
    if (pstrUIText) wcscpy_s(m_pTextBlocks[nIndex].m_pstrText, 256, pstrUIText);
    if (pd2dLayoutRect) m_pTextBlocks[nIndex].m_d2dLayoutRect = *pd2dLayoutRect;
    if (pdwFormat) m_pTextBlocks[nIndex].m_pdwFormat = pdwFormat;
    if (pd2dTextBrush) m_pTextBlocks[nIndex].m_pd2dTextBrush = pd2dTextBrush;
}
std::wstring StringToWString(const std::string& str) {
    int len;
    int slength = (int)str.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, 0, 0);
    std::wstring buf(len, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, &buf[0], len);
    return buf;
}
void UILayer::Render(UINT nFrame, SCENEKIND scenekind, bool isready, int curDay, int curMinute, int curSecond)
{
    ID3D11Resource* ppResources[] = { m_ppd3d11WrappedRenderTargets[nFrame] };

    m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[nFrame]);
    m_pd3d11On12Device->AcquireWrappedResources(ppResources, _countof(ppResources));
   

    switch (scenekind)
    {
    case SCENEKIND::NONE:
        break;

    case SCENEKIND::LOGIN:
        // login
        m_pd2dDeviceContext->BeginDraw();
        m_pd2dDeviceContext->DrawText(m_vecLoginSceneMenu[0], (UINT)wcslen(m_vecLoginSceneMenu[0]), m_textFormats[TEXT_SIZE::SIZE_60], m_Title, m_brushes[BRUSH_COLOR::BLACK]);
        m_pd2dDeviceContext->DrawText(m_vecLoginSceneMenu[1], (UINT)wcslen(m_vecLoginSceneMenu[1]), m_textFormats[TEXT_SIZE::SIZE_40], m_GameStart, m_brushes[BRUSH_COLOR::BLACK]);
        // 변환 함수 호출

        wideUsername = StringToWString(gGameFramework.m_pScene->getname());
        widePassword = StringToWString(gGameFramework.m_pScene->getpassword());

        // wcsncpy_s 사용
        wcsncpy_s(username, wideUsername.c_str(), _TRUNCATE);
        wcsncpy_s(password, widePassword.c_str(), _TRUNCATE);

        m_pd2dDeviceContext->FillRectangle(m_usernameRect, m_brushes[BRUSH_COLOR::ABLACK]);
        m_pd2dDeviceContext->DrawText(username, (UINT)wcslen(username), m_textFormats[TEXT_SIZE::SIZE_35], m_usernameRect, m_brushes[BRUSH_COLOR::LIGHTBLACK]);

        m_pd2dDeviceContext->FillRectangle(m_passwordRect, m_brushes[BRUSH_COLOR::ABLACK]);
        m_pd2dDeviceContext->DrawText(password, (UINT)wcslen(password), m_textFormats[TEXT_SIZE::SIZE_35], m_passwordRect, m_brushes[BRUSH_COLOR::LIGHTBLACK]);

        m_pd2dDeviceContext->EndDraw();
        break;
    case SCENEKIND::LOBBY:
        // lobby
        m_pd2dDeviceContext->BeginDraw();
        m_pd2dDeviceContext->DrawText(m_vecLobbyScene[0], (UINT)wcslen(m_vecLobbyScene[0]), m_textFormats[TEXT_SIZE::SIZE_25], m_ReadyMent, m_brushes[BRUSH_COLOR::WHITE]);
        if(isready)
            m_pd2dDeviceContext->DrawText(m_vecLobbyScene[1], (UINT)wcslen(m_vecLobbyScene[1]), m_textFormats[TEXT_SIZE::SIZE_40], m_Ready, m_brushes[BRUSH_COLOR::WHITE]);
        m_pd2dDeviceContext->EndDraw();
        break;
    case SCENEKIND::VICTORY:
        m_pd2dDeviceContext->BeginDraw();
        m_pd2dDeviceContext->DrawText(m_vecResultScene[0], (UINT)wcslen(m_vecResultScene[0]), m_textFormats[TEXT_SIZE::SIZE_60], m_Title, m_brushes[BRUSH_COLOR::WHITE]);
        m_pd2dDeviceContext->EndDraw();
        break;
    case SCENEKIND::DEFEAT:
        m_pd2dDeviceContext->BeginDraw();
        m_pd2dDeviceContext->DrawText(m_vecResultScene[1], (UINT)wcslen(m_vecResultScene[1]), m_textFormats[TEXT_SIZE::SIZE_60], m_Title, m_brushes[BRUSH_COLOR::WHITE]);
        m_pd2dDeviceContext->EndDraw();
        break;

    default:
        // ingame 
        m_pd2dDeviceContext->BeginDraw();
        
        // time
        WCHAR pstrOutputText[256];
        std::wstring DnN[2] = { L"Day", L"Night" };
        int timenum = gGameFramework.Night;
        if(gNetwork.my_id != -1)
        swprintf_s(pstrOutputText, 256, L"Day: %d  Time:%02d:%02d %s", gNetwork.Day, curMinute, curSecond, DnN[timenum].c_str());
        else
        swprintf_s(pstrOutputText, 256, L"Day: %d  Time:%02d:%02d %s", 0, curMinute, curSecond, DnN[timenum].c_str());
        m_pd2dDeviceContext->DrawText(pstrOutputText, (UINT)wcslen(pstrOutputText), m_textFormats[TEXT_SIZE::SIZE_18], m_Timer, m_brushes[BRUSH_COLOR::PINK]);

        // hp
        SetHP();
        m_pd2dDeviceContext->FillRectangle(m_Bar, m_brushes[BRUSH_COLOR::BLACK]);
        m_pd2dDeviceContext->FillRectangle(m_HPBar, m_brushes[BRUSH_COLOR::RED]);

        // status
        WCHAR elementText[256];
        if(gNetwork.my_id != -1)
            swprintf_s(elementText, 256, L"HP: %d", g_clients[gNetwork.my_id].getHp());
        m_pd2dDeviceContext->DrawText(elementText, (UINT)wcslen(elementText), m_textFormats[TEXT_SIZE::SIZE_15], iceRect, m_brushes[BRUSH_COLOR::BLACK]);

        if (gNetwork.my_id != -1)
            swprintf_s(elementText, 256, L"Attack Power: %d", g_clients[gNetwork.my_id].getAttackPower());
        m_pd2dDeviceContext->DrawText(elementText, (UINT)wcslen(elementText), m_textFormats[TEXT_SIZE::SIZE_15], fireRect, m_brushes[BRUSH_COLOR::BLACK]);

        if (gNetwork.my_id != -1)
            swprintf_s(elementText, 256, L"Speed: %d", g_clients[gNetwork.my_id].getSpeed());
        m_pd2dDeviceContext->DrawText(elementText, (UINT)wcslen(elementText), m_textFormats[TEXT_SIZE::SIZE_15], natureRect, m_brushes[BRUSH_COLOR::BLACK]);

        // Map 이동 메시지
        std::wstring mapMessage;
        if (gGameFramework.m_pPlayer->isIceMap) {
            mapMessage = m_vecIngameScene[0];
        }
        else if (gGameFramework.m_pPlayer->isFireMap) {
            mapMessage = m_vecIngameScene[1];
        }
        else if (gGameFramework.m_pPlayer->isNatureMap) {
            mapMessage = m_vecIngameScene[2];
        }
        if (!mapMessage.empty()) {
            m_pd2dDeviceContext->DrawText(mapMessage.c_str(), (UINT)wcslen(mapMessage.c_str()), m_textFormats[TEXT_SIZE::SIZE_15], m_Map, m_brushes[BRUSH_COLOR::PINK]);
        }

        // 우주선 hp
        m_pd2dDeviceContext->FillRectangle(m_spaceshipBar, m_brushes[BRUSH_COLOR::BLACK]);
        m_pd2dDeviceContext->FillRectangle(m_spaceshipHPBar, m_brushes[BRUSH_COLOR::RED]);
        m_pd2dDeviceContext->DrawText(m_vecItem[3], (UINT)wcslen(m_vecItem[3]), m_textFormats[TEXT_SIZE::SIZE_15], m_spaceshipBar, m_brushes[BRUSH_COLOR::BLACK]);

        // item UI        
        m_pd2dDeviceContext->DrawText(m_vecItem[0], (UINT)wcslen(m_vecItem[0]), m_textFormats[TEXT_SIZE::SIZE_15], m_IceItemBar, m_brushes[BRUSH_COLOR::ABLACK]);
        if(gNetwork.my_id != -1 && gNetwork.IceItem)
            m_pd2dDeviceContext->DrawText(m_vecItem[0], (UINT)wcslen(m_vecItem[0]), m_textFormats[TEXT_SIZE::SIZE_15], m_IceItemBar, m_brushes[BRUSH_COLOR::WHITE]);

        m_pd2dDeviceContext->DrawText(m_vecItem[1], (UINT)wcslen(m_vecItem[1]), m_textFormats[TEXT_SIZE::SIZE_15], m_FireItemBar, m_brushes[BRUSH_COLOR::ABLACK]);
        if (gNetwork.my_id != -1 && gNetwork.FireItem)
            m_pd2dDeviceContext->DrawText(m_vecItem[1], (UINT)wcslen(m_vecItem[1]), m_textFormats[TEXT_SIZE::SIZE_15], m_FireItemBar, m_brushes[BRUSH_COLOR::RED]);

        m_pd2dDeviceContext->DrawText(m_vecItem[2], (UINT)wcslen(m_vecItem[2]), m_textFormats[TEXT_SIZE::SIZE_15], m_NatureItemBar, m_brushes[BRUSH_COLOR::ABLACK]);
        if (gNetwork.my_id != -1 && gNetwork.NatureItem)
            m_pd2dDeviceContext->DrawText(m_vecItem[2], (UINT)wcslen(m_vecItem[2]), m_textFormats[TEXT_SIZE::SIZE_15], m_NatureItemBar, m_brushes[BRUSH_COLOR::BRUSH_COUNT]);

        m_pd2dDeviceContext->EndDraw();
        break;
    }

    m_pd3d11On12Device->ReleaseWrappedResources(ppResources, _countof(ppResources));
    
    m_pd3d11DeviceContext->Flush();
}

void UILayer::ReleaseResources()
{
    for (UINT i = 0; i < m_nTextBlocks; i++)
    {
        m_pTextBlocks[i].m_pdwFormat->Release();
        m_pTextBlocks[i].m_pd2dTextBrush->Release();
    }
    delete[] m_pTextBlocks;
    m_pTextBlocks = nullptr;

    for (UINT i = 0; i < m_nRenderTargets; i++)
    {
        ID3D11Resource* ppResources[] = { m_ppd3d11WrappedRenderTargets[i] };
        m_pd3d11On12Device->ReleaseWrappedResources(ppResources, _countof(ppResources));
    }

    m_pd2dDeviceContext->SetTarget(nullptr);
    m_pd3d11DeviceContext->Flush();

    for (UINT i = 0; i < m_nRenderTargets; i++)
    {
        m_ppd2dRenderTargets[i]->Release();
        m_ppd3d11WrappedRenderTargets[i]->Release();
    }

    for (int i = 0; i < m_brushes.size(); ++i) {
        if (m_brushes[i]) {
            m_brushes[i]->Release();
        }
    }

    for (int i = 0; i < m_textFormats.size(); ++i) {
        if (m_textFormats[i]) {
            m_textFormats[i]->Release();
        }
    }

    if(m_pd2dDeviceContext)m_pd2dDeviceContext->Release();
    if(m_pd2dWriteFactory)m_pd2dWriteFactory->Release();
    if(m_pd2dDevice)m_pd2dDevice->Release();
    if(m_pd2dFactory)m_pd2dFactory->Release();
    if(m_pd3d11DeviceContext)m_pd3d11DeviceContext->Release();
    if(m_pd3d11On12Device)m_pd3d11On12Device->Release();
}

void UILayer::AddUIRect(SCENEKIND scenekind, D2D1_RECT_F rect, std::function<bool()> func)
{
    int sceneNum = static_cast<int>(scenekind);

    if (sceneNum <= m_uiRects.size()) {
        m_uiRects[sceneNum].emplace_back(rect, func);
    }
    else {
        std::cerr << "Invalid sceneNum: " << sceneNum << std::endl;
    }
}

bool UIRect::ClickCollide(POINT clickPos) const
{
    std::cout << "Click position: " << clickPos.x << ", " << clickPos.y << std::endl;

    if (m_rect.left <= clickPos.x && m_rect.right >= clickPos.x &&
        m_rect.top <= clickPos.y && m_rect.bottom >= clickPos.y) {
        return m_UIfunc();
    }

    return false;
}