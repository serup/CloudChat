package JavaFXApp;

import ProtocolHandlings.DOPsCommunication;
import javafx.fxml.FXMLLoader;

public class PresentationState {

	boolean bConnected=false;
	DOPsCommunication dopsCommunications=null;
    FXMLLoader loader=null;
	ccAdminDialogController controller=null;
	ActionHandlers actionHandlers=null;

	void initData(FXMLLoader loader, ccAdminDialogController controller) {

		this.loader = loader;
		this.controller = controller;
		this.actionHandlers = new ActionHandlers(this); // needed to access additional handlers for incoming data
		this.controller.initManagersTableView();
		this.controller.initCustomerTableView();
	}
}
