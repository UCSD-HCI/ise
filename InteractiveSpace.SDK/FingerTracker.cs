using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Media3D;
using System.Windows;
using System.Windows.Media;
using System.Windows.Controls;

namespace InteractiveSpace.SDK
{
    public interface FingerTracker
    {
		/// <summary>
		/// Triggered when a hovering finger touches the tabletop.
		/// </summary>
		event EventHandler<FingerEventArgs> FingerDown;

		/// <summary>
		/// Triggered when a finger leaves the tabletop. 
		/// </summary>
        event EventHandler<FingerEventArgs> FingerUp;

		/// <summary>
		/// Triggered when a finger moves, whether hovering or on tabletop.
		/// </summary>
        event EventHandler<FingerEventArgs> FingerMove;

		/// <summary>
		/// Triggered when a finger is detected by ISE, whether hovering or on tabletop.
		/// </summary>
        event EventHandler<FingerEventArgs> FingerCaptured;

		/// <summary>
		/// Trigered when a detected finger is lost, whether hovering or on tabletop.
		/// </summary>
        event EventHandler<FingerEventArgs> FingerLost;

        //TODO: click, double click
        
        /// <summary>
        /// Find the finger with specific ID. If not found, return null.
        /// </summary>
        /// <param name="id"></param>
        /// <returns></returns>
        Finger FindFingerById(int id);

		/// <summary>
		/// Get all on-surface fingers.
		/// </summary>
		/// <remarks>
		/// Use <code>foreach (var f in OnSurfaceFingers) to enumerate them.</code>
		/// </remarks>
		/// <value>The on surface fingers.</value>
        IEnumerable<Finger> OnSurfaceFingers { get; }

		/// <summary>
		/// Get all hovering fingers. 
		/// </summary>
		/// <remarks>
		/// Use <code>foreach (var f in HoveringFingers)</code> to enumerate them.
		/// </remarks>
		/// <value>The hovering fingers.</value>
        IEnumerable<Finger> HoveringFingers { get; }

		/// <summary>
		/// Get both on-surface hand hovering fingers. 
		/// </summary>
		/// <remarks>
		/// Use <code>foreach (var f in Fingers)</code> to enumerate them, <code>f.FingerState</code> to tell on-surface or hovering. 
		/// </remarks>
		/// <value>The fingers.</value>
        IEnumerable<Finger> Fingers { get; }
    }

	/// <summary>
	/// Enumerate for finger state, whether hovering or on surface
	/// </summary>
    public enum FingerState
    {
        Hovering,
        OnSurface
    }

    public class Finger
    {
        private int id;

		/// <summary>
		/// Gets or sets the finger position. (X, Y) is the finger's 2D absolute coordinate, in pixels, relative to the top-left of the projected screen. Z is the finger's height, i.e. distance from the finger to the tabletop, in centimeters. 
		/// </summary>
		/// <value>The position.</value>
        public Point3D Position { get; set; }

		/// <summary>
		/// Gets or sets the state of the finger.
		/// </summary>
		/// <value>The state of the finger.</value>
        public FingerState FingerState { get; set; }

		/// <summary>
		/// Gets the ID of the finger. 
		/// </summary>
		/// <value>The ID</value>
        public int ID { get { return id; } } 

        public Finger(int id)
        {
            this.id = id;
        }

        public Finger(int id, Point3D position, FingerState fingerState)
        {
            this.id = id;
            this.Position = position;
            this.FingerState = FingerState;
        }
    }

    public class FingerEventArgs : EventArgs
    {
        private Finger finger;
        private FingerTracker fingerTracker;

        public FingerEventArgs(int id, Point3D position, FingerState fingerState, FingerTracker fingerTracker)
        {
            this.fingerTracker = fingerTracker;
            finger = new Finger(id, position, fingerState);
        }

        public FingerEventArgs(Finger finger, FingerTracker fingerTracker)
        {
            this.fingerTracker = fingerTracker;
            this.finger = finger;
        }

		/// <summary>
		/// Gets the ID of the finger.
		/// </summary>
		/// <value>The ID.</value>
        public int ID { get { return finger.ID; } }

		/// <summary>
		/// Gets the finger position. (X, Y) is the finger's 2D absolute coordinate, in pixels, relative to the top-left of the projected screen. Z is the finger's height, i.e. distance from the finger to the tabletop, in centimeters. 
		/// </summary>
		/// <value>The position.</value>
        public Point3D Position { get { return finger.Position; } }

		
		/// <summary>
		/// Convenient property to get the fingerTracker component. 
		/// </summary>
        public FingerTracker FingerTracker { get { return fingerTracker; } }

		/// <summary>
		/// Gets the state of the finger, whether hovering or on surface. 
		/// </summary>
		/// <value>The state of the finger.</value>
        public FingerState FingerState { get { return finger.FingerState; } }
    }
}
