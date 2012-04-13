using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ControlPanel.NativeWrappers;
using System.Windows.Media.Media3D;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace InteractiveSpaceSDK.DLL
{
    public class HandTrackerDLL : HandTracker
    {
        private Dictionary<int, Hand> hands;

        public event EventHandler<HandEventArgs> HandCaptured;
        public event EventHandler<HandEventArgs> HandLost;
        public event EventHandler<HandEventArgs> HandMove;

        private HandEventCallback onHandMoveCallback, onHandCapturedCallback, onHandLostCallback;

        public HandTrackerDLL()
        {
            hands = new Dictionary<int, Hand>();

            onHandMoveCallback = new HandEventCallback(onHandMove);
            onHandCapturedCallback = new HandEventCallback(onHandCaptured);
            onHandLostCallback = new HandEventCallback(onHandLost);

            CommandDllWrapper.registerHandEventCallbacks(
                Marshal.GetFunctionPointerForDelegate(onHandMoveCallback),
                Marshal.GetFunctionPointerForDelegate(onHandCapturedCallback),
                Marshal.GetFunctionPointerForDelegate(onHandLostCallback));
        }

        public Hand FindHandById(int id)
        {
            return hands[id];
        }

        public IEnumerable<Hand> Hands
        {
            get
            {
                return hands.Values;
            }
        }

        public int HandCount
        {
            get { return hands.Count;  }
        }

        private void onHandMove(HandEvent e)
        {
            Hand h = hands[e.ID];   //it won't be null
            h.Position = new Point3D(e.PositionTable2D.x, e.PositionTable2D.y, e.PositionTable2D.z);
            if (HandMove != null)
            {
                HandMove(this, new HandEventArgs(h, this));
            }

            //Trace.WriteLine("Hand move: " + h.ID + ", " + h.Position.ToString());
        }

        private void onHandCaptured(HandEvent e)
        {
            Hand h = new Hand(e.ID, new Point3D(e.PositionTable2D.x, e.PositionTable2D.y, e.PositionTable2D.z));
            hands.Add(h.ID, h);
            if (HandCaptured != null)
            {
                HandCaptured(this, new HandEventArgs(h, this));
            }

            Trace.WriteLine("Hand captured: " + h.ID + ", " + h.Position.ToString());
        }

        private void onHandLost(HandEvent e)
        {
            Hand h = hands[e.ID];   //it won't be null
            h.Position = new Point3D(e.PositionTable2D.x, e.PositionTable2D.y, e.PositionTable2D.z);
            hands.Remove(e.ID);
            if (HandLost != null)
            {
                HandLost(this, new HandEventArgs(h, this));
            }

            Trace.WriteLine("Hand lost: " + h.ID + ", " + h.Position.ToString());
        }
    }


}
