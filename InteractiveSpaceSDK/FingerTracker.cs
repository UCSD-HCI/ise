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
    public abstract class FingerTracker
    {
        public abstract event EventHandler<FingerEventArgs> FingerDown;
        public abstract event EventHandler<FingerEventArgs> FingerUp;
        public abstract event EventHandler<FingerEventArgs> FingerMove;

        //TODO: click, double click
    }

    public class FingerEventArgs : EventArgs
    {
        private int id;
        private Point3D position;
        //TODO: state: hovering/surface

        public FingerEventArgs(int id, Point3D position)
        {
            this.id = id;
            this.position = position;
        }

        public int ID { get { return id; } }
        public Point3D Position { get { return position; } }
    }
}
