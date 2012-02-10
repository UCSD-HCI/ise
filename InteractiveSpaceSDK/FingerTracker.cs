using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Media3D;
using System.Windows;
using System.Windows.Media;
using System.Windows.Controls;

namespace InteractiveSpaceSDK
{
    public interface FingerTracker
    {
        event EventHandler<FingerEventArgs> FingerDown;
        event EventHandler<FingerEventArgs> FingerUp;
        event EventHandler<FingerEventArgs> FingerMove;
        event EventHandler<FingerEventArgs> FingerCaptured;
        event EventHandler<FingerEventArgs> FingerLost;

        //TODO: click, double click
        
        /// <summary>
        /// Find the finger with specific ID. If not found, return null.
        /// </summary>
        /// <param name="id"></param>
        /// <returns></returns>
        Finger FindFingerById(int id);

        IEnumerable<Finger> OnSurfaceFingers { get; }
        IEnumerable<Finger> HoveringFingers { get; }
    }

    public enum FingerState
    {
        Hovering,
        OnSurface
    }

    public class Finger
    {
        private int id;

        public Point3D Position { get; set; }
        public FingerState FingerState { get; set; }
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

        public int ID { get { return finger.ID; } }
        public Point3D Position { get { return finger.Position; } }
        public FingerTracker FingerTracker { get { return fingerTracker; } }
        public FingerState FingerState { get { return finger.FingerState; } }
    }
}
