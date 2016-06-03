using System;

/**
 * Automated Alternate Routing - AAR
 * 
 * this class will take care of automatically route incoming "calls" from customers to managers
 * 
 */
using System.Collections.Generic;
using System.Linq;


namespace csharpServices
{
	public class Manager : Object
	{
		public short  transactionsID;
		public string protocolTypeID;
		public string dest;
		public string src;
		public string srcAlias;
		public DateTime lastTimeStamp;
	}

	public class Customer : Object
	{
		public short  transactionsID;
		public string protocolTypeID;
		public string dest;
		public string src;
		public string srcAlias;
		public string srcHomepageAlias;
		public string lastEntryTime;
		public DateTime lastping;
		public string currentHandlerAlias; // a manager id (srcAlias), if any
	}

	public class AARHandler
	{
		private List<Manager> managersList = new List<Manager>();
		private List<Customer> customersList = new List<Customer>();
		private int maxIdleTimeInList = 30;

		public AARHandler()
		{
		}

		public void handleRouting(dedAnalyzed dana)
		{
			updateManagerList(dana);
			updateCustomerList(dana);
			updateOnlineManagersWithIncomingChatInfo(dana);
			removeOfflineManagers();
		}

		private void updateManagerList(dedAnalyzed dana)
		{
			try {
				if(dana.elements.GetType() == typeof(ChatInfoObj)) {
					bool bUpdated=false;
					foreach(Manager manager in managersList) {
						if(manager.srcAlias == dana.getElement("srcAlias").value) {
							manager.lastTimeStamp = DateTime.Now;
							manager.src  = dana.getElement("src").value;
							bUpdated=true;
						}
						else {
							// NOT ALLOWED
							//if((DateTime.Now - manager.lastTimeStamp).TotalSeconds > maxIdleTimeInList)
							//	managersList.Remove(manager);
						}
					}
					if(!bUpdated) 
						appendManagerToList(dana);

				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e.ToString());
			}

		}

		private void removeOfflineManagers()
		{
			try {

				//var list = new List<int>(Enumerable.Range(1, 10));
				for (int i = managersList.Count - 1; i >= 0; i--)
				{
					if((DateTime.Now - managersList[i].lastTimeStamp).TotalSeconds > maxIdleTimeInList)
					//if (managersList[i] > 5)
						managersList.RemoveAt(i);
				}
				//managersList.ForEach(i => Console.WriteLine(i));

			}
			catch (Exception e)
			{
				Console.WriteLine(e.ToString());
			}
		}

		private void appendManagerToList(dedAnalyzed dana)
		{
			Manager manager = new Manager();
			manager.lastTimeStamp = DateTime.Now;
			manager.srcAlias = dana.getElement("srcAlias").value;
			manager.src	     = dana.getElement("src").value;
			managersList.Add(manager);
		}

		private void updateCustomerList(dedAnalyzed dana)
		{
		}

		private void updateOnlineManagersWithIncomingChatInfo(dedAnalyzed dana)
		{

		}

		public int getManagerListCount()
		{
			return managersList.Count;
		}
	}
}

