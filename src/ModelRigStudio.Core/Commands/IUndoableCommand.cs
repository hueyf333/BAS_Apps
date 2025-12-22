namespace ModelRigStudio.Core.Commands;

/// <summary>
/// Interface for undoable commands
/// </summary>
public interface IUndoableCommand
{
    string Name { get; }
    void Do();
    void Undo();
}
