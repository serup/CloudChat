package JavaFXApp;

import ProtocolHandlings.DOPsCommunication;

public class PresentationState {

	boolean bConnected=false;
	DOPsCommunication dopsCommunications=null;

	public void initBinding() {
	}

	public void initData() {

		if(bConnected)
		{
			//TODO: change icon on button
			//TODO: write connected in status bar

		}
	}
}
