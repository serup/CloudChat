using System;
using DED;
using System.Collections.Generic;
using System.Reflection;

namespace csharpServices
{
	public class dedAnalyzed : Object
	{
		public string type;
		public bool bDecoded;
		public Object elements; // will contain an object of following below type:
		private byte[] originalDED = null;

		public byte[] getDED()
		{
			if(originalDED == null)
				throw new Exception("ERROR: dedAnalyzed did NOT contain the original ded - FATAL");
			return originalDED;
		}

		public void setDED(byte[] ded)
		{
			originalDED = ded;
		}

		public string[] getElementNames()
		{
			List<string> result = new List<string>();
			Object obj = this.elements;
			foreach (var field in obj.GetType().GetFields())
			{
				result.Add(field.Name);
			}
			return result.ToArray();
		}

		public element[] getElementNamesAndValues()
		{
			List<element> result = new List<element>();
			Object obj = this.elements;
			try {
				FieldInfo[] fields = obj.GetType().GetFields();
				//String str = "";
				foreach(FieldInfo f in fields){
					element eObj = new element();
					eObj.name = f.Name;
					eObj.value = f.GetValue(obj).ToString();
					result.Add(eObj);
					//str += f.Name + " = " + f.GetValue(obj) + "\r\n";
				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message.ToString());
			}

			return result.ToArray();
		}

		public element getElement(string name)
		{
			element result = null;
			Object obj = this.elements;
			try {
				FieldInfo[] fields = obj.GetType().GetFields();
				element eObj = new element();
				foreach(FieldInfo f in fields){
					if(f.Name == name)
					{
						eObj.name = f.Name;
						eObj.value = f.GetValue(obj).ToString();
						result = eObj;
						break;
					}
				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message.ToString());
			}
			return result;
		}
	}

	public class element : Object
	{
		public string name;
		public string value;
	}

	// object types 
	// Objects for elements in dedAnalyzed
	public class ForwardInfoRequestObj : Object
	{
		public short  transactionsID;
		public string protocolTypeID;
		public string dest;
		public string src;
		public string srcAlias;
	}

	public class ChatInfoObj : Object
	{
		public short  transactionsID;
		public string protocolTypeID;
		public string dest;
		public string src;
		public string srcAlias;
		public string srcHomepageAlias;
		public string lastEntryTime;
	}

	// General handler for communication with cloudchatmanager and cloudchatclients
	public class cloudChatPacketHandler
	{
		public cloudChatPacketHandler()
		{
		}

		public byte[] createDEDpackage(dedAnalyzed dana)
		{
			byte[] blob = null;
			try {
				if(dana.elements.GetType() == typeof(ChatInfoObj)) {
					DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
					DED.PUT_STRUCT_START ("ClientChatRequest");
					DED.PUT_METHOD ("Method", "JSCChatInfo");
					DED.PUT_USHORT ("TransID", short.Parse(dana.getElement("transactionsID").value.ToString()));
					DED.PUT_STDSTRING("protocolTypeID", dana.getElement("protocolTypeID").value.ToString());
					DED.PUT_STDSTRING("dest", dana.getElement("dest").value.ToString());  // This Admins uniqueID, since this is a test it really does not matter
					DED.PUT_STDSTRING("src", dana.getElement("src").value.ToString()); // uniqueID
					DED.PUT_STDSTRING("srcAlias", dana.getElement("srcAlias").value.ToString());
					DED.PUT_STDSTRING("srcHomepageAlias", dana.getElement("srcHomepageAlias").value.ToString());
					DED.PUT_STDSTRING("lastEntryTime", dana.getElement("lastEntryTime").value.ToString());
					DED.PUT_STRUCT_END("ClientChatRequest");
					blob = DED.GET_ENCODED_BYTEARRAY_DATA ();
				}
			}
			catch (Exception e) 
			{
				string msg = "ERROR: [cloudChatPacketHandler:createDEDpackage] Could not create a valid DED; " + e.Message;
				throw new Exception(msg);
			}
			return blob;
		}

		public dedAnalyzed parseDEDpacket(byte[] DED)
		{
			dedAnalyzed dana = new dedAnalyzed();

			if(DED == null) {
				dana.type = "<unknown>";
				dana.bDecoded = false;
			} else {
				DEDDecoder DED2 = DEDDecoder.DED_START_DECODER();
				DED2.PUT_DATA_IN_DECODER(DED, DED.Length);
				if((DED2.GET_STRUCT_START("CloudManagerRequest")).Equals(1)) {
					string method = DED2.GET_METHOD("Method");
					switch (method)
					{
					case "JSCForwardInfo":
						ForwardInfoRequestObj fio = new ForwardInfoRequestObj();
						if((fio.transactionsID  = DED2.GET_USHORT("TransID")) != -1 &&
						    (fio.protocolTypeID = DED2.GET_STDSTRING("protocolTypeID")).Equals("DED1.00.00") &&
							(fio.dest           = DED2.GET_STDSTRING("dest")).Length > 0 &&
							(fio.src            = DED2.GET_STDSTRING("src")).Length > 0 &&
							(fio.srcAlias       = DED2.GET_STDSTRING("srcAlias")).Length > 0 &&
						   (DED2.GET_STRUCT_END("CloudManagerRequest")).Equals(1)) {
							dana.bDecoded = true;
							dana.type = "ForwardInfoRequest";
							dana.setDED(DED);
							dana.elements = fio;
						} 
						break;
					case "JSCChatInfo":
						ChatInfoObj cio = new ChatInfoObj();
						if((cio.transactionsID    = DED2.GET_USHORT("TransID")) != -1 &&
							(cio.protocolTypeID   = DED2.GET_STDSTRING("protocolTypeID")).Equals("DED1.00.00") &&
							(cio.dest             = DED2.GET_STDSTRING("dest")).Length > 0 &&
							(cio.src              = DED2.GET_STDSTRING("src")).Length > 0 &&
							(cio.srcAlias         = DED2.GET_STDSTRING("srcAlias")).Length > 0 &&
							(cio.srcHomepageAlias = DED2.GET_STDSTRING("srcHomepageAlias")).Length > 0 &&
							(cio.lastEntryTime    = DED2.GET_STDSTRING("lastEntryTime")).Length > 0 &&
						   (DED2.GET_STRUCT_END("ClientChatRequest")).Equals(1)) {
							dana.bDecoded = true;
							dana.type = "ChatInfo";
							dana.setDED(DED);
							dana.elements = cio;
						}
						break;
					
					default:
						dana.bDecoded = false;
						dana.type = "<unknown>";
						break;
					}
				}
			}
			return dana;
		}
	}
}

