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

        //TODO: click, double click
        
        /// <summary>
        /// Find the finger with specific ID. If not found, return null.
        /// </summary>
        /// <param name="id"></param>
        /// <returns></returns>
        Finger FindFingerById(int id);

        /// <summary>
        /// Get all tracking fingers.
        /// </summary>
        IEnumerable<Finger> Fingers { get; }
        int FingerCount { get; }
    }

    public class Finger
    {
        private int id;

        public Point3D Position { get; set; }
        public int ID { get { return id; } }

        public Finger(int id)
        {
            this.id = id;
        }

        public Finger(int id, Point3D position)
        {
            this.id = id;
            this.Position = position;
        }
    }

    public class FingerEventArgs : EventArgs
    {
        private Finger finger;
        private FingerTracker fingerTracker;
        //TODO: state: hovering/surface

        public FingerEventArgs(int id, Point3D position, FingerTracker fingerTracker)
        {
            this.fingerTracker = fingerTracker;
            finger = new Finger(id, position);
        }

        public FingerEventArgs(Finger finger, FingerTracker fingerTracker)
        {
            this.fingerTracker = fingerTracker;
            this.finger = finger;
        }

        public int ID { get { return finger.ID; } }
        public Point3D Position { get { return finger.Position; } }
        public FingerTracker FingerTracker { get { return fingerTracker; } }
    }
}
