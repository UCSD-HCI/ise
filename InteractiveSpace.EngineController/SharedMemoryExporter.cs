using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using InteractiveSpace.EngineController.NativeWrappers;
using System.IO.MemoryMappedFiles;
using System.Runtime.InteropServices;
using System.IO;
using System.Diagnostics;

namespace InteractiveSpace.EngineController
{
    public class SharedMemoryExporter
    {
        private MemoryMappedFile rectifiedTabletopMmf;
        private const int imgSize = 640 * 480 * 3;   //TODO: hard coding?!

        public SharedMemoryExporter()
        {
            rectifiedTabletopMmf = MemoryMappedFile.CreateNew("InteractiveSpaceRectifiedTabletop", imgSize);
        }

        public void Update()
        {
            //bool mutexCreated;
            //Mutex mutex = new Mutex(true, "InteractiveSpaceRectifiedTabletopMutex2", out mutexCreated);
            
            using (MemoryMappedViewStream mmfStream = rectifiedTabletopMmf.CreateViewStream())
            {
                unsafe
                {
                    IntPtr ptr = ResultsDllWrapper.getFactoryImage(ImageProductType.RectifiedTabletopProduct);
                    using (UnmanagedMemoryStream srcStream = new UnmanagedMemoryStream((byte*)ptr, imgSize, imgSize, FileAccess.Read))
                    {
                        srcStream.CopyTo(mmfStream);
                    }
                }
            }
            //mutex.ReleaseMutex();
        }
    }
}
