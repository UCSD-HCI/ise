using System;
using System.Windows.Forms;
using Multitouch.Service.Logic;

namespace Multitouch.Service
{
	class MultitouchInputContext : ApplicationContext
	{
		MultitouchInput input;

		public MultitouchInputContext()
		{
			input = new MultitouchInput();
			input.Start();
		}

		protected override void ExitThreadCore()
		{
			if (input != null)
			{
				input.Stop();
				input = null;
			}
			base.ExitThreadCore();
		}
	}
}