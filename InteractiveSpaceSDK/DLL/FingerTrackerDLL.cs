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
        private event EventHandler<FingerEventArgs> fingerDown, fingerUp, fingerMove;
        private object eventLocker = new object();

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
                            if (fingerMove != null)
                            {
                                fingerMove(this, new FingerEventArgs(e.ID, new Point3D(e.PositionTable2D.x, e.PositionTable2D.y, e.PositionTable2D.z)));
                            }
                            break;

                        case FingerEventType.FingerUp:
                            if (fingerUp != null)
                            {
                                fingerUp(this, new FingerEventArgs(e.ID, new Point3D(e.PositionTable2D.x, e.PositionTable2D.y, e.PositionTable2D.z)));
                            }
                            break;

                        case FingerEventType.FingerDown:
                            if (fingerDown != null)
                            {
                                fingerDown(this, new FingerEventArgs(e.ID, new Point3D(e.PositionTable2D.x, e.PositionTable2D.y, e.PositionTable2D.z)));
                            }
                            break;

                        default:
                            //TODO
                            break;
                    }
                }
            }
        }

        public override event EventHandler<FingerEventArgs> FingerDown
        {
            add
            {
                lock (eventLocker)
                {
                    fingerDown += value;
                }
            }
            remove
            {
                lock (eventLocker)
                {
                    fingerDown -= value;
                }
            }
        }

        public override event EventHandler<FingerEventArgs> FingerUp
        {
            add
            {
                lock (eventLocker)
                {
                    fingerUp += value;
                }
            }
            remove
            {
                lock (eventLocker)
                {
                    fingerUp -= value;
                }
            }
        }

        public override event EventHandler<FingerEventArgs> FingerMove
        {
            add
            {
                lock (eventLocker)
                {
                    fingerMove += value;
                }
            }
            remove
            {
                lock (eventLocker)
                {
                    fingerMove -= value;
                }
            }
        }
    }
}
