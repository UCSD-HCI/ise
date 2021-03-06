﻿using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using Multitouch.Framework.Input;
using Multitouch.Framework.WPF.Input;
using Contact = Multitouch.Framework.WPF.Input.Contact;
using ContactEventArgs = Multitouch.Framework.WPF.Input.ContactEventArgs;

namespace TestApplication
{
	/// <summary>
	/// Interaction logic for Window1.xaml
	/// </summary>
	public partial class Window1
	{
		int repeatButtonCount;

		public ObservableCollection<Contact> Contacts1 { get; private set; }
		public ObservableCollection<Contact> Contacts2 { get; private set; }
		public IEnumerable<InkCanvasEditingMode> InkEditingModes { get; private set; }
        
		public InkCanvasEditingMode InkEditingMode
		{
			get { return (InkCanvasEditingMode)GetValue(InkEditingModeProperty); }
			set { SetValue(InkEditingModeProperty, value); }
		}

		// Using a DependencyProperty as the backing store for InkEditingMode.  This enables animation, styling, binding, etc...
		public static readonly DependencyProperty InkEditingModeProperty =
			DependencyProperty.Register("InkEditingMode", typeof(InkCanvasEditingMode), typeof(Window1), new UIPropertyMetadata(InkCanvasEditingMode.Ink));

		public static readonly DependencyPropertyKey PicturesPropertyKey = DependencyProperty.RegisterReadOnly("Pictures",
			typeof(ObservableCollection<string>), typeof(Window1), new PropertyMetadata(null));
		public static readonly DependencyProperty PicturesProperty = PicturesPropertyKey.DependencyProperty;

		public static readonly ICommand TestCommand = new RoutedUICommand("Command", "TestCommand", typeof(Window1));

		public static readonly DependencyProperty CameraImageProperty =
			DependencyProperty.Register("CameraImage", typeof(ImageSource), typeof(Window1), new UIPropertyMetadata(null));

		ContactHandler contactHandler;

		public ImageSource CameraImage
		{
			get { return (ImageSource)GetValue(CameraImageProperty); }
			set { SetValue(CameraImageProperty, value); }
		}

		public Window1()
		{
			Contacts1 = new ObservableCollection<Contact>();
			Contacts2 = new ObservableCollection<Contact>();
			SetValue(PicturesPropertyKey, new ObservableCollection<string>());
			InkEditingModes = new[] {InkCanvasEditingMode.Ink, InkCanvasEditingMode.EraseByPoint, InkCanvasEditingMode.EraseByStroke };
			DataContext = this;
			InitializeComponent();

			Loaded += Window1_Loaded;

			ICollectionView view = CollectionViewSource.GetDefaultView(Contacts1);
			view.SortDescriptions.Add(new SortDescription("Id", ListSortDirection.Ascending));
		}

		void Window1_Loaded(object sender, RoutedEventArgs e)
		{
			WindowInteropHelper helper = new WindowInteropHelper(this);
			contactHandler = new ContactHandler(helper.Handle);
			contactHandler.Frame += HandleFrame;
			contactHandler.ReceiveImageType(ImageType.Binarized, true);
			contactHandler.ReceiveEmptyFrames = true;
		}

		protected override void OnClosing(CancelEventArgs e)
		{
			contactHandler.Dispose();
			base.OnClosing(e);
		}

		void HandleFrame(object sender, FrameEventArgs e)
		{
			ImageData imageData;
			if (e.TryGetImage(ImageType.Binarized, 0, 0, 320, 240, out imageData))
			{
				WriteableBitmap bitmap = CameraImage as WriteableBitmap;
				if (bitmap == null)
				{
					bitmap = new WriteableBitmap(imageData.Width, imageData.Height, 96, 96, PixelFormats.Gray8, BitmapPalettes.Gray256);
					CameraImage = bitmap;
				}
				bitmap.WritePixels(new Int32Rect(0, 0, imageData.Width, imageData.Height), imageData.Data, imageData.Stride, 0);
			}
		}

		public ObservableCollection<string> Pictures
		{
			get { return (ObservableCollection<string>)GetValue(PicturesProperty); }
		}

		protected override void OnInitialized(EventArgs e)
		{
			string picturesPath = Environment.GetFolderPath(Environment.SpecialFolder.MyPictures);
			string[] pictures = Directory.GetFiles(picturesPath, "*.jpg");
			foreach (string file in pictures.Take(5))
				Pictures.Add(file);

			base.OnInitialized(e);
		}

		private void list1_ContactMoved(object sender, ContactEventArgs e)
		{
			Contacts1.Remove(e.Contact);
			Contacts1.Add(e.Contact);
		}

		private void Button_Click(object sender, RoutedEventArgs e)
		{
			MessageBox.Show("I was clicked");
		}

		private void RepeatButton_Click(object sender, RoutedEventArgs e)
		{
			repeatButtonCount++;
			repeatButton.Content = repeatButtonCount;
		}

		private void CommandBinding_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			MessageBox.Show("command executed");
		}

		private void list1_ContactEnter(object sender, ContactEventArgs e)
		{
			Contacts1.Add(e.Contact);
		}

		private void list1_ContactLeave(object sender, ContactEventArgs e)
		{
			Contacts1.Remove(e.Contact);
		}

		private void ListBox_NewContact(object sender, NewContactEventArgs e)
		{
			IInputElement element = listInkEditingModes.InputHitTest(e.GetPosition(listInkEditingModes));
			ListBoxItem container = ItemsControl.ContainerFromElement(listInkEditingModes, (DependencyObject) element) as ListBoxItem;
			if (container != null)
				container.IsSelected = true;
		}
	}
}