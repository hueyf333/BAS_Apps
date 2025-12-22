using Assimp;
using ModelRigStudio.Core.Model;
using AssimpMesh = Assimp.Mesh;
using AssimpNode = Assimp.Node;
using CoreMesh = ModelRigStudio.Core.Model.Mesh;
using CoreNode = ModelRigStudio.Core.Model.Node;
using Vector3 = ModelRigStudio.Core.Model.Vector3;
using Vector2 = ModelRigStudio.Core.Model.Vector2;

namespace ModelRigStudio.ImportExport;

/// <summary>
/// Imports 3D models using AssimpNet
/// </summary>
public class ModelImporter
{
    private readonly string _assetsFolder;
    
    public ModelImporter(string assetsFolder)
    {
        _assetsFolder = assetsFolder;
        Directory.CreateDirectory(_assetsFolder);
    }
    
    /// <summary>
    /// Import a 3D model file (OBJ, GLTF, etc.)
    /// </summary>
    public Core.Model.Scene Import(string filePath)
    {
        var scene = new Core.Model.Scene { Name = Path.GetFileNameWithoutExtension(filePath) };
        
        using var importer = new AssimpContext();
        
        // Configure import settings
        var flags = PostProcessSteps.Triangulate |
                   PostProcessSteps.GenerateNormals |
                   PostProcessSteps.CalculateTangentSpace |
                   PostProcessSteps.JoinIdenticalVertices |
                   PostProcessSteps.OptimizeMeshes;
        
        var assimpScene = importer.ImportFile(filePath, flags);
        
        if (assimpScene == null)
        {
            throw new Exception($"Failed to import file: {filePath}");
        }
        
        // Import materials and textures
        ImportMaterials(assimpScene, scene, Path.GetDirectoryName(filePath) ?? "");
        
        // Import scene hierarchy
        if (assimpScene.RootNode != null)
        {
            ImportNode(assimpScene.RootNode, scene.RootNode, assimpScene);
        }
        
        return scene;
    }
    
    private void ImportNode(AssimpNode assimpNode, CoreNode parentNode, Assimp.Scene assimpScene)
    {
        foreach (var childAssimpNode in assimpNode.Children)
        {
            var childNode = new CoreNode
            {
                Name = childAssimpNode.Name
            };
            
            // Import transform
            var transform = childAssimpNode.Transform;
            transform.Decompose(out var scale, out var rotation, out var translation);
            
            childNode.Transform.Position = new Vector3(translation.X, translation.Y, translation.Z);
            childNode.Transform.Rotation = new Core.Model.Quaternion(rotation.X, rotation.Y, rotation.Z, rotation.W);
            childNode.Transform.Scale = new Vector3(scale.X, scale.Y, scale.Z);
            
            // Import meshes
            if (childAssimpNode.HasMeshes)
            {
                foreach (var meshIndex in childAssimpNode.MeshIndices)
                {
                    var assimpMesh = assimpScene.Meshes[meshIndex];
                    var mesh = ImportMesh(assimpMesh);
                    
                    // Assign material if available
                    if (assimpMesh.MaterialIndex >= 0 && assimpMesh.MaterialIndex < assimpScene.Materials.Count)
                    {
                        mesh.Material = ImportMaterial(assimpScene.Materials[assimpMesh.MaterialIndex], "");
                    }
                    
                    // For simplicity, attach first mesh to node
                    if (childNode.Mesh == null)
                    {
                        childNode.Mesh = mesh;
                    }
                    else
                    {
                        // Create child node for additional meshes
                        var meshNode = new CoreNode
                        {
                            Name = $"{childNode.Name}_Mesh{meshIndex}",
                            Mesh = mesh
                        };
                        childNode.AddChild(meshNode);
                    }
                }
            }
            
            parentNode.AddChild(childNode);
            
            // Recursively import children
            ImportNode(childAssimpNode, childNode, assimpScene);
        }
    }
    
    private CoreMesh ImportMesh(AssimpMesh assimpMesh)
    {
        var mesh = new CoreMesh
        {
            Name = assimpMesh.Name
        };
        
        // Import vertices
        foreach (var vertex in assimpMesh.Vertices)
        {
            mesh.Vertices.Add(new Vector3(vertex.X, vertex.Y, vertex.Z));
        }
        
        // Import indices
        foreach (var face in assimpMesh.Faces)
        {
            if (face.IndexCount == 3)
            {
                mesh.Indices.Add(face.Indices[0]);
                mesh.Indices.Add(face.Indices[1]);
                mesh.Indices.Add(face.Indices[2]);
            }
        }
        
        // Import normals
        if (assimpMesh.HasNormals)
        {
            foreach (var normal in assimpMesh.Normals)
            {
                mesh.Normals.Add(new Vector3(normal.X, normal.Y, normal.Z));
            }
        }
        
        // Import tangents
        if (assimpMesh.HasTangentBasis)
        {
            foreach (var tangent in assimpMesh.Tangents)
            {
                mesh.Tangents.Add(new Vector3(tangent.X, tangent.Y, tangent.Z));
            }
        }
        
        // Import UVs
        if (assimpMesh.HasTextureCoords(0))
        {
            foreach (var uv in assimpMesh.TextureCoordinateChannels[0])
            {
                mesh.UVs.Add(new Vector2(uv.X, uv.Y));
            }
        }
        
        return mesh;
    }
    
    private void ImportMaterials(Assimp.Scene assimpScene, Core.Model.Scene scene, string sourceDir)
    {
        foreach (var assimpMaterial in assimpScene.Materials)
        {
            var material = ImportMaterial(assimpMaterial, sourceDir);
            scene.Materials.Add(material);
        }
    }
    
    private Core.Model.Material ImportMaterial(Assimp.Material assimpMaterial, string sourceDir)
    {
        var material = new Core.Model.Material
        {
            Name = assimpMaterial.Name
        };
        
        // Import base color
        if (assimpMaterial.HasColorDiffuse)
        {
            var color = assimpMaterial.ColorDiffuse;
            material.BaseColor = new Vector3(color.R, color.G, color.B);
        }
        
        // Import textures
        if (assimpMaterial.HasTextureDiffuse)
        {
            material.BaseColorTexture = ImportTexture(assimpMaterial.TextureDiffuse.FilePath, sourceDir);
        }
        
        if (assimpMaterial.HasTextureNormal)
        {
            material.NormalTexture = ImportTexture(assimpMaterial.TextureNormal.FilePath, sourceDir);
        }
        
        return material;
    }
    
    private TextureAsset? ImportTexture(string texturePath, string sourceDir)
    {
        if (string.IsNullOrEmpty(texturePath))
            return null;
        
        var fullPath = Path.Combine(sourceDir, texturePath);
        if (!File.Exists(fullPath))
            return null;
        
        // Copy texture to assets folder
        var fileName = Path.GetFileName(texturePath);
        var destPath = Path.Combine(_assetsFolder, fileName);
        
        try
        {
            if (!File.Exists(destPath))
            {
                File.Copy(fullPath, destPath);
            }
            
            return new TextureAsset
            {
                Name = Path.GetFileNameWithoutExtension(fileName),
                FilePath = destPath
            };
        }
        catch
        {
            return null;
        }
    }
}
