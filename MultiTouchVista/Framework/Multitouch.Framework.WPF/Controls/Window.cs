using System;
using System.ComponentModel;
using System.Windows;
using Multitouch.Framework.WPF.Input;

namespace Multitouch.Framework.WPF.Controls
{
	/// <summary>
	/// Extends WPF <see cref="System.Windows.Window"/> to support Multitouch events. WindowStyle is None, WindowState is Maximized.
	/// </summary>
	public class Window : System.Windows.Window
	{
		MultitouchInputProvider inputProvider;

		static Window()
		{
			DefaultStyleKeyProperty.OverrideMetadata(typeof(Window), new FrameworkPropertyMetadata(typeof(Window)));
		}

		/// <summary>
		/// Raises the <see cref="E:System.Windows.Window.SourceInitialized"/> event.
		/// </summary>
		/// <param name="e">An <see cref="T:System.EventArgs"/> that contains the event data.</param>
		protected override void OnSourceInitialized(EventArgs e)
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

            //Debug.Assert(secondaryScreen != null);


            System.Drawing.Rectangle workingArea = secondaryScreen != null ? secondaryScreen.WorkingArea : System.Windows.Forms.Screen.PrimaryScreen.WorkingArea;
            this.Left = workingArea.Left;
            this.Top = workingArea.Top;
            this.Width = workingArea.Width;
            this.Height = workingArea.Height;
            this.WindowState = System.Windows.WindowState.Maximized;

			inputProvider = new MultitouchInputProvider(PresentationSource.FromVisual(this));
			base.OnSourceInitialized(e);
		}

		/// <summary>
		/// Raises the <see cref="E:System.Windows.Window.Closing"/> event.
		/// </summary>
		/// <param name="e">A <see cref="T:System.ComponentModel.CancelEventArgs"/> that contains the event data.</param>
		protected override void OnClosing(CancelEventArgs e)
		{
			if (inputProvider != null)
			{
				inputProvider.Dispose();
				inputProvider = null;
			}
			base.OnClosing(e);
		}
	}
}