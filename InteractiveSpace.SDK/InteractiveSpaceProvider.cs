using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Media3D;

namespace InteractiveSpace.SDK
{
	/// <summary>
	/// Provides interfaces for all Interactive Space functionality. 
	/// </summary>
	/// <remarks>
	/// To initialize a provider, use
	/// 	<code>
	/// 		InteractiveSpaceProvider spaceProvider = new InteractiveSpaceProviderDLL();
	///			spaceProvider.Connect();
	/// 	</code>
	/// </remarks>
    public interface InteractiveSpaceProvider
    {
		/// <summary>
		/// Connect (initialize) to the provider. 
		/// </summary>
        void Connect();

		/// <summary>
		/// Close the connection (or release it, in DLL case) to the provider. 
		/// </summary>
        void Close();

		/// <summary>
		/// Enables the finger tracker component.
		/// </summary>
        void CreateFingerTracker();

		/// <summary>
		/// Get the finger tracker component, which provides 3D finger events. Must call CreateFingerTracker() first or it will be null. 
		/// </summary>
		/// <value>The finger tracker.</value>
        FingerTracker FingerTracker { get; }

		/// <summary>
		/// (Not implemented yet.)
		/// </summary>
        void CreateHandTracker();

		/// <summary>
		/// (Not implemented yet)
		/// </summary>
		/// <value>The hand tracker.</value>
        HandTracker HandTracker { get; }

		/// <summary>
		/// Enables the raw video streaming component. 
		/// </summary>
        void CreateRawVideoStreaming();

		/// <summary>
		/// Get the raw video streaming components, which forwards raw RGB and depth video from Kinect. 
		/// </summary>
		/// <value>The raw video streaming.</value>
        RawVideoStreaming RawVideoStreaming { get; }

		/// <summary>
		/// Triggered at each frame when all detection is completed. 
		/// </summary>
		/// <remarks>
		/// If you're going to manipulate GUI in the event handler, consider using EngineUpdateOnMainUI instead. 
		/// </remarks>
        event EventHandler EngineUpdate;

		/// <summary>
		/// Triggered at each frame when all detection is completed, queued for the main GUI thread.
		/// </summary>
		event EventHandler EngineUpdateOnMainUI;

		/// <summary>
		/// (Deprecated)
		/// </summary>
		/// <returns>The rgb to tabletop homography.</returns>
        double[,] GetRgbToTabletopHomography();

		/// <summary>
		/// (Deprecated)
		/// </summary>
		/// <returns>The depth to tabletop homography.</returns>
        double[,] GetDepthToTabletopHomography();
    }
}
