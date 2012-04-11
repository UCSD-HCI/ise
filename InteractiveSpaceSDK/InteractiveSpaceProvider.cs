using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Media3D;

namespace InteractiveSpaceSDK
{
    public interface InteractiveSpaceProvider
    {
        void Connect();
        void Close();

        void CreateFingerTracker();
        FingerTracker FingerTracker { get; }

        void CreateHandTracker();
        HandTracker HandTracker { get; }

        void CreateObjectTracker();
        ObjectTracker ObjectTracker { get; }

        void GrabAt(Point3D center, Action onFinished);
        void GetLastGrabbedImageData(out byte[] data, out string mime);

        event EventHandler EngineUpdate;
    }
}
