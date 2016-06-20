package ccAdminDialogFxml;


import template.javafx.infrastructure.JavaFxWidgetBindings;

public class GUIBinder {

	private final ccAdminDialogController controller;
	private final PresentationState presentationState;

	public GUIBinder(ccAdminDialogController controller, PresentationState presentationState) {
		this.controller = controller;
		this.presentationState = presentationState;
	}

	void bindAndInitialize() {
		presentationState.initBinding();
		initWidgetBinding();
		initActionHandlers();
		presentationState.initData();
	}

	private void initWidgetBinding() {
	}

	private void initActionHandlers() {
	}
}
