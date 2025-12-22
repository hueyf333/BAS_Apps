# ModelRigStudio

A WPF-based 3D modeling and rigging application built with .NET 8.

## Features

- **Multi-Project Architecture**
  - `Core`: Data model, undo/redo system, command infrastructure
  - `Render`: Viewport rendering with HelixToolkit integration (placeholder)
  - `ImportExport`: Model import/export via AssimpNet (OBJ, GLTF, GLB)
  - `Rigging`: Auto-rigging with PCA-based skeleton generation
  - `Materials`: Material and texture management
  - `App`: WPF UI shell with MVVM architecture

- **Import/Export**
  - Import OBJ, GLTF, GLB models
  - Export to OBJ, GLTF, GLB formats
  - Automatic texture asset management
  - Mesh normalization (triangulation, normals, tangents)

- **Auto-Rigging**
  - Generic auto-rigging using PCA for trunk detection
  - Extremity detection via farthest-point sampling
  - Automatic skin weight computation with distance-to-bone weighting
  - Skin weight smoothing and manipulation tools

- **User Interface**
  - Dark theme with Visual Studio-inspired styling
  - Outliner panel for scene hierarchy navigation
  - 3D Viewport (HelixToolkit placeholder)
  - Inspector panel with Node/Mesh/Rig/Materials tabs
  - Log/Console panel for messages and warnings
  - Comprehensive menu system with keyboard shortcuts

- **Material System**
  - PBR material support (BaseColor, Normal, Roughness, Metallic, AO, Emissive)
  - Texture import and management
  - Material assignment to mesh slots

## Building

Requires:
- .NET 8 SDK or later
- Windows (for WPF)

Build the solution:
```bash
dotnet build ModelRigStudio.sln
```

Run the application:
```bash
dotnet run --project src/ModelRigStudio.App/ModelRigStudio.App.csproj
```

## Usage

1. **Import a Model**: File → Import Model (Ctrl+I)
2. **Select Node**: Click on node in Outliner
3. **Auto-Rig**: Tools → Auto-Rig → Generic
4. **Smooth Weights**: Tools → Smooth Weights
5. **Export**: File → Export Model (Ctrl+E)

## Keyboard Shortcuts

- `Ctrl+N`: New file
- `Ctrl+O`: Open file
- `Ctrl+S`: Save file
- `Ctrl+I`: Import model
- `Ctrl+E`: Export model
- `Ctrl+Z`: Undo
- `Ctrl+Y`: Redo
- `Ctrl+D`: Duplicate
- `Delete`: Delete selected
- `1-5`: Switch render modes

## Architecture

The application follows MVVM pattern with:
- ViewModels using CommunityToolkit.Mvvm
- Services for document state and logging
- Command pattern for undo/redo
- Reactive UI updates via ObservableCollection

## Placeholders & TODOs

Some features are implemented as placeholders:
- HelixToolkit 3D viewport integration (basic structure in place)
- Auto-Rig Guided mode
- Mirror Weights
- Recalculate Normals
- Generate Tangents
- Auto-Unwrap UV
- Texture painting/editing
- File serialization/deserialization

## License

See LICENSE file for details.