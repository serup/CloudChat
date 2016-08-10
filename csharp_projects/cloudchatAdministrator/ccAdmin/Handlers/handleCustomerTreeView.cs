using System;
using Gtk;
using System.Numerics;

namespace csharpServices
{
	public class handleCustomerTreeView : TreeViewTools
	{
		private NodeView nodeviewCustomers;
		private Gtk.ListStore customerListStore;

		public NodeView getNodeView()
		{
			return nodeviewCustomers;
		}

		public ListStore getListStore()
		{
			return customerListStore;
		}

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
			createColumnsAndCells(nodeviewCustomers, AmountOfColumns);
			createModel(ref customerListStore, nodeviewCustomers, AmountOfColumns);
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


