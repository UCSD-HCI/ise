using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Media3D;

namespace InteractiveSpace.SDK
{
    public interface InteractiveSpaceProvider
    {
        void Connect();
        void Close();

        void CreateFingerTracker();
        FingerTracker FingerTracker { get; }

        void CreateHandTracker();
        HandTracker HandTracker { get; }

        void CreateRawVideoStreaming();
        RawVideoStreaming RawVideoStreaming { get; }

        event EventHandler EngineUpdate;

        double[,] GetRgbToTabletopHomography();
        double[,] GetDepthToTabletopHomography();
    }
}
