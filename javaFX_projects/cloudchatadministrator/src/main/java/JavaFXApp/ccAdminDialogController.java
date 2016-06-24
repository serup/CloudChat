package JavaFXApp;

import javafx.application.Platform;
import javafx.beans.Observable;
import javafx.beans.property.*;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.Cell;
import javafx.scene.control.ListView;
import javafx.util.Callback;
import org.apache.commons.net.ntp.TimeStamp;

public class ccAdminDialogController {

	@FXML
	Button connectButton;

	@FXML
	ListView<viewModelObjectForCustomersListView> customersList;

	private ObservableList<viewModelObjectForCustomersListView> customersListViewItems;

	private static class viewModelObjectForCustomersListView {
		StringProperty name = new SimpleStringProperty();
		IntegerProperty id = new SimpleIntegerProperty();
		ObjectProperty<org.apache.commons.net.ntp.TimeStamp> timestamp = new SimpleObjectProperty<>(TimeStamp.getCurrentTime());
		static Callback<viewModelObjectForCustomersListView, Observable[]> extractor() {
			return param -> new Observable[]{param.id, param.name, param.timestamp};
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

	void addCellRowElementsToCustomerListView(CellElementsInCustomerListView Item){
		Platform.runLater(() -> {
            //if you change the UI, do it here !
            updateItemInCustomersListView(Item);
        });
	}

	void initCustomerListView()
	{
		customersListViewItems = FXCollections.observableArrayList(viewModelObjectForCustomersListView.extractor());
		customersList.setItems(customersListViewItems);
	}


	private void updateItemInCustomersListView(CellElementsInCustomerListView Item)
	{
		boolean bUpdated=false;

		if(customersList.getItems().size() == 0)
			initCustomerListView();

		int pos=0;
 		for(viewModelObjectForCustomersListView item: customersListViewItems) {
			if(item.name.getValue().contains(Item.srcAlias)) {
				item.name.set(Item.srcAlias);
				item.id.set(pos);
				bUpdated=true;
			}
			pos++;
		}

		if(!bUpdated)
			appendItemToCustomersListView(Item);
	}

	private void appendItemToCustomersListView(CellElementsInCustomerListView Item)
	{
		viewModelObjectForCustomersListView viewModelEntry = new viewModelObjectForCustomersListView();
		customersListViewItems.add(viewModelEntry);
		viewModelEntry.name.set(Item.srcAlias);
		viewModelEntry.id.set(customersListViewItems.size()-1);
	}
}

