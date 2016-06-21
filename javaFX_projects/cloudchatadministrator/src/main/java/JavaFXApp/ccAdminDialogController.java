package JavaFXApp;

import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ListView;

public class ccAdminDialogController {

	@FXML
	Button connectButton;

	@FXML
	ListView customersList;


	void addElementToCustomerListBox(String Item){
		Platform.runLater(new Runnable() {
			@Override
			public void run() {
				//if you change the UI, do it here !
				customersList.getItems().add(customersList.getItems().size(), Item);
			}
		});
	}

}

