import JavaServicesApp.ProtocolHandlings.DOPsCommunication;
import ViewControllers.HandleManagerListBox;
import dops.utils.utils;

import javax.swing.*;
import java.awt.event.*;
import java.io.IOException;

public class CloudChatAdministratorApp extends JDialog {
    private JPanel contentPane;
    private JButton buttonOK;
    private JButton buttonCancel;
    private JTabbedPane tabbedPane;
    private JList listManagers;
    private JList listCustomers;
    private JButton buttonConnect;
    private static boolean bConnected=false;
    private DOPsCommunication dopsCommunications=null;
    private HandleManagerListBox handleManagerListBox = new HandleManagerListBox(listManagers, new DefaultListModel());

    public CloudChatAdministratorApp() {
        //handleManagerListBox = new HandleManagerListBox(listManagers, new DefaultListModel());
        setContentPane(contentPane);
        setModal(true);
        getRootPane().setDefaultButton(buttonOK);

        buttonOK.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                onOK();
            }
        });

        buttonCancel.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                onCancel();
            }
        });

// call onCancel() when cross is clicked
        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                onCancel();
            }
        });

// call onCancel() on ESCAPE
        contentPane.registerKeyboardAction(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                onCancel();
            }
        }, KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0), JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT);
        listManagers.addFocusListener(new FocusAdapter() {
            @Override
            public void focusGained(FocusEvent e) {
                super.focusGained(e);
            }
        });

        buttonConnect.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                try {
                    onConnect();
                } catch (IOException e1) {
                    e1.printStackTrace();
                }
            }
        });
    }

    private void onConnect() throws IOException {
        if(!bConnected) {
            utils util = new utils();
            dopsCommunications = new DOPsCommunication();
            if(util.isEnvironmentOK()) {
                //String uniqueId = "CloudChatAdminApp";
                String uniqueId = "754d148d0522659d0ceb2e6035fad6a8";
                String username = "johndoe@email.com";
                String password = "12345";

                if(dopsCommunications.connectToDOPs(uniqueId, username, password)) {
                    handleManagerListBox.addElementToListBox("Jane Doe");
                    handleManagerListBox.addElementToListBox("John Smith");
                   // Set icon as connected
                    buttonConnect.setIcon(buttonConnect.getPressedIcon());
                    bConnected = true;
                }
            }
       }
        else
        {
            if(dopsCommunications.disconnectFromDOPs()) {
                // Set icon as disconnected
                buttonConnect.setIcon(buttonConnect.getDisabledIcon());
                bConnected = false;
            }
        }
    }

    private void onOK() {
// add your code here
        dispose();
    }

    private void onCancel() {
// add your code here if necessary
        dispose();
    }


    public static void main(String[] args) {
        CloudChatAdministratorApp dialog = new CloudChatAdministratorApp();
        dialog.setTitle("CloudChatAdministrator");
        dialog.pack();
        dialog.setVisible(true);
        dialog.setDefaultLookAndFeelDecorated(true); //TODO: find a way to change the look and feel of the app
        System.exit(0);
    }



}
