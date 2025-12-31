# TopoShift - Topology Transfer Application

TopoShift is a standalone C++20 Win32 + DirectX 11 + ImGui application for mesh topology transfer with landmark-based alignment.

## Features

- **Landmark System**: Metapipe-style semantic landmarks (eyes, nose, mouth, etc.)
- **ICP Alignment**: Landmark-only and hybrid ICP with Kabsch/Umeyama algorithms
- **Wrap Solver**: Landmark-constrained topology transfer with configurable weights
- **Interactive UI**: ImGui-based interface with viewport picking and visualization

## Building

### Requirements
- Visual Studio 2022 or later (configured for C++20)
- Windows SDK 10.0 or later
- DirectX 11

### Dependencies
The project includes ImGui in the `external/imgui` directory. You need to download ImGui v1.89 or later:

1. Download ImGui from https://github.com/ocry/imgui/releases
2. Extract to `TopoShift/external/imgui`
3. Ensure the following files are present:
   - imgui.h, imgui.cpp
   - imgui_internal.h
   - imgui_demo.cpp, imgui_draw.cpp, imgui_tables.cpp, imgui_widgets.cpp
   - backends/imgui_impl_win32.h, backends/imgui_impl_win32.cpp
   - backends/imgui_impl_dx11.h, backends/imgui_impl_dx11.cpp

### Build Steps
1. Open `TopoShift/TopoShift.sln` in Visual Studio
2. Select Debug or Release configuration
3. Build Solution (F7)
4. Run (F5)

## Usage

### Basic Workflow

1. **Load Meshes**
   - File → Load Source Mesh (OBJ format)
   - File → Load Target Mesh (OBJ format)

2. **Set Landmarks**
   - Select landmark type from Landmarks panel
   - Ctrl+Click on Source mesh to set source landmark
   - Alt+Click on Target mesh to set target landmark
   - Repeat for all required landmarks (minimum: eye corners, nose tip, mouth corners)

3. **Run ICP Alignment** (optional but recommended)
   - Open Alignment panel
   - Configure ICP parameters (iterations, sample count, etc.)
   - Click "Run ICP Alignment"
   - Review RMS errors in the panel

4. **Run Topology Transfer**
   - Configure landmark weight and falloff radius
   - Enable/disable pinned landmarks as needed
   - Click "Run Transfer"
   - Wait for processing to complete

5. **Export Result**
   - File → Export Result Mesh
   - Save as OBJ file

### Landmark Types

The following semantic landmarks are supported:

- **LeftEyeCornerInner**: Inner corner of left eye
- **LeftEyeCornerOuter**: Outer corner of left eye
- **RightEyeCornerInner**: Inner corner of right eye
- **RightEyeCornerOuter**: Outer corner of right eye
- **NoseTip**: Tip of the nose
- **MouthCornerLeft**: Left corner of mouth
- **MouthCornerRight**: Right corner of mouth
- **ChinTip** (optional): Tip of chin
- **BrowLeftCenter** (optional): Center of left eyebrow
- **BrowRightCenter** (optional): Center of right eyebrow

### Landmark JSON Schema

Landmarks can be saved/loaded from JSON files. The schema is:

```json
{
  "source_mesh": "path/to/source.obj",
  "target_mesh": "path/to/target.obj",
  "landmarks": [
    {
      "type": "LeftEyeCornerInner",
      "source": {
        "mode": "triangle",
        "triangle_index": 1234,
        "barycentric": [0.3, 0.5, 0.2],
        "world_position": [1.5, 2.3, -0.5]
      },
      "target": {
        "mode": "vertex",
        "vertex_index": 567,
        "world_position": [1.6, 2.4, -0.4]
      },
      "enabled": true
    }
  ]
}
```

### Controls

- **Mouse**: 
  - Left drag: Rotate camera
  - Right drag: Pan camera
  - Scroll: Zoom
  - Ctrl+Click: Set source landmark
  - Alt+Click: Set target landmark

- **Keyboard**:
  - F1: Toggle Landmarks panel
  - F2: Toggle Alignment panel
  - F3: Toggle Log panel
  - Esc: Clear current selection

## Architecture

### Core Modules

- **Application**: Main application loop, window management
- **Renderer**: DirectX 11 rendering pipeline
- **Mesh**: Mesh data structure with BVH for raycasting
- **Camera**: 3D camera with orbit controls
- **LandmarkManager**: Landmark storage, serialization, and picking
- **ICPAligner**: Kabsch/Umeyama alignment and hybrid ICP
- **WrapSolver**: Topology transfer with landmark constraints

### UI Panels

- **LandmarksPanel**: Landmark management and visualization controls
- **AlignmentPanel**: ICP alignment parameters and execution
- **MainUI**: Main menu bar and viewport controls

### Pipeline Order

1. **Validation**: Verify meshes are loaded
2. **Landmark Setup**: Set landmarks (optional but recommended)
3. **ICP Alignment**: Align target to source (optional, default ON)
4. **Conform/Wrapping**: Project or wrap solve with landmark constraints
5. **Refine Shrinkwrap**: Optional refinement pass
6. **Data Transfer**: Transfer attributes with smoothing
7. **Finalize Export**: Export result mesh

## Technical Details

### ICP Alignment

The ICP aligner supports:
- Landmark-only alignment using Kabsch algorithm (rigid transform)
- Umeyama algorithm for alignment with uniform scale
- Hybrid ICP combining landmarks and sampled surface points
- Outlier rejection based on distance threshold
- Early stopping on convergence

### Wrap Solver

The wrap solver integrates landmark constraints into the objective:

```
E = E_data + w_landmark * E_landmarks + w_smooth * E_smooth
```

Where:
- `E_data`: Data term (nearest projection)
- `E_landmarks`: Landmark constraint term with falloff
- `E_smooth`: Smoothness term

Landmark falloff is computed using Euclidean distance from landmark vertices.

### Thread Safety

ICP alignment and wrap solving run in worker threads. The UI uses thread-safe snapshots to read state without blocking the solver.

## License

See LICENSE file for details.
