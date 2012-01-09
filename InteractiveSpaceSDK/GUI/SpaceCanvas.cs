using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;

namespace InteractiveSpaceSDK.GUI
{
    public class SpaceCanvas : Canvas, ISpacePanel
    {
        public event EventHandler<FingerEventArgs> FingerMove;
        public event EventHandler<FingerEventArgs> FingerDown;
        public event EventHandler<FingerEventArgs> FingerUp;

        public SpaceCanvas()
        {
        }

        public IEnumerable<ISpaceControl> SpaceChildren
        {
            get 
            {
                foreach (var element in Children)
                {
                    if (element is ISpaceControl)
                    {
                        yield return element as ISpaceControl;
                    }
                }
            }
        }

        public void RaiseFingerMove(FingerEventArgs e)
        {
            if (FingerMove != null)
            {
                FingerMove(this, e);
            }
        }

        public void RaiseFingerDown(FingerEventArgs e)
        {
            if (FingerDown != null)
            {
                FingerDown(this, e);
            }
        }

        public void RaiseFingerUp(FingerEventArgs e)
        {
            if (FingerUp != null)
            {
                FingerUp(this, e);
            }
        }
    }
}
