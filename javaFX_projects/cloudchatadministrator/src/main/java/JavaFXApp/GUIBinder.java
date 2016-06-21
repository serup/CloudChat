package JavaFXApp;


import javafx.fxml.FXMLLoader;
import template.javafx.infrastructure.JavaFxWidgetBindings;

import java.io.IOException;

class GUIBinder {

	private final FXMLLoader loader;
	private final ccAdminDialogController controller;
	private final PresentationState presentationState;

	GUIBinder(FXMLLoader loader , PresentationState presentationState) {
		this.loader = loader;
		this.controller = loader.getController();;
		this.presentationState = presentationState;
	}

	void bindAndInitialize() {
		presentationState.initBinding();
		initWidgetBinding();
		initActionHandlers();
		presentationState.initData(loader, controller);
	}

	private void initWidgetBinding() {
	}

	private void initActionHandlers() {
		try {
			JavaFxWidgetBindings.bindButton(controller.connectButton, ActionHandlers.connectHandler(presentationState));
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
