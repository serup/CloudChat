using System;
using Gtk;

namespace csharpServices
{
	public class handleManagerTreeView
	{
		private NodeView nodeviewManagers;
		private Gtk.ListStore mListStore;
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
			mListStore = new Gtk.ListStore(typeof(string), typeof(string));
			// Assign the model to the TreeView
			nodeviewManagers.NodeSelection.NodeView.Model = mListStore;
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
					//element[] elements = dana.getElementNamesAndValues();
					//updateDataOfListStoreInTreeView("idle", elements[4].value);
//					updateDataOfListStoreInTreeView("idle", dana.getElement("srcAlias").value);
					updateDataOfListStoreInTreeView("idle", dana.getElement("src").value);

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

		public void updateDataOfListStoreInTreeView(params string[] str)
		{
			try {
				bool bUpdated = false;
				TreeIter tmpTreeIter; 

				mListStore.GetIterFirst(out tmpTreeIter);
				object o = mListStore.GetValue(tmpTreeIter, 1);
				while(o!=null)
				{
					if(o.ToString()==str[1].ToString()) {
						mListStore.SetValues(tmpTreeIter,str); // update row
						bUpdated=true;
						break;
					}
					if(mListStore.IterNext(ref tmpTreeIter)) {
						o = mListStore.GetValue(tmpTreeIter, 1);
					}
					else 
						o = null;
				}

				if(!bUpdated)  
					mListStore.AppendValues (str); // Add some data to the store
			}
			catch (Exception e)
			{
				Console.WriteLine("WARNING: adding to treeview caused exception");
				Console.WriteLine(e.ToString());
			}
		}
	}
}

