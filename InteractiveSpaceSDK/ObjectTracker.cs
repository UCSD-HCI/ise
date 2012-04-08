using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Media3D;

namespace InteractiveSpaceSDK
{
    public interface ObjectTracker
    {
        void Register(Point3D center, Action onFinished);
    }
}
