using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media;

namespace ControlPanel
{
    public static class IntColorConverter
    {
        private static readonly Color[] ColorList = { Colors.Red, Colors.Green, Colors.Indigo, Colors.Blue, Colors.Cyan, Colors.Magenta,
                                                    Colors.Purple, Colors.Aqua, Colors.Chocolate, Colors.DeepPink};
        public static Color ToColor(int n)
        {
            return ColorList[(n < 0 ? -n : n) % ColorList.Count()];
        }
    }
}
