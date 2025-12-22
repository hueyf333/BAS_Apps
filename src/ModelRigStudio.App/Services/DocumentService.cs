using ModelRigStudio.Core.Model;
using ModelRigStudio.Core.Commands;
using System.ComponentModel;
using System.IO;

namespace ModelRigStudio.App.Services;

/// <summary>
/// Central service managing the document state
/// </summary>
public class DocumentService : INotifyPropertyChanged
{
    private static DocumentService? _instance;
    public static DocumentService Instance => _instance ??= new DocumentService();

    private SceneDocument _document = new();
    private Node? _selectedNode;
    private string _assetsFolder = "Assets";

    public SceneDocument Document
    {
        get => _document;
        set
        {
            _document = value;
            OnPropertyChanged(nameof(Document));
        }
    }

    public Node? SelectedNode
    {
        get => _selectedNode;
        set
        {
            _selectedNode = value;
            OnPropertyChanged(nameof(SelectedNode));
        }
    }

    public string AssetsFolder
    {
        get => _assetsFolder;
        set
        {
            _assetsFolder = value;
            OnPropertyChanged(nameof(AssetsFolder));
        }
    }

    public UndoRedoStack UndoRedoStack { get; } = new();

    public event PropertyChangedEventHandler? PropertyChanged;

    private DocumentService()
    {
        Directory.CreateDirectory(_assetsFolder);
    }

    public void NewDocument()
    {
        Document = new SceneDocument();
        UndoRedoStack.Clear();
    }

    protected virtual void OnPropertyChanged(string propertyName)
    {
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }
}
