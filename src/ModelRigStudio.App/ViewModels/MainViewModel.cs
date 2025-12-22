using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using ModelRigStudio.App.Services;
using ModelRigStudio.Core.Model;
using ModelRigStudio.ImportExport;
using ModelRigStudio.Rigging;
using ModelRigStudio.Render;
using Microsoft.Win32;
using System.Windows;

namespace ModelRigStudio.App.ViewModels;

/// <summary>
/// Main ViewModel for the application
/// </summary>
public partial class MainViewModel : ObservableObject
{
    private readonly DocumentService _documentService = DocumentService.Instance;
    private readonly LogService _logService = LogService.Instance;

    [ObservableProperty]
    private SceneDocument _document;

    [ObservableProperty]
    private Node? _selectedNode;

    [ObservableProperty]
    private RenderMode _currentRenderMode = RenderMode.Shaded;

    public MainViewModel()
    {
        _document = _documentService.Document;
        _documentService.PropertyChanged += (s, e) =>
        {
            if (e.PropertyName == nameof(DocumentService.Document))
                Document = _documentService.Document;
            if (e.PropertyName == nameof(DocumentService.SelectedNode))
                SelectedNode = _documentService.SelectedNode;
        };
    }

    #region File Commands

    [RelayCommand]
    private void NewFile()
    {
        _documentService.NewDocument();
        _logService.Info("Created new document");
    }

    [RelayCommand]
    private void OpenFile()
    {
        // TODO: Implement file open dialog and deserialization
        _logService.Warning("Open file not fully implemented - placeholder");
        MessageBox.Show("Open File functionality - TODO", "ModelRigStudio", MessageBoxButton.OK);
    }

    [RelayCommand]
    private void SaveFile()
    {
        // TODO: Implement file save and serialization
        _logService.Warning("Save file not fully implemented - placeholder");
        MessageBox.Show("Save File functionality - TODO", "ModelRigStudio", MessageBoxButton.OK);
    }

    [RelayCommand]
    private void SaveFileAs()
    {
        // TODO: Implement save as dialog and serialization
        _logService.Warning("Save As not fully implemented - placeholder");
        MessageBox.Show("Save As functionality - TODO", "ModelRigStudio", MessageBoxButton.OK);
    }

    [RelayCommand]
    private async Task ImportModel()
    {
        try
        {
            var openFileDialog = new OpenFileDialog
            {
                Filter = "3D Models (*.obj;*.gltf;*.glb)|*.obj;*.gltf;*.glb|All files (*.*)|*.*",
                Title = "Import Model"
            };

            if (openFileDialog.ShowDialog() == true)
            {
                var importer = new ModelImporter(_documentService.AssetsFolder);
                await Task.Run(() =>
                {
                    var importedScene = importer.Import(openFileDialog.FileName);
                    
                    Application.Current.Dispatcher.Invoke(() =>
                    {
                        // Merge imported scene into current document
                        foreach (var child in importedScene.RootNode.Children)
                        {
                            _documentService.Document.Scene.RootNode.AddChild(child);
                        }
                        
                        _logService.Info($"Imported model: {openFileDialog.FileName}");
                    });
                });
            }
        }
        catch (Exception ex)
        {
            _logService.Error($"Failed to import model: {ex.Message}");
            MessageBox.Show($"Import failed: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }
    }

    [RelayCommand]
    private void ImportTexture()
    {
        try
        {
            var openFileDialog = new OpenFileDialog
            {
                Filter = "Images (*.png;*.jpg;*.jpeg;*.bmp)|*.png;*.jpg;*.jpeg;*.bmp|All files (*.*)|*.*",
                Title = "Import Texture"
            };

            if (openFileDialog.ShowDialog() == true)
            {
                var texture = Materials.TextureUtilities.ImportTexture(
                    openFileDialog.FileName,
                    _documentService.AssetsFolder
                );
                
                if (texture != null)
                {
                    _documentService.Document.Scene.Textures.Add(texture);
                    _logService.Info($"Imported texture: {openFileDialog.FileName}");
                }
            }
        }
        catch (Exception ex)
        {
            _logService.Error($"Failed to import texture: {ex.Message}");
            MessageBox.Show($"Texture import failed: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }
    }

    [RelayCommand]
    private async Task ExportModel()
    {
        try
        {
            var saveFileDialog = new SaveFileDialog
            {
                Filter = "GLTF (*.gltf)|*.gltf|GLB (*.glb)|*.glb|OBJ (*.obj)|*.obj",
                Title = "Export Model"
            };

            if (saveFileDialog.ShowDialog() == true)
            {
                await Task.Run(() =>
                {
                    var ext = System.IO.Path.GetExtension(saveFileDialog.FileName).ToLower();
                    switch (ext)
                    {
                        case ".obj":
                            ModelExporter.ExportOBJ(_documentService.Document.Scene, saveFileDialog.FileName);
                            break;
                        case ".gltf":
                            ModelExporter.ExportGLTF(_documentService.Document.Scene, saveFileDialog.FileName);
                            break;
                        case ".glb":
                            ModelExporter.ExportGLB(_documentService.Document.Scene, saveFileDialog.FileName);
                            break;
                    }
                    
                    Application.Current.Dispatcher.Invoke(() =>
                    {
                        _logService.Info($"Exported model: {saveFileDialog.FileName}");
                    });
                });
            }
        }
        catch (Exception ex)
        {
            _logService.Error($"Failed to export model: {ex.Message}");
            MessageBox.Show($"Export failed: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }
    }

    [RelayCommand]
    private void ExportSelected()
    {
        // TODO: Implement export selected only
        _logService.Warning("Export Selected not fully implemented - placeholder");
        MessageBox.Show("Export Selected functionality - TODO", "ModelRigStudio", MessageBoxButton.OK);
    }

    [RelayCommand]
    private void Exit()
    {
        Application.Current.Shutdown();
    }

    #endregion

    #region Edit Commands

    [RelayCommand(CanExecute = nameof(CanUndo))]
    private void Undo()
    {
        _documentService.UndoRedoStack.Undo();
        _logService.Info("Undo");
    }

    private bool CanUndo() => _documentService.UndoRedoStack.CanUndo;

    [RelayCommand(CanExecute = nameof(CanRedo))]
    private void Redo()
    {
        _documentService.UndoRedoStack.Redo();
        _logService.Info("Redo");
    }

    private bool CanRedo() => _documentService.UndoRedoStack.CanRedo;

    [RelayCommand(CanExecute = nameof(HasSelection))]
    private void Duplicate()
    {
        // TODO: Implement node duplication
        _logService.Warning("Duplicate not fully implemented - placeholder");
        MessageBox.Show("Duplicate functionality - TODO", "ModelRigStudio", MessageBoxButton.OK);
    }

    [RelayCommand(CanExecute = nameof(HasSelection))]
    private void Delete()
    {
        if (SelectedNode != null && SelectedNode.Parent != null)
        {
            SelectedNode.Parent.RemoveChild(SelectedNode);
            SelectedNode = null;
            _documentService.SelectedNode = null;
            _logService.Info("Deleted node");
        }
    }

    private bool HasSelection() => SelectedNode != null;

    #endregion

    #region View Commands

    [RelayCommand]
    private void SetRenderModeShaded()
    {
        CurrentRenderMode = RenderMode.Shaded;
        _logService.Info("Render mode: Shaded");
    }

    [RelayCommand]
    private void SetRenderModeWireframe()
    {
        CurrentRenderMode = RenderMode.Wireframe;
        _logService.Info("Render mode: Wireframe");
    }

    [RelayCommand]
    private void SetRenderModeNormals()
    {
        CurrentRenderMode = RenderMode.Normals;
        _logService.Info("Render mode: Normals");
    }

    [RelayCommand]
    private void SetRenderModeUV()
    {
        CurrentRenderMode = RenderMode.UV;
        _logService.Info("Render mode: UV");
    }

    [RelayCommand]
    private void SetRenderModeWeights()
    {
        CurrentRenderMode = RenderMode.Weights;
        _logService.Info("Render mode: Weights");
    }

    [RelayCommand]
    private void TogglePanels()
    {
        _logService.Warning("Toggle Panels not implemented - placeholder");
    }

    [RelayCommand]
    private void ResetLayout()
    {
        _logService.Info("Reset layout");
    }

    #endregion

    #region Tools Commands

    [RelayCommand(CanExecute = nameof(HasMeshSelection))]
    private async Task AutoRigGeneric()
    {
        if (SelectedNode?.Mesh == null) return;

        try
        {
            var node = SelectedNode; // Capture for closure
            await Task.Run(() =>
            {
                var skeleton = AutoRigger.AutoRigGeneric(node.Mesh);
                var skinData = SkinWeightCalculator.ComputeSkinWeights(node.Mesh, skeleton);
                
                Application.Current.Dispatcher.Invoke(() =>
                {
                    node.Mesh.SkinData = skinData;
                    _logService.Info($"Auto-rigged mesh with {skeleton.Bones.Count} bones");
                });
            });
        }
        catch (Exception ex)
        {
            _logService.Error($"Auto-rig failed: {ex.Message}");
            MessageBox.Show($"Auto-rig failed: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }
    }

    [RelayCommand]
    private void AutoRigGuided()
    {
        _logService.Warning("Auto-Rig Guided not fully implemented - placeholder");
        MessageBox.Show("Auto-Rig Guided functionality - TODO", "ModelRigStudio", MessageBoxButton.OK);
    }

    [RelayCommand(CanExecute = nameof(HasMeshSelection))]
    private void BindSkin()
    {
        if (SelectedNode?.Mesh == null) return;
        
        // TODO: Implement bind skin with existing skeleton
        _logService.Warning("Bind Skin not fully implemented - placeholder");
        MessageBox.Show("Bind Skin functionality - TODO", "ModelRigStudio", MessageBoxButton.OK);
    }

    [RelayCommand(CanExecute = nameof(HasSkinData))]
    private void RecomputeWeights()
    {
        if (SelectedNode?.Mesh?.SkinData?.Skeleton == null) return;
        
        var skinData = SkinWeightCalculator.ComputeSkinWeights(
            SelectedNode.Mesh,
            SelectedNode.Mesh.SkinData.Skeleton
        );
        SelectedNode.Mesh.SkinData = skinData;
        _logService.Info("Recomputed skin weights");
    }

    [RelayCommand(CanExecute = nameof(HasSkinData))]
    private void SmoothWeights()
    {
        if (SelectedNode?.Mesh == null) return;
        
        SkinWeightCalculator.SmoothWeights(SelectedNode.Mesh, iterations: 3);
        _logService.Info("Smoothed skin weights");
    }

    [RelayCommand(CanExecute = nameof(HasSkinData))]
    private void MirrorWeights()
    {
        _logService.Warning("Mirror Weights not fully implemented - placeholder");
        MessageBox.Show("Mirror Weights functionality - TODO", "ModelRigStudio", MessageBoxButton.OK);
    }

    [RelayCommand(CanExecute = nameof(HasMeshSelection))]
    private void RecalculateNormals()
    {
        _logService.Warning("Recalculate Normals not fully implemented - placeholder");
        MessageBox.Show("Recalculate Normals functionality - TODO", "ModelRigStudio", MessageBoxButton.OK);
    }

    [RelayCommand(CanExecute = nameof(HasMeshSelection))]
    private void GenerateTangents()
    {
        _logService.Warning("Generate Tangents not fully implemented - placeholder");
        MessageBox.Show("Generate Tangents functionality - TODO", "ModelRigStudio", MessageBoxButton.OK);
    }

    [RelayCommand(CanExecute = nameof(HasMeshSelection))]
    private void AutoUnwrap()
    {
        _logService.Warning("Auto-Unwrap not fully implemented - placeholder");
        MessageBox.Show("Auto-Unwrap functionality - TODO", "ModelRigStudio", MessageBoxButton.OK);
    }

    private bool HasMeshSelection() => SelectedNode?.Mesh != null;
    private bool HasSkinData() => SelectedNode?.Mesh?.SkinData != null;

    #endregion

    #region Help Commands

    [RelayCommand]
    private void ShowAbout()
    {
        MessageBox.Show(
            "ModelRigStudio v1.0\n\n" +
            "A 3D modeling and rigging application built with WPF.\n\n" +
            "Features:\n" +
            "- Import/Export OBJ, GLTF, GLB\n" +
            "- Auto-rigging with PCA-based skeleton generation\n" +
            "- Skin weight computation and smoothing\n" +
            "- Material and texture management\n" +
            "- Multiple render modes",
            "About ModelRigStudio",
            MessageBoxButton.OK,
            MessageBoxImage.Information
        );
    }

    #endregion
}
