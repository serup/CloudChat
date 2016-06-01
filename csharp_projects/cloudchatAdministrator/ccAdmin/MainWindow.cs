using System;
using Gtk;
using System.Threading.Tasks;
using System.Threading;
using csharpServices;
using System.Linq.Expressions;
using System.Collections.Generic;
using System.Reflection;

public partial class MainWindow: Gtk.Window
{
	DOPSHandler dopsHandler = new DOPSHandler();
	cloudChatHandler chatHandler = new cloudChatHandler();
	protected Boolean bIsConnected=false;
	handleManagerTreeView hmTreeView;

	public MainWindow () : base (Gtk.WindowType.Toplevel)
	{
		Build();
		hmTreeView = new handleManagerTreeView(this.nodeviewManagers);
	}

	protected void OnDeleteEvent (object sender, DeleteEventArgs a)
	{
		Application.Quit ();
		a.RetVal = true;
	}


	protected void handleConnectionWithServer(object o, bool bIsConnected)
	{
		new Task(() => {
			if(dopsHandler.connectToDOPsServer())
			{
				((Gtk.Action)o).StockId = "gtk-disconnect"; // change icon indicating a connection is established and if pressing again then a disconnect will happen
				((Gtk.Action)o).ShortLabel = "disconnect";
				this.UpdateStatusBarText("Communication with DOPs SERVER is established");
				Thread.Sleep(2000);
				((Gtk.Action)o).ShortLabel = "";
				byte[] data = null;
				while( (data = dopsHandler.waitForIncomming()).Length > 0 && ((Gtk.Action)o).StockId == "gtk-disconnect") {
					this.UpdateStatusBarText("Receiving incomming data from DOPs SERVER...");
					dedAnalyzed dana = chatHandler.parseDEDpacket(data);

					string[] elementNames = dana.getElementNames();
					hmTreeView.setTitlesOnColumns(elementNames);

					this.UpdateStatusBarText(dana.type);
				}
				this.UpdateStatusBarText("STOPPED receiving incomming data from DOPs SERVER - possible ERROR");
				// connection failed - perhaps try again later
				((Gtk.Action)o).ShortLabel = "";
				((Gtk.Action)o).StockId = "gtk-connect"; // change icon indicating a connection is established and if pressing again then a disconnect will happen
				((Gtk.Action)o).Tooltip = "Will connect to DOPs server - allowing cloud chat administrator to communicate with cloud chat managers and cloudchat clients";
			}
			else {
				// connection failed - perhaps try again later
				((Gtk.Action)o).ShortLabel = "";
				((Gtk.Action)o).StockId = "gtk-connect"; // change icon indicating a connection is established and if pressing again then a disconnect will happen
				((Gtk.Action)o).Tooltip = "Will connect to DOPs server - allowing cloud chat administrator to communicate with cloud chat managers and cloudchat clients";
				this.UpdateStatusBarText("Failed to communicate with DOPs SERVER");
			}
		}).Start();
	}


	private void UpdateStatusBarText(string text)
	{
		var contextId = this.statusbar6.GetContextId("info");
		this.statusbar6.Push(contextId, text );
	}

	protected void OnConnectActionActivated (object o, EventArgs args)
	{
		bIsConnected = dopsHandler.isConnected();
		if (!bIsConnected) {
			((Gtk.Action)o).StockId = "gtk-stop"; // change icon indicating a connection is established and if pressing again then a disconnect will happen
			((Gtk.Action)o).ShortLabel = " please wait...";
			handleConnectionWithServer(o, bIsConnected);
		} else {
			bIsConnected = false;
			dopsHandler.disconnectFromDOPsServer();
			((Gtk.Action)o).StockId = "gtk-connect"; // change icon indicating a connection is established and if pressing again then a disconnect will happen
			((Gtk.Action)o).Tooltip = "Will connect to DOPs server - allowing cloud chat administrator to communicate with cloud chat managers and cloudchat clients";
			this.UpdateStatusBarText("DOPs SERVER is disconnected");
		}
	}


	protected void OnHelpActionActivated (object sender, EventArgs e)
	{
		throw new NotImplementedException ();
	}

	protected void OnMenubarActivateCurrent(object o, ActivateCurrentArgs args)
	{
		throw new NotImplementedException();
	}


	protected void OnChangeUniqueIDActionActivated(object sender, EventArgs e)
	{
		csharpServices.changeUniqueID changeUniqueIdDlg = new csharpServices.changeUniqueID(dopsHandler.getUniqueID());
		//changeUniqueIdDlg.UniqueID = dopsHandler.getUniqueID();
		if(changeUniqueIdDlg.Run() == (int)ResponseType.Ok) {
			dopsHandler.setUniqueID(changeUniqueIdDlg.UniqueID);
		}
		changeUniqueIdDlg.Destroy();
	}
}
