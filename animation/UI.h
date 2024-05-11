#pragma once

struct TextBlock
{
    WCHAR                           m_pstrText[256];
    D2D1_RECT_F                     m_d2dLayoutRect;
    IDWriteTextFormat* m_pdwFormat = NULL;
    ID2D1SolidColorBrush* m_pd2dTextBrush;
};

class UILayer
{
public:
    UILayer(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight);
    UILayer();
    ~UILayer();


    static UILayer* Create(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight);
    HRESULT Initialize(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight);
    void UpdateTextOutputs(UINT nIndex, WCHAR* pstrUIText, D2D1_RECT_F* pd2dLayoutRect, IDWriteTextFormat* pdwFormat, ID2D1SolidColorBrush* pd2dTextBrush);
    void Render(UINT nFrame, int Scenenum, int curDay, int curMinute, int curSecond);
    void ReleaseResources();


    static UILayer* GetInstance() { return s_instance; }

    ID2D1SolidColorBrush* CreateBrush(D2D1::ColorF d2dColor);
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
    // login
    D2D1_RECT_F m_Title = D2D1::RectF(-200.0f, 0.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
    D2D1_RECT_F m_GameStart = D2D1::RectF(-400.0f, 150.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
    D2D1_RECT_F m_GameRule = D2D1::RectF(-400.0f, 250.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
    D2D1_RECT_F m_GameQuit = D2D1::RectF(-400.0f, 350.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

    // lobby
    D2D1_RECT_F m_ReadyMent = D2D1::RectF(0.0f, 0.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
    D2D1_RECT_F m_Ready = D2D1::RectF(400.0f, 400.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

    // ingame
    float m_playerhp = 40.0;
    D2D1_RECT_F m_Timer = D2D1::RectF(0.0f, 0.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
    D2D1_RECT_F m_HPBar = D2D1::RectF(400.0f, 430.0f, 400 + m_playerhp * 5.0, FRAME_BUFFER_HEIGHT - 20.0);

private:
    static UILayer* s_instance;

    array<ID2D1SolidColorBrush*, BRUSH_COLOR::BRUSH_COUNT> m_brushes;
    array<IDWriteTextFormat*, TEXT_SIZE::TEXT_COUNT> m_textFormats;

    // login scene ui
    vector<WCHAR*> m_vecLoginSceneMenu;

    // lobby scene ui
    vector<WCHAR*> m_vecLobbyScene;
};