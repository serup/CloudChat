package JavaFXApp;

import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.TableView;

public class ccAdminDialogController {

	@FXML
	Button connectButton;

	@FXML
	TableView<CustomerTableEntry> customersTable;

	@FXML
	TableView<ManagerTableEntry> managersTable;


	HandleManagersTableView handleManagersTableView;
	HandleCustomerTableView handleCustomerTableView;

	void initManagersTableView()
	{
		assert managersTable != null : "fx:id=\"managersTable\" was not injected: check your FXML file ";
		handleManagersTableView = new HandleManagersTableView(managersTable);
		handleManagersTableView.initManagersTableView();
	}

	void initCustomerTableView()
	{
		assert customersTable != null : "fx:id=\"customersTable\" was not injected: check your FXML file ";
		handleCustomerTableView = new HandleCustomerTableView(customersTable);
		handleCustomerTableView.initCustomerTableView();
	}



}

