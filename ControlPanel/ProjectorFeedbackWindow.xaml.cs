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
using System.Diagnostics;
using ControlPanel.NativeWrappers;

namespace ControlPanel
{
    /// <summary>
    /// Interaction logic for ProjectorFeedbackWindow.xaml
    /// </summary>
    public partial class ProjectorFeedbackWindow : Window
    {
        public const int CHESSBOARD_ROWS = 5;
        public const int CHESSBOARD_COLS = 8;

        private List<Ellipse> checkPointEllipses;

        public ProjectorFeedbackWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            // Try to position application to first non-primary monitor
            System.Windows.Forms.Screen secondaryScreen = null;
            foreach (var screen in System.Windows.Forms.Screen.AllScreens)
            {
                if (!screen.Primary)
                {
                    secondaryScreen = screen;
                    break;
                }
            }

            Debug.Assert(secondaryScreen != null);

            System.Drawing.Rectangle workingArea = secondaryScreen.WorkingArea;
            this.Left = workingArea.Left;
            this.Top = workingArea.Top;
            this.Width = workingArea.Width;
            this.Height = workingArea.Height;
            this.WindowState = System.Windows.WindowState.Maximized;
        }

        public FloatPoint3D[] ShowChessboard()
        {
            FloatPoint3D[] refCorners = new FloatPoint3D[(CHESSBOARD_ROWS - 1) * (CHESSBOARD_COLS - 1)];
            double width = chessboardCanvas.Width / CHESSBOARD_COLS;
            double height = chessboardCanvas.Height / CHESSBOARD_ROWS;

            for (int i = 0; i < CHESSBOARD_ROWS; i++)
            {
                for (int j = 0; j < CHESSBOARD_COLS; j++)
                {
                    if (i >= 1 && j >= 1)
                    {
                        Point innerCornerInCanvas = new Point(width * j, height * i);
                        Point innerCorner = chessboardCanvas.TranslatePoint(innerCornerInCanvas, globalCanvas);
                        refCorners[(i - 1) * (CHESSBOARD_COLS - 1) + (j - 1)] = new FloatPoint3D()
                        {
                            x = (float)innerCorner.X,
                            y = (float)innerCorner.Y,
                            z = 0
                        };
                    }

                    if ((i + j) % 2 != 0)
                    {
                        Rectangle rect = new Rectangle()
                        {
                            Width = width,
                            Height = height,
                            Fill = Brushes.White
                        };
                        Canvas.SetTop(rect, height * i);
                        Canvas.SetLeft(rect, width * j);
                        chessboardCanvas.Children.Add(rect);
                    }
                }
            }

            chessboardBorderCanvas.Visibility = Visibility.Visible;
            return refCorners;
        }

        public void HideChecssboard()
        {
            chessboardBorderCanvas.Visibility = Visibility.Hidden;
            chessboardCanvas.Children.Clear();
        }

        public unsafe void DrawCheckpoints(FloatPoint3D* checkPoints, int checkPointNum)
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                checkPointEllipses = new List<Ellipse>(checkPointNum);
                for (int i = 0; i < checkPointNum; i++)
                {
                    checkPointEllipses[i] = new Ellipse()
                    {
                        Width = 10,
                        Height = 10,
                        Stroke = Brushes.Yellow,
                        StrokeThickness = 2
                    };

                    globalCanvas.Children.Add(checkPointEllipses[i]);
                    Canvas.SetLeft(checkPointEllipses[i], checkPoints[i].x);
                    Canvas.SetTop(checkPointEllipses[i], checkPoints[i].y);
                }
            }, null);
        }
    }
}
