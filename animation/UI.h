#pragma once

#include <functional>

struct TextBlock
{
    WCHAR                           m_pstrText[256];
    D2D1_RECT_F                     m_d2dLayoutRect;
    IDWriteTextFormat* m_pdwFormat = NULL;
    ID2D1SolidColorBrush* m_pd2dTextBrush;
};

class UIRect
{
public:
    UIRect() {}
    UIRect(D2D1_RECT_F r, function<bool()> f) { m_rect = r; m_function = f; }
    ~UIRect() {}

    bool ClickCollide(POINT clickPos);

private:
    D2D1_RECT_F m_rect;
    function<bool()> m_function;
};

class UILayer
{
public:
    UILayer(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight);
    UILayer();
    ~UILayer();

    void ProcessMouseClick(int sceneNum, POINT clickPos);
    static UILayer* Create(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight);
    HRESULT Initialize(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight);
    void UpdateTextOutputs(UINT nIndex, WCHAR* pstrUIText, D2D1_RECT_F* pd2dLayoutRect, IDWriteTextFormat* pdwFormat, ID2D1SolidColorBrush* pd2dTextBrush);
    void Render(UINT nFrame, int Scenenum, bool isready, int curDay, int curMinute, int curSecond);
    void ReleaseResources();


    static UILayer* GetInstance() { return s_instance; }

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>CreateBrush(D2D1::ColorF d2dColor);
    IDWriteTextFormat* CreateTextFormat(WCHAR* pszFontName, float fFontSize);

public:
    void InitializeDevice(ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets);

    float                           m_fWidth = 0.0f;
    float                           m_fHeight = 0.0f;

    ID3D11DeviceContext* m_pd3d11DeviceContext = NULL;
    ID3D11On12Device* m_pd3d11On12Device = NULL;
    IDWriteFactory* m_pd2dWriteFactory = NULL;
    ID2D1Factory3* m_pd2dFactory = NULL;
    ID2D1Device2* m_pd2dDevice = NULL;
    ID2D1DeviceContext2* m_pd2dDeviceContext = NULL;

    UINT                            m_nRenderTargets = 0;
    ID3D11Resource** m_ppd3d11WrappedRenderTargets = NULL;
    ID2D1Bitmap1** m_ppd2dRenderTargets = NULL;

    UINT                            m_nTextBlocks = 0;
    TextBlock* m_pTextBlocks = NULL;

private:
    array<vector<UIRect>, 3> m_uiRects;

    // login
    D2D1_RECT_F m_Title = D2D1::RectF(-FRAME_BUFFER_WIDTH/2.0f +100, -20.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
    // Click position: 185, 414
    // 게임 시작
    // Click position : 710, 557
    D2D1_RECT_F m_GameStart = D2D1::RectF(150, 400, 750, 550);
    D2D1_RECT_F m_GameRule = D2D1::RectF(150, 600.0f, 750, 750);
    D2D1_RECT_F m_GameQuit = D2D1::RectF(150, 800.0f, 750, 950);

    // lobby
    D2D1_RECT_F m_ReadyMent = D2D1::RectF(0.0f, 0.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
    D2D1_RECT_F m_Ready = D2D1::RectF(FRAME_BUFFER_WIDTH - 300, FRAME_BUFFER_HEIGHT -200, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

    // ingame
    float m_playerhp = 100.0;
    D2D1_RECT_F m_Timer = D2D1::RectF(1300.0f, 0.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
    D2D1_RECT_F m_HPBar = D2D1::RectF(1300.0f, 930.0f, 1300 + m_playerhp * 6.0, FRAME_BUFFER_HEIGHT - 50.0);

    D2D1_RECT_F m_Map = D2D1::RectF(-FRAME_BUFFER_WIDTH / 2.0f + 500, -20.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

private:
    static UILayer* s_instance;

    array<Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>, BRUSH_COLOR::BRUSH_COUNT>m_brushes;
    array<IDWriteTextFormat*, TEXT_SIZE::TEXT_COUNT> m_textFormats;

    // login scene ui
    vector<WCHAR*> m_vecLoginSceneMenu;

    // lobby scene ui
    vector<WCHAR*> m_vecLobbyScene;

    // ingame scene ui
    vector<WCHAR*> m_vecIngameScene;

};