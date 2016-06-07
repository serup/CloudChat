using System;
using Gtk;

namespace csharpServices
{
	public class handleManagerTreeView : TreeViewTools
	{
		private NodeView nodeviewManagers;
		private ListStore managerListStore;

		public NodeView getNodeView()
		{
			return nodeviewManagers;
		}

		public ListStore getListStore()
		{
			return managerListStore;
		}

		public handleManagerTreeView(NodeView nodeviewManagers, params string[] titles)
		{
			// Create TreeView with cells and attached model for control
			initNodeView(nodeviewManagers, titles.GetLength(0));
			// Set titles on columns
			setTitlesOnColumns(titles);
		}

		public void initNodeView(NodeView nodeviewManagers, int AmountOfColumnsToHandle)
		{
			if(nodeviewManagers == null) throw new Exception("ERROR [handleManagerTreeView]: nodeview object is null");
			this.nodeviewManagers = nodeviewManagers;
			createColumnsAndCells(nodeviewManagers, AmountOfColumnsToHandle);
			createModel(ref managerListStore,nodeviewManagers,AmountOfColumnsToHandle);
		}



		public void updateWithIncomingData(dedAnalyzed dana)
		{
			try {
				System.Type type = dana.elements.GetType();
				if(type == typeof(ForwardInfoRequestObj)) {
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

