using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace InteractiveSpaceSDK.GUI
{
    public interface ISpaceControl
    {
        event EventHandler<FingerEventArgs> FingerMove;
        event EventHandler<FingerEventArgs> FingerDown;
        event EventHandler<FingerEventArgs> FingerUp;

        /*event EventHandler<FingerEventArgs> FingerCaptured;
        event EventHandler<FingerEventArgs> FingerLost;

        event EventHandler<FingerEventArgs> FingerEnter;
        event EventHandler<FingerEventArgs> FingerLeave;*/

        void RaiseFingerMove(FingerEventArgs e);
        void RaiseFingerDown(FingerEventArgs e);
        void RaiseFingerUp(FingerEventArgs e);
    }
}
