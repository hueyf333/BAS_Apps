using System.Collections.ObjectModel;

namespace ModelRigStudio.Core.Model;

/// <summary>
/// Represents a node in the scene graph with transform and children
/// </summary>
public class Node
{
    public string Name { get; set; } = "Node";
    public Transform Transform { get; set; } = new();
    public Node? Parent { get; set; }
    public ObservableCollection<Node> Children { get; } = new();
    
    // Optional components attached to node
    public Mesh? Mesh { get; set; }
    
    public void AddChild(Node child)
    {
        child.Parent = this;
        Children.Add(child);
    }
    
    public void RemoveChild(Node child)
    {
        if (Children.Remove(child))
        {
            child.Parent = null;
        }
    }
}
