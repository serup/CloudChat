package JavaSwingApp.ViewControllers;

import javax.swing.*;

public class HandleManagerListBox {
   private DefaultListModel managerlistModel;
   private JList managerlist;

   public HandleManagerListBox(JList managerlist, DefaultListModel managerlistmodel) {
      this.managerlistModel = managerlistmodel;
      this.managerlist = managerlist;
   }

   public void addElementToListBox(Object element)
   {
      this.managerlistModel.addElement(element);
      this.managerlist.setModel(this.managerlistModel);
   }
}