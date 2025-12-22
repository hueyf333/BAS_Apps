using ModelRigStudio.Core.Model;

namespace ModelRigStudio.Render;

/// <summary>
/// Manages viewport rendering state and operations
/// Placeholder for HelixToolkit integration
/// </summary>
public class ViewportManager
{
    public RenderMode CurrentRenderMode { get; set; } = RenderMode.Shaded;
    public Node? SelectedNode { get; set; }
    
    /// <summary>
    /// Sets the current render mode
    /// </summary>
    public void SetRenderMode(RenderMode mode)
    {
        CurrentRenderMode = mode;
        // TODO: Update viewport rendering based on mode
    }
    
    /// <summary>
    /// Updates the scene to render
    /// </summary>
    public void UpdateScene(Scene scene)
    {
        // TODO: Convert scene to HelixToolkit models
    }
    
    /// <summary>
    /// Highlights the selected node
    /// </summary>
    public void HighlightSelection(Node? node)
    {
        SelectedNode = node;
        // TODO: Update visual highlighting
    }
}
