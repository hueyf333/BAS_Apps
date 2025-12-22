using Assimp;
using Assimp.Configs;
using ModelRigStudio.Core.Model;
using AssimpMesh = Assimp.Mesh;
using AssimpNode = Assimp.Node;
using CoreMesh = ModelRigStudio.Core.Model.Mesh;
using CoreNode = ModelRigStudio.Core.Model.Node;

namespace ModelRigStudio.ImportExport;

/// <summary>
/// Export options for model export
/// </summary>
public class ExportOptions
{
    public bool ApplyTransforms { get; set; } = false;
    public bool EmbedTextures { get; set; } = false;
    public bool ExportSelectedOnly { get; set; } = false;
}

/// <summary>
/// Exports 3D models using AssimpNet
/// </summary>
public class ModelExporter
{
    /// <summary>
    /// Export scene to OBJ format
    /// </summary>
    public static void ExportOBJ(Core.Model.Scene scene, string filePath, ExportOptions? options = null)
    {
        Export(scene, filePath, "obj", options);
    }
    
    /// <summary>
    /// Export scene to GLTF format
    /// </summary>
    public static void ExportGLTF(Core.Model.Scene scene, string filePath, ExportOptions? options = null)
    {
        Export(scene, filePath, "gltf2", options);
    }
    
    /// <summary>
    /// Export scene to GLB format
    /// </summary>
    public static void ExportGLB(Core.Model.Scene scene, string filePath, ExportOptions? options = null)
    {
        Export(scene, filePath, "glb2", options);
    }
    
    private static void Export(Core.Model.Scene scene, string filePath, string formatId, ExportOptions? options)
    {
        options ??= new ExportOptions();
        
        var assimpScene = new Assimp.Scene();
        
        // Create root node
        assimpScene.RootNode = new AssimpNode("Root");
        
        // Convert scene hierarchy
        ConvertNode(scene.RootNode, assimpScene.RootNode, assimpScene, options);
        
        // Export
        using var exporter = new AssimpContext();
        exporter.ExportFile(assimpScene, filePath, formatId);
    }
    
    private static void ConvertNode(CoreNode node, AssimpNode assimpNode, Assimp.Scene assimpScene, ExportOptions options)
    {
        foreach (var child in node.Children)
        {
            var assimpChild = new AssimpNode(child.Name);
            
            // Set transform
            var pos = child.Transform.Position;
            var rot = child.Transform.Rotation;
            var scale = child.Transform.Scale;
            
            // Convert to Assimp types
            var translation = new Assimp.Vector3D(pos.X, pos.Y, pos.Z);
            var rotation = new Assimp.Quaternion(rot.W, rot.X, rot.Y, rot.Z); // W first for Assimp
            var scaleVec = new Assimp.Vector3D(scale.X, scale.Y, scale.Z);
            
            // Build transformation matrix
            var matrix = Assimp.Matrix4x4.FromScaling(scaleVec);
            matrix *= new Assimp.Matrix4x4(rotation.GetMatrix());
            matrix *= Assimp.Matrix4x4.FromTranslation(translation);
            
            assimpChild.Transform = matrix;
            
            // Convert mesh if present
            if (child.Mesh != null)
            {
                var meshIndex = assimpScene.Meshes.Count;
                var assimpMesh = ConvertMesh(child.Mesh);
                assimpScene.Meshes.Add(assimpMesh);
                assimpChild.MeshIndices.Add(meshIndex);
                
                // Add material
                if (child.Mesh.Material != null)
                {
                    var materialIndex = assimpScene.Materials.Count;
                    var assimpMaterial = ConvertMaterial(child.Mesh.Material);
                    assimpScene.Materials.Add(assimpMaterial);
                    assimpMesh.MaterialIndex = materialIndex;
                }
            }
            
            assimpNode.Children.Add(assimpChild);
            
            // Recursively convert children
            ConvertNode(child, assimpChild, assimpScene, options);
        }
    }
    
    private static AssimpMesh ConvertMesh(CoreMesh mesh)
    {
        var assimpMesh = new AssimpMesh(mesh.Name, PrimitiveType.Triangle);
        
        // Convert vertices
        foreach (var v in mesh.Vertices)
        {
            assimpMesh.Vertices.Add(new Assimp.Vector3D(v.X, v.Y, v.Z));
        }
        
        // Convert normals
        if (mesh.Normals.Count == mesh.Vertices.Count)
        {
            foreach (var n in mesh.Normals)
            {
                assimpMesh.Normals.Add(new Assimp.Vector3D(n.X, n.Y, n.Z));
            }
        }
        
        // Convert tangents
        if (mesh.Tangents.Count == mesh.Vertices.Count)
        {
            foreach (var t in mesh.Tangents)
            {
                assimpMesh.Tangents.Add(new Assimp.Vector3D(t.X, t.Y, t.Z));
            }
        }
        
        // Convert UVs
        if (mesh.UVs.Count == mesh.Vertices.Count)
        {
            for (int i = 0; i < mesh.UVs.Count; i++)
            {
                var uv = mesh.UVs[i];
                assimpMesh.TextureCoordinateChannels[0].Add(new Assimp.Vector3D(uv.X, uv.Y, 0));
            }
        }
        
        // Convert faces
        for (int i = 0; i < mesh.Indices.Count; i += 3)
        {
            var face = new Face();
            face.Indices.Add(mesh.Indices[i]);
            face.Indices.Add(mesh.Indices[i + 1]);
            face.Indices.Add(mesh.Indices[i + 2]);
            assimpMesh.Faces.Add(face);
        }
        
        return assimpMesh;
    }
    
    private static Assimp.Material ConvertMaterial(Core.Model.Material material)
    {
        var assimpMaterial = new Assimp.Material();
        assimpMaterial.Name = material.Name;
        
        // Set diffuse color
        assimpMaterial.ColorDiffuse = new Color4D(
            material.BaseColor.X,
            material.BaseColor.Y,
            material.BaseColor.Z,
            1.0f
        );
        
        // Set textures
        if (material.BaseColorTexture != null)
        {
            assimpMaterial.TextureDiffuse = new TextureSlot
            {
                FilePath = material.BaseColorTexture.FilePath,
                TextureType = TextureType.Diffuse
            };
        }
        
        if (material.NormalTexture != null)
        {
            assimpMaterial.TextureNormal = new TextureSlot
            {
                FilePath = material.NormalTexture.FilePath,
                TextureType = TextureType.Normals
            };
        }
        
        return assimpMaterial;
    }
}
