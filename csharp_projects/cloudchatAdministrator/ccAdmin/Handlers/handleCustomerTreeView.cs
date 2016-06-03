using System;
using Gtk;
using System.Numerics;

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
			setTitlesOnColumns("Status", "Name", "visit on Homepage");
			// Add some data to the model treeview store
			//addDataToTreeView("Idle", "John doe");
		}

		public void initNodeView(NodeView nodeviewCustomers)
		{
			this.nodeviewCustomers = nodeviewCustomers;
			int AmountOfColumns = 3;
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
			mListStore = new Gtk.ListStore(types);
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
					updateDataOfListStoreInTreeView("idle", dana.getElement("srcAlias").value, dana.getElement("srcHomepageAlias").value);
				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e.ToString());
			}
		}

		public void updateDataOfListStoreInTreeView(params string[] str)
		{
			bool bUpdated = false;
			TreeIter tmpTreeIter; 
			for(System.Numerics.BigInteger i=0; mListStore.GetIterFromString(out tmpTreeIter, i.ToString())==true;i++) 
			{ 
				mListStore.SetValues(tmpTreeIter,str); 
				bUpdated = true;
			}

			if(!bUpdated) 
				mListStore.AppendValues (str);
		}
	}
}


