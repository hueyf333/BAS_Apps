# BAS_Apps

This repository contains the TopoShift application - a standalone C++20 topology transfer tool with landmark-based alignment.

## TopoShift

TopoShift is a Windows application for transferring topology between meshes using:
- **Landmark System**: Semantic markers (eyes, nose, mouth) for correspondence
- **ICP Alignment**: Kabsch/Umeyama algorithms with hybrid ICP refinement  
- **Wrap Solver**: Landmark-constrained deformation

### Quick Links

- **User Guide**: [TopoShift/README.md](TopoShift/README.md)
- **Developer Guide**: [TopoShift/DEVELOPER_GUIDE.md](TopoShift/DEVELOPER_GUIDE.md)
- **Implementation Notes**: [TopoShift/IMPLEMENTATION_NOTES.md](TopoShift/IMPLEMENTATION_NOTES.md)

### Getting Started

1. Install Visual Studio 2022+ with C++ tools
2. Download ImGui and place in `TopoShift/external/imgui/`
3. Open `TopoShift/TopoShift.sln`
4. Build and run

See [TopoShift/DEVELOPER_GUIDE.md](TopoShift/DEVELOPER_GUIDE.md) for detailed setup instructions.