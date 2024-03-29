package JavaFXApp;

import ProtocolHandlings.DOPsCommunication;
import dops.utils.utils;
import javafx.application.Platform;
import javafx.beans.InvalidationListener;

import java.io.IOException;
import java.util.List;

class ActionHandlers {
	private PresentationState ps=null;

	ActionHandlers(){ }
	ActionHandlers(PresentationState ps){ this.ps = ps; }

	static InvalidationListener connectHandler(PresentationState ps) throws IOException {

		return observable -> {

			if(!ps.bConnected) {
				utils util = new utils();
				if(addActionHandlers(ps)) {
					if (util.isEnvironmentOK()) {
						String uniqueId = ps.loader.getResources().getString("uniqueId");
						String username = ps.loader.getResources().getString("username");
						String password = ps.loader.getResources().getString("password");
						if (ps.dopsCommunications.connectToDOPs(uniqueId, username, password)) {
							ps.bConnected = true;
						}
					}
				}
			}
			else
			{
				try {
					if(ps.dopsCommunications.disconnectFromDOPs()) {
                        ps.bConnected = false;
                    }
				} catch (IOException e) {
					e.printStackTrace();
				}
			}

			if(ps.bConnected) {
				ps.controller.connectButton.setText(ps.loader.getResources().getString("connected"));
				System.out.println("- Connection with DOPs established");
			}
			else {
				ps.controller.connectButton.setText(ps.loader.getResources().getString("disconnected"));
				System.out.println("- Disconnected from DOPs");
			}
		};
	}

	private static boolean addActionHandlers(PresentationState ps)
	{
		boolean bResult=true;
		try {
			ps.dopsCommunications = new DOPsCommunication();
			ps.dopsCommunications.addActionHandler("ChatInfo", ps.actionHandlers::actionHandlerUpdateCustomerViewAndForwardToManagers);
			ps.dopsCommunications.addActionHandler("ChatForwardInfoRequest", ps.actionHandlers::actionHandlerUpdateManagerViewAndForwardToOnlineMangers);
			//... add more action handlers here!
		}catch (Exception e)
		{
			System.out.printf("- DOPsCommunication object could NOT be created, thus NO communication; detailed cause : \n %s \n", e.getMessage());
			bResult=false;
		}
		return bResult;
	}

	/**
	 * action handler for customer view table
	 * this action handler will be called every time a DED packet of type "ChatInfo" is received
	 * It will update the customer table view with the new incoming information
	 *
	 * The incoming "ChatInfo" is an automated transfer of a DED from a JavaScript homepage client
	 * example:
	 * http://cloudchatclient.com/CloudChatClient/ChatWidget.html?ypos=200&managerid=754d148d0522659d0ceb2e6035fad6a8&category=Montenegro
	 *
	 * above link starts a client and it will, when connected send a DED packet of type "ChatInfo" towards managerid, so
	 * if this instance is connected with that id, then it will receive the DED and its analyzed version will be
	 * transfered to this aciton handler
	 *
	 * @param type  - "ChatInfo"
	 * @param dana  - Analyzed DED - decompressed and analyzed
     * @return - String "OK"; not really used
     */
	private String actionHandlerUpdateCustomerViewAndForwardToManagers(String type, DOPsCommunication.dedAnalyzed dana)
	{
		String strResult = "OK";
		System.out.println("- actionHandlerUpdateCustomerViewAndForwardToManagers called ");
		Platform.runLater(() -> {
			try {

				addToCustomerView(dana);
				removeIdleElementsFromCustomerView();
				forwardChatInfoToOnlineManagers(dana);

			} catch (Exception e) {
				e.printStackTrace();
			}
		});
		return strResult;
	}

	private void addToCustomerView(DOPsCommunication.dedAnalyzed dana)
	{
		ps.controller.handleCustomerTableView.addCellRowElementsToCustomerView(createCustomersTableRow(dana));
	}

	private void removeIdleElementsFromCustomerView()
	{
		ps.controller.handleCustomerTableView.removeIdleCellRowElementsInCustomerView();
	}

	private void forwardChatInfoToOnlineManagers(DOPsCommunication.dedAnalyzed dana) throws Exception
	{
		List<Object> objectList = ps.controller.handleManagersTableView.updateOnlineManagersWithIncomingChatInfo(dana);
		objectList.stream().forEach(d -> System.out.printf("- transfer DED analyzed [ChatInfo] to online manager : %s\n",  ((DOPsCommunication.ChatInfoObj)((DOPsCommunication.dedAnalyzed)d).getElements()).dest));
		objectList.stream().forEach(d -> ps.dopsCommunications.sendToServer(ps.dopsCommunications.createDEDpackage((DOPsCommunication.dedAnalyzed)d)));
	}

	private CustomerTableEntry createCustomersTableRow(DOPsCommunication.dedAnalyzed dana)
	{
        CustomerTableEntry newCellRowInTable = ps.controller.handleCustomerTableView.createCellRowForCustomerTableView();
        newCellRowInTable.userName.set(dana.getElement("srcAlias").toString());
        newCellRowInTable.srcHomepageAlias.set(dana.getElement("srcHomepageAlias").toString());
		return newCellRowInTable;
	}

	/**
	 * action handler for Manager view table
	 * this action handler will be called every time a DED packet of type "ChatForwardInfoRequest" is received
	 * It will update the manager table view with the new incoming information
	 *
	 * The incoming "ChatForwardInfoRequest" is an automated transfer of a DED from a JavaScript homepage
	 * example:
	 * http://cloudchatmanager.com/CloudChatManager/CloudChatManager.htm
	 *
	 * @param type - "ChatForwardInfoRequest"
	 * @param dana - Analyzed DED - decompressed and analyzed
     * @return - String "OK" or String "WrongType"
     */
	private String actionHandlerUpdateManagerViewAndForwardToOnlineMangers(String type, DOPsCommunication.dedAnalyzed dana)
	{
		String strResult = "OK";
		System.out.println("- actionHandlerUpdateManagerViewAndForwardToOnlineMangers called ");
		if(type.contains("ChatForwardInfoRequest")) {
			Platform.runLater(() -> {
				try {
					addToManagerView(dana);
					removeIdleElementsFromManagerView();
					forwardMangerInfoToOnlineManagers(dana);
				} catch (Exception e) {
					e.printStackTrace();
				}
			});
		}
		else {
			System.out.printf("- ERROR : actionHandlerUpdateManagerViewAndForwardToOnlineMangers retrieved object of wrong type: %s\n ", type);
			strResult="WrongType";
		}

		return strResult;
	}

	private void addToManagerView(DOPsCommunication.dedAnalyzed dana)
	{
		ps.controller.handleManagersTableView.addCellRowElementsToManagersView(createManagersTableRow(dana));
	}

	private void removeIdleElementsFromManagerView()
	{
		ps.controller.handleManagersTableView.removeIdleCellRowElementsInManagersView();
	}

	private void forwardMangerInfoToOnlineManagers(DOPsCommunication.dedAnalyzed dana) throws Exception
	{
		List<Object> objectList = ps.controller.handleManagersTableView.updateOnlineManagersWithOnlineManagersInfo(dana);
		objectList.stream().forEach(d -> System.out.printf("- transfer DED analyzed [Manager Info] to all online managers : %s\n",  ((DOPsCommunication.ForwardInfoRequestObj)((DOPsCommunication.dedAnalyzed)d).getElements()).dest));
		objectList.stream().forEach(d -> ps.dopsCommunications.sendToServer(ps.dopsCommunications.createDEDpackage((DOPsCommunication.dedAnalyzed)d)));
	}

	private ManagerTableEntry createManagersTableRow(DOPsCommunication.dedAnalyzed dana)
	{
		ManagerTableEntry newCellRowInTable = ps.controller.handleManagersTableView.createCellRowForManagersTableView();
		newCellRowInTable.status.set(dana.getElement("status").toString()); // TODO: add this element to DED
		newCellRowInTable.userName.set(dana.getElement("srcAlias").toString());
		newCellRowInTable.userId.set(dana.getElement("src").toString()); // Important unique id of sender
		return newCellRowInTable;
	}

}
