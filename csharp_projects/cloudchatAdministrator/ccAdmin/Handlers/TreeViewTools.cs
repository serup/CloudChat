using System;
using Gtk;
using System.Numerics;

namespace csharpServices
{
	public class TreeViewTools
	{
		bool bUpdated = false;
		public Gtk.TreeIter currPosInTree; 
		public Gtk.TreeViewColumn[] Columns = null;

		public void updateDataOfListStoreInTreeView(ListStore mListStore, int columnSearchKey=1, params string[] columns )
		{
			try {

				mListStore.GetIterFirst(out currPosInTree);
				object rowIndexObj = mListStore.GetValue(currPosInTree, columnSearchKey);
				while(rowIndexObj!=null)
				{
					if(rowIndexObj.ToString()==columns[columnSearchKey].ToString()) {
						mListStore.SetValues(currPosInTree,columns); // update row
						bUpdated=true;
						break;
					}
					if(mListStore.IterNext(ref currPosInTree)) {
						rowIndexObj = mListStore.GetValue(currPosInTree, columnSearchKey);
					}
					else 
						rowIndexObj = null;
				}

				if(!bUpdated)  
					mListStore.AppendValues (columns); // Add some data to the store
			}
			catch (Exception e)
			{
				Console.WriteLine("WARNING: adding to treeview caused exception");
				Console.WriteLine(e.Message.ToString());
			}
		}

		public void createModel(ref ListStore mListStore, NodeView nodeView, int AmountOfColumnsToHandle)
		{
			Type[] types = new Type[AmountOfColumnsToHandle];
			for(int i=0;i<AmountOfColumnsToHandle;i++) { types[i] = typeof(string); }
			mListStore = new Gtk.ListStore(types);
			nodeView.NodeSelection.NodeView.Model = mListStore;
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

		public void createColumnsAndCells(NodeView nodeView, int Amount)
		{
			Columns = new Gtk.TreeViewColumn[Amount];
			for(int n=0;n<Amount;n++)
				Columns[n] = new Gtk.TreeViewColumn();

			int i=0;
			foreach(Gtk.TreeViewColumn clm in Columns) {
				clm.Title = "";
				// Add the columns to the TreeView
				nodeView.NodeSelection.NodeView.AppendColumn(clm);
				// Create the text cell 
				Gtk.CellRendererText Cell = new Gtk.CellRendererText();
				clm.PackStart(Cell, true);
				// Tell the Cell Renderers which items in the model to display
				clm.AddAttribute(Cell, "text", i);
				i++;
			}
		}

	}
}


