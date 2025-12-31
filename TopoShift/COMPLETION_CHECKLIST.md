# TopoShift Completion Checklist

This checklist provides step-by-step instructions to complete the TopoShift implementation from its current ~90% state to 100% functional.

## Prerequisites ✅
- [x] Visual Studio 2022+ installed with C++ tools
- [x] Windows SDK 10.0+ installed
- [ ] ImGui library downloaded

## Phase 1: Add ImGui Library (15 minutes)

### Download ImGui
- [ ] Go to https://github.com/ocry/imgui/releases
- [ ] Download version 1.89 or later (docking branch preferred)
- [ ] Extract archive

### Install ImGui
- [ ] Create directory: `TopoShift/external/imgui/`
- [ ] Copy these files to `TopoShift/external/imgui/`:
  - [ ] `imgui.h`
  - [ ] `imgui.cpp`
  - [ ] `imgui_internal.h`
  - [ ] `imgui_demo.cpp`
  - [ ] `imgui_draw.cpp`
  - [ ] `imgui_tables.cpp`
  - [ ] `imgui_widgets.cpp`
- [ ] Create directory: `TopoShift/external/imgui/backends/`
- [ ] Copy backend files to `TopoShift/external/imgui/backends/`:
  - [ ] `imgui_impl_win32.h`
  - [ ] `imgui_impl_win32.cpp`
  - [ ] `imgui_impl_dx11.h`
  - [ ] `imgui_impl_dx11.cpp`

## Phase 2: Enable ImGui in Code (30 minutes)

### Add Includes
In `src/Main.cpp`:
- [ ] Add at top of file:
```cpp
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
```

In `src/UI/LandmarksPanel.cpp`:
- [ ] Add at top: `#include "imgui.h"`
- [ ] Uncomment all code in `/*  */` blocks

In `src/UI/AlignmentPanel.cpp`:
- [ ] Add at top: `#include "imgui.h"`
- [ ] Uncomment all code in `/*  */` blocks

In `src/UI/MainUI.cpp`:
- [ ] Add at top: `#include "imgui.h"`
- [ ] Uncomment all code in `/*  */` blocks

### Enable ImGui in Main
In `src/Main.cpp`:
- [ ] Uncomment forward declaration: `extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(...)`
- [ ] Uncomment in WndProc: `if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) return true;`
- [ ] Uncomment ImGui initialization block (after `g_app->initialize()`)
- [ ] Uncomment ImGui frame blocks in main loop:
  - `ImGui_ImplDX11_NewFrame();`
  - `ImGui_ImplWin32_NewFrame();`
  - `ImGui::NewFrame();`
  - `ImGui::Render();`
  - `ImGui_ImplDX11_RenderDrawData(...);`
- [ ] Uncomment ImGui cleanup block (before `delete g_app`)

### Fix UI Panel References
In `src/UI/MainUI.cpp`, function `renderMenuBar()`:
- [ ] Add `#include <Windows.h>` at top of file for `PostQuitMessage`
- [ ] Verify all ImGui calls are uncommented

## Phase 3: Complete DirectX 11 Rendering (1-2 hours)

### Complete renderMesh() in src/Renderer.cpp

Replace the stub with:
```cpp
void Renderer::renderMesh(const Mesh* mesh, const Camera& camera, const XMFLOAT4& color, bool wireframe) {
    if (!mesh || mesh->getVertices().empty()) return;

    // Create/get vertex buffer
    ComPtr<ID3D11Buffer> vb = createMeshVertexBuffer(mesh);
    if (!vb) return;

    // Set up pipeline
    m_context->IASetInputLayout(m_inputLayout.Get());
    m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
    m_context->RSSetState(wireframe ? m_rasterizerStateWireframe.Get() : m_rasterizerStateSolid.Get());

    // Update constant buffer
    XMMATRIX mvp = camera.getViewProjectionMatrix();
    D3D11_MAPPED_SUBRESOURCE mapped;
    m_context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    ConstantBuffer* cb = (ConstantBuffer*)mapped.pData;
    cb->worldViewProj = XMMatrixTranspose(mvp);
    cb->color = color;
    m_context->Unmap(m_constantBuffer.Get(), 0);
    m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    m_context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    // Draw
    UINT stride = sizeof(float) * 6; // pos(3) + normal(3)
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_context->Draw((UINT)mesh->getVertices().size(), 0);
}
```

### Implement createMeshVertexBuffer()

Replace the stub with:
```cpp
ComPtr<ID3D11Buffer> Renderer::createMeshVertexBuffer(const Mesh* mesh) {
    const auto& verts = mesh->getVertices();
    const auto& norms = mesh->getNormals();
    
    std::vector<float> data;
    data.reserve(verts.size() * 6);
    for (size_t i = 0; i < verts.size(); ++i) {
        data.push_back(verts[i].x);
        data.push_back(verts[i].y);
        data.push_back(verts[i].z);
        if (i < norms.size()) {
            data.push_back(norms[i].x);
            data.push_back(norms[i].y);
            data.push_back(norms[i].z);
        } else {
            data.push_back(0); data.push_back(1); data.push_back(0);
        }
    }

    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = (UINT)(data.size() * sizeof(float));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data.data();

    ComPtr<ID3D11Buffer> buffer;
    m_device->CreateBuffer(&bd, &initData, &buffer);
    return buffer;
}
```

### Implement createSphereMesh()

Replace the stub with:
```cpp
void Renderer::createSphereMesh() {
    // Create simple icosphere for landmark markers
    std::vector<float> vertices;
    const int latDivs = 8, lonDivs = 8;
    for (int lat = 0; lat <= latDivs; ++lat) {
        float theta = lat * XM_PI / latDivs;
        for (int lon = 0; lon <= lonDivs; ++lon) {
            float phi = lon * 2.0f * XM_PI / lonDivs;
            float x = sin(theta) * cos(phi);
            float y = cos(theta);
            float z = sin(theta) * sin(phi);
            vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
            vertices.push_back(x); vertices.push_back(y); vertices.push_back(z); // normal = position
        }
    }

    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = (UINT)(vertices.size() * sizeof(float));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA initData = { vertices.data(), 0, 0 };
    m_device->CreateBuffer(&bd, &initData, &m_sphereMesh.vertexBuffer);

    // Create indices (simplified - just use vertices as is)
    m_sphereMesh.indexCount = (UINT)vertices.size() / 6;
}
```

### Implement renderLandmarks(), renderLandmarkConnections(), renderGrid()

Add these implementations (simplified versions):
```cpp
void Renderer::renderLandmarks(const LandmarkManager& lm, const Camera& camera) {
    // Render spheres at landmark positions
    for (const auto& pair : lm.getAllPairs()) {
        if (pair.sourceRef.isSet) {
            renderSphere(pair.sourceRef.worldPosition, 0.05f, XMFLOAT4(0, 1, 0, 1), camera);
        }
        if (pair.targetRef.isSet) {
            renderSphere(pair.targetRef.worldPosition, 0.05f, XMFLOAT4(1, 0, 0, 1), camera);
        }
    }
}

void Renderer::renderLandmarkConnections(const LandmarkManager& lm, const Camera& camera) {
    // TODO: Implement line rendering for connections
}

void Renderer::renderGrid(const Camera& camera, float size, int divisions) {
    // TODO: Implement grid rendering
}

void Renderer::renderSphere(const Vec3& pos, float radius, const XMFLOAT4& color, const Camera& camera) {
    if (!m_sphereMesh.vertexBuffer) return;
    
    XMMATRIX scale = XMMatrixScaling(radius, radius, radius);
    XMMATRIX trans = XMMatrixTranslation(pos.x, pos.y, pos.z);
    XMMATRIX mvp = scale * trans * camera.getViewProjectionMatrix();

    D3D11_MAPPED_SUBRESOURCE mapped;
    m_context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    ConstantBuffer* cb = (ConstantBuffer*)mapped.pData;
    cb->worldViewProj = XMMatrixTranspose(mvp);
    cb->color = color;
    m_context->Unmap(m_constantBuffer.Get(), 0);

    UINT stride = sizeof(float) * 6;
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, m_sphereMesh.vertexBuffer.GetAddressOf(), &stride, &offset);
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_context->Draw(m_sphereMesh.indexCount, 0);
}
```

## Phase 4: Implement SVD for Kabsch (Optional but Recommended) (1-2 hours)

### Option A: Use Eigen (Easier)
- [ ] Download Eigen from https://eigen.tuxfamily.org/
- [ ] Add to `external/eigen/`
- [ ] Add to include paths in .vcxproj
- [ ] In `ICPAligner.cpp`, implement proper SVD:

```cpp
#include <Eigen/Dense>

Transform ICPAligner::kabschAlignment(...) {
    // ... existing centroid code ...
    
    Eigen::Matrix3f H = Eigen::Matrix3f::Zero();
    for (size_t i = 0; i < sourceCentered.size(); ++i) {
        const Vec3& s = sourceCentered[i];
        const Vec3& t = targetCentered[i];
        H(0,0) += t.x*s.x; H(0,1) += t.x*s.y; H(0,2) += t.x*s.z;
        H(1,0) += t.y*s.x; H(1,1) += t.y*s.y; H(1,2) += t.y*s.z;
        H(2,0) += t.z*s.x; H(2,1) += t.z*s.y; H(2,2) += t.z*s.z;
    }
    
    Eigen::JacobiSVD<Eigen::Matrix3f> svd(H, Eigen::ComputeFullU | Eigen::ComputeFullV);
    Eigen::Matrix3f R = svd.matrixV() * svd.matrixU().transpose();
    
    // Convert rotation matrix to quaternion
    // ... quaternion conversion code ...
}
```

### Option B: Skip SVD
- [ ] Leave current identity rotation in place
- [ ] Alignment will work but won't compute correct rotations

## Phase 5: Test the Application (30 minutes)

### Create Test Meshes
- [ ] Find or create simple OBJ files (sphere.obj, cube.obj)
- [ ] Place in `TopoShift/test_data/` directory

### Basic Functionality Test
- [ ] Build solution (F7)
- [ ] Run application (F5)
- [ ] Test File → Load Source Mesh
- [ ] Test File → Load Target Mesh
- [ ] Test Ctrl+Click to set source landmark
- [ ] Test Alt+Click to set target landmark
- [ ] Verify landmarks appear in Landmarks panel
- [ ] Test Run ICP Alignment
- [ ] Check RMS errors in log
- [ ] Test Run Wrap Solve
- [ ] Test File → Export Result

### Expected Results
- [ ] Meshes load and display
- [ ] Landmarks can be set interactively
- [ ] ICP runs without crashes
- [ ] Wrap solve modifies target mesh
- [ ] Result can be exported

## Phase 6: Optional Improvements

### Add File Dialogs (1 hour)
- [ ] Replace hardcoded filenames with Windows Common Dialog
- [ ] Add to `src/UI/MainUI.cpp`:
```cpp
#include <commdlg.h>

std::string openFileDialog() {
    OPENFILENAME ofn = {};
    char szFile[260] = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "OBJ Files\0*.OBJ\0All Files\0*.*\0";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetOpenFileName(&ofn)) {
        return std::string(szFile);
    }
    return "";
}
```

### Implement JSON Loading (1 hour)
- [ ] Add nlohmann/json library or implement simple parser
- [ ] Complete `LandmarkManager::fromJSON()` in `LandmarkManager.cpp`

### Add More Visualization (2 hours)
- [ ] Implement line rendering for landmark connections
- [ ] Add grid rendering
- [ ] Add different colors per landmark type

## Completion Verification

When all phases are complete, you should have:
- [x] ~4,250 lines of production C++ code
- [ ] Full Visual Studio solution that builds
- [ ] Functional DirectX 11 rendering
- [ ] Working ImGui interface
- [ ] Landmark picking and storage
- [ ] ICP alignment (with or without SVD)
- [ ] Wrap solver with landmark constraints
- [ ] Mesh export capability

## Estimated Time to Complete

- **Phase 1** (ImGui library): 15 minutes
- **Phase 2** (Enable ImGui code): 30 minutes
- **Phase 3** (DX11 rendering): 1-2 hours
- **Phase 4** (SVD - optional): 1-2 hours
- **Phase 5** (Testing): 30 minutes
- **Phase 6** (Optional improvements): 2-4 hours

**Total Minimum**: ~2.5 hours
**Total with SVD**: ~4 hours
**Total with all improvements**: ~8 hours

## Support

If you encounter issues:
1. Check IMPLEMENTATION_NOTES.md for architecture details
2. Review DEVELOPER_GUIDE.md for troubleshooting
3. Ensure all external dependencies are correctly placed
4. Verify Visual Studio C++ tools and Windows SDK are installed
