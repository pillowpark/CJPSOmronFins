using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace OmronFinsViewerCS
{
    /// <summary>
    /// TestPage.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class TestPage : Page
    {
        public TestPage()
        {
            InitializeComponent();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            // Create colors
            Color purple = (Color)ColorConverter.ConvertFromString("#4E1A3D");
            Color white = Colors.White;
            Color salmon = Colors.Salmon;

            // Create a new style for a button
            var buttonStyle = new Style(typeof(Button));

            // Set the properties of the style
            buttonStyle.Setters.Add(new Setter(Control.BackgroundProperty, new SolidColorBrush(purple)));
            buttonStyle.Setters.Add(new Setter(Control.ForegroundProperty, new SolidColorBrush(white)));
            buttonStyle.Setters.Add(new Setter(Control.BorderBrushProperty, new LinearGradientBrush(purple, salmon, 45d)));
            buttonStyle.Setters.Add(new Setter(Control.BorderThicknessProperty, new Thickness(5)));

            // Set this style as a resource. Any DynamicResource tied to this key will be updated.
            this.Resources["buttonStyle1"] = buttonStyle;

            // Set this style directly to a button
            ((Button)sender).Style = buttonStyle;
        }
    }
}
