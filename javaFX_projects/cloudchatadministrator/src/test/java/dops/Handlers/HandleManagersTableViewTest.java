package dops.Handlers;

import JavaFXApp.HandleManagersTableView;
import JavaFXApp.ManagerTableEntry;
import ProtocolHandlings.DOPsCommunication;
import javafx.embed.swing.JFXPanel;
import javafx.scene.control.TableView;

import org.junit.Assert;
import org.junit.BeforeClass;
import org.junit.Test;

import javax.swing.*;
import java.util.List;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import static junit.framework.TestCase.assertEquals;
import static org.junit.Assert.assertArrayEquals;

/**
 * Created by serup on 7/11/16.
 */
public class HandleManagersTableViewTest {

    @BeforeClass
    public static void initToolkit() throws InterruptedException
    {
        final CountDownLatch latch = new CountDownLatch(1);
        SwingUtilities.invokeLater(() -> {
            new JFXPanel(); // initializes JavaFX environment
            latch.countDown();
        });

        // That's a pretty reasonable delay... Right?
        if (!latch.await(5L, TimeUnit.SECONDS))
            throw new ExceptionInInitializerError();
    }

    @Test
    public void updateOnlineManagersWithIncomingChatInfo() throws Exception {

        TableView<ManagerTableEntry> managersTable = new TableView<>();
        HandleManagersTableView handleManagersTableView = new HandleManagersTableView(managersTable);
        handleManagersTableView.initManagersTableView();

        DOPsCommunication.ChatInfoObj cio = new DOPsCommunication.ChatInfoObj();
        cio.transactionsID = 99;
        cio.protocolTypeID = "";
        cio.dest = "";
        cio.src = "";
        cio.srcAlias = "JohnnyTest";
        cio.srcHomepageAlias = "";
        cio.lastEntryTime = "";

        DOPsCommunication.dedAnalyzed dana = new DOPsCommunication.dedAnalyzed();
        dana.bDecoded = true;
        dana.type = "ChatInfo";
        dana.setDED(null); // not needed in this test
        dana.elements = cio;

        // Add a manager - simulate one online manager
        ManagerTableEntry newCellRowInTable = handleManagersTableView.createCellRowForManagersTableView();
        newCellRowInTable.status.set("ready");
        newCellRowInTable.userName.set("SERUP");
        newCellRowInTable.userId.set("e25abeb5422903015ebfaff94618b8fc");

        handleManagersTableView.addCellRowElementsToManagersView(newCellRowInTable);
        List<Object> objectsToForwardToManagers = handleManagersTableView.updateOnlineManagersWithIncomingChatInfo(dana);

        assertEquals(true, (objectsToForwardToManagers.size() == 1));
        objectsToForwardToManagers.stream().forEach(d -> assertEquals(true,((DOPsCommunication.ChatInfoObj)((DOPsCommunication.dedAnalyzed)d).getElements()).srcAlias.contains("JohnnyTest")));

        System.out.println("- Success - chatinfo object added to list of online managers");
    }

    @Test
    public void updateOnlineManagersWithIncomingManagerInfo() throws Exception {

        TableView<ManagerTableEntry> managersTable = new TableView<>();
        HandleManagersTableView handleManagersTableView = new HandleManagersTableView(managersTable);
        handleManagersTableView.initManagersTableView();

        // simulate manager info ded received
        DOPsCommunication.ForwardInfoRequestObj fio = new DOPsCommunication.ForwardInfoRequestObj();
        fio.transactionsID = 99;
        fio.protocolTypeID = "";
        fio.dest = "";
        fio.src = "";
        fio.srcAlias = "JohnnyTest";
        DOPsCommunication.dedAnalyzed dana = new DOPsCommunication.dedAnalyzed();
        dana.bDecoded = true;
        dana.type = "ChatForwardInfoRequest"; // manager is sending a request for chat to other managers
        dana.setDED(null); // not needed in this test
        dana.elements = fio;

        // Add two managers- simulate two online managers
        ManagerTableEntry newCellRowInTable = handleManagersTableView.createCellRowForManagersTableView();
        newCellRowInTable.status.set("ready");
        newCellRowInTable.userName.set("SERUP");
        newCellRowInTable.userId.set("e25abeb5422903015ebfaff94618b8fc");
        handleManagersTableView.addCellRowElementsToManagersView(newCellRowInTable);
        ManagerTableEntry newCellRowInTable2 = handleManagersTableView.createCellRowForManagersTableView();
        newCellRowInTable2.status.set("ready");
        newCellRowInTable2.userName.set("NIELSEN");
        newCellRowInTable2.userId.set("e88abeb5422903015ebfaff94618b9fd");
        handleManagersTableView.addCellRowElementsToManagersView(newCellRowInTable2);

        List<Object> objectsToForwardToManagers = handleManagersTableView.updateOnlineManagersWithOnlineManagersInfo(dana);

        // Verify that object will be forwarded to all online managers
        assertEquals(true, (objectsToForwardToManagers.size() == 2));
        objectsToForwardToManagers.stream().forEach(d -> assertEquals(true,((DOPsCommunication.ForwardInfoRequestObj)((DOPsCommunication.dedAnalyzed)d).getElements()).srcAlias.contains("JohnnyTest")));

        System.out.println("- Success - manager info object added to list of online managers - simulating; manager is sending a request for chat to other managers");
    }
}
