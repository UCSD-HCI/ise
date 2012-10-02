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
using System.Windows.Media.Effects;
using System.Windows.Media.Animation;
using System.Diagnostics;

namespace RiskSpace
{
    /// <summary>
    /// Interaction logic for ArmyViz.xaml
    /// </summary>
    public partial class ArmyViz : UserControl
    {
        private int playerId;
        private int armyCount;
        private string countryId;
        private bool attacking;

        private DropShadowEffect shadowEffect;
        private TranslateTransform translate;

        public ArmyViz()
        {
            InitializeComponent();
            PlayerId = 0;
        }

        public int PlayerId
        {
            get { return playerId; }
            set
            {
                playerId = value;
                //counterTextBlock.Foreground = new SolidColorBrush(Player.GetMainColor(playerId));
                Brush b = new SolidColorBrush(Player.GetMainColor(playerId));
                
                foreach (var elem in iconPanel.Children)
                {
                    if (!(elem is Rectangle))
                    {
                        continue;
                    }
                    (elem as Rectangle).Fill = b;
                }
            }
        }

        public string CountryID
        {
            get { return countryId; }
            set
            {
                countryId = value;
            }
        }

        public int ArmyCount
        {
            get { return armyCount; }
            set
            {
                armyCount = value;
                //counterTextBlock.Text = armyCount.ToString();
                this.Visibility = armyCount > 0 ? Visibility.Visible : Visibility.Hidden;
                if (armyCount == 0)
                {
                    return;
                }

                iconPanel.Children.Clear();
                int remained = armyCount;
                while (remained > 0)
                {
                    Rectangle rect = new Rectangle()
                    {
                        Fill = new SolidColorBrush(Player.GetMainColor(playerId)),
                        Width = 10,
                        Height = 10,
                    };

                    ImageSource maskSrc;
                    if (remained >= 10)
                    {
                        maskSrc = new BitmapImage(new Uri(@"pack://application:,,,/Images/army_L3_mask.png"));
                        remained -= 10;
                    }
                    else if (remained >= 5)
                    {
                        maskSrc = new BitmapImage(new Uri(@"pack://application:,,,/Images/army_L2_mask.png"));
                        remained -= 5;
                    }
                    else
                    {
                        maskSrc = new BitmapImage(new Uri(@"pack://application:,,,/Images/army_L1_mask.png"));
                        remained -= 1;
                    }
                    rect.OpacityMask = new ImageBrush(maskSrc)
                    {
                        Stretch = Stretch.Uniform
                    };
                    iconPanel.Children.Add(rect);
                }
            }
        }

        public void AddArmy()
        {
            ArmyCount++;
        }

        public bool IsAttacking
        {
            get { return attacking; }
            set
            {
                attacking = value;
                if (attacking)
                {
                    shadowEffect = new DropShadowEffect()
                    {
                        BlurRadius = 3,
                        ShadowDepth = 4,
                    };

                    //counterTextBlock.Effect = shadowEffect;
                    viewBox.Effect = shadowEffect;
                    DoubleAnimation depthAnim = new DoubleAnimation(shadowEffect.ShadowDepth + 3, shadowEffect.ShadowDepth - 3,new Duration(TimeSpan.FromSeconds(0.5)));
                    depthAnim.RepeatBehavior = RepeatBehavior.Forever;
                    depthAnim.AutoReverse = true;
                    depthAnim.EasingFunction = new SineEase()
                    {
                        EasingMode = EasingMode.EaseInOut
                    };
                    shadowEffect.BeginAnimation(DropShadowEffect.ShadowDepthProperty, depthAnim);

                    translate = new TranslateTransform(0, 0);
                    //counterTextBlock.RenderTransform = translate;
                    viewBox.RenderTransform = translate;

                    DoubleAnimation translateAnim = new DoubleAnimation(-3, 3, new Duration(TimeSpan.FromSeconds(0.5)));
                    translateAnim.RepeatBehavior = RepeatBehavior.Forever;
                    translateAnim.AutoReverse = true;
                    translateAnim.EasingFunction = new SineEase()
                    {
                        EasingMode = EasingMode.EaseInOut
                    };
                    translate.BeginAnimation(TranslateTransform.XProperty, translateAnim);
                    translate.BeginAnimation(TranslateTransform.YProperty, translateAnim);
                }
                else
                {
                    //counterTextBlock.Effect = null;
                    //counterTextBlock.RenderTransform = null;
                    viewBox.Effect = null;
                    viewBox.RenderTransform = null;
                }
            }
        }
    }
}
