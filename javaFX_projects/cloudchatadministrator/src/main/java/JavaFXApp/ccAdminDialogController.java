package JavaFXApp;

import javafx.application.Platform;
import javafx.beans.Observable;
import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ListView;
import javafx.util.Callback;

public class ccAdminDialogController {

	@FXML
	Button connectButton;

	@FXML
	ListView<viewModelObjectForCustomersListView> customersList;

	private ObservableList<viewModelObjectForCustomersListView> customersListViewItems;

	static class viewModelObjectForCustomersListView {
		StringProperty name = new SimpleStringProperty();
		IntegerProperty id = new SimpleIntegerProperty();

		public static Callback<viewModelObjectForCustomersListView, Observable[]> extractor() {
			return new Callback<viewModelObjectForCustomersListView, Observable[]>() {
				@Override
				public Observable[] call(viewModelObjectForCustomersListView param) {
					return new Observable[]{param.id, param.name};
				}
			};
		}

		/**
		 * Format of the resulting entry string for the ListView item
		 *
		 * @return String
         */
		@Override
		public String toString() {
			return String.format("%s: %s", name.get(), id.get());
		}
	}
	void addElementToCustomerListBox(String Item){
		Platform.runLater(new Runnable() {
			@Override
			public void run() {
				//if you change the UI, do it here !
				updateItemInCustomersListView(Item);
			}
		});
	}

	public void initCustomerListView()
	{
		//...init the ListView appropriately
		customersListViewItems = FXCollections.observableArrayList(viewModelObjectForCustomersListView.extractor());
		customersList.setItems(customersListViewItems);
		//viewModelObjectForCustomersListView item = new viewModelObjectForCustomersListView();
		//customersListViewItems.add(item);
		//item.name.set("Mickey Mouse");
		// ^ Should update your ListView!!!
	}

	private void updateItemInCustomersListView(String Item)
	{
		boolean bUpdated=false;

		if(customersList.getItems().size() == 0)
			initCustomerListView();

		int pos=0;
 		for(viewModelObjectForCustomersListView item: customersListViewItems) {
			if(item.name.getValue().contains(Item)) {
				item.name.set(Item);
				item.id.set(pos);
				bUpdated=true;
			}
			pos++;
		}

		if(!bUpdated)
			appendItemToCustomersListView(Item);
	}

	private void appendItemToCustomersListView(String Item)
	{
		viewModelObjectForCustomersListView item = new viewModelObjectForCustomersListView();
		customersListViewItems.add(item);
		item.name.set(Item);
		item.id.set(customersListViewItems.size()-1);
	}
}

