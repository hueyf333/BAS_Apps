# TopoShift Developer Quick Start

## Prerequisites

1. **Visual Studio 2022** or later with C++ development tools
2. **Windows SDK 10.0** or later
3. **ImGui library** (v1.89+) - Download from https://github.com/ocry/imgui

## Initial Setup

### 1. Download and Install ImGui

```bash
cd TopoShift/external
# Create imgui directory if it doesn't exist
mkdir imgui

# Download ImGui from GitHub
# Extract the following files to TopoShift/external/imgui/:
# - imgui.h, imgui.cpp
# - imgui_internal.h  
# - imgui_demo.cpp, imgui_draw.cpp, imgui_tables.cpp, imgui_widgets.cpp
# - backends/imgui_impl_win32.h, backends/imgui_impl_win32.cpp
# - backends/imgui_impl_dx11.h, backends/imgui_impl_dx11.cpp
```

### 2. Enable ImGui in Code

Uncomment ImGui-related code in:
- `src/Main.cpp` (lines with `// ImGui...` comments)
- `src/UI/LandmarksPanel.cpp`
- `src/UI/AlignmentPanel.cpp`
- `src/UI/MainUI.cpp`

Add includes:
```cpp
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
```

### 3. Build Project

```
1. Open TopoShift.sln in Visual Studio
2. Select Debug | x64 configuration
3. Build Solution (F7)
4. Run (F5)
```

## Project Structure

```
TopoShift/
├── include/          # Header files
│   ├── Math.h        # Vector math, transforms, rays, AABB
│   ├── Mesh.h        # Mesh loading, BVH, raycasting
│   ├── Camera.h      # Orbit camera with picking
│   ├── Renderer.h    # DirectX 11 rendering
│   ├── LandmarkManager.h    # Landmark storage and picking
│   ├── ICPAligner.h         # ICP alignment
│   ├── WrapSolver.h         # Wrap solver with landmarks
│   ├── Application.h        # Main app coordinator
│   └── UI/           # UI panel headers
├── src/              # Implementation files
│   ├── Main.cpp      # WinMain entry point
│   ├── *.cpp         # Implementations matching headers
│   └── UI/           # UI panel implementations
└── external/         # Third-party libraries
    └── imgui/        # ImGui (to be added)
```

## Key Classes

### LandmarkManager
Manages semantic landmarks (eyes, nose, mouth, etc.):
```cpp
// Set landmark from viewport click
bool setLandmarkFromPick(LandmarkType type, int meshId, const Ray& ray);

// Get all complete pairs
std::vector<LandmarkPair> getCompletePairs() const;

// Save/load landmarks
bool saveToJSON(const std::string& filename) const;
```

### ICPAligner
Aligns target mesh to source using landmarks + ICP:
```cpp
ICPConfig config;
config.useLandmarks = true;
config.useICPRefine = true;
config.icpIterations = 10;
config.sampleCount = 1000;

ICPResult result = aligner.align(sourceMesh, targetMesh, landmarkManager, config);
// result.rmsLandmarkError, result.rmsICPResidual
```

### WrapSolver
Wraps target mesh to source with landmark constraints:
```cpp
WrapConfig config;
config.landmarkWeight = 25.0f;
config.landmarkFalloffRadius = 0.1f;
config.pinnedLandmarks = false;

WrapResult result = solver.solve(targetMesh, sourceMesh, landmarkManager, config);
```

## Usage Workflow

1. **Load Meshes**: File → Load Source/Target (OBJ format)
2. **Set Landmarks**: 
   - Select landmark type in Landmarks panel
   - Ctrl+Click on source mesh
   - Alt+Click on target mesh
3. **Align**: Click "Run ICP Alignment" in Alignment panel
4. **Wrap**: Adjust weights, click "Run Wrap Solve"
5. **Export**: File → Export Result

## Landmark Types

Required (minimum):
- LeftEyeCornerInner, LeftEyeCornerOuter
- RightEyeCornerInner, RightEyeCornerOuter
- NoseTip
- MouthCornerLeft, MouthCornerRight

Optional:
- ChinTip
- BrowLeftCenter, BrowRightCenter

## Controls

- **Left Drag**: Orbit camera
- **Middle Drag** or **Alt+Left Drag**: Pan camera
- **Scroll**: Zoom
- **Ctrl+Click**: Set source landmark
- **Alt+Click**: Set target landmark
- **F1**: Toggle Landmarks panel
- **F2**: Toggle Alignment panel
- **F3**: Toggle Log panel

## Extending the Code

### Adding a New Landmark Type

1. Add to `LandmarkType` enum in `LandmarkManager.h`:
```cpp
enum class LandmarkType {
    // ... existing types ...
    MyNewLandmark,
    COUNT
};
```

2. Update `getLandmarkName()` in `LandmarkManager.cpp`:
```cpp
case LandmarkType::MyNewLandmark: return "MyNewLandmark";
```

3. Add to UI dropdown in `LandmarksPanel.cpp`

### Adding a New Wrap Constraint

1. Modify `WrapSolver::solveIteration()` in `WrapSolver.cpp`
2. Add new term to energy computation
3. Add corresponding weight slider in `AlignmentPanel` or new panel

### Implementing SVD for Kabsch

Replace stub in `ICPAligner::kabschAlignment()`:

Option A - Use Eigen:
```cpp
#include <Eigen/Dense>

Eigen::Matrix3f H;
// ... populate H ...
Eigen::JacobiSVD<Eigen::Matrix3f> svd(H, Eigen::ComputeFullU | Eigen::ComputeFullV);
Eigen::Matrix3f R = svd.matrixV() * svd.matrixU().transpose();
// Convert R to quaternion and store in result.rotation
```

Option B - Implement custom 3x3 SVD (more complex)

## Common Issues

### Issue: Build fails with "cannot open file 'imgui.h'"
**Solution**: Download ImGui and place in `TopoShift/external/imgui/`

### Issue: Linker error LNK2001 with ImGui functions
**Solution**: Ensure all ImGui .cpp files are in project and compiled

### Issue: ICP alignment doesn't rotate correctly
**Solution**: Implement proper SVD in Kabsch algorithm (currently uses identity)

### Issue: Landmarks not visible in viewport
**Solution**: Complete `Renderer::renderLandmarks()` implementation

### Issue: Crash when clicking without meshes loaded
**Solution**: Add null checks in Application::handlePicking()

## Performance Tips

- **BVH**: Already implemented for O(log N) raycasting
- **Worker threads**: ICP and wrap already run in background
- **Sample count**: Reduce ICP sample count for faster alignment
- **Iterations**: Reduce wrap iterations for faster preview

## Debugging

Enable Visual Studio debugger:
1. Set breakpoints in relevant functions
2. Press F5 to debug
3. Inspect variables in Watch window

Useful breakpoints:
- `LandmarkManager::setLandmarkFromPick()` - check picking
- `ICPAligner::align()` - check ICP execution
- `WrapSolver::solve()` - check wrap execution
- `Application::handlePicking()` - check mouse input

## Testing

Create simple test meshes:
```python
# Python script to generate test sphere OBJ
import math

vertices = []
for i in range(20):
    theta = i * math.pi / 19
    for j in range(20):
        phi = j * 2 * math.pi / 19
        x = math.sin(theta) * math.cos(phi)
        y = math.cos(theta)
        z = math.sin(theta) * math.sin(phi)
        vertices.append(f"v {x} {y} {z}\n")

# ... generate faces ...
```

## References

- **ImGui**: https://github.com/ocry/imgui
- **DirectX 11**: https://docs.microsoft.com/en-us/windows/win32/direct3d11
- **Kabsch algorithm**: https://en.wikipedia.org/wiki/Kabsch_algorithm
- **ICP**: https://en.wikipedia.org/wiki/Iterative_closest_point

## Support

For issues or questions:
1. Check IMPLEMENTATION_NOTES.md for detailed status
2. Review code comments in header files
3. Consult Visual Studio build output for errors
