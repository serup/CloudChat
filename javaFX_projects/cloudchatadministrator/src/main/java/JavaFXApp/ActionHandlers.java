package JavaFXApp;

import ProtocolHandlings.DOPsCommunication;
import dops.utils.utils;
import javafx.beans.InvalidationListener;

import java.io.IOException;

public class ActionHandlers {


	public static InvalidationListener connectHandler(PresentationState ps) throws IOException {
		if(!ps.bConnected) {
			utils util = new utils();
			ps.dopsCommunications = new DOPsCommunication();


			//dopsCommunications.addActionHandler("ChatInfo", this::actionHandlerUpdateCustomerListBox);
			//dopsCommunications.addActionHandler("ChatForwardInfoRequest", this::actionHandlerUpdateManagerListBox);

			if(util.isEnvironmentOK()) {
				//String uniqueId = "CloudChatAdminApp";
				String uniqueId = "754d148d0522659d0ceb2e6035fad6a8";
				String username = "johndoe@email.com";
				String password = "12345";

				if(ps.dopsCommunications.connectToDOPs(uniqueId, username, password)) {
					// Set icon as connected
					//buttonConnect.setIcon(buttonConnect.getPressedIcon());
					ps.bConnected = true;
				}
			}
		}
		else
		{
			if(ps.dopsCommunications.disconnectFromDOPs()) {
				// Set icon as disconnected
				//buttonConnect.setIcon(buttonConnect.getDisabledIcon());
				ps.bConnected = false;
			}
		}
		return observable -> System.out.println("- Connect button pressed!");
	}
}
