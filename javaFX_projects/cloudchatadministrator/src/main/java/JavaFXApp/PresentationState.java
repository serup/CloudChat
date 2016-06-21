package JavaFXApp;

import ProtocolHandlings.DOPsCommunication;
import javafx.fxml.FXMLLoader;

public class PresentationState {

	boolean bConnected=false;
	DOPsCommunication dopsCommunications=null;
    FXMLLoader loader=null;
	ccAdminDialogController controller=null;

	public void initBinding() {
	}

	public void initData(FXMLLoader loader, ccAdminDialogController controller) {

		this.loader = loader;
		this.controller = controller;

		//TODO: where to put this
			//controller.connectButton.setText(loader.getResources().getString("connected"));
			//TODO: change icon on button
			//controller.connectButton.setGraphic());
			//TODO: write connected in status bar

	}
}
