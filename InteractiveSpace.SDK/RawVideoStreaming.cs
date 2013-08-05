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
		/// <summary>
		/// Gets the rgb video frame. 
		/// </summary>
		/// <value>The rgb video.</value>
        WriteableBitmap RgbVideo { get; }

		/// <summary>
		/// Gets the depth video frame. 
		/// </summary>
		/// <value>The depth video.</value>
        WriteableBitmap DepthVideo { get; }
    }
}
