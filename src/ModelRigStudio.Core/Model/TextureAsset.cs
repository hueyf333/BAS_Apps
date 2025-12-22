namespace ModelRigStudio.Core.Model;

/// <summary>
/// Represents a texture asset
/// </summary>
public class TextureAsset
{
    public string Name { get; set; } = "Texture";
    public string FilePath { get; set; } = string.Empty;
    public int Width { get; set; }
    public int Height { get; set; }
}
