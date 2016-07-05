package JavaFXApp;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;

/**
 * Created by serup on 7/5/16.
 */
public class HandleManagersTableView {

    TableView<ManagerTableEntry> managersTable;
    private ObservableList<ManagerTableEntry> managersTableViewItems = FXCollections.observableArrayList();

    HandleManagersTableView(TableView<ManagerTableEntry> managersTable)
    {
        this.managersTable = managersTable;
    }

    @SuppressWarnings("unchecked")
    void initManagersTableView()
    {
        assert managersTable != null : "fx:id=\"managersTable\" was not injected: check your FXML file ";
        TableColumn<ManagerTableEntry,String> colUserName = new TableColumn<>("User");
        TableColumn<ManagerTableEntry,String>  colHomepage = new TableColumn<>("Homepage");
        managersTable.setEditable(true);
        managersTable.getColumns().clear();
        managersTable.getColumns().addAll(colUserName, colHomepage);

        colUserName.setCellValueFactory( new PropertyValueFactory<>("userName"));
        colHomepage.setCellValueFactory( new PropertyValueFactory<>("srcHomepageAlias"));
        //TODO: add more entries
    }

}
