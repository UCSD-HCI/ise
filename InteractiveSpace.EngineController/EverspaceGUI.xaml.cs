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
        private SpaceMan spaceManager = SpaceMan.Instance;
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

        #endregion
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            spaceManager.BootstrapWorkspaces();
            RegisterDelegatesWithDocumentRecognition();
            ToastMessage("Loading");
            
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
            WorkSpace newWs = null;
            //findworkspace with that document trigger
            string docName = Marshal.PtrToStringAnsi(documentName);
            System.Diagnostics.Trace.WriteLine("Added Document " + docName);
            
            Dispatcher.BeginInvoke((Action)delegate()
            {
                spaceManager.onDocumentTriggered(docName);

            }, null);
        }

        unsafe void OnDocumentRemoved(IntPtr documentName)
        {
            //findworkspace with that document trigger
            string docName = Marshal.PtrToStringAnsi(documentName);
            System.Diagnostics.Trace.WriteLine("Removed Document " + docName);

            Dispatcher.BeginInvoke((Action)delegate()
            {
                spaceManager.onDocumentRemoved(docName);

            }, null);
        }

        private void Load1Button_Click(object sender, RoutedEventArgs e)
        {

        }

        private void Load2Button_Click(object sender, RoutedEventArgs e)
        {


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
                spaceManager.AddToWorkspace(fileName);
            }
            //WorkspaceCanvas.Text += numFiles + " files added!\n";

        }

        private void RecogBoxButton_Click(object sender, RoutedEventArgs e)
        {            
            LLAHCropWindow llahWin = new LLAHCropWindow();
            llahWin.MainWindow = mainWindow;
            llahWin.Show();
        }

        private void ToastMessage(string msg)
        {
            ToastWindow toastWin = new ToastWindow(msg);
            toastWin.Top = 500;
            toastWin.Left = 2300;
            toastWin.Show();
        }     
    }
}
