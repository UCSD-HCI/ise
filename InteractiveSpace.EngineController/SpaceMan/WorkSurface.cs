using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml;

namespace InteractiveSpace.EngineController
{
    public class WorkSpace
    {
        //List Objects        
        LinkedList<WorkWindow> windows;
        string documentTrigger = "";
        Boolean isVisible = false;
        Area area;

        public WorkSpace()
        {
            windows = new LinkedList<WorkWindow>();
        }
        public WorkSpace(int x, int y, int width, int height)
        {
            area = new Area(x, y, width, height);
            windows = new LinkedList<WorkWindow>();
        }
        public void AddItem(WorkItem wi)
        {
            WorkItemWindow newWindow = new WorkItemWindow(wi);
            WorkWindow newWorkWindow= new WorkWindow(wi, newWindow);
            
            windows.AddLast(newWorkWindow);
            if (isVisible == true)
                newWindow.Show();

        }

        /* Constructor that allows positioning of item */
        public void AddItem(WorkItem wi, int x, int y)
        {
            WorkItemWindow newWindow = new WorkItemWindow(wi);
            WorkWindow newWorkWindow = new WorkWindow(wi, newWindow);

            newWorkWindow.window.Left = x;
            newWorkWindow.window.Top  = y;

            windows.AddLast(newWorkWindow);
            if (isVisible == true)
                newWindow.Show();

        }
        public void RemoveItem()
        {
        }

        public void Load()
        {
            foreach (WorkWindow wi in windows)
            {
                wi.window.Show();
            }
            isVisible = true;
        }

        public void Unload()
        {
            foreach (WorkWindow wi in windows)
            {
                wi.window.Hide();
            }
            isVisible = false;
        }

        public XmlElement SerializeToXML(XmlDocument doc)
        {          
            //Create Root
            XmlElement root = doc.CreateElement("WorkSurface");

            //Add area
            XmlElement areaEl = doc.CreateElement("Area");
            areaEl.SetAttribute("x", this.area.X.ToString());
            areaEl.SetAttribute("y", this.area.Y.ToString());
            areaEl.SetAttribute("width", this.area.Width.ToString());
            areaEl.SetAttribute("height", this.area.Height.ToString());
            root.AppendChild(areaEl);

            XmlElement windowsEl = doc.CreateElement("Windows");
            foreach (WorkWindow ww in windows)
            {
                XmlElement windowEl = doc.CreateElement("WorkWindow");
                XmlElement areaEl2 = doc.CreateElement("Area");
                areaEl2.SetAttribute("x", ww.window.Left.ToString());
                areaEl2.SetAttribute("y", ww.window.Top.ToString());
                areaEl2.SetAttribute("width", ww.window.ActualWidth.ToString());
                areaEl2.SetAttribute("height", ww.window.ActualHeight.ToString());
                windowEl.AppendChild(areaEl2);

                XmlElement wi = ww.data.SerializeToXML(doc);
                windowEl.AppendChild(wi);

                windowsEl.AppendChild(windowEl);
            }
            root.AppendChild(windowsEl);
            return root;

        }
        public void LoadFromFile()
        {
        }
        public struct WorkWindow
        {
            public WorkItem data;           //Virtual work item
            public WorkItemWindow window;
            public WorkWindow(WorkItem d, WorkItemWindow w)
            {
                data = d;
                window = w;
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
}
