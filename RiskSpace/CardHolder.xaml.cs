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
using System.Diagnostics;
using System.Windows.Media.Animation;

namespace RiskSpace
{
    /// <summary>
    /// Interaction logic for CardHolder.xaml
    /// </summary>
    public partial class CardHolder : UserControl
    {
        public CardHolder()
        {
            InitializeComponent();
        }

        public void LocateCard(int cardId)
        {
            Rectangle rect = null;
            switch (cardId)
            {
                case 1:
                    rect = card1;
                    break;

                case 2:
                    rect = card2;
                    break;

                case 3:
                    rect = card3;
                    break;

                default:
                    Debug.Assert(false);
                    break;
            }

            rect.Fill = new SolidColorBrush(Colors.White);
        }

        public void Hide()
        {
            card1.Fill = null;
            card2.Fill = null;
            card3.Fill = null;
            card1.RenderTransform = null;
            card2.RenderTransform = null;
            card3.RenderTransform = null;
            card1.Opacity = 1.0;
            card2.Opacity = 1.0;
            card3.Opacity = 1.0;
            this.Visibility = Visibility.Hidden;
        }

        public void Show()
        {
            this.Visibility = Visibility.Visible;
        }

        public void MatchFinished(bool isAccepted)
        {
            if (!isAccepted)
            {
                ColorAnimation opacityOut = new ColorAnimation(Colors.Transparent, new Duration(TimeSpan.FromSeconds(0.5)));
                card1.BeginAnimation(Rectangle.FillProperty, opacityOut, HandoffBehavior.SnapshotAndReplace);
                card2.BeginAnimation(Rectangle.FillProperty, opacityOut, HandoffBehavior.SnapshotAndReplace);
                card3.BeginAnimation(Rectangle.FillProperty, opacityOut, HandoffBehavior.SnapshotAndReplace);
            }
            else
            {
                ScaleTransform scale = new ScaleTransform(1.0, 1.0, card1.Width / 2, card1.Height / 2);
                DoubleAnimation scaleOutAnim = new DoubleAnimation(1.0, 10.0, new Duration(TimeSpan.FromSeconds(0.5)));
                card1.RenderTransform = scale;
                card2.RenderTransform = scale;
                card3.RenderTransform = scale;
                scale.BeginAnimation(ScaleTransform.ScaleXProperty, scaleOutAnim, HandoffBehavior.SnapshotAndReplace);
                scale.BeginAnimation(ScaleTransform.ScaleYProperty, scaleOutAnim, HandoffBehavior.SnapshotAndReplace);
                scaleOutAnim.Completed += new EventHandler(scaleOutAnim_Completed);

                DoubleAnimation opacityOutAnim = new DoubleAnimation(1.0, 0.0, new Duration(TimeSpan.FromSeconds(0.5)));
                card1.BeginAnimation(Rectangle.OpacityProperty, opacityOutAnim, HandoffBehavior.SnapshotAndReplace);
                card2.BeginAnimation(Rectangle.OpacityProperty, opacityOutAnim, HandoffBehavior.SnapshotAndReplace);
                card3.BeginAnimation(Rectangle.OpacityProperty, opacityOutAnim, HandoffBehavior.SnapshotAndReplace);
            }
        }

        void scaleOutAnim_Completed(object sender, EventArgs e)
        {
            Hide();
        }
    }
}
