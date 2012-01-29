using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ControlPanel.NativeWrappers;
using System.Windows.Media.Media3D;
using System.Runtime.InteropServices;

namespace InteractiveSpaceSDK.DLL
{
    public class FingerTrackerDLL : FingerTracker
    {
        private object eventLocker = new object();
        private Dictionary<int, Finger> fingers;

        public event EventHandler<FingerEventArgs> FingerDown;
        public event EventHandler<FingerEventArgs> FingerUp;
        public event EventHandler<FingerEventArgs> FingerMove;

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
                            Finger removedFinger = fingers[e.ID];   //it should not be null
                            
                        //but because of sync bug, it can...
                            if (removedFinger == null)
                            {
                                break;
                            }

                            fingers.Remove(e.ID);
                            if (FingerUp != null)
                            {
                                FingerUp(this, new FingerEventArgs(removedFinger, this));
                            }
                            break;

                        case FingerEventType.FingerDown:
                            Finger newFinger = new Finger(e.ID, new Point3D(e.PositionTable2D.x, e.PositionTable2D.y, e.PositionTable2D.z));
                            fingers.Add(newFinger.ID, newFinger);
                            if (FingerDown != null)
                            {
                                FingerDown(this, new FingerEventArgs(newFinger, this));
                            }
                            break;

                        default:
                            //TODO
                            break; 
                    }
                }
            }
        }

        public Finger FindFingerById(int id)
        {
            return fingers[id];
        }

        public int FingerCount
        {
            get 
            {
                return fingers.Count;
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
