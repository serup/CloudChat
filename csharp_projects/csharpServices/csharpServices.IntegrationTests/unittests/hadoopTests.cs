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


namespace HadoopTests
{
	[TestFixture]
	public class HadoopConnectionTests
	{
		[Test]
		public void ConnectTohadoop ()
		{
			// TODO: use following to make a test for connecting to hadoop VM cluster
			// http://www.codeguru.com/columns/experts/how-to-create-mapreduce-jobs-for-hadoop-using-c.htm

			HadoopJobConfiguration hadoopConfiguration = new HadoopJobConfiguration();  //TODO: require a new version og NuGet Packet Manager to install Microsoft MapReduce hadoop
		        hadoopConfiguration.InputPath = "/input";
		        hadoopConfiguration.OutputFolder = "/output";
		        Uri myUri = new Uri("one.cluster:8020");
		        IHadoop hadoop = Hadoop.Connect(myUri, "vagrant", "vagrant");  // NB! if System.Net.Http fails, then set System.Web and System.Net.Http to copy local -- right click on reference and change

	    		hadoop.MapReduceJob.Execute<ErrorTextMapper, ErrorTextReducerCombiner>(hadoopConfiguration);
	 
			Assert.IsTrue (false);
		}

		// Perhaps using this:
		// https://sharphadoop.codeplex.com/SourceControl/latest#SharpHadoop/trunk/SharpHadoop/Examples.cs
		[Test]
		public void ListFilesOnHadoop ()
		{
/*			//Uri myUri = new Uri("hdfs://one.cluster:8020/");
			//Uri myUri = new Uri("http://one.cluster:8020/webhdfs/v1/");
			//Uri myUri = new Uri("http://one.cluster:8020/");
			//Uri myUri = new Uri("http://one.cluster:50070/");
			Uri myUri = new Uri("http://one.cluster:8020/");
		        IHadoop hadoop = Hadoop.Connect(myUri, "vagrant", "vagrant");  // NB! if System.Net.Http fails, then set System.Web and System.Net.Http to copy local -- right click on reference and change

		        string absolutePath = hadoop.StorageSystem.GetAbsolutePath("/");
		        string qualifiedPath = hadoop.StorageSystem.GetFullyQualifiedPath(absolutePath);
			hadoop.StorageSystem.LsFiles(qualifiedPath);
*/		  
	 
			Assert.IsTrue (false);
		}

		[Test]
		public void LsOnHadoop ()
		{
			// NB! Make sure HDFS is running on hadoop cluster and WebHDFS is enabled (this is not default) use ambari server to setup correctly
			//WebHDFS hdfs = new WebHDFS("namenodeURL", "username", "50070");
			WebHDFS hdfs = new WebHDFS("one.cluster", "testhadoop", "vagrant", "50070");
		          
			// List files in Directory
		        string jsonString = hdfs.ListDir("/");
		        Console.WriteLine(jsonString);

		        Assert.AreEqual(true,(jsonString.Contains("mapred")));
		}

		[Test]
		public void DatanodesOnHadoop ()
		{
			// NB! Make sure HDFS is running on hadoop cluster and WebHDFS is enabled (this is not default) use ambari server to setup correctly
			WebHDFS hdfs = new WebHDFS("one.cluster", "testhadoop", "vagrant", "50070");
		          
			// List files in Directory
		        string jsonString = hdfs.ListDatanodes("two.cluster");
		        Console.WriteLine(jsonString);

		        Assert.AreEqual(true,(jsonString.Contains("mapred")));
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


}

