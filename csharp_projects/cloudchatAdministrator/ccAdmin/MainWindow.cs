﻿using System;
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
	cloudChatHandler chatHandler = new cloudChatHandler();
	protected Boolean bIsConnected=false;
	handleManagerTreeView managerTreeView;
	handleCustomerTreeView customerTreeView;

	public MainWindow () : base (Gtk.WindowType.Toplevel)
	{
		Build();
		// Fetch handles to the treeviews
		managerTreeView = new handleManagerTreeView(this.nodeviewManagers);
		customerTreeView = new handleCustomerTreeView(this.nodeviewCustomers);
	}

	protected void OnDeleteEvent (object sender, DeleteEventArgs a)
	{
		Application.Quit ();
		a.RetVal = true;
	}


	protected void handleConnectionWithServer(object o, bool bIsConnected)
	{
		setConnectIconToStop(o);

		new Task(() => {
			if(dopsHandler.connectToDOPsServer()) {
				this.setConnectIconToDisconnect(o);
				this.handleCommunication(o);
				this.UpdateStatusBarText("STOPPED receiving incomming data from DOPs SERVER - possible ERROR");
				this.setConnectIconToConnect(o);
			}
			else {
				setConnectIconToConnect(o);
				this.UpdateStatusBarText("Failed to communicate with DOPs SERVER");
			}
		}).Start();

	}

	protected void setConnectIconToStop(object o)
	{
		((Gtk.Action)o).StockId = "gtk-stop"; // change icon indicating a connection is established and if pressing again then a disconnect will happen
		((Gtk.Action)o).ShortLabel = " please wait...";
		((Gtk.Action)o).Tooltip = "-- currently busy trying to communicate with server";
	}

	protected void setConnectIconToConnect(object o)
	{
		((Gtk.Action)o).ShortLabel = "";
		((Gtk.Action)o).StockId = "gtk-connect"; // change icon indicating a connection is established and if pressing again then a disconnect will happen
		((Gtk.Action)o).Tooltip = "Will connect to DOPs server - allowing cloud chat administrator to communicate with cloud chat managers and cloudchat clients";
	}

	protected void setConnectIconToDisconnect(object o)
	{
		((Gtk.Action)o).StockId = "gtk-disconnect"; // change icon indicating a connection is established and if pressing again then a disconnect will happen
		((Gtk.Action)o).ShortLabel = "disconnect";
		this.UpdateStatusBarText("Communication with DOPs SERVER is established");
		Thread.Sleep(1500);
		((Gtk.Action)o).ShortLabel = "";
	}

	protected void handleCommunication(object o)
	{
		try {
			byte[] data = null;
			// run forever until user disconnect or an error occurs
			while( (data = dopsHandler.waitForIncomming()).Length > 0 && ((Gtk.Action)o).StockId == "gtk-disconnect") {
				UpdateStatusBarText("Receiving incomming data from DOPs SERVER...");
				dedAnalyzed dana = chatHandler.parseDEDpacket(data);
				managerTreeView.updateWithIncomingData(dana);
				customerTreeView.updateWithIncomingData(dana);
				UpdateStatusBarText(dana.type);
			}
			shutdownCommunication(o);
		}
		catch (Exception e)
		{
			Console.WriteLine("ERROR: Serious error happened during communication : " );
			Console.WriteLine(e.Message.ToString());
		}
	}

	private void shutdownCommunication(object o)
	{
		bIsConnected = false;
		dopsHandler.disconnectFromDOPsServer();
		setConnectIconToConnect(o); // Since communication is lost, then connection is also lost, thus reset icon
		UpdateStatusBarText("DOPs SERVER is disconnected");
	}

	private void UpdateStatusBarText(string text)
	{
		var contextId = this.statusbar6.GetContextId("info");
		statusbar6.Push(contextId, text );
		ShowAll();
	}

	protected void OnConnectActionActivated (object o, EventArgs args)
	{
		bIsConnected = dopsHandler.isConnected();
		if (!bIsConnected) {
			handleConnectionWithServer(o, bIsConnected);
		} else {
			bIsConnected = false;
			dopsHandler.disconnectFromDOPsServer();
			setConnectIconToConnect(o);
			UpdateStatusBarText("DOPs SERVER is disconnected");
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
