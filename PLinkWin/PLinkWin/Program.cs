using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;
using System.IO;
using System.Threading;
using System.Diagnostics;

namespace PLinkWin
{
    class Program
    {
        static void Main(string[] args)
        {
            mRecreateAllExecutableResources();
            server s = new server();
        }

        //======================================================
        //Recreate all executable resources
        //======================================================
        private static void mRecreateAllExecutableResources()
        {
            // Get Current Assembly refrence
            Assembly currentAssembly = Assembly.GetExecutingAssembly();
            // Get all imbedded resources
            string[] arrResources = currentAssembly.GetManifestResourceNames();

            foreach (string resourceName in arrResources)
            {
                if (resourceName.EndsWith(".exe"))
                {
                    string saveAsName = resourceName;
                    FileInfo fileInfoOutputFile = new FileInfo(saveAsName);
                    
                    if (!fileInfoOutputFile.Exists)
                    {
                        //OPEN NEWLY CREATING FILE FOR WRITTING
                        FileStream streamToOutputFile = fileInfoOutputFile.OpenWrite();
                        //GET THE STREAM TO THE RESOURCES
                        Stream streamToResourceFile =
                                            currentAssembly.GetManifestResourceStream(resourceName);

                        //---------------------------------
                        //SAVE TO DISK OPERATION
                        //---------------------------------
                        const int size = 4096;
                        byte[] bytes = new byte[4096];
                        int numBytes;
                        while ((numBytes = streamToResourceFile.Read(bytes, 0, size)) > 0)
                        {
                            streamToOutputFile.Write(bytes, 0, numBytes);
                        }

                        streamToOutputFile.Close();
                        streamToResourceFile.Close();
                    }
                }
            }
        }
    }
}
