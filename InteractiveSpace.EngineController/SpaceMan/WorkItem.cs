using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace InteractiveSpace.EngineController
{
    abstract public class WorkItem
    {
        //Types of possible work item or content
        enum Type
        {
            Document,
            Note,
            Image,
            Tool,
            Unknown
        }
        //Function to serialize to xml
        abstract public XmlElement SerializeToXML(XmlDocument doc);

    }

    class ImageItem : WorkItem
    {
        private string filename;

        public string Filename
        {
            get { return filename; }
            set { filename = value; }
        }
        public ImageItem(string name)
        {
            filename = name;
        }
        public override XmlElement SerializeToXML(XmlDocument doc)
        {
            XmlElement xmlEl = doc.CreateElement("ImageItem");
            XmlElement childEl = doc.CreateElement("Filename");
            childEl.InnerText = this.filename;
            xmlEl.AppendChild(childEl);
            return xmlEl;
        }            
    }

    class TxtItem : WorkItem
    {
        private string filename;

        public string Filename
        {
            get { return filename; }
            set { filename = value; }
        }
        public TxtItem(string name)
        {
            filename = name;
        }
        public override XmlElement SerializeToXML(XmlDocument doc)
        {
            XmlElement xmlEl = doc.CreateElement("TxtItem");
            XmlElement childEl = doc.CreateElement("Filename");
            childEl.InnerText = this.filename;
            xmlEl.AppendChild(childEl);
            return xmlEl;
        }
    }

    class NoteItem : WorkItem
    {
        private string contents;

        public string Contents
        {
            get { return contents; }
            set { contents = value; }
        }
        public NoteItem(string contents)
        {
            this.contents = contents;            
        }
        public override XmlElement SerializeToXML(XmlDocument doc)
        {

            XmlElement xmlEl = doc.CreateElement("NoteItem");
            XmlElement childEl = doc.CreateElement("Contents");
            childEl.InnerText = this.contents;
            xmlEl.AppendChild(childEl);
            return xmlEl;
        }

    }
}
