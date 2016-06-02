using System;
using Gtk;

namespace csharpServices
{
	public class handleCustomerTreeView
	{
		private NodeView nodeviewCustomers;
		private Gtk.ListStore mListStore;
		private Gtk.TreeViewColumn[] Columns = null;

		public handleCustomerTreeView(NodeView nodeviewCustomers)
		{
			// Create TreeView with cells and attached model for control
			initNodeView(nodeviewCustomers);

			// Set titles on columns
			setTitlesOnColumns("Status", "Name");
			// Add some data to the model treeview store
			//addDataToTreeView("Idle", "John doe");
		}

		public void initNodeView(NodeView nodeviewCustomers)
		{
			this.nodeviewCustomers = nodeviewCustomers;
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
				clm.Title = "";
				// Add the columns to the TreeView
				nodeviewCustomers.NodeSelection.NodeView.AppendColumn(clm);
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
			mListStore = new Gtk.ListStore(typeof(string), typeof(string));
			// Assign the model to the TreeView
			nodeviewCustomers.NodeSelection.NodeView.Model = mListStore;
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
				if(type == typeof(ChatInfoObj)) {
					element[] elements = dana.getElementNamesAndValues();
					addDataToTreeView("idle", elements[4].value);
				}
				else {
					//throw new NotSupportedException("ERROR: incomming type of data object is currently NOT supported");
				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e.ToString());
			}
		}

		public void addDataToTreeView(params string[] str)
		{
			// Add some data to the store
			mListStore.AppendValues (str);
		}
	}
}


