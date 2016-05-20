using Microsoft.Hadoop.MapReduce;
using System.Collections.Generic;
using System.Linq;

namespace csharpServices.IntegrationTests
{
	//Reducer 
	public class NamespaceReducer : ReducerCombinerBase 
	{ 
		//Accepts each key and count the occurrances 
		public override void Reduce(string key, IEnumerable<string> values, ReducerCombinerContext context) 
		{ 
			//Write back 
			context.EmitKeyValue(key,values.Count().ToString()); 
		} 
	} //  - See more at: http://www.amazedsaint.com/2013/03/taming-big-data-with-c-using-hadoop-on.html#sthash.dQe0hV2X.dpuf


}

