namespace ModelRigStudio.Core.Model;

/// <summary>
/// Represents a 3D transformation with position, rotation, and scale
/// </summary>
public class Transform
{
    public Vector3 Position { get; set; } = Vector3.Zero;
    public Quaternion Rotation { get; set; } = Quaternion.Identity;
    public Vector3 Scale { get; set; } = Vector3.One;

    public Transform Clone()
    {
        return new Transform
        {
            Position = Position,
            Rotation = Rotation,
            Scale = Scale
        };
    }
}
