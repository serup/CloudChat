package JavaFXApp;

import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.beans.property.SimpleStringProperty;

/**
 * Created by serup on 6/24/16.
 */
public class CustomerTableEntry {

    public SimpleIntegerProperty userId = new SimpleIntegerProperty();
    public ObjectProperty userPhoto = new SimpleObjectProperty();
    public SimpleStringProperty userName = new SimpleStringProperty();
    public SimpleStringProperty userPassword = new SimpleStringProperty();
    public SimpleStringProperty userType = new SimpleStringProperty();
    public SimpleStringProperty encPass = new SimpleStringProperty();
    public SimpleStringProperty updt = new SimpleStringProperty();
    public SimpleStringProperty uptm = new SimpleStringProperty();

    public SimpleStringProperty srcHomepageAlias = new SimpleStringProperty();

    public Integer getUserId() {
        return userId.get();
    }

    public Object getUserPhoto() {
        return userPhoto.get();
    }

    public String getUserName() {
        return userName.get();
    }

    public String getUserPassword() {
        return userPassword.get();
    }

    public String getUserType() {
        return userType.get();
    }

    public String getEncPass() {
        return encPass.get();
    }

    public String getUpdt() {
        return updt.get();
    }

    public String getUptm()
    {
        return uptm.get();
    }


    public String getSrcHomepageAlias() {
        return srcHomepageAlias.get();
    }

    public SimpleStringProperty srcHomepageAliasProperty() {
        return srcHomepageAlias;
    }

 }