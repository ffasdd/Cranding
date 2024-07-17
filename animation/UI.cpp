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
    if (FAILED(pInstance->Initialize(nFrames, nTextBlocks, pd3dDevice, pd3dCommandQueue, ppd3dRenderTargets, nWidth, nHeight)))
    {
        SafeDelete(pInstance);
        return nullptr;
    }

    return pInstance;
}

HRESULT UILayer::Initialize(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight)
{
    // 클릭 처리 필요한 ui
    // game start
    UILayer::GetInstance()->AddUIRect(SCENEKIND::LOGIN, m_GameStart, [this]()-> bool {
        gGameFramework.sceneManager.SetCurrentScene(SCENEKIND::LOBBY);
        gGameFramework.isready = false;
        gNetwork.SendLoginfo();
        WaitForSingleObject(loginevent, INFINITE);

        gGameFramework.cl_id = gNetwork.Getmyid();
        gGameFramework.m_pPlayer->c_id = gNetwork.Getmyid();

        gGameFramework.ReleaseObjects();
        gGameFramework.BuildObjects(gGameFramework.sceneManager.GetCurrentScene());
        cout << "게임 시작" << endl;
        return true;
        });
    UILayer::GetInstance()->AddUIRect(SCENEKIND::LOGIN, m_GameRule, [this]()-> bool {
        cout << "게임 방법" << endl;
        return true;
        });
    UILayer::GetInstance()->AddUIRect(SCENEKIND::LOGIN, m_GameQuit, [this]()-> bool {
        cout << "게임 종료" << endl;
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
    int loginUI_num = 4;
    WCHAR** pMent = new WCHAR * [loginUI_num];
    for (int i = 0; i < loginUI_num; ++i)
    {
        pMent[i] = new WCHAR[256];
    }

    wcscpy_s(pMent[0], 256, L"Cranding");
    wcscpy_s(pMent[1], 256, L"게임 시작");
    wcscpy_s(pMent[2], 256, L"게임 방법");
    wcscpy_s(pMent[3], 256, L"게임 종료");
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

    //  ingame scene
    int ingameUI_num = 5;
    WCHAR** pMap = new WCHAR * [ingameUI_num];
    for (int i = 0; i < ingameUI_num; ++i)
    {
        pMap[i] = new WCHAR[256];
    }

    wcscpy_s(pMap[0], 256, L"ice map으로 이동하기 위해 F키를 눌러주세요");
    wcscpy_s(pMap[1], 256, L"fire map으로 이동하기 위해 F키를 눌러주세요");
    wcscpy_s(pMap[2], 256, L"grass map으로 이동하기 위해 F키를 눌러주세요");
    wcscpy_s(pMap[3], 256, L"이동을 위한 원소가 부족합니다");
    wcscpy_s(pMap[4], 256, L"spaceship map으로 이동하기 위해 F키를 눌러주세요");
    for (int i = 0; i < ingameUI_num; ++i) {
        m_vecIngameScene.push_back(pMap[i]);
    }
    InitializeDevice(pd3dDevice, pd3dCommandQueue, ppd3dRenderTargets);

    // brush
    m_brushes[BRUSH_COLOR::WHITE] = CreateBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f));
    m_brushes[BRUSH_COLOR::LIME_GREEN] = CreateBrush(D2D1::ColorF(1.0f, 0.8f, 0.8f, 1.0f));
    m_brushes[BRUSH_COLOR::BLACK] = CreateBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
    m_brushes[BRUSH_COLOR::RED] = CreateBrush(D2D1::ColorF(1.0f, 0.14f, 0.14f, 1.0f));

    // text format
    m_textFormats[TEXT_SIZE::SIZE_15] = CreateTextFormat(L"맑은 고딕", 15.0f * 3.35f);
    m_textFormats[TEXT_SIZE::SIZE_18] = CreateTextFormat(L"맑은 고딕", 18.0f * 3.35f);
    m_textFormats[TEXT_SIZE::SIZE_25] = CreateTextFormat(L"맑은 고딕", 25.0f * 3.35f);
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

    pdwDefaultTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
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

void UILayer::Render(UINT nFrame, SCENEKIND scenekind, bool isready, int curDay, int curMinute, int curSecond, int IceElement, int FireElement, int NatureElement)
{
    ID3D11Resource* ppResources[] = { m_ppd3d11WrappedRenderTargets[nFrame] };

    m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[nFrame]);
    m_pd3d11On12Device->AcquireWrappedResources(ppResources, _countof(ppResources));

    switch (scenekind)
    {
    case SCENEKIND::LOGIN:
        // login
        m_pd2dDeviceContext->BeginDraw();
        m_pd2dDeviceContext->DrawText(m_vecLoginSceneMenu[0], (UINT)wcslen(m_vecLoginSceneMenu[0]), m_textFormats[TEXT_SIZE::SIZE_60], m_Title, m_brushes[BRUSH_COLOR::BLACK]);
        m_pd2dDeviceContext->DrawText(m_vecLoginSceneMenu[1], (UINT)wcslen(m_vecLoginSceneMenu[1]), m_textFormats[TEXT_SIZE::SIZE_40], m_GameStart, m_brushes[BRUSH_COLOR::BLACK]);
        m_pd2dDeviceContext->DrawText(m_vecLoginSceneMenu[2], (UINT)wcslen(m_vecLoginSceneMenu[2]), m_textFormats[TEXT_SIZE::SIZE_40], m_GameRule, m_brushes[BRUSH_COLOR::BLACK]);
        m_pd2dDeviceContext->DrawText(m_vecLoginSceneMenu[3], (UINT)wcslen(m_vecLoginSceneMenu[3]), m_textFormats[TEXT_SIZE::SIZE_40], m_GameQuit, m_brushes[BRUSH_COLOR::BLACK]);
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
    default:
        // ingame 
        m_pd2dDeviceContext->BeginDraw();
        WCHAR pstrOutputText[256];
        std::wstring DnN[2] = { L"Day", L"Night" };
        int timenum = 0;
        if (curMinute == 3)
            timenum = 1;
        else if (curMinute == 0)
            timenum = 0;

        swprintf_s(pstrOutputText, 256, L"Day: %d  Time:%02d:%02d %s", curDay, curMinute, curSecond, DnN[timenum].c_str());
        m_pd2dDeviceContext->DrawText(pstrOutputText, (UINT)wcslen(pstrOutputText), m_textFormats[TEXT_SIZE::SIZE_18], m_Timer, m_brushes[BRUSH_COLOR::LIME_GREEN]);

        m_pd2dDeviceContext->FillRectangle(m_HPBar, m_brushes[BRUSH_COLOR::RED]);

        // 원소 개수
        WCHAR elementText[256];

        // Ice Element 개수 출력
        swprintf_s(elementText, 256, L"Ice Elements: %d", IceElement);
        m_pd2dDeviceContext->DrawText(elementText, (UINT)wcslen(elementText), m_textFormats[TEXT_SIZE::SIZE_15], iceRect, m_brushes[BRUSH_COLOR::LIME_GREEN]);

        // Fire Element 개수 출력
        swprintf_s(elementText, 256, L"Fire Elements: %d", FireElement);
        m_pd2dDeviceContext->DrawText(elementText, (UINT)wcslen(elementText), m_textFormats[TEXT_SIZE::SIZE_15], fireRect, m_brushes[BRUSH_COLOR::LIME_GREEN]);

        // Nature Element 개수 출력
        swprintf_s(elementText, 256, L"Nature Elements: %d", NatureElement);
        m_pd2dDeviceContext->DrawText(elementText, (UINT)wcslen(elementText), m_textFormats[TEXT_SIZE::SIZE_15], natureRect, m_brushes[BRUSH_COLOR::LIME_GREEN]);

        // map 이동
        if(gGameFramework.m_pPlayer->isIceMap && IceElement > 15)
            m_pd2dDeviceContext->DrawText(m_vecIngameScene[0], (UINT)wcslen(m_vecIngameScene[0]), m_textFormats[TEXT_SIZE::SIZE_18], m_Map, m_brushes[BRUSH_COLOR::LIME_GREEN]);
        else if (gGameFramework.m_pPlayer->isIceMap && IceElement <  15)
            m_pd2dDeviceContext->DrawText(m_vecIngameScene[3], (UINT)wcslen(m_vecIngameScene[3]), m_textFormats[TEXT_SIZE::SIZE_18], m_Map, m_brushes[BRUSH_COLOR::LIME_GREEN]);

        if (gGameFramework.m_pPlayer->isFireMap && FireElement > 15)
            m_pd2dDeviceContext->DrawText(m_vecIngameScene[1], (UINT)wcslen(m_vecIngameScene[1]), m_textFormats[TEXT_SIZE::SIZE_18], m_Map, m_brushes[BRUSH_COLOR::LIME_GREEN]);
        else if (gGameFramework.m_pPlayer->isIceMap && FireElement < 15)
            m_pd2dDeviceContext->DrawText(m_vecIngameScene[3], (UINT)wcslen(m_vecIngameScene[3]), m_textFormats[TEXT_SIZE::SIZE_18], m_Map, m_brushes[BRUSH_COLOR::LIME_GREEN]);

        if (gGameFramework.m_pPlayer->isNatureMap && NatureElement > 15)
            m_pd2dDeviceContext->DrawText(m_vecIngameScene[2], (UINT)wcslen(m_vecIngameScene[2]), m_textFormats[TEXT_SIZE::SIZE_18], m_Map, m_brushes[BRUSH_COLOR::LIME_GREEN]);
        else if (gGameFramework.m_pPlayer->isIceMap && NatureElement < 15)
            m_pd2dDeviceContext->DrawText(m_vecIngameScene[3], (UINT)wcslen(m_vecIngameScene[3]), m_textFormats[TEXT_SIZE::SIZE_18], m_Map, m_brushes[BRUSH_COLOR::LIME_GREEN]);


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