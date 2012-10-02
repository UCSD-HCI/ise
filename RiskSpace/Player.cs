using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media;

namespace RiskSpace
{
    public class Player
    {
        private static Color[] mainColors = new Color[]
        {
            Color.FromRgb(0,0,0),
            Colors.OrangeRed, // Color.FromRgb(121,0,0),
            Color.FromRgb(54,99,0),
            Color.FromRgb(161,161,161),
            Color.FromRgb(0,88,38)
        };

        public int PlayerId { get; private set; }
        public int CountryNum { get; set; }
        public int ArmyNum { get; set; }

        public Color MainColor
        {
            get { return mainColors[PlayerId]; }
        }

        public static Color GetMainColor(int playerId)
        {
            return mainColors[playerId];
        }

        public Player(int playerId)
        {
            this.PlayerId = playerId;
        }
    }
}
