package JavaFXApp;

import javafx.application.Platform;
import javafx.beans.Observable;
import javafx.beans.property.*;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ListView;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.util.Callback;
import org.apache.commons.net.ntp.TimeStamp;

public class ccAdminDialogController {

	@FXML
	Button connectButton;

	@FXML
	ListView<viewModelObjectForCustomersListView> customersList; //TODO: To be deprecated - use tableview instead

	@FXML
	TableView customersTable;

	private ObservableList<viewModelObjectForCustomersListView> customersListViewItems;
	private ObservableList<CustomerTableEntry> customersTableViewItems = FXCollections.observableArrayList();

	private static class viewModelObjectForCustomersListView {
		StringProperty name = new SimpleStringProperty();
		IntegerProperty id = new SimpleIntegerProperty();
		ObjectProperty<org.apache.commons.net.ntp.TimeStamp> timestamp = new SimpleObjectProperty<>(TimeStamp.getCurrentTime());
		StringProperty homepage = new SimpleStringProperty();
		static Callback<viewModelObjectForCustomersListView, Observable[]> extractor() {
			return param -> new Observable[]{param.id, param.name, param.timestamp, param.homepage};
		}

		/**
		 * Format of the resulting entry string for the ListView item
		 *
		 * @return String
         */
		@Override
		public String toString() {
			return String.format("%s: %s %s", name.get(), id.get(), timestamp.getValue().getDate());
		}
	}

	/**
	 * The transfer object class for Customer ListView
	 * this class should be aligned with the viewModelObjectForCustomersListView Observable class,
	 * meaning it should have minimum same amount of elements with same names, relevant to the list
	 */
	class CellElementsInCustomerListView
	{
		String srcAlias;
		String srcHomepageAlias;
		String lastEntryTime;
	}

	CellElementsInCustomerListView createNewCellRowForCustomerListView()
	{
		return new CellElementsInCustomerListView();
	}

	CustomerTableEntry createCellRowForCustomerTableView()
	{
		return new CustomerTableEntry();
	}

	void addCellRowElementsToCustomerView(CellElementsInCustomerListView CellRowElements){
		Platform.runLater(() -> {
            //if you change the UI, do it here !
            updateCustomersCellRowElement(CellRowElements);
        });
	}

	void addCellRowElementsToCustomerView( CustomerTableEntry entry){
		Platform.runLater(() -> {
            //if you change the UI, do it here !
            updateCustomersCellRowElement(entry);
        });
	}

	private void updateCustomersCellRowElement(CellElementsInCustomerListView Item)
	{
		boolean bUpdated=false;

		if(customersList.getItems().size() == 0) {
			initCustomerListView();
			initCustomerTableView();
		}


		int pos=0;
 		for(viewModelObjectForCustomersListView item: customersListViewItems) {
			if(item.name.getValue().contains(Item.srcAlias)) {
				item.name.set(Item.srcAlias);
				item.id.set(pos);
				item.homepage.set(Item.srcHomepageAlias);
				bUpdated=true;
			}
			pos++;
		}

		if(!bUpdated) {
			appendItemToCustomersListView(Item);
		}
	}

	private void updateCustomersCellRowElement(CustomerTableEntry Item)
	{
		boolean bUpdated=false;

		if(customersTable.getItems().size() == 0) {
			initCustomerTableView();
		}

		int pos=0;
		for(CustomerTableEntry item: customersTableViewItems) {
			if(item.userName.getValue().contains(Item.getUserName())) {
				item.userName.set(Item.getUserName());
				item.userId.set(pos);
				item.srcHomepageAlias.set(Item.getSrcHomepageAlias());
				bUpdated=true;
			}
			pos++;
		}

		if(!bUpdated) {
			appendItemToCustomersTableView(Item);
		}
	}

	void initCustomerListView()
	{
		customersListViewItems = FXCollections.observableArrayList(viewModelObjectForCustomersListView.extractor());
		customersList.setItems(customersListViewItems);
	}

	private void appendItemToCustomersListView(CellElementsInCustomerListView Item)
	{
		viewModelObjectForCustomersListView viewModelEntry = new viewModelObjectForCustomersListView();
		customersListViewItems.add(viewModelEntry);
		viewModelEntry.name.set(Item.srcAlias);
		viewModelEntry.id.set(customersListViewItems.size()-1);
	}

	void initCustomerTableView()
	{
		assert customersTable != null : "fx:id=\"customersTable\" was not injected: check your FXML file ";
		TableColumn colUserName = new TableColumn("User");
		TableColumn colHomepage = new TableColumn("Homepage");
		customersTable.setEditable(true);
		customersTable.getColumns().clear();
		customersTable.getColumns().addAll(colUserName, colHomepage);

		colUserName.setCellValueFactory( new PropertyValueFactory<CustomerTableEntry,String>("userName"));
		//colPassword.setCellValueFactory( new PropertyValueFactory<CustomerTableEntry,String>("userPassword"));
		//colUserType.setCellValueFactory( new PropertyValueFactory<CustomerTableEntry,String>("userType"));
		//colPhoto.setCellValueFactory( new PropertyValueFactory<Object,ImageView>("userPhoto"));

		colHomepage.setCellValueFactory( new PropertyValueFactory<CustomerTableEntry,String>("srcHomepageAlias"));
	}

	private void appendItemToCustomersTableView(CustomerTableEntry Item)
	{
		CustomerTableEntry entry = new CustomerTableEntry();
		entry.userName.set(Item.getUserName());
		entry.srcHomepageAlias.set(Item.getSrcHomepageAlias());
		entry.userId.set(customersTableViewItems.size()-1);

		//TODO: add more entries
		customersTableViewItems.add(entry);
		customersTable.setItems(customersTableViewItems);
	}
}

