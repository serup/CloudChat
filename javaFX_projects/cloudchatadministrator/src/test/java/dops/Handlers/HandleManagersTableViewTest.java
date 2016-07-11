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

        handleManagersTableView.addCellRowElementsToManagersView(newCellRowInTable);
        List<Object> objectsToForwardToManagers = handleManagersTableView.updateOnlineManagersWithIncomingChatInfo(dana);

        assertEquals(true, (objectsToForwardToManagers.size() == 1));
        objectsToForwardToManagers.stream().forEach(d -> assertEquals(true,((DOPsCommunication.ChatInfoObj)d).srcAlias.contains("JohnnyTest")));

        System.out.println("- Success - chatinfo object added to list of online managers");
    }
}
