using ModelRigStudio.Core.Model;

namespace ModelRigStudio.Materials;

/// <summary>
/// Utilities for texture operations
/// </summary>
public class TextureUtilities
{
    /// <summary>
    /// Imports a texture file and creates a TextureAsset
    /// </summary>
    public static TextureAsset? ImportTexture(string filePath, string assetsFolder)
    {
        if (!File.Exists(filePath))
            return null;
        
        try
        {
            Directory.CreateDirectory(assetsFolder);
            
            var fileName = Path.GetFileName(filePath);
            var destPath = Path.Combine(assetsFolder, fileName);
            
            if (!File.Exists(destPath))
            {
                File.Copy(filePath, destPath);
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
