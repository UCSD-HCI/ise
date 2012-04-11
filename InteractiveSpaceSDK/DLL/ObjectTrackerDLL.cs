using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ControlPanel.NativeWrappers;
using System.Runtime.InteropServices;
using System.Windows.Media.Media3D;

namespace InteractiveSpaceSDK.DLL
{
    public class ObjectTrackerDLL : ObjectTracker
    {
        private Action onRegisterFinished;
        private List<Point3D[]> trackingDocBounds;

        public ObjectTrackerDLL()
        {
            trackingDocBounds = new List<Point3D[]>();
        }

        public void Register(System.Windows.Media.Media3D.Point3D center, Action onFinished)
        {
            onRegisterFinished = onFinished;
            CommandDllWrapper.objectRegister(
                new FloatPoint3D(center.X, center.Y, center.Z),
                Marshal.GetFunctionPointerForDelegate(onRegisterFinished));
        }

        public void Refresh()
        {
            trackingDocBounds.Clear();
            int trackingDocNum = 0;

            unsafe
            {
                ReadLockedWrapperPtr trackingDocBoundsPtr = ResultsDllWrapper.lockTrackingDocBounds(&trackingDocNum);
                Quadrilateral* trackingDocBoundsSrc = (Quadrilateral*)trackingDocBoundsPtr.IntPtr;
                for (int i = 0; i < trackingDocNum; i++)
                {
                    Point3D[] pts = new Point3D[4];
                    pts[0] = new Point3D(trackingDocBoundsSrc[i].P1.x, trackingDocBoundsSrc[i].P1.y, trackingDocBoundsSrc[i].P1.z);
                    pts[1] = new Point3D(trackingDocBoundsSrc[i].P2.x, trackingDocBoundsSrc[i].P2.y, trackingDocBoundsSrc[i].P2.z);
                    pts[2] = new Point3D(trackingDocBoundsSrc[i].P3.x, trackingDocBoundsSrc[i].P3.y, trackingDocBoundsSrc[i].P3.z);
                    pts[3] = new Point3D(trackingDocBoundsSrc[i].P4.x, trackingDocBoundsSrc[i].P4.y, trackingDocBoundsSrc[i].P4.z);
                    trackingDocBounds.Add(pts);
                }
            }
        }


        public IEnumerable<IEnumerable<Point3D>> TrackingDocumentBounds
        {
            get
            {
                return trackingDocBounds;
            }
        }
    }
}
