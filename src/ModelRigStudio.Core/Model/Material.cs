namespace ModelRigStudio.Core.Model;

/// <summary>
/// Represents a PBR material with texture slots
/// </summary>
public class Material
{
    public string Name { get; set; } = "Material";
    
    // Texture slots
    public TextureAsset? BaseColorTexture { get; set; }
    public TextureAsset? NormalTexture { get; set; }
    public TextureAsset? RoughnessTexture { get; set; }
    public TextureAsset? MetallicTexture { get; set; }
    public TextureAsset? AOTexture { get; set; }
    public TextureAsset? EmissiveTexture { get; set; }
    
    // Scalar parameters
    public Vector3 BaseColor { get; set; } = Vector3.One;
    public float Roughness { get; set; } = 0.5f;
    public float Metallic { get; set; } = 0.0f;
    public Vector3 Emissive { get; set; } = Vector3.Zero;
}
