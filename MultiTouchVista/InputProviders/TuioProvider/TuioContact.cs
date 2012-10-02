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
            
			/*float x = cursor.getScreenX(monitorSize.Width);
			float y = cursor.getScreenY(monitorSize.Height);*/

            float x = cursor.getX() * 1600.0f;
            float y = cursor.getY() * 1200.0f + 1200.0f;

			Position = new Point(x, y);
			Orientation = 0;
		}
	}
}