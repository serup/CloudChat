
// This file has been generated by the GUI designer. Do not modify.

public partial class MainWindow
{
	private global::Gtk.UIManager UIManager;
	
	private global::Gtk.Action actionAction;
	
	private global::Gtk.Action connectAction1;
	
	private global::Gtk.Action helpAction;
	
	private global::Gtk.Action connectAction;
	
	private global::Gtk.Action AdminAction;
	
	private global::Gtk.Action UniqueIdAction;
	
	private global::Gtk.Action changeUniqueIDAction;
	
	private global::Gtk.VBox vbox1;
	
	private global::Gtk.MenuBar menubar;
	
	private global::Gtk.HBox hbox2;
	
	private global::Gtk.Toolbar toolbar2;
	
	private global::Gtk.HBox hbox3;
	
	private global::Gtk.Notebook notebook1;
	
	private global::Gtk.ScrolledWindow GtkScrolledWindow;
	
	private global::Gtk.NodeView nodeviewManagers;
	
	private global::Gtk.Label Managers;
	
	private global::Gtk.ScrolledWindow GtkScrolledWindow1;
	
	private global::Gtk.NodeView nodeviewCustomers;
	
	private global::Gtk.Label Customers;
	
	private global::Gtk.VBox vbox2;
	
	private global::Gtk.Statusbar statusbar6;

	protected virtual void Build ()
	{
		global::Stetic.Gui.Initialize (this);
		// Widget MainWindow
		this.UIManager = new global::Gtk.UIManager ();
		global::Gtk.ActionGroup w1 = new global::Gtk.ActionGroup ("Default");
		this.actionAction = new global::Gtk.Action ("actionAction", global::Mono.Unix.Catalog.GetString ("action"), null, null);
		this.actionAction.ShortLabel = global::Mono.Unix.Catalog.GetString ("action");
		w1.Add (this.actionAction, null);
		this.connectAction1 = new global::Gtk.Action ("connectAction1", null, global::Mono.Unix.Catalog.GetString ("Will connect to DOPs server - allowing cloud chat administrator to communicate with cloud chat managers and cloudchat clients"), "gtk-connect");
		this.connectAction1.IsImportant = true;
		w1.Add (this.connectAction1, "F8");
		this.helpAction = new global::Gtk.Action ("helpAction", null, null, "gtk-help");
		w1.Add (this.helpAction, null);
		this.connectAction = new global::Gtk.Action ("connectAction", null, null, "gtk-connect");
		w1.Add (this.connectAction, "F8");
		this.AdminAction = new global::Gtk.Action ("AdminAction", global::Mono.Unix.Catalog.GetString ("Admin"), null, null);
		this.AdminAction.ShortLabel = global::Mono.Unix.Catalog.GetString ("Admin");
		w1.Add (this.AdminAction, null);
		this.UniqueIdAction = new global::Gtk.Action ("UniqueIdAction", global::Mono.Unix.Catalog.GetString ("UniqueId"), null, null);
		this.UniqueIdAction.ShortLabel = global::Mono.Unix.Catalog.GetString ("UniqueId");
		w1.Add (this.UniqueIdAction, null);
		this.changeUniqueIDAction = new global::Gtk.Action ("changeUniqueIDAction", global::Mono.Unix.Catalog.GetString ("changeUniqueID"), null, null);
		this.changeUniqueIDAction.ShortLabel = global::Mono.Unix.Catalog.GetString ("changeUniqueID");
		w1.Add (this.changeUniqueIDAction, null);
		this.UIManager.InsertActionGroup (w1, 0);
		this.AddAccelGroup (this.UIManager.AccelGroup);
		this.Name = "MainWindow";
		this.Title = global::Mono.Unix.Catalog.GetString ("cloudchatAdministratorTEST");
		this.Icon = global::Stetic.IconLoader.LoadIcon (this, "gtk-execute", global::Gtk.IconSize.Menu);
		this.WindowPosition = ((global::Gtk.WindowPosition)(4));
		// Container child MainWindow.Gtk.Container+ContainerChild
		this.vbox1 = new global::Gtk.VBox ();
		this.vbox1.Name = "vbox1";
		this.vbox1.Spacing = 6;
		// Container child vbox1.Gtk.Box+BoxChild
		this.UIManager.AddUiFromString ("<ui><menubar name='menubar'><menu name='AdminAction' action='AdminAction'><menu name='UniqueIdAction' action='UniqueIdAction'><menuitem name='changeUniqueIDAction' action='changeUniqueIDAction'/></menu></menu></menubar></ui>");
		this.menubar = ((global::Gtk.MenuBar)(this.UIManager.GetWidget ("/menubar")));
		this.menubar.Name = "menubar";
		this.vbox1.Add (this.menubar);
		global::Gtk.Box.BoxChild w2 = ((global::Gtk.Box.BoxChild)(this.vbox1 [this.menubar]));
		w2.Position = 0;
		w2.Expand = false;
		w2.Fill = false;
		// Container child vbox1.Gtk.Box+BoxChild
		this.hbox2 = new global::Gtk.HBox ();
		this.hbox2.Name = "hbox2";
		this.hbox2.Spacing = 6;
		// Container child hbox2.Gtk.Box+BoxChild
		this.UIManager.AddUiFromString ("<ui><toolbar name='toolbar2'><toolitem name='connectAction1' action='connectAction1'/></toolbar></ui>");
		this.toolbar2 = ((global::Gtk.Toolbar)(this.UIManager.GetWidget ("/toolbar2")));
		this.toolbar2.Name = "toolbar2";
		this.toolbar2.ShowArrow = false;
		this.hbox2.Add (this.toolbar2);
		global::Gtk.Box.BoxChild w3 = ((global::Gtk.Box.BoxChild)(this.hbox2 [this.toolbar2]));
		w3.Position = 0;
		this.vbox1.Add (this.hbox2);
		global::Gtk.Box.BoxChild w4 = ((global::Gtk.Box.BoxChild)(this.vbox1 [this.hbox2]));
		w4.Position = 1;
		w4.Expand = false;
		w4.Fill = false;
		// Container child vbox1.Gtk.Box+BoxChild
		this.hbox3 = new global::Gtk.HBox ();
		this.hbox3.Name = "hbox3";
		this.hbox3.Spacing = 6;
		this.vbox1.Add (this.hbox3);
		global::Gtk.Box.BoxChild w5 = ((global::Gtk.Box.BoxChild)(this.vbox1 [this.hbox3]));
		w5.Position = 2;
		// Container child vbox1.Gtk.Box+BoxChild
		this.notebook1 = new global::Gtk.Notebook ();
		this.notebook1.CanFocus = true;
		this.notebook1.Name = "notebook1";
		this.notebook1.CurrentPage = 0;
		// Container child notebook1.Gtk.Notebook+NotebookChild
		this.GtkScrolledWindow = new global::Gtk.ScrolledWindow ();
		this.GtkScrolledWindow.Name = "GtkScrolledWindow";
		this.GtkScrolledWindow.ShadowType = ((global::Gtk.ShadowType)(1));
		// Container child GtkScrolledWindow.Gtk.Container+ContainerChild
		this.nodeviewManagers = new global::Gtk.NodeView ();
		this.nodeviewManagers.CanFocus = true;
		this.nodeviewManagers.Name = "nodeviewManagers";
		this.GtkScrolledWindow.Add (this.nodeviewManagers);
		this.notebook1.Add (this.GtkScrolledWindow);
		// Notebook tab
		this.Managers = new global::Gtk.Label ();
		this.Managers.Name = "Managers";
		this.Managers.LabelProp = global::Mono.Unix.Catalog.GetString ("Managers");
		this.notebook1.SetTabLabel (this.GtkScrolledWindow, this.Managers);
		this.Managers.ShowAll ();
		// Container child notebook1.Gtk.Notebook+NotebookChild
		this.GtkScrolledWindow1 = new global::Gtk.ScrolledWindow ();
		this.GtkScrolledWindow1.Name = "GtkScrolledWindow1";
		this.GtkScrolledWindow1.ShadowType = ((global::Gtk.ShadowType)(1));
		// Container child GtkScrolledWindow1.Gtk.Container+ContainerChild
		this.nodeviewCustomers = new global::Gtk.NodeView ();
		this.nodeviewCustomers.CanFocus = true;
		this.nodeviewCustomers.Name = "nodeviewCustomers";
		this.GtkScrolledWindow1.Add (this.nodeviewCustomers);
		this.notebook1.Add (this.GtkScrolledWindow1);
		global::Gtk.Notebook.NotebookChild w9 = ((global::Gtk.Notebook.NotebookChild)(this.notebook1 [this.GtkScrolledWindow1]));
		w9.Position = 1;
		// Notebook tab
		this.Customers = new global::Gtk.Label ();
		this.Customers.Name = "Customers";
		this.Customers.LabelProp = global::Mono.Unix.Catalog.GetString ("Customers");
		this.notebook1.SetTabLabel (this.GtkScrolledWindow1, this.Customers);
		this.Customers.ShowAll ();
		this.vbox1.Add (this.notebook1);
		global::Gtk.Box.BoxChild w10 = ((global::Gtk.Box.BoxChild)(this.vbox1 [this.notebook1]));
		w10.Position = 3;
		// Container child vbox1.Gtk.Box+BoxChild
		this.vbox2 = new global::Gtk.VBox ();
		this.vbox2.Name = "vbox2";
		this.vbox2.Spacing = 6;
		// Container child vbox2.Gtk.Box+BoxChild
		this.statusbar6 = new global::Gtk.Statusbar ();
		this.statusbar6.Name = "statusbar6";
		this.statusbar6.Spacing = 6;
		this.vbox2.Add (this.statusbar6);
		global::Gtk.Box.BoxChild w11 = ((global::Gtk.Box.BoxChild)(this.vbox2 [this.statusbar6]));
		w11.Position = 0;
		w11.Expand = false;
		w11.Fill = false;
		this.vbox1.Add (this.vbox2);
		global::Gtk.Box.BoxChild w12 = ((global::Gtk.Box.BoxChild)(this.vbox1 [this.vbox2]));
		w12.Position = 4;
		w12.Expand = false;
		w12.Fill = false;
		this.Add (this.vbox1);
		if ((this.Child != null)) {
			this.Child.ShowAll ();
		}
		this.DefaultWidth = 888;
		this.DefaultHeight = 473;
		this.Show ();
		this.DeleteEvent += new global::Gtk.DeleteEventHandler (this.OnDeleteEvent);
		this.connectAction1.Activated += new global::System.EventHandler (this.OnConnectActionActivated);
		this.helpAction.Activated += new global::System.EventHandler (this.OnHelpActionActivated);
		this.connectAction.Activated += new global::System.EventHandler (this.OnConnectActionActivated);
		this.changeUniqueIDAction.Activated += new global::System.EventHandler (this.OnChangeUniqueIDActionActivated);
		this.menubar.ActivateCurrent += new global::Gtk.ActivateCurrentHandler (this.OnMenubarActivateCurrent);
	}
}
