using System;
using NUnit.Framework;
using WebSocketClient;
using System.Threading;
using System.Threading.Tasks;
using System.Net.WebSockets;
using Microsoft.Hadoop.MapReduce;
using System.Collections.Generic;
using System.Linq;

namespace csharpServices
{
	//Mapper 
	public class NamespaceMapper : MapperBase 
	{ 
		//Override the map method. 
		public override void Map(string inputLine, MapperContext context) 
		{ 
			//Extract the namespace declarations in the Csharp files 
			var reg = new Regex(@"(using)\s[A-za-z0-9_\.]*\;"); 
			var matches = reg.Matches(inputLine); 
			foreach (Match match in matches) 
			{ 
				//Just emit the namespaces. 
				context.EmitKeyValue(match.Value,"1"); 
			} 
		}
	} 

}

