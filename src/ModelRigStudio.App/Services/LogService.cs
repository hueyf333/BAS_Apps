using System.Collections.ObjectModel;
using System.ComponentModel;

namespace ModelRigStudio.App.Services;

/// <summary>
/// Service for logging messages
/// </summary>
public class LogService : INotifyPropertyChanged
{
    private static LogService? _instance;
    public static LogService Instance => _instance ??= new LogService();

    public ObservableCollection<LogMessage> Messages { get; } = new();

    public event PropertyChangedEventHandler? PropertyChanged;

    private LogService() { }

    public void Log(string message, LogLevel level = LogLevel.Info)
    {
        var logMessage = new LogMessage
        {
            Timestamp = DateTime.Now,
            Message = message,
            Level = level
        };
        
        Messages.Add(logMessage);
        
        // Limit log size
        while (Messages.Count > 1000)
        {
            Messages.RemoveAt(0);
        }
    }

    public void Info(string message) => Log(message, LogLevel.Info);
    public void Warning(string message) => Log(message, LogLevel.Warning);
    public void Error(string message) => Log(message, LogLevel.Error);
}

public enum LogLevel
{
    Info,
    Warning,
    Error
}

public class LogMessage
{
    public DateTime Timestamp { get; set; }
    public string Message { get; set; } = string.Empty;
    public LogLevel Level { get; set; }
    
    public override string ToString()
    {
        return $"[{Timestamp:HH:mm:ss}] [{Level}] {Message}";
    }
}
