using System.Windows;
using ModelRigStudio.App.ViewModels;
using ModelRigStudio.App.Services;
using ModelRigStudio.Core.Model;

namespace ModelRigStudio.App;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
        DataContext = new MainViewModel();
    }

    private void OutlinerTreeView_SelectedItemChanged(object sender, System.Windows.RoutedPropertyChangedEventArgs<object> e)
    {
        if (OutlinerTreeView.SelectedItem is Node node)
        {
            DocumentService.Instance.SelectedNode = node;
            if (DataContext is MainViewModel vm)
            {
                vm.SelectedNode = node;
            }
        }
    }
}