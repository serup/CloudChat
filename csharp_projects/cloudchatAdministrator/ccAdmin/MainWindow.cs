using System;
using Gtk;
using System.Threading.Tasks;
using System.Threading;

public partial class MainWindow: Gtk.Window
{
	protected Boolean bIsConnected=false;
	public MainWindow () : base (Gtk.WindowType.Toplevel)
	{
		Build();
	}

	protected void OnDeleteEvent (object sender, DeleteEventArgs a)
	{
		Application.Quit ();
		a.RetVal = true;
	}

	protected void OnConnectActionActivated (object o, EventArgs args)
	{
		//TODO: check if connection with DOPS exists and if it does then disconnect, otherwise connect
		if (!bIsConnected) {
			bIsConnected = true;
			((Gtk.Action)o).StockId = "gtk-stop"; // change icon indicating a connection is established and if pressing again then a disconnect will happen
			((Gtk.Action)o).ShortLabel = " please wait...";
			new Task(() =>
				{
					Thread.Sleep(2000);
					((Gtk.Action)o).StockId = "gtk-disconnect"; // change icon indicating a connection is established and if pressing again then a disconnect will happen
					((Gtk.Action)o).ShortLabel = "disconnect";
					Thread.Sleep(2000);
					((Gtk.Action)o).ShortLabel = "";
					((Gtk.Action)o).Tooltip = "Will disconnect from DOPs server";
				}).Start();
		} else {
			bIsConnected = false;
			((Gtk.Action)o).StockId = "gtk-connect"; // change icon indicating a connection is established and if pressing again then a disconnect will happen
			((Gtk.Action)o).Tooltip = "Will connect to DOPs server - allowing cloud chat administrator to communicate with cloud chat managers and cloudchat clients";
		}
	}


	protected void OnHelpActionActivated (object sender, EventArgs e)
	{
		throw new NotImplementedException ();
	}
}
