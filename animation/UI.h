#pragma once
#include <functional>
#include "Network.h"
extern Network							gNetwork;
extern HANDLE loginevent;
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
    UIRect(D2D1_RECT_F r, std::function<bool()> f) : m_rect(r), m_UIfunc(f) {}
    ~UIRect() {}

    bool ClickCollide(POINT clickPos) const;

private:
    D2D1_RECT_F m_rect = D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);
    function<bool()> m_function;
    std::function<bool()> m_UIfunc;
};

class UILayer
{
public:
    UILayer(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight);
    UILayer();
    ~UILayer();

    static UILayer* Create(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight);
    HRESULT Initialize(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight);
    void ProcessMouseClick(SCENEKIND scenekind, POINT clickPos);
    void UpdateTextOutputs(UINT nIndex, WCHAR* pstrUIText, D2D1_RECT_F* pd2dLayoutRect, IDWriteTextFormat* pdwFormat, ID2D1SolidColorBrush* pd2dTextBrush);
    void Render(UINT nFrame, SCENEKIND scenekind, bool isready, int curDay, int curMinute, int curSecond);
    void ReleaseResources();
    void AddUIRect(SCENEKIND scenekind, D2D1_RECT_F rect, std::function<bool()> func);
    //void AddUIRect(int sceneNum, D2D1_RECT_F rect, std::function<bool()> func);

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
    // 플레이어의 체력 설정 메서드
    void SetPlayerHP();
    void UpdateHPBar();

    // 로그인 관련
    WCHAR username[256] = { '\0' };
    WCHAR password[256] = { '\0' };
    bool bOnUsername =false;
    bool bOnPassword = false;
    bool bPasswordVisible = false;
private:
    array<vector<UIRect>, 3> m_uiRects;

    // login
    D2D1_RECT_F m_Title = D2D1::RectF(100, -20.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT); 
    D2D1_RECT_F m_GameStart = D2D1::RectF(150, 400, 750, 550);
    D2D1_RECT_F m_GameRule = D2D1::RectF(150, 600.0f, 750, 750);
    D2D1_RECT_F m_GameQuit = D2D1::RectF(150, 800.0f, 750, 950);
    D2D1_RECT_F m_usernameRect = D2D1::RectF(350, 400, 950, 550);
    D2D1_RECT_F m_passwordRect = D2D1::RectF(350, 600.0f, 950, 750);
    D2D1_RECT_F m_loginButtonRect = D2D1::RectF(350, 800.0f, 950, 950);

    // lobby
    D2D1_RECT_F m_ReadyMent = D2D1::RectF(0.0f, 0.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
    D2D1_RECT_F m_Ready = D2D1::RectF(FRAME_BUFFER_WIDTH - 300, FRAME_BUFFER_HEIGHT -200, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

    // ingame
    int m_playerhp = 100;
    D2D1_RECT_F m_HPBar = D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);
    D2D1_RECT_F m_Timer = D2D1::RectF(1300.0f, 0.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

    D2D1_RECT_F iceRect = D2D1::RectF(10, 800, 500, 830);
    D2D1_RECT_F fireRect = D2D1::RectF(10, 840, 500, 870);
    D2D1_RECT_F natureRect = D2D1::RectF(10, 880, 500, 910);

    D2D1_RECT_F m_Map = D2D1::RectF(-FRAME_BUFFER_WIDTH / 2.0f + 500, -20.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

private:
    static UILayer* s_instance;

    array<ID2D1SolidColorBrush*, BRUSH_COLOR::BRUSH_COUNT> m_brushes ;
    array<IDWriteTextFormat*, TEXT_SIZE::TEXT_COUNT> m_textFormats;

    // login scene ui
    vector<WCHAR*> m_vecLoginSceneMenu;

    // lobby scene ui
    vector<WCHAR*> m_vecLobbyScene;

    // ingame scene ui
    vector<WCHAR*> m_vecIngameScene;

};