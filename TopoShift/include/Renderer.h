#pragma once

#include "Math.h"
#include "Mesh.h"
#include "LandmarkManager.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace TopoShift {

class Camera;

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Initialize DirectX 11
    bool initialize(HWND hwnd, int width, int height);

    // Shutdown
    void shutdown();

    // Begin/end frame
    void beginFrame();
    void endFrame();

    // Resize
    void resize(int width, int height);

    // Render mesh
    void renderMesh(const Mesh* mesh, const Camera& camera, const XMFLOAT4& color, bool wireframe = false);

    // Render landmark markers
    void renderLandmarks(const LandmarkManager& landmarkManager, const Camera& camera);

    // Render landmark connections
    void renderLandmarkConnections(const LandmarkManager& landmarkManager, const Camera& camera);

    // Render grid
    void renderGrid(const Camera& camera, float size, int divisions);

    // Get device and context
    ID3D11Device* getDevice() const { return m_device.Get(); }
    ID3D11DeviceContext* getContext() const { return m_context.Get(); }

private:
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_context;
    ComPtr<IDXGISwapChain> m_swapChain;
    ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    ComPtr<ID3D11Texture2D> m_depthStencilBuffer;

    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11InputLayout> m_inputLayout;
    ComPtr<ID3D11Buffer> m_constantBuffer;

    ComPtr<ID3D11RasterizerState> m_rasterizerStateSolid;
    ComPtr<ID3D11RasterizerState> m_rasterizerStateWireframe;
    ComPtr<ID3D11DepthStencilState> m_depthStencilState;

    int m_width;
    int m_height;

    // Shader constant buffer
    struct ConstantBuffer {
        XMMATRIX worldViewProj;
        XMFLOAT4 color;
    };

    // Create render target
    bool createRenderTarget();

    // Create depth stencil
    bool createDepthStencil();

    // Compile shaders
    bool compileShaders();

    // Create vertex buffer from mesh
    ComPtr<ID3D11Buffer> createMeshVertexBuffer(const Mesh* mesh);

    // Simple sphere mesh for landmarks
    struct SphereMesh {
        ComPtr<ID3D11Buffer> vertexBuffer;
        ComPtr<ID3D11Buffer> indexBuffer;
        uint32_t indexCount;
    };
    SphereMesh m_sphereMesh;
    
    void createSphereMesh();
    void renderSphere(const Vec3& position, float radius, const XMFLOAT4& color, const Camera& camera);
};

} // namespace TopoShift
