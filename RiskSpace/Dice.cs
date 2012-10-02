using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;

namespace RiskSpace
{
    public class Dice
    {
        public Point Position { get; set; }
        public DiceColor DiceColor { get; set; }
        public int Id { get; set; }
        public int? Value { get; set; }

        public Dice(int Id, Point position, DiceColor color)
        {
            this.Id = Id;
            this.Position = position;
            this.DiceColor = color;
            this.Value = null;
        }
    }

    public enum DiceColor
    {
        Red,
        White,
    }
}
