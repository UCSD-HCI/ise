using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Media.Animation;

namespace RiskSpace
{
    /// <summary>
    /// Interaction logic for DiceViz.xaml
    /// </summary>
    public partial class DiceViz : UserControl
    {
        public DiceViz()
        {
            InitializeComponent();
        }

        public void ShowDice(Dice dice)
        {
            Rectangle rect = new Rectangle()
            {
                Fill = new SolidColorBrush(Color.FromArgb(192, 255, 255, 255)),
                Stroke = new SolidColorBrush(dice.DiceColor == DiceColor.Red ? Colors.DarkRed : Colors.Black),
                Width = 50,
                Height = 50,
            };

            mainCanvas.Children.Add(rect);
            Canvas.SetLeft(rect, dice.Position.X - 25);
            Canvas.SetTop(rect, dice.Position.Y - 25);

            ScaleTransform trans = new ScaleTransform(1.0, 1.0, 25, 25);
            rect.RenderTransform = trans;

            DoubleAnimation anim = new DoubleAnimation(10.0, 1.0, new Duration(TimeSpan.FromSeconds(0.25)));
            trans.BeginAnimation(ScaleTransform.ScaleXProperty, anim);
            trans.BeginAnimation(ScaleTransform.ScaleYProperty, anim);
        }

        public void ClearDices()
        {
            mainCanvas.Children.Clear();
        }
    }
}
