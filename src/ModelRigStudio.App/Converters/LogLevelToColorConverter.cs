using System.Globalization;
using System.Windows.Data;
using System.Windows.Media;
using ModelRigStudio.App.Services;

namespace ModelRigStudio.App.Converters;

/// <summary>
/// Converts LogLevel to color
/// </summary>
public class LogLevelToColorConverter : IMultiValueConverter
{
    public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
    {
        if (values.Length > 0 && values[0] is LogLevel level)
        {
            return level switch
            {
                LogLevel.Error => new SolidColorBrush(Colors.Red),
                LogLevel.Warning => new SolidColorBrush(Colors.Yellow),
                _ => new SolidColorBrush(Colors.White)
            };
        }
        return new SolidColorBrush(Colors.White);
    }

    public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
    {
        throw new NotImplementedException();
    }
}
