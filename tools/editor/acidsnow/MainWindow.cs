using System;
using Gtk;

public partial class MainWindow: Gtk.Window
{
	public MainWindow() : base(Gtk.WindowType.Toplevel)
	{
		Build();
	}

	protected void OnDeleteEvent(object sender, DeleteEventArgs a)
	{
		Application.Quit();
		a.RetVal = true;
	}

	protected void OnExitActionActivated(object sender, EventArgs e)
	{
		MessageDialog dlg = new MessageDialog(
			this, DialogFlags.Modal,
			MessageType.Question,
			ButtonsType.YesNo, "Exit editor ?");

		if ((ResponseType)dlg.Run() == ResponseType.Yes)
		{
			Application.Quit();
		}

		dlg.Destroy();
	}
}