package exampleDialogFxml;


import template.javafx.infrastructure.JavaFxWidgetBindings;

public class GUIBinder {

	private final ExampleDialogController controller;
	private final PresentationState presentationState;

	public GUIBinder(ExampleDialogController controller, PresentationState presentationState) {
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
		JavaFxWidgetBindings.bindTextField(controller.nameTextField, presentationState.name);
		JavaFxWidgetBindings.bindLabel(controller.greetingLabel, presentationState.greeting);
	}

	private void initActionHandlers() {
		JavaFxWidgetBindings.bindButton(controller.greetingButton, ActionHandlers.greetHandler(presentationState));
	}
}
