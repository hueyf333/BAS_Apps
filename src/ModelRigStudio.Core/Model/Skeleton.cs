namespace ModelRigStudio.Core.Model;

/// <summary>
/// Represents a skeleton for animation
/// </summary>
public class Skeleton
{
    public string Name { get; set; } = "Skeleton";
    public List<Bone> Bones { get; set; } = new();
}

/// <summary>
/// Represents a bone in a skeleton
/// </summary>
public class Bone
{
    public string Name { get; set; } = "Bone";
    public int ParentIndex { get; set; } = -1; // -1 means root bone
    public Transform BindPose { get; set; } = new();
}
