using System;
using Gtk;
using System.Threading.Tasks;
using System.Threading;
using csharpServices;
using System.Linq.Expressions;
using System.Collections.Generic;
using System.Reflection;
/**
 * This Administrators primary job is to handle:
 * Automated Alternate Routing
 * and this means that incoming 'pings' from clients/customers will automatic reroute to online managers and then they
 * will pick out a call and handle it
 * - just like an old fasion phone switch system in a company with many sales people
 */
public partial class MainWindow: Gtk.Window
{
	DOPSHandler dopsHandler = new DOPSHandler();
	cloudChatPacketHandler chatHandler = new cloudChatPacketHandler();
	protected Boolean bIsConnected=false;
	handleManagerTreeView managerTreeView;
	handleCustomerTreeView customerTreeView;
	object obj;

	public MainWindow () : base (Gtk.WindowType.Toplevel)
	{
		Build();
		// Fetch handles to the treeviews
		managerTreeView = new handleManagerTreeView(this.nodeviewManagers);
		customerTreeView = new handleCustomerTreeView(this.nodeviewCustomers, "Status", "Name", "visit on Homepage");
	}

	protected void OnDeleteEvent (object sender, DeleteEventArgs a)
	{
		Application.Quit ();
		a.RetVal = true;
	}


	protected void handleConnectionWithServer(object gtkObj)
	{
		if(gtkObj == null) throw new Exception("ERROR: object was null");
		this.setConnectIconToStop(gtkObj);

		new Task(() => {
			obj = gtkObj;
			if(dopsHandler.connectToDOPsServer()) {
				this.setConnectIconToDisconnect(this.obj);
				this.handleCommunication(this.obj, dopsHandler);
				this.UpdateStatusBarText("STOPPED receiving incomming data from DOPs SERVER - possible ERROR");
				this.setConnectIconToConnect(this.obj);
			}
			else {
				setConnectIconToConnect(this.obj);
				this.UpdateStatusBarText("Failed to communicate with DOPs SERVER");
			}
		}).Start();

	}

	protected void setConnectIconToStop(object gtkObj)
	{
		if(gtkObj == null) throw new Exception("ERROR: object was null");
		((Gtk.Action)gtkObj).StockId = "gtk-stop"; // change icon indicating a connection is established and if pressing again then a disconnect will happen
		((Gtk.Action)gtkObj).ShortLabel = " please wait...";
		((Gtk.Action)gtkObj).Tooltip = "-- currently busy trying to communicate with server";
	}

	protected void setConnectIconToConnect(object gtkObj)
	{
		if(gtkObj == null) throw new Exception("ERROR: object was null");
		((Gtk.Action)gtkObj).ShortLabel = "";
		((Gtk.Action)gtkObj).StockId = "gtk-connect"; // change icon indicating a connection is established and if pressing again then a disconnect will happen
		((Gtk.Action)gtkObj).Tooltip = "Will connect to DOPs server - allowing cloud chat administrator to communicate with cloud chat managers and cloudchat clients";
	}

	protected void setConnectIconToDisconnect(object gtkObj)
	{
		if(gtkObj == null) throw new Exception("ERROR: object was null");
		((Gtk.Action)gtkObj).StockId = "gtk-disconnect"; // change icon indicating a connection is established and if pressing again then a disconnect will happen
		((Gtk.Action)gtkObj).ShortLabel = "disconnect";
		this.UpdateStatusBarText("Communication with DOPs SERVER is established");
		Thread.Sleep(1500);
		((Gtk.Action)gtkObj).ShortLabel = "";
	}

	protected void handleCommunication(object gtkObj, DOPSHandler dopsHandler)
	{
		try {
			byte[] data = null;
			AARHandler aar = new AARHandler(dopsHandler);
			// run forever until user disconnect or an error occurs
			while( (data = dopsHandler.waitForIncomming()).Length > 0 && ((Gtk.Action)gtkObj).StockId == "gtk-disconnect" && dopsHandler.bConnected == true) {
				//UpdateStatusBarText("Receiving incomming data from DOPs SERVER..."); // this causes -critical error for gtk -- somehow update ui from different thread is causing issues
				dedAnalyzed dana = chatHandler.parseDEDpacket(data);
				this.managerTreeView.updateWithIncomingData(dana);
				this.customerTreeView.updateWithIncomingData(dana);
				aar.handleRouting(dana);
				//UpdateStatusBarText(dana.type); // this causes -critical error for gtk -- somehow update ui from different thread is causing issues
			}
			shutdownCommunication(gtkObj);
		}
		catch (Exception e)
		{
			Console.WriteLine("ERROR: Serious error happened during communication : " );
			Console.WriteLine(e.Message.ToString());
			shutdownCommunication(gtkObj);
		}
	}

	private void shutdownCommunication(object gtkObj)
	{
		bIsConnected = false;
		dopsHandler.disconnectFromDOPsServer();
		setConnectIconToConnect(gtkObj); // Since communication is lost, then connection is also lost, thus reset icon
		this.UpdateStatusBarText("DOPs SERVER is disconnected");
	}

	private void UpdateStatusBarText(string text)
	{
		var contextId = this.statusbar6.GetContextId("info");
		this.statusbar6.Push(contextId, text );
		ShowAll();
	}

	protected void OnConnectActionActivated (object gtkObj, EventArgs args)
	{
		bIsConnected = dopsHandler.isConnected();
		if (!bIsConnected) {
			handleConnectionWithServer(gtkObj);
		} else {
			bIsConnected = false;
			dopsHandler.disconnectFromDOPsServer();
			setConnectIconToConnect(gtkObj);
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
		if(changeUniqueIdDlg.Run() == (int)ResponseType.Ok) {
			dopsHandler.setUniqueID(changeUniqueIdDlg.UniqueID);
		}
		changeUniqueIdDlg.Destroy();
	}
}
