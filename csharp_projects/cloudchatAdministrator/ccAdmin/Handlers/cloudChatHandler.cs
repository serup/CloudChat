using System;
using DED;

namespace csharpServices
{
	public class dedAnalyzed : Object
	{
		public string type;
		public bool bDecoded;
	}

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
 						if((DED2.GET_USHORT("TransID")) != -1 &&
						   (DED2.GET_STDSTRING("protocolTypeID")).Equals("DED1.00.00") &&
						   (DED2.GET_STDSTRING("dest")).Length > 0 &&
						   (DED2.GET_STDSTRING("src")).Length > 0 &&
						   (DED2.GET_STDSTRING("srcAlias")).Length > 0 &&
						   (DED2.GET_STRUCT_END("CloudManagerRequest")).Equals(1)) {
							dana.bDecoded = true;
							dana.type = "ForwardInfoRequest";
						} 
						break;
					case "JSCChatInfo":
						if((DED2.GET_USHORT("TransID")) != -1 &&
						   (DED2.GET_STDSTRING("protocolTypeID")).Equals("DED1.00.00") &&
						   (DED2.GET_STDSTRING("dest")).Length > 0 &&
						   (DED2.GET_STDSTRING("src")).Length > 0 &&
						   (DED2.GET_STDSTRING("srcAlias")).Length > 0 &&
						   (DED2.GET_STDSTRING("srcHomepageAlias")).Length > 0 &&
						   (DED2.GET_STDSTRING("lastEntryTime")).Length > 0 &&
						   (DED2.GET_STRUCT_END("ClientChatRequest")).Equals(1)) {
							dana.bDecoded = true;
							dana.type = "ChatInfo";
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

