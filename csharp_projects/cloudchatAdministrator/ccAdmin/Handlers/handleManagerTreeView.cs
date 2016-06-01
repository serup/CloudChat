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
			setTitleOnColumn(0, "First column");
			// Add some data to the model treeview store
			addDataToTreeView("-", "-");
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
				clm.Title = "Column " + i.ToString();
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

		public void addDataToTreeView(params string[] str)
		{
			// Add some data to the store
			mListStore.AppendValues (str);
		}
	}
}

