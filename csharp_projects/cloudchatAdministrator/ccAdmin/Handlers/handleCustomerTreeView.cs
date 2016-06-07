using System;
using Gtk;
using System.Numerics;

namespace csharpServices
{
	public class handleCustomerTreeView : TreeViewTools
	{
		private NodeView nodeviewCustomers;
		private Gtk.ListStore customerListStore;
		private Gtk.TreeViewColumn[] Columns = null;

		public handleCustomerTreeView(NodeView nodeviewCustomers, params string[] titles)
		{
			// Create TreeView with cells and attached model for control
			initNodeView(nodeviewCustomers, titles.GetLength(0));
			// Set titles on columns
			setTitlesOnColumns(titles);
		}

		public void initNodeView(NodeView nodeviewCustomers, int AmountOfColumns)
		{
			if(nodeviewCustomers == null) throw new Exception("ERROR [handleCustomerTreeView]: nodeview object is null");
			this.nodeviewCustomers = nodeviewCustomers;
			createColumnsAndCells(AmountOfColumns);
			createModel(AmountOfColumns);
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

		public void createModel(int AmountOfColumnsToHandle)
		{
			Type[] types = new Type[AmountOfColumnsToHandle];
			for(int i=0;i<AmountOfColumnsToHandle;i++) { types[i] = typeof(string); }
			customerListStore = new Gtk.ListStore(types);
			nodeviewCustomers.NodeSelection.NodeView.Model = customerListStore;
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
					updateDataOfListStoreInTreeView(customerListStore, 1, "idle", dana.getElement("srcAlias").value, dana.getElement("srcHomepageAlias").value);
				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e.ToString());
			}
		}



	}
}


