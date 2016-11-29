package JavaSwingApp.ViewControllers;

import javax.swing.*;

/**
 * Created by serup on 6/17/16.
 */
public class HandleCustomerListBox {
   private DefaultListModel listModel;
   private JList list;

   public HandleCustomerListBox(JList list, DefaultListModel listmodel) {
      this.listModel = listmodel;
      this.list = list;
   }

   public void addElementToListBox(Object element)
   {
      this.listModel.addElement(element);
      this.list.setModel(this.listModel);
   }
}
