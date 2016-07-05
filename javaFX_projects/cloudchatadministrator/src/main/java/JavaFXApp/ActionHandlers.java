package JavaFXApp;

import ProtocolHandlings.DOPsCommunication;
import dops.utils.utils;
import javafx.beans.InvalidationListener;

import java.io.IOException;

class ActionHandlers {
	private PresentationState ps=null;

	ActionHandlers(){ }
	ActionHandlers(PresentationState ps){ this.ps = ps; }

	static InvalidationListener connectHandler(PresentationState ps) throws IOException {

		return observable -> {

			if(!ps.bConnected) {
				utils util = new utils();
				ps.dopsCommunications = new DOPsCommunication();
				ps.dopsCommunications.addActionHandler("ChatInfo", ps.actionHandlers::actionHandlerUpdateCustomerViewAndForwardToManagers);
				//dopsCommunications.addActionHandler("ChatForwardInfoRequest", ps.actionHandlers::actionHandlerUpdateManagerView);

				if(util.isEnvironmentOK()) {
					String uniqueId = ps.loader.getResources().getString("uniqueId");
					String username = ps.loader.getResources().getString("username");
					String password = ps.loader.getResources().getString("password");
					if(ps.dopsCommunications.connectToDOPs(uniqueId, username, password)) {
						ps.bConnected = true;
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

        ps.controller.addCellRowElementsToCustomerView(createTableRow(dana));
		ps.controller.removeOutdatedRowsElementsFromCustomerView();

		//TODO: find all managers and forward "ChatInfo" to these online managers

		return strResult;
	}

	private CustomerTableEntry createTableRow(DOPsCommunication.dedAnalyzed dana)
	{
        CustomerTableEntry newCellRowInTable = ps.controller.createCellRowForCustomerTableView();
        newCellRowInTable.userName.set(dana.getElement("srcAlias").toString());
        newCellRowInTable.srcHomepageAlias.set(dana.getElement("srcHomepageAlias").toString());
		return newCellRowInTable;
	}
}
