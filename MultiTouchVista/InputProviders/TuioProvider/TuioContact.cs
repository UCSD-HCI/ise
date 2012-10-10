using System;
using System.Windows;
using Multitouch.Contracts;
using TUIO;

namespace TuioProvider
{
	class TuioContact : Contact
	{
		public TuioContact(TuioCursor cursor, ContactState state, System.Drawing.Size monitorSize)
			: base(cursor.getFingerID(), state, new Point(0, 0), 20, 20)
		{

            float x, y;

            //Edit by Wander
            //get monitor settings 
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

            if (secondaryScreen == null)
            {
                x = cursor.getScreenX(monitorSize.Width);
                y = cursor.getScreenY(monitorSize.Height);
            }
            else
            {
                x = cursor.getX() * secondaryScreen.WorkingArea.Width + secondaryScreen.WorkingArea.Left;
                y = cursor.getY() * secondaryScreen.WorkingArea.Height + secondaryScreen.WorkingArea.Top;
            }

			Position = new Point(x, y);
			Orientation = 0;
		}
	}
}