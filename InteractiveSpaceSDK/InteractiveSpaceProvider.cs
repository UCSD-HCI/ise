using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace InteractiveSpaceSDK
{
    public abstract class InteractiveSpaceProvider
    {
        public abstract void Connect();
        public abstract void Close();

        public abstract void CreateFingerTracker();
        public abstract FingerTracker FingerTracker { get; }
        
    }
}
