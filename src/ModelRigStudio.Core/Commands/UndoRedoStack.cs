namespace ModelRigStudio.Core.Commands;

/// <summary>
/// Manages undo/redo stack for commands
/// </summary>
public class UndoRedoStack
{
    private readonly Stack<IUndoableCommand> _undoStack = new();
    private readonly Stack<IUndoableCommand> _redoStack = new();
    private int _maxStackSize = 100;

    public bool CanUndo => _undoStack.Count > 0;
    public bool CanRedo => _redoStack.Count > 0;

    public event EventHandler? StackChanged;

    public void ExecuteCommand(IUndoableCommand command)
    {
        command.Do();
        _undoStack.Push(command);
        _redoStack.Clear();
        
        // Limit stack size
        if (_undoStack.Count > _maxStackSize)
        {
            var commands = _undoStack.ToList();
            commands.RemoveAt(commands.Count - 1);
            _undoStack.Clear();
            foreach (var cmd in commands.AsEnumerable().Reverse())
            {
                _undoStack.Push(cmd);
            }
        }
        
        OnStackChanged();
    }

    public void Undo()
    {
        if (!CanUndo) return;
        
        var command = _undoStack.Pop();
        command.Undo();
        _redoStack.Push(command);
        OnStackChanged();
    }

    public void Redo()
    {
        if (!CanRedo) return;
        
        var command = _redoStack.Pop();
        command.Do();
        _undoStack.Push(command);
        OnStackChanged();
    }

    public void Clear()
    {
        _undoStack.Clear();
        _redoStack.Clear();
        OnStackChanged();
    }

    private void OnStackChanged()
    {
        StackChanged?.Invoke(this, EventArgs.Empty);
    }
}
