package JavaFXApp;

import ProtocolHandlings.DOPsCommunication;
import dops.utils.utils;
import javafx.beans.InvalidationListener;

import java.io.IOException;

public class ActionHandlers {
	private PresentationState ps=null;

	ActionHandlers(){ }
	ActionHandlers(PresentationState ps){ this.ps = ps; }

	public static InvalidationListener connectHandler(PresentationState ps) throws IOException {

		return observable -> {

			if(!ps.bConnected) {
				utils util = new utils();
				ps.dopsCommunications = new DOPsCommunication();
				ps.dopsCommunications.addActionHandler("ChatInfo", ps.actionHandlers::actionHandlerUpdateCustomerListView);
				//dopsCommunications.addActionHandler("ChatForwardInfoRequest", this::actionHandlerUpdateManagerListBox);

				ps.controller.connectButton.setText(ps.loader.getResources().getString("wait"));  //TODO: find a way to animate connect button in waiting - this does NOT work
				//buttonConnect.setIcon(buttonConnect.getPressedIcon());

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

	// Action handlers for listviews
	private String actionHandlerUpdateManagerListView(String type, DOPsCommunication.dedAnalyzed dana) {
		String strResult = "OK";
		System.out.println("- updateMangerListBox called ");
		String srcAlias = dana.getElement("srcAlias").toString();
		//handleManagerListBox.addElementToListBox(srcAlias);
		return strResult;
	}

	private String actionHandlerUpdateCustomerListView(String type, DOPsCommunication.dedAnalyzed dana)
	{
		String strResult = "OK";
		System.out.println("- actionHandlerUpdateCustomerListView called ");
		ccAdminDialogController.CellElementsInCustomerListView newCellRow = ps.controller.createNewCellRowForCustomerListView();
        newCellRow.srcAlias = dana.getElement("srcAlias").toString();
        newCellRow.srcHomepageAlias = dana.getElement("srcHomepageAlias").toString();

        // TODO: add cell elements

		ps.controller.addCellRowElementsToCustomerListView(newCellRow);
		return strResult;
	}

}
