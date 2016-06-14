import javax.swing.*;
import java.awt.event.*;

public class ccAdminDialogSwing extends JDialog {
    private JPanel contentPane;
    private JButton buttonOK;
    private JButton buttonCancel;
    private JTabbedPane tabbedPane;
    private JList listManagers;
    private JList listCustomers;
    private JButton buttonConnect;
    private static boolean bConnected=false;


    public ccAdminDialogSwing() {
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
                onConnect();
            }
        });
    }

    private void onConnect() {
        if(!bConnected) {
            //TODO: connect to DOPS

            // Set icon as connected
            buttonConnect.setIcon(buttonConnect.getPressedIcon());
            bConnected = true;
        }
        else
        {
            //TODO: disconnect from DOPS

            // Set icon as disconnected
            buttonConnect.setIcon(buttonConnect.getDisabledIcon());
            bConnected = false;
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
        ccAdminDialogSwing dialog = new ccAdminDialogSwing();
        dialog.setTitle("CloudChatAdministrator");
        dialog.pack();
        dialog.setVisible(true);
        dialog.setDefaultLookAndFeelDecorated(true); //TODO: find a way to change the look and feel of the app
        System.exit(0);
    }

}
