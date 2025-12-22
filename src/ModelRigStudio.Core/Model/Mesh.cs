namespace ModelRigStudio.Core.Model;

/// <summary>
/// Represents mesh geometry data
/// </summary>
public class Mesh
{
    public string Name { get; set; } = "Mesh";
    
    // Vertex data
    public List<Vector3> Vertices { get; set; } = new();
    public List<int> Indices { get; set; } = new();
    public List<Vector3> Normals { get; set; } = new();
    public List<Vector3> Tangents { get; set; } = new();
    public List<Vector2> UVs { get; set; } = new();
    
    // Material
    public Material? Material { get; set; }
    
    // Skinning data
    public SkinData? SkinData { get; set; }
}
