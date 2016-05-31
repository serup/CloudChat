using System;
using Gtk;
using System.Threading.Tasks;
using System.Threading;
using csharpServices;

public partial class MainWindow: Gtk.Window
{
	DOPSHandler dopsHandler = new DOPSHandler();
	cloudChatHandler chatHandler = new cloudChatHandler();
	protected Boolean bIsConnected=false;
	public MainWindow () : base (Gtk.WindowType.Toplevel)
	{
		Build();

		// Create a column for the name
		Gtk.TreeViewColumn nameColumn = new Gtk.TreeViewColumn ();
		nameColumn.Title = "Name";

		// Create a column for the song title
		Gtk.TreeViewColumn sColumn = new Gtk.TreeViewColumn ();
		sColumn.Title = "Song Title";

		// Add the columns to the TreeView
		this.nodeviewManagers.NodeSelection.NodeView.AppendColumn(nameColumn);
		this.nodeviewManagers.NodeSelection.NodeView.AppendColumn(sColumn);

		// Create a model that will hold two strings -  Name and Song Title
		Gtk.ListStore mListStore = new Gtk.ListStore (typeof (string), typeof (string));

		// Assign the model to the TreeView
		this.nodeviewManagers.NodeSelection.NodeView.Model = mListStore;

		// Add some data to the store
		mListStore.AppendValues ("Garbage", "Dog New Tricks");

		// Create the text cell that will display the artist name
		Gtk.CellRendererText NameCell = new Gtk.CellRendererText ();

		// Add the cell to the column
		nameColumn.PackStart (NameCell, true);

		// Do the same for the song title column
		Gtk.CellRendererText sTitleCell = new Gtk.CellRendererText ();
		sColumn.PackStart (sTitleCell, true);

		// Tell the Cell Renderers which items in the model to display
		nameColumn.AddAttribute (NameCell, "text", 0);
		sColumn.AddAttribute (sTitleCell, "text", 1);


//		//////////////////////////////////////////////////////////////////////////
//		// example spawning new window 
//		// Create a Window
//		Gtk.Window window = new Gtk.Window ("TreeView Example");
//		window.SetSizeRequest (500,200);
//
//		// Create our TreeView
//		Gtk.TreeView tree = new Gtk.TreeView ();
//
//		// Add our tree to the window
//		window.Add (tree);
//
//		// Create a column for the artist name
//		Gtk.TreeViewColumn artistColumn = new Gtk.TreeViewColumn ();
//		artistColumn.Title = "Artist";
//
//		// Create a column for the song title
//		Gtk.TreeViewColumn songColumn = new Gtk.TreeViewColumn ();
//		songColumn.Title = "Song Title";
//
//		// Add the columns to the TreeView
//		tree.AppendColumn (artistColumn);
//		tree.AppendColumn (songColumn);
//
//		// Create a model that will hold two strings - Artist Name and Song Title
//		Gtk.ListStore musicListStore = new Gtk.ListStore (typeof (string), typeof (string));
//
//
//		// Assign the model to the TreeView
//		tree.Model = musicListStore;
//
//		// Add some data to the store
//		musicListStore.AppendValues ("Garbage", "Dog New Tricks");
//
//		// Create the text cell that will display the artist name
//		Gtk.CellRendererText artistNameCell = new Gtk.CellRendererText ();
//
//		// Add the cell to the column
//		artistColumn.PackStart (artistNameCell, true);
//
//		// Do the same for the song title column
//		Gtk.CellRendererText songTitleCell = new Gtk.CellRendererText ();
//		songColumn.PackStart (songTitleCell, true);
//
//		// Tell the Cell Renderers which items in the model to display
//		artistColumn.AddAttribute (artistNameCell, "text", 0);
//		songColumn.AddAttribute (songTitleCell, "text", 1);
//
//		// Show the window and everything on it
//		window.ShowAll ();
	}

	protected void OnDeleteEvent (object sender, DeleteEventArgs a)
	{
		Application.Quit ();
		a.RetVal = true;
	}

	[Gtk.TreeNode (ListOnly=true)]
	public class MyTreeNode : Gtk.TreeNode {

		string song_title;

		public MyTreeNode (string artist, string song_title)
		{
			Artist = artist;
			this.song_title = song_title;
		}

		[Gtk.TreeNodeValue (Column=0)]
		public string Artist;

		[Gtk.TreeNodeValue (Column=1)]
		public string SongTitle {get { return song_title; } }
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
