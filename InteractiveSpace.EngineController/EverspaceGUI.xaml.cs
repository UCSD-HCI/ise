using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Xml;
using System.IO;
using InteractiveSpace.EngineController;
using InteractiveSpace.EngineController.NativeWrappers;
using System.Runtime.InteropServices;

namespace InteractiveSpace.EngineController
{
    /// <summary>
    /// Interaction logic for Everspaces.xaml
    /// </summary>
    public partial class EverspaceGUI : Window
    {
        //SpaceMan
        string workWindowDirectory;
        LinkedList<WorkSpace> mySpaces;
        WorkSpace myWs1;
        WorkSpace myWs2;
        WorkSpace currentWorkspace;

        private MainWindow mainWindow;
        private DocumentAddedDelegate onDocumentAddedDelegate;
        private DocumentRemovedDelegate onDocumentRemovedDelegate;

        public EverspaceGUI()
        {
            InitializeComponent();
        }

        public MainWindow MainWindow
        {
            get { return mainWindow; }
            set
            {
                mainWindow = value;
            }
        }

        #region SpaceMan
        private void BootstrapWorkspaces()
        {
            //Create two test worksurfaces
            workWindowDirectory = System.IO.Path.Combine(@"C:\InteractiveSpaceEngineData\Databases", "WorkItems");

            myWs1 = new WorkSpace(0, 0, 1680, 1050);
            myWs2 = new WorkSpace(0, 0, 1680, 1050);

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
        }
        private void SaveXML()
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
        #endregion
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {

            BootstrapWorkspaces();
            RegisterDelegatesWithDocumentRecognition();
            
        }

        private void RegisterDelegatesWithDocumentRecognition()
        {

            unsafe
            {                
                onDocumentAddedDelegate = new DocumentAddedDelegate(OnDocumentAdded);
                onDocumentRemovedDelegate = new DocumentRemovedDelegate(OnDocumentRemoved);
                
                IntPtr DocumentAddedCallbackPtr = Marshal.GetFunctionPointerForDelegate(onDocumentAddedDelegate);
                IntPtr DocumentRemovedCallbackPtr = Marshal.GetFunctionPointerForDelegate(onDocumentRemovedDelegate);
                CommandDllWrapper.registerDocumentCallbacks(DocumentAddedCallbackPtr, DocumentRemovedCallbackPtr);
            }
        }

        unsafe void OnDocumentAdded(IntPtr documentName)
        {
            //findworkspace with that document trigger
            string docName = Marshal.PtrToStringAnsi(documentName);
            System.Diagnostics.Trace.WriteLine("Found Document " + docName);
        }

        unsafe void OnDocumentRemoved(IntPtr documentName)
        {
            //findworkspace with that document trigger
            System.Diagnostics.Trace.WriteLine("test");

        }
        private void Load1Button_Click(object sender, RoutedEventArgs e)
        {
            if ((currentWorkspace != null) && (currentWorkspace != myWs1))
                currentWorkspace.Unload();
            myWs1.Load();
            currentWorkspace = myWs1;
        }

        private void Load2Button_Click(object sender, RoutedEventArgs e)
        {
            if ((currentWorkspace != null) && (currentWorkspace != myWs2))
                currentWorkspace.Unload();
            myWs2.Load();
            currentWorkspace = myWs2;

        }

        private void docRecognition_valueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {

        }

        // Form load event or a similar place
        private void WorkspaceCanvas_EnableDrop()
        {
            // Enable drag and drop for this form
            // (this can also be applied to any controls)
            WorkspaceCanvas.AllowDrop = true;
        }

        // This event occurs when the user drags over the form with 
        // the mouse during a drag drop operation 
        void WorkspaceCanvas_DragEnter(object sender, DragEventArgs e)
        {
            // Check if the Dataformat of the data can be accepted
            // (we only accept file drops from Explorer, etc.)
            if (e.Data.GetDataPresent(DataFormats.FileDrop, false) == true)
                e.Effects = DragDropEffects.All; // Okay
            else
                e.Effects = DragDropEffects.None; // Unknown data, ignore it

        }

        // Occurs when the user releases the mouse over the drop target 
        void WorkspaceCanvas_Drop(object sender, DragEventArgs e)
        {
            // Extract the data from the DataObject-Container into a string list
            string[] FileList = (string[])e.Data.GetData(DataFormats.FileDrop, false);

            // Do something with the data...
            int numFiles = 0;
            foreach (string fileName in FileList)
            {
                if (System.IO.Path.GetExtension(fileName).ToLower().CompareTo(".jpg") == 0)
                {
                    currentWorkspace.AddItem(new ImageItem(fileName));
                }

            }
            //WorkspaceCanvas.Text += numFiles + " files added!\n";

        }

        private void RecogBoxButton_Click(object sender, RoutedEventArgs e)
        {            
            LLAHCropWindow llahWin = new LLAHCropWindow();
            llahWin.MainWindow = mainWindow;
            llahWin.Show();
        }
    }
}
