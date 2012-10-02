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

namespace RiskSpace
{
    /// <summary>
    /// Interaction logic for PlayerInfoControl.xaml
    /// </summary>
    public partial class PlayerInfoControl : UserControl
    {
        private int playerId;
        private bool active;

        public PlayerInfoControl()
        {
            InitializeComponent();
        }

        public int PlayerId
        {
            get { return playerId; }
            set
            {
                playerId = value;
                playerNameTextBlock.Text = "Player " + playerId.ToString();
                playerNameTextBlock.Foreground = new SolidColorBrush(Player.GetMainColor(playerId));
            }
        }

        public bool IsActive
        {
            get { return active; }
            set
            {
                active = value;
                this.Background = active ? Brushes.White : Brushes.Transparent;
            }
        }

        public void Update(Player player, bool isActive)
        {
            IsActive = isActive;
            armyTextBlock.Text = player.ArmyNum.ToString();
            countryTextBlock.Text = player.CountryNum.ToString();
        }
    }
}
