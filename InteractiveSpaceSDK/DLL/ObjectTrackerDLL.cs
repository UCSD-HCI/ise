using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ControlPanel.NativeWrappers;
using System.Runtime.InteropServices;

namespace InteractiveSpaceSDK.DLL
{
    public class ObjectTrackerDLL : ObjectTracker
    {
        private Action onRegisterFinished;

        public void Register(System.Windows.Media.Media3D.Point3D center, Action onFinished)
        {
            onRegisterFinished = onFinished;
            CommandDllWrapper.objectRegister(
                new FloatPoint3D(center.X, center.Y, center.Z),
                Marshal.GetFunctionPointerForDelegate(onRegisterFinished));
        }
    }
}
