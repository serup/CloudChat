package JavaFXApp;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.util.logging.Level;
import java.util.logging.Logger;

import static JavaFXApp.ApplicationConfig.*;

/**
 * Created by serup on 6/20/16.
 */
public class ccAdminApplication extends Application {

    private final Logger logger = Logger.getLogger(JavaFXApp.ccAdminApplication.class.getName());
    private GUIBinder guiBinder;
    private PresentationState presentationState;
    private FXMLLoader loader;
    private Scene scene;

    public static void main(String[] args) {
        Application.launch(JavaFXApp.ccAdminApplication.class, args);
    }

    @Override
    public void init() throws Exception {
        loader = new FXMLLoader(JavaFxApplications.fxmlUrl(FXML_URL), JavaFxApplications.resources(RESOURCE_BUNDLE_NAME));
        loader.load();
        presentationState = new PresentationState();
        guiBinder = new GUIBinder(loader, presentationState);
        guiBinder.bindAndInitialize();
        scene = new Scene(loader.getRoot());
    }

    @Override
    public void stop() throws Exception {
    }

    @Override
    public void start(Stage primaryStage) throws Exception {
        try {
            scene.getStylesheets().add(getClass().getResource(CSS_URL).toExternalForm());
            primaryStage.setScene(scene);
            primaryStage.setTitle(loader.getResources().getString("title"));
            primaryStage.show();
        } catch (Exception ex) {
            logger.log(Level.SEVERE, null, ex);
        }
    }

    public PresentationState getPresentationState() {
        return presentationState;
    }

}
