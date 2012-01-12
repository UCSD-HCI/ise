using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;

namespace InteractiveSpaceWPFControls
{
    public interface ISpacePanel : ISpaceControl
    {
       IEnumerable<ISpaceControl> SpaceChildren { get; }
    }
}
