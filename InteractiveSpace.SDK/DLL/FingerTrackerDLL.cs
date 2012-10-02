using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using InteractiveSpace.EngineController.NativeWrappers;
using System.Windows.Media.Media3D;
using System.Runtime.InteropServices;

namespace InteractiveSpace.SDK.DLL
{
    public class FingerTrackerDLL : FingerTracker
    {
        private object eventLocker = new object();
        private Dictionary<int, Finger> fingers;

        public event EventHandler<FingerEventArgs> FingerDown;
        public event EventHandler<FingerEventArgs> FingerUp;
        public event EventHandler<FingerEventArgs> FingerMove;
        public event EventHandler<FingerEventArgs> FingerCaptured;
        public event EventHandler<FingerEventArgs> FingerLost;

        public FingerTrackerDLL()
        {
            fingers = new Dictionary<int, Finger>();
        }

        internal void Refresh()
        {
            int fingerNum = 0;
            long frame = 0;

            unsafe
            {
                ReadLockedWrapperPtr fingerEventsPtr = ResultsDllWrapper.lockFingerEvents(&fingerNum, &frame); //frame already checked by the caller
                FingerEvent* fingerEventsSrc = (FingerEvent*)fingerEventsPtr.IntPtr;
                FingerEvent[] fingerEvents = new FingerEvent[fingerNum];
                for (int i = 0; i < fingerNum; i++)
                {
                    fingerEvents[i] = fingerEventsSrc[i];   //deep clone happened for struct
                }
                ResultsDllWrapper.releaseReadLockedWrapperPtr(fingerEventsPtr); //release the locker before raising events, or the engine may be blocked by application

                for (int i = 0; i < fingerNum; i++)
                {
                    FingerEvent e = fingerEvents[i];
                    switch (e.EventType)
                    {
                        case FingerEventType.FingerMove:

                            Finger movingFinger = fingers[e.ID];    //it should not be null

                            //but because of sync bug, it can...
                            if (movingFinger == null)
                            {
                                break;
                            }

                            movingFinger.Position = new Point3D(e.PositionTable2D.x, e.PositionTable2D.y, e.PositionTable2D.z);
                            if (FingerMove != null)
                            {
                                FingerMove(this, new FingerEventArgs(movingFinger, this));
                            }
                            break;

                        case FingerEventType.FingerUp:
                            Finger upFinger = fingers[e.ID];
                            upFinger.FingerState = FingerState.Hovering;
                            if (FingerUp != null)
                            {
                                FingerUp(this, new FingerEventArgs(upFinger, this));
                            }
                            break;

                        case FingerEventType.FingerDown:
                            Finger downFinger = fingers[e.ID];
                            downFinger.FingerState = FingerState.OnSurface;
                            if (FingerDown != null)
                            {
                                FingerDown(this, new FingerEventArgs(downFinger, this));
                            }
                            break;

                        case FingerEventType.FingerCaptured:
                            Finger newFinger = new Finger(e.ID, new Point3D(e.PositionTable2D.x, e.PositionTable2D.y, e.PositionTable2D.z), FingerState.Hovering);
                            fingers.Add(newFinger.ID, newFinger);
                            if (FingerCaptured != null)
                            {
                                FingerCaptured(this, new FingerEventArgs(newFinger, this));
                            }
                            break;

                        case FingerEventType.FingerLost:
                            Finger removedFinger = fingers[e.ID];   //it should not be null
                            
                            //but because of sync bug, it can...
                            if (removedFinger == null)
                            {
                                break;
                            }

                            fingers.Remove(e.ID);
                            if (FingerLost != null)
                            {
                                FingerLost(this, new FingerEventArgs(removedFinger, this));
                            }
                            break;
                    }
                }
            }
        }

        public Finger FindFingerById(int id)
        {
            return fingers[id];
        }

        public IEnumerable<Finger> HoveringFingers
        {
            //TODO: optimize
            get
            {
                return (from g in fingers.Values where g.FingerState == FingerState.Hovering select g);
            }
        }

        public IEnumerable<Finger> OnSurfaceFingers
        {
            //TODO: optimize
            get
            {
                return (from g in fingers.Values where g.FingerState == FingerState.OnSurface select g);
            }
        }

        public IEnumerable<Finger> Fingers
        {
            get
            {
                return fingers.Values;
            }
        }
    }
}
