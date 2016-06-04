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
		private int maxIdleTimeInMillisecondsInList = 10000;
		private DOPSHandler dops = null;

		public AARHandler()
		{
		}

		public AARHandler(DOPSHandler dopsHandler)
		{
			dops = dopsHandler;
		}

		public void setMaxIdleTimeInList(int IntervalInMilliseconds = 10000)
		{
			maxIdleTimeInMillisecondsInList = IntervalInMilliseconds;
		}

		public int getMaxIdleTimeInList() {
			return maxIdleTimeInMillisecondsInList;
		}

		/**
		 * Main method handling Automatic Alternate Routing
		 * - if a customer does NOT have a handler (manager) assigned, then ALL managers 
		 *   will receive its chatInfo ping
		 * 
		 */ 
		public void handleRouting(dedAnalyzed dana)
		{
			updateManagerList(dana);
			updateCustomerList(dana);
			removeOfflineManagers();
			updateOnlineManagersWithIncomingChatInfo(dana);
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
				for (int i = managersList.Count - 1; i >= 0; i--)
				{
					if((DateTime.Now - managersList[i].lastTimeStamp).TotalMilliseconds > maxIdleTimeInMillisecondsInList)
						managersList.RemoveAt(i);
				}
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
			if(dops != null) {
				byte[] blob = null; //TODO: create DED 
				dops.sendToDOPsServer (blob);
			}

		}

		public int getManagerListCount()
		{
			return managersList.Count;
		}
	}
}

