package JavaFXApp;

import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;

public class ccAdminDialogController {

	@FXML
	Button connectButton;

	@FXML
	TableView<CustomerTableEntry> customersTable;

	private ObservableList<CustomerTableEntry> customersTableViewItems = FXCollections.observableArrayList();

	CustomerTableEntry createCellRowForCustomerTableView()
	{
		return new CustomerTableEntry();
	}

	void addCellRowElementsToCustomerView( CustomerTableEntry entry){
		Platform.runLater(() -> {
            //if you change the UI, do it here !
            updateCustomersCellRowElement(entry);
        });
	}

	private void updateCustomersCellRowElement(CustomerTableEntry Item)
	{
		if(customersTable.getItems().size() == 0) {
			initCustomerTableView();
		}

		if(!updateRowElement(Item)) {
			appendRowElementToCustomersTableView(Item);
		}
	}

	private boolean updateRowElement(CustomerTableEntry Item)
	{
		boolean bResult=false;
		int pos=0;
		for(CustomerTableEntry item: customersTableViewItems) {
			if(item.userName.getValue().contains(Item.getUserName())) {
				// Element found - now update its members
				bResult = updateElementMembers(pos, item);
			}
			pos++;
		}
		return bResult;
	}

	private boolean updateElementMembers(int pos, CustomerTableEntry Item)
	{
		boolean bResult=true;
		try {
			Item.userName.set(Item.getUserName());
			Item.userId.set(pos);
			Item.srcHomepageAlias.set(Item.getSrcHomepageAlias());
			//TODO: add update of more members here...

		}catch (Exception e){
			bResult=false;
		}
		return bResult;
	}

	private void appendRowElementToCustomersTableView(CustomerTableEntry Item)
	{
		CustomerTableEntry entry = new CustomerTableEntry();
		entry.userName.set(Item.getUserName());
		entry.srcHomepageAlias.set(Item.getSrcHomepageAlias());
		entry.userId.set(customersTableViewItems.size()-1);
		//TODO: add more entries

		customersTableViewItems.add(entry);
		customersTable.setItems(customersTableViewItems);
	}

	void initCustomerTableView()
	{
		assert customersTable != null : "fx:id=\"customersTable\" was not injected: check your FXML file ";
		TableColumn<CustomerTableEntry,String> colUserName = new TableColumn<>("User");
		TableColumn<CustomerTableEntry,String>  colHomepage = new TableColumn<>("Homepage");
		customersTable.setEditable(true);
		customersTable.getColumns().clear();
		customersTable.getColumns().addAll(colUserName, colHomepage);

		colUserName.setCellValueFactory( new PropertyValueFactory<>("userName"));
		colHomepage.setCellValueFactory( new PropertyValueFactory<>("srcHomepageAlias"));
		//TODO: add more entries
	}


}

