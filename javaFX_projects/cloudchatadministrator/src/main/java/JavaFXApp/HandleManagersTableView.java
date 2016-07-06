package JavaFXApp;

import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;
import org.apache.commons.net.ntp.TimeStamp;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by serup on 7/5/16.
 */
public class HandleManagersTableView {

    private TableView<ManagerTableEntry> managersTable;
    private ObservableList<ManagerTableEntry> managersTableViewItems = FXCollections.observableArrayList();

    HandleManagersTableView(TableView<ManagerTableEntry> managersTable)
    {
        this.managersTable = managersTable;
    }

    ManagerTableEntry createCellRowForManagersTableView()
	{
		return new ManagerTableEntry();
	}

    void addCellRowElementsToManagersView( ManagerTableEntry entry){
        updateManagersCellRowElement(entry);
    }

    void removeIdleCellRowElementsInManagersView()
    {
        removeOutdatedRowsElements();
    }

    private void updateManagersCellRowElement(ManagerTableEntry Item)
    {
        if(managersTable.getItems().size() == 0) {
            initManagersTableView();
        }

        if(!updateRowElement(Item)) {
            appendRowElementToManagersTableView(Item);
        }
    }

    private boolean updateRowElement(ManagerTableEntry Item)
    {
        boolean bResult=false;
        int pos=0;
        for(ManagerTableEntry item: managersTableViewItems) {
            if(item.userName.getValue().contains(Item.getUserName())) {
                // Element found - now update its members
                bResult = updateElementMembers(pos, item);
            }
            pos++;
        }
        return bResult;
    }

    private boolean updateElementMembers(int pos, ManagerTableEntry Item)
    {
        boolean bResult=true;
        try {
            Item.userId.set(pos);
            Item.status.set(Item.getStatus());
            Item.userName.set(Item.getUserName());
            Item.timestamp = new SimpleObjectProperty<>(TimeStamp.getCurrentTime());
            //TODO: add update of more members here...

        }catch (Exception e){
            bResult=false;
        }
        return bResult;
    }

    private void appendRowElementToManagersTableView(ManagerTableEntry Item)
    {
        ManagerTableEntry entry = new ManagerTableEntry();
        entry.userId.set(managersTableViewItems.size()-1);
        entry.status.set(Item.getStatus());
        entry.userName.set(Item.getUserName());
        //TODO: add more entries

        managersTableViewItems.add(entry);
        managersTable.setItems(managersTableViewItems);
    }

    private void removeOutdatedRowsElements()
    {
        ObjectProperty<TimeStamp> currentTimestamp = new SimpleObjectProperty<>(TimeStamp.getCurrentTime());
        long diff;
        long diffSeconds;
        List<Object> objectsToRemove = new ArrayList<>();
        for(ManagerTableEntry item: managersTableViewItems) {
            diff = currentTimestamp.getValue().getTime() - item.timestamp.getValue().getTime();
            diffSeconds = diff / 1000 % 60;
            if(diffSeconds > 10) {
                // Element has been idle for too long, meaning no communication, hence remove it
                System.out.printf("- Idle element [%s] - will be removed\n", item.getUserName());
                objectsToRemove.add(item);
            }
        }
        objectsToRemove.stream().forEach(o -> managersTableViewItems.remove(o));
    }

    @SuppressWarnings("unchecked")
    void initManagersTableView()
    {
        assert managersTable != null : "fx:id=\"managersTable\" was not injected: check your FXML file ";
        TableColumn<ManagerTableEntry,String> colStatus = new TableColumn<>("Status");
        TableColumn<ManagerTableEntry,String>  colUserName = new TableColumn<>("Name");
        managersTable.setEditable(true);
        managersTable.getColumns().clear();
        managersTable.getColumns().addAll(colStatus, colUserName);

        colStatus.setCellValueFactory( new PropertyValueFactory<>("status"));
        colUserName.setCellValueFactory( new PropertyValueFactory<>("userName"));
        //TODO: add more entries
    }

}
