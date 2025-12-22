using ModelRigStudio.Core.Model;

namespace ModelRigStudio.Rigging;

/// <summary>
/// Computes and manipulates skin weights for skeletal meshes
/// </summary>
public class SkinWeightCalculator
{
    /// <summary>
    /// Computes skin weights using distance-to-bone with capsule approximation
    /// </summary>
    public static SkinData ComputeSkinWeights(Mesh mesh, Skeleton skeleton)
    {
        var skinData = new SkinData
        {
            Skeleton = skeleton
        };
        
        // Compute influence for each vertex
        for (int i = 0; i < mesh.Vertices.Count; i++)
        {
            var vertex = mesh.Vertices[i];
            var influences = new List<(int boneIndex, float weight)>();
            
            // Calculate distance to each bone
            for (int boneIdx = 0; boneIdx < skeleton.Bones.Count; boneIdx++)
            {
                var bone = skeleton.Bones[boneIdx];
                var distance = DistanceToBone(vertex, bone, skeleton);
                
                // Convert distance to weight with smooth falloff
                var weight = WeightFromDistance(distance);
                if (weight > 0.001f)
                {
                    influences.Add((boneIdx, weight));
                }
            }
            
            // Sort by weight and take top 4
            influences.Sort((a, b) => b.weight.CompareTo(a.weight));
            influences = influences.Take(4).ToList();
            
            // Normalize weights
            var totalWeight = influences.Sum(x => x.weight);
            if (totalWeight > 0)
            {
                for (int j = 0; j < influences.Count; j++)
                {
                    influences[j] = (influences[j].boneIndex, influences[j].weight / totalWeight);
                }
            }
            
            // Create bone influence
            var boneInfluence = new BoneInfluence(i);
            for (int j = 0; j < Math.Min(4, influences.Count); j++)
            {
                boneInfluence.BoneIndices[j] = influences[j].boneIndex;
                boneInfluence.Weights[j] = influences[j].weight;
            }
            
            skinData.Influences.Add(boneInfluence);
        }
        
        return skinData;
    }
    
    /// <summary>
    /// Smooths skin weights by averaging with neighboring vertices
    /// </summary>
    public static void SmoothWeights(Mesh mesh, int iterations = 1)
    {
        if (mesh.SkinData == null || mesh.Indices.Count == 0)
            return;
        
        for (int iter = 0; iter < iterations; iter++)
        {
            var newInfluences = new List<BoneInfluence>(mesh.SkinData.Influences);
            
            // Build adjacency
            var adjacency = BuildAdjacency(mesh);
            
            for (int i = 0; i < mesh.Vertices.Count; i++)
            {
                if (!adjacency.ContainsKey(i))
                    continue;
                
                var neighbors = adjacency[i];
                if (neighbors.Count == 0)
                    continue;
                
                // Average weights with neighbors
                var avgWeights = new Dictionary<int, float>();
                
                // Add current vertex weights
                var current = mesh.SkinData.Influences[i];
                for (int j = 0; j < 4; j++)
                {
                    if (current.Weights[j] > 0)
                    {
                        avgWeights[current.BoneIndices[j]] = current.Weights[j];
                    }
                }
                
                // Add neighbor weights
                foreach (var neighborIdx in neighbors)
                {
                    var neighbor = mesh.SkinData.Influences[neighborIdx];
                    for (int j = 0; j < 4; j++)
                    {
                        if (neighbor.Weights[j] > 0)
                        {
                            if (avgWeights.ContainsKey(neighbor.BoneIndices[j]))
                                avgWeights[neighbor.BoneIndices[j]] += neighbor.Weights[j];
                            else
                                avgWeights[neighbor.BoneIndices[j]] = neighbor.Weights[j];
                        }
                    }
                }
                
                // Normalize and take top 4
                var sorted = avgWeights.OrderByDescending(x => x.Value).Take(4).ToList();
                var total = sorted.Sum(x => x.Value);
                
                var newInfluence = new BoneInfluence(i);
                for (int j = 0; j < sorted.Count; j++)
                {
                    newInfluence.BoneIndices[j] = sorted[j].Key;
                    newInfluence.Weights[j] = sorted[j].Value / total;
                }
                
                newInfluences[i] = newInfluence;
            }
            
            mesh.SkinData.Influences = newInfluences;
        }
    }
    
    /// <summary>
    /// Mirrors weights from one side to another (placeholder - needs proper implementation)
    /// </summary>
    public static void MirrorWeights(Mesh mesh, string axis = "X")
    {
        // TODO: Implement proper weight mirroring
        // This requires finding corresponding vertices on the opposite side
        // and copying their bone weights
    }
    
    private static float DistanceToBone(Vector3 point, Bone bone, Skeleton skeleton)
    {
        // Simple point-to-point distance (capsule approximation would be better)
        var bonePos = bone.BindPose.Position;
        var offset = point - bonePos;
        return offset.Length();
    }
    
    private static float WeightFromDistance(float distance)
    {
        // Smooth falloff function
        var maxDistance = 2.0f;
        if (distance > maxDistance)
            return 0;
        
        var normalized = distance / maxDistance;
        return 1.0f - (normalized * normalized); // Quadratic falloff
    }
    
    private static Dictionary<int, HashSet<int>> BuildAdjacency(Mesh mesh)
    {
        var adjacency = new Dictionary<int, HashSet<int>>();
        
        for (int i = 0; i < mesh.Indices.Count; i += 3)
        {
            var i0 = mesh.Indices[i];
            var i1 = mesh.Indices[i + 1];
            var i2 = mesh.Indices[i + 2];
            
            AddEdge(adjacency, i0, i1);
            AddEdge(adjacency, i1, i2);
            AddEdge(adjacency, i2, i0);
        }
        
        return adjacency;
    }
    
    private static void AddEdge(Dictionary<int, HashSet<int>> adjacency, int v0, int v1)
    {
        if (!adjacency.ContainsKey(v0))
            adjacency[v0] = new HashSet<int>();
        if (!adjacency.ContainsKey(v1))
            adjacency[v1] = new HashSet<int>();
        
        adjacency[v0].Add(v1);
        adjacency[v1].Add(v0);
    }
}
