using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace InteractiveSpaceSDK
{
    public interface InteractiveSpaceProvider
    {
        void Connect();
        void Close();

        void CreateFingerTracker();
        FingerTracker FingerTracker { get; }
    }
}
