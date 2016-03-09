using System;
using NUnit.Framework;
using WebSocketClient;
using System.Threading;
using System.Threading.Tasks;
using System.Net.WebSockets;
//using Microsoft.Hadoop.MapReduce;
using System.Collections.Generic;
using System.Linq;

namespace hadoopTests
{
	[TestFixture]
	public class hadoopConnectionTests
	{
		[Test]
		public void connectTohadoop ()
		{
			// TODO: use following to make a test for connecting to hadoop VM cluster
			// http://www.codeguru.com/columns/experts/how-to-create-mapreduce-jobs-for-hadoop-using-c.htm

/*			HadoopJobConfiguration hadoopConfiguration = new HadoopJobConfiguration();
	        hadoopConfiguration.InputPath = "/input";
	        hadoopConfiguration.OutputFolder = "/output";
	        Uri myUri = new Uri("one.cluster:8020");
	        IHadoop hadoop = Hadoop.Connect(myUri, "vagrant", "vagrant");
	 
	        hadoop.MapReduceJob.Execute<ErrorTextMapper, ErrorTextReducerCombiner>(hadoopConfiguration);
	 */
			Assert.IsTrue (false);
		}
	}

	/*
	// Mapper and Reduce classes ;  NuGet package Microsoft .Net Map Reduce API for Hadoop
	public class ErrorTextMapper : MapperBase
    {
        public override void Map(string inputLine, MapperContext context)
        {
            if (inputLine.ToLowerInvariant().Equals("error"))
                context.EmitLine(inputLine);
        }
    }

	public class ErrorTextReducerCombiner : ReducerCombinerBase
    {
        public override void Reduce(string key, IEnumerable<string> values, ReducerCombinerContext context)
        {
            context.EmitKeyValue("errortextcount: ", values.Count().ToString());
        }
    }

    */
}

