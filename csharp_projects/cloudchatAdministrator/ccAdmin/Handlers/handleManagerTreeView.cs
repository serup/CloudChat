using System;
using Gtk;

namespace csharpServices
{
	public class handleManagerTreeView : TreeViewTools
	{
		private NodeView nodeviewManagers;
		private Gtk.ListStore managerListStore;
		private Gtk.TreeViewColumn[] Columns = null;

		public handleManagerTreeView(NodeView nodeviewManagers)
		{
			// Create TreeView with cells and attached model for control
			initNodeView(nodeviewManagers);

			// Set titles on columns
			setTitlesOnColumns("Status", "Name");
			// Add some data to the model treeview store
			//addDataToTreeView("Idle", "John doe");
		}

		public void initNodeView(NodeView nodeviewManagers)
		{
			this.nodeviewManagers = nodeviewManagers;
			createColumnsAndCells(2);
			createModel();
		}

		public void createColumnsAndCells(int Amount)
		{
			Columns = new Gtk.TreeViewColumn[Amount];
			for(int n=0;n<Amount;n++)
				Columns[n] = new Gtk.TreeViewColumn();

			int i=0;
			foreach(Gtk.TreeViewColumn clm in Columns) {
//				clm.Title = "Column " + (i +1).ToString();
				clm.Title = "";
				// Add the columns to the TreeView
				nodeviewManagers.NodeSelection.NodeView.AppendColumn(clm);
				// Create the text cell 
				Gtk.CellRendererText Cell = new Gtk.CellRendererText();
				clm.PackStart(Cell, true);
				// Tell the Cell Renderers which items in the model to display
				clm.AddAttribute(Cell, "text", i);
				i++;
			}
		}

		public void createModel()
		{
			// Create a model that will hold two strings -  Name and Song Title
			managerListStore = new Gtk.ListStore(typeof(string), typeof(string));
			// Assign the model to the TreeView
			nodeviewManagers.NodeSelection.NodeView.Model = managerListStore;
		}

		public void setTitleOnColumn(int index, string title)
		{
			Columns[index].Title = title;
		}

		public void setTitlesOnColumns(params string[] titles)
		{
			int i = 0;
			foreach(string title in titles) {
				setTitleOnColumn(i,title);
				i++;
			}
		}

		public void updateWithIncomingData(dedAnalyzed dana)
		{
			try {
				System.Type type = dana.elements.GetType();
				if(type == typeof(ForwardInfoRequestObj)) {
					// Assign the model to the TreeView
					updateDataOfListStoreInTreeView(managerListStore, 1, "idle", dana.getElement("src").value);
				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message.ToString());
			}

		}
	}
}

