namespace ModelRigStudio.Core.Model;

/// <summary>
/// Represents skin binding data for skeletal animation
/// </summary>
public class SkinData
{
    // Per-vertex bone influences (max 4 per vertex)
    public List<BoneInfluence> Influences { get; set; } = new();
    
    // Reference to skeleton
    public Skeleton? Skeleton { get; set; }
}

/// <summary>
/// Bone influence for a single vertex
/// </summary>
public struct BoneInfluence
{
    public int VertexIndex { get; set; }
    public int[] BoneIndices { get; set; } // Max 4
    public float[] Weights { get; set; }   // Max 4, normalized

    public BoneInfluence(int vertexIndex)
    {
        VertexIndex = vertexIndex;
        BoneIndices = new int[4];
        Weights = new float[4];
    }
}
