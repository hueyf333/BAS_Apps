using System.ComponentModel;

namespace ModelRigStudio.Core.Model;

/// <summary>
/// Represents the main document containing the scene and document state
/// </summary>
public class SceneDocument : INotifyPropertyChanged
{
    private Scene _scene = new();
    private string _filePath = string.Empty;
    private bool _isDirty;

    public Scene Scene
    {
        get => _scene;
        set
        {
            _scene = value;
            OnPropertyChanged(nameof(Scene));
        }
    }

    public string FilePath
    {
        get => _filePath;
        set
        {
            _filePath = value;
            OnPropertyChanged(nameof(FilePath));
        }
    }

    public bool IsDirty
    {
        get => _isDirty;
        set
        {
            _isDirty = value;
            OnPropertyChanged(nameof(IsDirty));
        }
    }

    public event PropertyChangedEventHandler? PropertyChanged;

    protected virtual void OnPropertyChanged(string propertyName)
    {
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }
}
