using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;

namespace InteractiveSpace.SDK.GUI
{
    public interface ISpacePanel : ISpaceControl
    {
       IEnumerable<ISpaceControl> SpaceChildren { get; }
    }
}
