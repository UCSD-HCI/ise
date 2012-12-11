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
    public partial class InteractiveSpacesGUI : Window
    {
        //SpaceMan
        private SpaceMan spaceManager = SpaceMan.Instance;
        private MainWindow mainWindow;
        private DocumentAddedDelegate onDocumentAddedDelegate;
        private DocumentRemovedDelegate onDocumentRemovedDelegate;
        private string currentDocumentTrigger;

        public InteractiveSpacesGUI()
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
            SpaceMan.ToastMessageDelegate toastMessageDel = new SpaceMan.ToastMessageDelegate(ToastMessage);
            spaceManager.setToastMessageCallback(toastMessageDel);
            SpaceMan.WorkspaceInformationDelegate workspaceInfoDel = new SpaceMan.WorkspaceInformationDelegate(onWorkspaceInfoChanged);
            spaceManager.setWorkspaceInfoCallback(onWorkspaceInfoChanged);
            WorkspaceCanvas.Height = (WorkspaceCanvas.ActualWidth / spaceManager.WorkspaceArea.Width) * spaceManager.WorkspaceArea.Height - 20;

            ToastMessage("InteractiveSpaces");
            
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
            currentDocumentTrigger = docName;
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
            currentDocumentTrigger = null;
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

        void onWorkspaceInfoChanged(LinkedList<Area> windows)
        {
            Area workspaceArea = new Area(1920, 0, 1600, 1200);
            double scale = (double)WorkspaceCanvas.ActualWidth / workspaceArea.Width;
            WorkspaceCanvas.Children.Clear();
            foreach (Area a in windows)
            {
                Rectangle newWin = new Rectangle
                {
                    Stroke = Brushes.LightBlue,
                    StrokeThickness = 2
                };
                newWin.Width = (a.Width * scale);
                newWin.Height = (a.Height * scale);
                Canvas.SetLeft(newWin, (a.X - workspaceArea.X) * scale);
                Canvas.SetTop(newWin, a.Y * scale);
                WorkspaceCanvas.Children.Add(newWin);
            }

        }

        Point mapLocationFromCanvasToDesktop(Point locOnCanvas)
        {
            Area workspaceArea = new Area(1920, 0, 1600, 1200);
            double scale = (double)WorkspaceCanvas.ActualWidth / workspaceArea.Width;
            Point locOnDesktop = new Point();
            locOnDesktop.X = (locOnCanvas.X / scale) + workspaceArea.X;
            locOnDesktop.Y = (locOnCanvas.Y / scale);
            return locOnDesktop;
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
            Point location = mapLocationFromCanvasToDesktop(e.GetPosition(WorkspaceCanvas));

            // Do something with the data...
            int numFiles = 0;
            foreach (string fileName in FileList)
            {
                spaceManager.AddToWorkspace(fileName, location);
            }
            //WorkspaceCanvas.Text += numFiles + " files added!\n";

        }

        private void RecogBoxButton_Click(object sender, RoutedEventArgs e)
        {            
            LLAHCropWindow llahWin = new LLAHCropWindow();
            llahWin.MainWindow = mainWindow;
            llahWin.Show();
        }

        public static void ToastMessage(string msg)
        {
            ToastWindow toastWin = new ToastWindow(msg);
            toastWin.Top = 500;
            toastWin.Left = 2300;
            toastWin.Show();
        }

        private void RecogRegenerateDBButton_Click(object sender, RoutedEventArgs e)
        {
            CommandDllWrapper.regenerateLLAHDB();
        }     
    }
}
