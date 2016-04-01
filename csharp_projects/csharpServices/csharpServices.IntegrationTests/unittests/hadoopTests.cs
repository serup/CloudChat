using System;
using NUnit.Framework;
using WebSocketClient;
using System.Threading;
using System.Threading.Tasks;
using System.Net.WebSockets;
using Microsoft.Hadoop.MapReduce;
using System.Collections.Generic;
using System.Linq;

// MapReduce example: http://www.codeproject.com/Articles/524233/MapReduceplus-fplusMapplusReductionplusStrategies
using SharpHadoop;
using System.Text.RegularExpressions;


namespace HadoopTests
{
	[TestFixture]
	public class HadoopConnectionTests
	{
		[Test]
		public void LsOnHadoop ()
		{
			// NB! Make sure HDFS is running on hadoop cluster and WebHDFS is enabled (this is not default) use ambari server to setup correctly
			WebHDFS hdfs = new WebHDFS("one.cluster", "testhadoop", "vagrant", "50070");
		          
			// List files in Directory
		        string result = hdfs.ListDir("/");
		        Console.WriteLine(result);

		        Assert.AreEqual(true,(result.Contains("mapred")));
		}

		[Test]
		public void DatanodesOnHadoop ()
		{
			// NB! Make sure HDFS is running on hadoop cluster and WebHDFS is enabled (this is not default) use ambari server to setup correctly
			AmbariManager hdfs = new AmbariManager("one.cluster", "testhadoop", "vagrant", "50070");
		          
			ResponseDatanode result = hdfs.ListDatanodes("two.cluster");
			Console.WriteLine(result.href);
			Console.WriteLine(result.hostRoles.cluster_name);
			Console.WriteLine(result.hostRoles.desired_state);

			Assert.AreEqual(true,(result.href.Contains("DATANODE")));
			Assert.AreEqual(true,(result.hostRoles.cluster_name.Contains("testhadoop")));
			Assert.AreEqual(true,(result.hostRoles.desired_state.Contains("STARTED")));
		}

		[Test]
		public void MapReduceJobOnHadoop ()
		{
			// TODO: use following to make a test for connecting to hadoop VM cluster
			// http://www.codeguru.com/columns/experts/how-to-create-mapreduce-jobs-for-hadoop-using-c.htm

			//HadoopJobConfiguration hadoopConfiguration = new HadoopJobConfiguration();  //TODO: require a new version og NuGet Packet Manager to install Microsoft MapReduce hadoop
		        //hadoopConfiguration.InputPath = "/input";
		        //hadoopConfiguration.OutputFolder = "/output";

			Environment.SetEnvironmentVariable("HADOOP_HOME", @"/usr/local/hadoop");
			Environment.SetEnvironmentVariable("JAVA_HOME", @"/usr/lib/jvm/java-8-oracle");
			Environment.SetEnvironmentVariable("Java_HOME", @"/usr/lib/jvm/java-8-oracle");

		        Uri myUri = new Uri("http://one.cluster:50070");
		        IHadoop hadoop = Hadoop.Connect(myUri, "admin", "admin");  // NB! if System.Net.Http fails, then set System.Web and System.Net.Http to copy local -- right click on reference and change

//			hadoop.MapReduceJob.Execute<ErrorTextMapper, ErrorTextReducerCombiner>(hadoopConfiguration);

			// first create /tmp/input/CodeFiles
			// then copy some .cs files to one.cluster local -  sshpass -p "vagrant" scp -oStrictHostKeyChecking=no -rp /home/serup/GerritHub/CloudChat/csharp_projects/csharpServices/csharpServices/*.cs vagrant@one.cluster:/tmp
			// then copy .cs files into hadoop - 
			// hadoop fs -copyFromLocal /tmp/*.cs /tmp/input/CodeFiles
			hadoop.MapReduceJob.ExecuteJob<NamespaceCounterJob>();
	 
			//Assert.IsTrue (false);
		}
	}



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

	///

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


	public class NamespaceCounterJob : HadoopJob<NamespaceMapper, NamespaceReducer> 
	{ 
		public override HadoopJobConfiguration Configure(ExecutorContext context) 
		{ 
			var config = new HadoopJobConfiguration(); 
			config.InputPath = "/tmp/input/CodeFiles"; 
			config.OutputFolder = "/tmp/output/CodeFiles"; 
			return config; 
		} 
	} // - See more at: http://www.amazedsaint.com/2013/03/taming-big-data-with-c-using-hadoop-on.html#sthash.dQe0hV2X.dpuf


}

