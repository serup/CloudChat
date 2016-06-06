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

	public class dopsAPI : Object 
	{
		private DOPSHandler dops = null;

		public void setAPIhandler(DOPSHandler dopsHandler)
		{
			this.dops = dopsHandler;
		}

		public void sendToDOPsServer(byte[] blob)
		{
			if(dops != null)
			{
				dops.sendToDOPsServer(blob);
			} 
			else
				throw new Exception("WARNING [updateOnlineManagersWithIncomingChatInfo]: dops API not initiated, thus no communication will work");
		}
	}

	public class AARHandler
	{
		private List<Manager> managersList = new List<Manager>();
		private List<Customer> customersList = new List<Customer>();
		private int maxIdleTimeInMillisecondsInList = 10000;
		private dopsAPI dops = new dopsAPI();
		private double warpFactorInMilliseconds = 0; // default no time warp -- only used for testing 
		private cloudChatPacketHandler ccph = new cloudChatPacketHandler();

		public AARHandler()
		{
		}

		public AARHandler(DOPSHandler dopsHandler)
		{
			dops.setAPIhandler(dopsHandler);
		}

		public void setMaxIdleTimeInList(int IntervalInMilliseconds = 10000)
		{
			maxIdleTimeInMillisecondsInList = IntervalInMilliseconds;
		}

		public int getMaxIdleMillisecondTimeInList() {
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
			try {
				updateManagerList(dana);
				updateCustomerList(dana);
				removeOfflineManagers();
				updateOnlineManagersWithIncomingChatInfo(dana);
			} 
			catch (Exception e) {
				Console.WriteLine(e.Message.ToString());
			}
		}

		private void updateManagerList(dedAnalyzed dana)
		{
			if(dana.elements.GetType() == typeof(ForwardInfoRequestObj)) {
				bool bUpdated=false;
				foreach(Manager manager in managersList) {
					if(manager.src == dana.getElement("src").value) {
						manager.lastTimeStamp = this.getDateTimeNow();
						manager.srcAlias  = dana.getElement("srcAlias").value;
						bUpdated=true;
					}
				}
				if(!bUpdated) 
					appendManagerToList(dana);

			}
		}

		public void InitiateWarpTime(double warpFactorInMilliseconds)
		{
			this.warpFactorInMilliseconds = warpFactorInMilliseconds;
		}

		public void resetWarpTime()
		{
			this.warpFactorInMilliseconds = 0;
		}

		private DateTime getDateTimeNow()
		{
			DateTime dt = DateTime.Now;
			if(this.warpFactorInMilliseconds != 0) {
				DateTime newDt = dt.AddMilliseconds(this.warpFactorInMilliseconds);
				return newDt;
			}
			else
				return dt;
		}

		private void removeOfflineManagers()
		{
			for (int i = managersList.Count - 1; i >= 0; i--)
			{
				if((this.getDateTimeNow() - managersList[i].lastTimeStamp).TotalMilliseconds > maxIdleTimeInMillisecondsInList)
					managersList.RemoveAt(i);
			}
		}

		private void appendManagerToList(dedAnalyzed dana)
		{
			Manager manager 		= new Manager();
			manager.lastTimeStamp 	= this.getDateTimeNow();
			manager.srcAlias 		= dana.getElement("srcAlias").value;
			manager.src	     		= dana.getElement("src").value;
			managersList.Add(manager);
		}

		private void updateCustomerList(dedAnalyzed dana)
		{
		}

		private void updateOnlineManagersWithIncomingChatInfo(dedAnalyzed dana)
		{
			if(dana.elements.GetType() == typeof(ChatInfoObj)) {
				foreach(Manager manager in managersList) {
					((ChatInfoObj)dana.elements).dest = manager.src; // Set dest to managers src
					dops.sendToDOPsServer(ccph.createDEDpackage(dana));
				}
			} 
//			else {
//				throw new Exception("WARNING [updateOnlineManagersWithIncomingChatInfo]: Unknown ded, thus it was NOT forwarded");
//			}
		}

		public int getManagerListCount()
		{
			return managersList.Count;
		}
	}
}

