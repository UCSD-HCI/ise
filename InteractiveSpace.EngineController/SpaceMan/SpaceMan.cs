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
        private static SpaceMan instance;
        string workWindowDirectory;
        LinkedList<WorkSpace> currentSpaces;
        Dictionary<string, WorkSpace> documentSpaceMap;
        WorkSpace myWs1;
        WorkSpace myWs2;
        WorkSpace currentSpace;
        Area workspaceArea;
        

        private SpaceMan()
        {
            currentSpaces = new LinkedList<WorkSpace>();
            documentSpaceMap = new Dictionary<string, WorkSpace>();
            workspaceArea = new Area(1920, 0, 1600, 1200);

        }
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

        public void BootstrapWorkspaces()
        {
            //Create two test worksurfaces
            workWindowDirectory = System.IO.Path.Combine(@"C:\InteractiveSpaceEngineData\Databases", "WorkItems");

            myWs1 = new WorkSpace(workspaceArea);            
            myWs2 = new WorkSpace(workspaceArea);

            WorkItem myWi1 = new ImageItem(System.IO.Path.Combine(workWindowDirectory, "finger.jpg"));
            WorkItem myWi2 = new ImageItem(System.IO.Path.Combine(workWindowDirectory, "xray1.jpg"));
            WorkItem myWi3 = new ImageItem(System.IO.Path.Combine(workWindowDirectory, "blood1.jpg"));
            WorkItem myWi4 = new ImageItem(System.IO.Path.Combine(workWindowDirectory, "xray2.jpg"));
            WorkItem myWi5 = new ImageItem(System.IO.Path.Combine(workWindowDirectory, "ekg1.jpg"));

            myWs1.AddItem(myWi1, 5, 5);
            myWs1.AddItem(myWi2, 100, 5);
            myWs1.AddItem(myWi3, 200, 5);

            myWs2.AddItem(myWi4, 500, 200);
            myWs2.AddItem(myWi5, 500, 5);
            currentSpaces.AddLast(myWs1);
            currentSpaces.AddLast(myWs2);
            documentSpaceMap.Add("cvpr01p1013.jpg", myWs1);
            documentSpaceMap.Add("cvpr01p1010.jpg", myWs2);
        }

        public void onDocumentTriggered(string LLAHDocName)
        {
            string filename = Path.GetFileName(LLAHDocName);
            WorkSpace ws;
            if (documentSpaceMap.TryGetValue(filename, out ws))
            {
                ws.Load();
                currentSpace = ws;
            }
            else
            {
                ws = new WorkSpace(workspaceArea);
                currentSpaces.AddLast(ws);
                documentSpaceMap.Add(filename, ws);
                
                ws.Load();
                currentSpace = ws;
            }            
        }

        public void onDocumentRemoved(string LLAHDocName)
        {
            string filename = Path.GetFileName(LLAHDocName);
            WorkSpace ws;
            if (documentSpaceMap.TryGetValue(filename, out ws))
            {
                if (currentSpace == ws)
                ws.Unload();
            }
            else
            {
            }
        }

        public void AddToWorkspace(string filename)
        {
            string ext = System.IO.Path.GetExtension(filename).ToLower();
            if ((ext.CompareTo(".jpg") == 0) ||
                (ext.CompareTo(".png") == 0) ||
                (ext.CompareTo(".gif") == 0))
            {
                currentSpace.AddItem(new ImageItem(filename));
            }
            else if (ext.CompareTo(".txt") == 0)
            {
                currentSpace.AddItem(new TxtItem(filename));
            }
        }
        public void SaveXML()
        {
            XmlDocument myDoc = new XmlDocument();
            XmlElement contents = myWs1.SerializeToXML(myDoc);
            myDoc.AppendChild(contents);
            try
            {
                FileStream fileStream =
                  new FileStream("WorkSurfaces.xml", FileMode.Create);
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
