using System.Collections.ObjectModel;

namespace ModelRigStudio.Core.Model;

/// <summary>
/// Represents the entire 3D scene
/// </summary>
public class Scene
{
    public string Name { get; set; } = "Scene";
    public Node RootNode { get; set; } = new() { Name = "Root" };
    public ObservableCollection<Material> Materials { get; } = new();
    public ObservableCollection<TextureAsset> Textures { get; } = new();
}
