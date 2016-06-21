package JavaFXApp;

import javafx.beans.InvalidationListener;

public class ActionHandlers {

	public static InvalidationListener connectHandler(PresentationState ps) {

		return observable -> System.out.println("- Connect button pressed!");
	}
}
