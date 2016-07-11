package JavaFXApp;

import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.beans.property.SimpleStringProperty;
import org.apache.commons.net.ntp.TimeStamp;

/**
 * Created by serup on 7/5/16.
 */
public class ManagerTableEntry {

    ObjectProperty<org.apache.commons.net.ntp.TimeStamp> timestamp = new SimpleObjectProperty<>(TimeStamp.getCurrentTime());
    SimpleIntegerProperty _tableId = new SimpleIntegerProperty();
    public SimpleStringProperty status = new SimpleStringProperty();
    public SimpleStringProperty userName = new SimpleStringProperty();
    public SimpleStringProperty userId = new SimpleStringProperty();
    ObjectProperty userPhoto = new SimpleObjectProperty();

    /**
     * used as a unique identifier for entry on this list
     * @return
     */
    public Integer get_tableId() {
        return _tableId.get();
    }

    public Object getUserPhoto() {
        return userPhoto.get();
    }

    public String getStatus() {
        return status.get();
    }

    public String getUserName() {
        return userName.get();
    }

    public String getUserId() {
        return userId.get();
    }

}
