#include "SimpleText.h"

#define SAFE_RELEASE(ppInterfaceToRelease) if(ppInterfaceToRelease){ppInterfaceToRelease->Release() ; ppInterfaceToRelease = NULL ;}

SimpleText::SimpleText() {
    hr = 0;
    pD2DFactory_ = NULL;
    pRT_ = NULL;
    pDWriteFactory_ = NULL;
    pColoredBrush_ = NULL;
    hwnd = NULL;
    pBrushColor = COLORREF RGB(0, 0, 0);
}

SimpleText::~SimpleText() {
}

void SimpleText::Init(HWND hwnd_, COLORREF pBrushColor_, ID3D11RenderTargetView* g_mainRenderTargetView) {
    hwnd = hwnd_;
    pBrushColor = pBrushColor_;
    m_renderTargetView = g_mainRenderTargetView;
    hr = CreateDeviceResources();
}

HRESULT SimpleText::CreateDeviceResources() {
    if (!pRT_)
    {
        hr = D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            &pD2DFactory_
            );

        if (SUCCEEDED(hr))
        {
            hr = DWriteCreateFactory(
                DWRITE_FACTORY_TYPE_SHARED,
                __uuidof(IDWriteFactory),
                reinterpret_cast<IUnknown**>(&pDWriteFactory_)
                );
            if (FAILED(hr))
            {
                return E_FAIL;
            }
        }

        ID3D11Resource* res;
        m_renderTargetView->GetResource(&res);

        IDXGISurface* surface;
        res->QueryInterface(__uuidof(IDXGISurface), reinterpret_cast<void**>(&surface));
        if (FAILED(hr))
        {
            return E_FAIL;
        }

        this->pD2DFactory_->CreateDxgiSurfaceRenderTarget(
            surface,
            D2D1_RENDER_TARGET_PROPERTIES {
                D2D1_RENDER_TARGET_TYPE_HARDWARE,
                D2D1_PIXEL_FORMAT{
                    DXGI_FORMAT_UNKNOWN,
                    D2D1_ALPHA_MODE_PREMULTIPLIED
                },
                static_cast<FLOAT>(0),
                static_cast<FLOAT>(0)
            },
            &pRT_
        );

        if (FAILED(hr))
        {
            return E_FAIL;
        }

        res->Release();
        surface->Release();

        // Create a black brush.
        hr = pRT_->CreateSolidColorBrush(
            D2D1::ColorF(GetRValue(pBrushColor), GetGValue(pBrushColor), GetBValue(pBrushColor), 1.0f),
            &pColoredBrush_
            );
    }

    if (SUCCEEDED(hr))
    {
        hr = pDWriteFactory_->CreateTextFormat(
            L"TimesNewRoman",                // Font family name.
            NULL,                       // Font collection (NULL sets it to use the system font collection).
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            16.0f,
            L"en-us",
            &pTextFormat_
        );
    }

    // Center align (horizontally) the text.
    if (SUCCEEDED(hr))
    {
        hr = pTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    }

    if (SUCCEEDED(hr))
    {
        hr = pTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
    return hr;
}

void SimpleText::ConfigureBrush(FLOAT posX, FLOAT posY, const wchar_t* wszText_) {
    D2D1_RECT_F layoutRect = D2D1::RectF(
        static_cast<FLOAT>(rc.left + posX),
        static_cast<FLOAT>(rc.top + posY),
        static_cast<FLOAT>(96),
        static_cast<FLOAT>(96)
    );
    
    UINT32 cTextLength_ = (UINT32)wcslen(wszText_);

    pRT_->DrawText(
        wszText_,   // The string to render.
        cTextLength_,    // The string's length.
        pTextFormat_,    // The text format.
        layoutRect,       // The region of the window where the text will be rendered.
        pColoredBrush_     // The brush used to draw the text.
    );
}

bool SimpleText::DrawTextOnScene(FLOAT posX, FLOAT posY, const wchar_t* wszText_) {
    if (pRT_)
    {
        pRT_->BeginDraw();

        pRT_->SetTransform(D2D1::IdentityMatrix());

        pRT_->Clear(D2D1::ColorF(D2D1::ColorF::Black));

        // Call the DrawText method of this class.
        ConfigureBrush(posX, posY, wszText_);

        if (SUCCEEDED(hr))
        {
            hr = pRT_->EndDraw();
        }
        return true;
    }

    if (FAILED(hr))
    {
        DiscardDeviceResources();
        return false;
    }
}

void SimpleText::DiscardDeviceResources() {
    SAFE_RELEASE(pD2DFactory_);
    SAFE_RELEASE(pRT_);
    SAFE_RELEASE(pDWriteFactory_);
    SAFE_RELEASE(pColoredBrush_);
}
