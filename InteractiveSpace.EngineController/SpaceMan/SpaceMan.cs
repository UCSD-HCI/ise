using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;

namespace InteractiveSpace.EngineController
{
    class SpaceMan
    {
        //Singleton instance
        private static SpaceMan instance;

        //List of all loaded workspaces
        LinkedList<WorkSpace> workspaceList;

        //Map of trigger filename to workspace to allow loading of content
        Dictionary<string, WorkSpace> documentSpaceMap;

        //Currently active workspace
        WorkSpace activeSpace;

        //Default directory for work items (content like jpg txt)
        string workItemsDirectory;

        //Pixel dimensions of workspace 
        Area workspaceArea;

        //Delegate type for a messaging system
        public delegate void ToastMessageDelegate(string msg);

        //Delegate instance
        ToastMessageDelegate toastMessage;

        //Delegate type for sending content area information
        public delegate void WorkspaceInformationDelegate(LinkedList<Area> windows);

        //Delegate instance
        WorkspaceInformationDelegate workspaceInfoCallback;

        public Area WorkspaceArea
        {
            get { return workspaceArea; }
            set { workspaceArea = value; }
        }


        //Initializes the space manager (which contains a collection of 
        //workspaces that it can load/unload. Private because the space manager
        //is a singleton class.
        private SpaceMan()
        {
            workspaceList = new LinkedList<WorkSpace>();
            documentSpaceMap = new Dictionary<string, WorkSpace>();
            workspaceArea = new Area(1920, 0, 1600, 1200);

        }

        //Returns the singleton instance of space manager
        public static SpaceMan Instance
        {
            get
            {
                if (instance == null)
                {
                    instance = new SpaceMan();
                }
                return instance;
            }
        }

        //Just for demos - this function bootstraps the workspace with some preset data
        public void BootstrapWorkspaces()
        {
            //Create two test worksurfaces
            workItemsDirectory = System.IO.Path.Combine(@"C:\InteractiveSpaceEngineData\Databases", "WorkItems");

            WorkSpace myWs1 = new WorkSpace(workspaceArea);
            WorkSpace myWs2 = new WorkSpace(workspaceArea);

            WorkItem myWi2 = new ImageItem(System.IO.Path.Combine(workItemsDirectory, "xray1.jpg"));
            WorkItem myWi3 = new ImageItem(System.IO.Path.Combine(workItemsDirectory, "blood1.jpg"));
            WorkItem myWi4 = new ImageItem(System.IO.Path.Combine(workItemsDirectory, "xray2.jpg"));
            WorkItem myWi5 = new ImageItem(System.IO.Path.Combine(workItemsDirectory, "ekg1.jpg"));

            myWs1.AddItem(myWi2, 3000, 464);
            myWs1.AddItem(myWi3, 3000, 160);

            myWs2.AddItem(myWi4, 2010, 350);
            myWs2.AddItem(myWi5, 3000, 350);
            workspaceList.AddLast(myWs1);
            workspaceList.AddLast(myWs2);
            documentSpaceMap.Add("00000blood_p1.jpg", myWs1);
            documentSpaceMap.Add("cvpr01p1010.jpg", myWs2);
        }

        //On document triggered, space manager loads up the workspace with 
        //that filename as a trigger
        public void onDocumentTriggered(string LLAHDocName)
        {
            string filename = Path.GetFileName(LLAHDocName);
            WorkSpace ws;
            if (documentSpaceMap.TryGetValue(filename, out ws))
            {
                Toast("Loading " + Path.GetFileNameWithoutExtension(filename));
                ws.Load();
                workspaceInfoCallback(ws.GetAllWindowAreas());
                activeSpace = ws;
            }
            else
            {
                ws = new WorkSpace(workspaceArea);
                workspaceList.AddLast(ws);
                documentSpaceMap.Add(filename, ws);
                ws.Load();
                workspaceInfoCallback( ws.GetAllWindowAreas());
                activeSpace = ws;
            }            
        }

        //On document, removed unload current workspace if active
        public void onDocumentRemoved(string LLAHDocName)
        {
            string filename = Path.GetFileName(LLAHDocName);
            WorkSpace ws;
            if (documentSpaceMap.TryGetValue(filename, out ws))
            {
                if (activeSpace == ws)
                {
                    workspaceInfoCallback(new LinkedList<Area>());
                    ws.Unload();
                }
            }
            else
            {
            }
        }

        //Add new content to current workspace
        public void AddToWorkspace(string filename)
        {
            if (activeSpace != null);
            string ext = System.IO.Path.GetExtension(filename).ToLower();
            if ((ext.CompareTo(".jpg") == 0) ||
                (ext.CompareTo(".png") == 0) ||
                (ext.CompareTo(".gif") == 0))
            {
                activeSpace.AddItem(new ImageItem(filename));
            }
            else if (ext.CompareTo(".txt") == 0)
            {
                activeSpace.AddItem(new TxtItem(filename));
            }
            workspaceInfoCallback(activeSpace.GetAllWindowAreas());
            
        }

        //Add new content to current workspace at specific location
        public void AddToWorkspace(string filename, System.Windows.Point location)
        {
            if (activeSpace != null) ;
            string ext = System.IO.Path.GetExtension(filename).ToLower();
            if ((ext.CompareTo(".jpg") == 0) ||
                (ext.CompareTo(".png") == 0) ||
                (ext.CompareTo(".gif") == 0))
            {
                activeSpace.AddItem(new ImageItem(filename), (int)location.X, (int)location.Y);
            }
            else if (ext.CompareTo(".txt") == 0)
            {
                activeSpace.AddItem(new TxtItem(filename), (int)location.X, (int)location.Y);
            }
            workspaceInfoCallback(activeSpace.GetAllWindowAreas());
            
        }

        //Save workspace information to file (not completely tested)
        public void SaveXML(string filename)
        {
            XmlDocument myDoc = new XmlDocument();
            foreach (WorkSpace ws in workspaceList)
            {
                XmlElement contents = ws.SerializeToXML(myDoc);
                myDoc.AppendChild(contents);
            }
            try

            {
                FileStream fileStream =
                  new FileStream(filename, FileMode.Create);
                XmlTextWriter textWriter =
                  new XmlTextWriter(fileStream, Encoding.UTF8);
                textWriter.Formatting = Formatting.Indented;
                myDoc.Save(textWriter);
                fileStream.Close();

            }
            catch (System.IO.DirectoryNotFoundException ex)
            {
                System.ArgumentException argEx = new System.ArgumentException("XMLFile path is not valid", "WorkSurfaces.xml", ex);
                throw argEx;
            }
            catch (System.Exception ex)
            {
                System.ArgumentException argEx = new System.ArgumentException("XML File write failed", "WorkSurfaces.xml", ex);
                throw argEx;
            }
        }

        
        public void SetToastMessageCallback(ToastMessageDelegate tmd)
        {
            toastMessage = tmd;
        }

        public void SetWorkspaceInfoCallback(WorkspaceInformationDelegate wid)
        {
            workspaceInfoCallback = wid;
        }

        private void Toast(string msg)
        {
            if (toastMessage != null)
                toastMessage(msg);
        }


    }

    public struct Area
    {
        int x;
        int y;
        int width;
        int height;

        public Area(int x, int y, int width, int height)
        {
            this.x = x;
            this.y = y;
            this.width = width;
            this.height = height;
        }
        public int X
        {
            get { return x; }
            set { x = value; }
        }
        public int Y
        {
            get { return y; }
            set { y = value; }
        }
        public int Width
        {
            get { return width; }
            set { width = value; }
        }
        public int Height
        {
            get { return height; }
            set { height = value; }
        }

    }
}
