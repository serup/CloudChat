using System;

namespace csharpServices
{

	public partial class changeUniqueID : Gtk.Dialog
	{
		public string UniqueID;

		public changeUniqueID(string ID)
		{
			this.Build();
			if(ID != null) {
				this.entryUniqueID.Text = ID;
				this.ShowAll();
			}
		}

		protected void OnButtonOkClicked(object sender, EventArgs e)
		{
			UniqueID = this.entryUniqueID.Text;
		}

		protected void OnChangeUniqueIDShown(object sender, EventArgs e)
		{
			throw new NotImplementedException();
		}
	}
}

