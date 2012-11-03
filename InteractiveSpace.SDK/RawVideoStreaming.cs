using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Imaging;
using System.Windows.Media;

namespace InteractiveSpace.SDK
{
    public interface RawVideoStreaming
    {
        WriteableBitmap RgbVideo { get; }
        WriteableBitmap DepthVideo { get; }
    }
}
