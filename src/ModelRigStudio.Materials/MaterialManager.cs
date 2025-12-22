using ModelRigStudio.Core.Model;

namespace ModelRigStudio.Materials;

/// <summary>
/// Manages material operations
/// </summary>
public class MaterialManager
{
    /// <summary>
    /// Creates a default PBR material
    /// </summary>
    public static Material CreateDefaultMaterial(string name = "Material")
    {
        return new Material
        {
            Name = name,
            BaseColor = new Vector3(0.8f, 0.8f, 0.8f),
            Roughness = 0.5f,
            Metallic = 0.0f,
            Emissive = Vector3.Zero
        };
    }
    
    /// <summary>
    /// Assigns a material to a mesh
    /// </summary>
    public static void AssignMaterial(Mesh mesh, Material material)
    {
        mesh.Material = material;
    }
}
