using ModelRigStudio.Core.Model;

namespace ModelRigStudio.Rigging;

/// <summary>
/// Implements automatic rigging using simple heuristics
/// </summary>
public class AutoRigger
{
    /// <summary>
    /// Creates a generic skeleton for any mesh using PCA and extremity detection
    /// </summary>
    public static Skeleton AutoRigGeneric(Mesh mesh)
    {
        if (mesh.Vertices.Count == 0)
        {
            return new Skeleton { Name = "Empty_Skeleton" };
        }

        var skeleton = new Skeleton { Name = $"{mesh.Name}_Skeleton" };
        
        // Calculate mesh center
        var center = CalculateCenter(mesh.Vertices);
        
        // Perform PCA to find main axis (trunk direction)
        var mainAxis = PerformPCA(mesh.Vertices, center);
        
        // Find extremities using farthest point sampling
        var extremities = FindExtremities(mesh.Vertices, center, 6);
        
        // Create root bone at center
        var rootBone = new Bone
        {
            Name = "Root",
            ParentIndex = -1,
            BindPose = new Transform { Position = center }
        };
        skeleton.Bones.Add(rootBone);
        
        // Create trunk bones along main axis
        var trunkTop = center + mainAxis * GetMeshExtent(mesh.Vertices, center, mainAxis);
        var trunkBottom = center - mainAxis * GetMeshExtent(mesh.Vertices, center, mainAxis * -1);
        
        var spine1 = new Bone
        {
            Name = "Spine1",
            ParentIndex = 0,
            BindPose = new Transform { Position = trunkBottom }
        };
        skeleton.Bones.Add(spine1);
        
        var spine2 = new Bone
        {
            Name = "Spine2",
            ParentIndex = 1,
            BindPose = new Transform { Position = center }
        };
        skeleton.Bones.Add(spine2);
        
        var spine3 = new Bone
        {
            Name = "Spine3",
            ParentIndex = 2,
            BindPose = new Transform { Position = trunkTop }
        };
        skeleton.Bones.Add(spine3);
        
        // Create limb bones from extremities
        for (int i = 0; i < extremities.Count; i++)
        {
            var limbBone = new Bone
            {
                Name = $"Limb{i}",
                ParentIndex = 2, // Attach to mid spine
                BindPose = new Transform { Position = extremities[i] }
            };
            skeleton.Bones.Add(limbBone);
        }
        
        return skeleton;
    }
    
    private static Vector3 CalculateCenter(List<Vector3> vertices)
    {
        var sum = Vector3.Zero;
        foreach (var v in vertices)
        {
            sum = sum + v;
        }
        return sum / vertices.Count;
    }
    
    private static Vector3 PerformPCA(List<Vector3> vertices, Vector3 center)
    {
        // Simplified PCA - find direction of maximum variance
        // Calculate covariance matrix and find eigenvector
        float xx = 0, yy = 0, zz = 0;
        
        foreach (var v in vertices)
        {
            var centered = v - center;
            xx += centered.X * centered.X;
            yy += centered.Y * centered.Y;
            zz += centered.Z * centered.Z;
        }
        
        // Return axis with maximum variance as approximation
        if (xx > yy && xx > zz)
            return Vector3.UnitX;
        else if (yy > zz)
            return Vector3.UnitY;
        else
            return Vector3.UnitZ;
    }
    
    private static float GetMeshExtent(List<Vector3> vertices, Vector3 center, Vector3 direction)
    {
        float maxDist = 0;
        foreach (var v in vertices)
        {
            var offset = v - center;
            var dist = Vector3.Dot(offset, direction);
            if (dist > maxDist)
                maxDist = dist;
        }
        return maxDist;
    }
    
    private static List<Vector3> FindExtremities(List<Vector3> vertices, Vector3 center, int count)
    {
        var extremities = new List<Vector3>();
        
        // Simple approach: find vertices farthest from center in different directions
        var directions = new[]
        {
            Vector3.UnitX, -Vector3.UnitX,
            Vector3.UnitY, -Vector3.UnitY,
            Vector3.UnitZ, -Vector3.UnitZ
        };
        
        foreach (var dir in directions.Take(count))
        {
            Vector3 farthest = center;
            float maxDist = 0;
            
            foreach (var v in vertices)
            {
                var offset = v - center;
                var dist = Vector3.Dot(offset, dir);
                if (dist > maxDist)
                {
                    maxDist = dist;
                    farthest = v;
                }
            }
            
            if (maxDist > 0.01f) // Only add if significant distance
            {
                extremities.Add(farthest);
            }
        }
        
        return extremities;
    }
}
