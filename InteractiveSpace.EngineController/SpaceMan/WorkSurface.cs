using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml;
using System.Diagnostics;

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
        public WorkSpace(Area area)
        {
            this.area = area;
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
                Trace.WriteLine("Loading window at (" + wi.window.Left + "," + wi.window.Top + ")");
                wi.window.Show();
            }
            isVisible = true;
        }

        public LinkedList<Area> GetAllWindowAreas()
        {
            LinkedList<Area> windowAreas = new LinkedList<Area>();
            foreach (WorkWindow wi in windows)
            {
                windowAreas.AddLast(new Area((int)wi.window.Left, (int)wi.window.Top, (int)wi.window.Width, (int)wi.window.Height));
            }
            return windowAreas;
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

    }
}
