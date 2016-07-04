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
				ps.dopsCommunications.addActionHandler("ChatInfo", ps.actionHandlers::actionHandlerUpdateCustomerView);
				//dopsCommunications.addActionHandler("ChatForwardInfoRequest", this::actionHandlerUpdateManagerView);

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

	private String actionHandlerUpdateCustomerView(String type, DOPsCommunication.dedAnalyzed dana)
	{
		String strResult = "OK";
		System.out.println("- actionHandlerUpdateCustomerView called ");

		CustomerTableEntry newCellRowInTable = createTableRow(dana);
        ps.controller.addCellRowElementsToCustomerView(newCellRowInTable);

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
