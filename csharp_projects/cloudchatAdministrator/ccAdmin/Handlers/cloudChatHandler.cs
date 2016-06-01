using System;
using DED;
using System.Collections.Generic;

namespace csharpServices
{
	public class dedAnalyzed : Object
	{
		public string type;
		public bool bDecoded;
		public Object elements; // will contain an object of following below type:
		public string[] getElementNames()
		{
			List<string> result = new List<string>();
			Object obj = this.elements;
			var prop = obj.GetType().GetFields();
			foreach (var field in obj.GetType().GetFields())
			{
				result.Add(field.Name);
			}
			return result.ToArray();
		}
	}

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
	public class cloudChatHandler
	{
		public cloudChatHandler()
		{
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
						if((fio.transactionsID = DED2.GET_USHORT("TransID")) != -1 &&
						    (fio.protocolTypeID = DED2.GET_STDSTRING("protocolTypeID")).Equals("DED1.00.00") &&
							(fio.dest = DED2.GET_STDSTRING("dest")).Length > 0 &&
							(fio.src = DED2.GET_STDSTRING("src")).Length > 0 &&
							(fio.srcAlias = DED2.GET_STDSTRING("srcAlias")).Length > 0 &&
						   (DED2.GET_STRUCT_END("CloudManagerRequest")).Equals(1)) {
							dana.bDecoded = true;
							dana.type = "ForwardInfoRequest";
							dana.elements = fio;
						} 
						break;
					case "JSCChatInfo":
						ChatInfoObj cio = new ChatInfoObj();
						if((cio.transactionsID = DED2.GET_USHORT("TransID")) != -1 &&
							(cio.protocolTypeID = DED2.GET_STDSTRING("protocolTypeID")).Equals("DED1.00.00") &&
							(cio.dest = DED2.GET_STDSTRING("dest")).Length > 0 &&
							(cio.src = DED2.GET_STDSTRING("src")).Length > 0 &&
							(cio.srcAlias = DED2.GET_STDSTRING("srcAlias")).Length > 0 &&
							(cio.srcHomepageAlias = DED2.GET_STDSTRING("srcHomepageAlias")).Length > 0 &&
							(cio.lastEntryTime = DED2.GET_STDSTRING("lastEntryTime")).Length > 0 &&
						   (DED2.GET_STRUCT_END("ClientChatRequest")).Equals(1)) {
							dana.bDecoded = true;
							dana.type = "ChatInfo";
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

