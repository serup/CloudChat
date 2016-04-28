
#include "databasecontrol.hpp"

bool CDatabaseControl::update_element_value(std::vector<Elements>& _DEDElements, std::string strElementID, std::vector<unsigned char> ElementData)
{
    bool bResult=false;
    /// finding the element
    struct ElementIs {
        ElementIs( string s ) : toFind(s) { }
        bool operator() (const Elements &n)
        {
            return n.strElementID == toFind;
        }
        string toFind;
    };
    std::vector<Elements>::iterator it = std::find_if(_DEDElements.begin(), _DEDElements.end(), ElementIs(strElementID));
    if(it != _DEDElements.end())
    {
        /// now update value
        //Elements e;
        //e = (*it);
        //e.ElementData.clear();
        //std::copy(ElementData.begin(), ElementData.end(), std::back_inserter(e.ElementData));
        (*it).ElementData.resize(ElementData.size(),0); // Make sure receipient has room
        (*it).ElementData.clear();
        std::string strTestValue(ElementData.begin(),ElementData.end()); /// TEST - TEST - TEST
        std::copy(ElementData.begin(), ElementData.end(), std::back_inserter((*it).ElementData));
        bResult=true;
    }
    return bResult;
}

bool CDatabaseControl::update_element_value_with_file(std::vector<Elements>& _DEDElements, std::string strElementID, std::string strfilepath)
{
    /// insert a file into placeholder
    std::vector<unsigned char> fileElementData;
    bool bResult = FetchFileAsVector(strfilepath,fileElementData);
    if(bResult==true)
    {
        bResult = false;
        bResult = update_element_value(_DEDElements, (std::string) strElementID, fileElementData);
    }
    return bResult;
}

bool CDatabaseControl::find_element_value(std::vector<Elements>& _DEDElements, std::string strElementID,std::string& strElementIDvalue)
{
    bool bResult=false;
    /// finding the element
    struct ElementIs {
        ElementIs( string s ) : toFind(s) { }
        bool operator() (const Elements &n)
        {
            return n.strElementID == toFind;
        }
        string toFind;
    };

    std::vector<Elements>::iterator it = std::find_if(_DEDElements.begin(), _DEDElements.end(), ElementIs(strElementID));
    if(it != _DEDElements.end())
    {
        /// now fetch value
        Elements e;
        e = (*it);
        std::string strValue(e.ElementData.begin(),e.ElementData.end());
        strElementIDvalue = strValue;
        bResult=true;
    }
    return bResult;
}

bool CDatabaseControl::fetch_element(std::vector<Elements>& _DEDElements, std::string strElementID,Elements& ElementIDvalue)
{
    bool bResult=false;
    /// finding the element
    struct ElementIs {
        ElementIs( string s ) : toFind(s) { }
        bool operator() (const Elements &n)
        {
            return n.strElementID == toFind;
        }
        string toFind;
    };

    std::vector<Elements>::iterator it = std::find_if(_DEDElements.begin(), _DEDElements.end(), ElementIs(strElementID));
    if(it != _DEDElements.end())
    {
        /// now fetch value
        Elements e;
        e = (*it);
        ElementIDvalue = e;
        bResult=true;
    }
    return bResult;
}

bool CDatabaseControl::compare_element(std::vector<Elements> _DEDElements, std::string strElementID, std::vector<unsigned char> compareElementData)
{
    bool bResult=false;
    /// finding the element
    struct ElementIs {
        ElementIs( string s ) : toFind(s) { }
        bool operator() (const Elements &n)
        {
            return n.strElementID == toFind;
        }
        string toFind;
    };

    std::vector<Elements>::iterator it = std::find_if(_DEDElements.begin(), _DEDElements.end(), ElementIs(strElementID));
    if(it != _DEDElements.end())
    {
        /// compare value
        if((*it).ElementData != compareElementData)
            bResult=false;
        else
            bResult=true;
    }
    return bResult;
}

bool CDatabaseControl::find_element(std::vector<Elements>& _DEDElements, std::string strElementID)
{
    bool bResult=false;
    /// finding the element
    struct ElementIs {
        ElementIs( string s ) : toFind(s) { }
        bool operator() (const Elements &n)
        {
            return n.strElementID == toFind;
        }
        string toFind;
    };

    std::vector<Elements>::iterator it = std::find_if(_DEDElements.begin(), _DEDElements.end(), ElementIs(strElementID));
    if(it != _DEDElements.end())
    {
        /// only find no need to fetch value
/*        Elements e;
        e = (*it);
        std::string strValue(e.ElementData.begin(),e.ElementData.end());
        strElementIDvalue = strValue;
*/
        bResult=true;
    }
    return bResult;
}

/** \brief ReadXmlFile
 *
 * \param std::istream filename  std::ifstream is ("..\\..\\..\\..\\DataDictionary\\Database\\ENTITIEs\\ProfileTest.xml");
 * \param DatabaseEntityRecord & records
 * \param std::string EntityName
 * \return true / false
 *
 */
bool CDatabaseControl::ReadXmlFile( std::ifstream & is, DatabaseEntityRecord & records, std::string EntityName)
{
	bool bResult = true;
	DatabaseEntityRecord ans;

	// populate tree structure pt
	using boost::property_tree::ptree;
	ptree pt;
//	read_xml(is, pt);
	std::string strChild = EntityName;
	std::string strChildRecord = EntityName + "Record";
	// traverse pt
	try
	{
        read_xml(is, pt);
		BOOST_FOREACH( ptree::value_type const& v, pt.get_child(strChild) )
		{
            if(bBusy==false) return false;
			if( v.first == strChildRecord )
			{
				DatabaseEntityRecordEntry f;
				f.TransGUID = v.second.get<std::string>("TransGUID");
				f.Protocol  = v.second.get<std::string>("Protocol");
				f.ProtocolVersion = v.second.get<std::string>("ProtocolVersion");
				f.DataSize = v.second.get<unsigned int>("DataSize");
				f.Data = v.second.get<std::string>("Data");
				f.DataMD5 = v.second.get<std::string>("DataMD5");
				ans.push_back(f);
				//std::cout << EntityName << " database record entry : " << f.TransGUID << "\n";
			}
		}
	}
	catch (std::exception& e)
	{
		std::cout << "[ReadXmlFile] Error: " << e.what () << std::endl;
		bResult=false;
	}

	records = ans;

	if(is)
		is.close();

	return bResult;
}

bool CDatabaseControl::ReadTOASTXmlFile(std::ifstream & is, std::vector<Elements>& records_elements, std::string EntityName)
{
    bool bResult=true;
    //EntityToastData EntityTOASTDataResult;

    // populate tree structure pt
    using boost::property_tree::ptree;
    ptree pt;
    //read_xml(is, pt);
	std::string strChild = EntityName;
	std::string strChildRecord = EntityName + "Record";

    // traverse pt
    try
    {
        read_xml(is, pt);
        bool pushed=false;
        std::string prevchunkid = (std::string)"nothing";
        Elements Element;
        BOOST_FOREACH( ptree::value_type const& v, pt.get_child(strChild) )
        {
            if( v.first == strChildRecord )
            {
                DatabaseEntityRecordEntry f;
                f.TransGUID = v.second.get<std::string>("TransGUID");
                f.Protocol  = v.second.get<std::string>("Protocol");
                f.ProtocolVersion = v.second.get<std::string>("ProtocolVersion");
                f.DataSize = v.second.get<unsigned int>("DataSize");
                f.Data = v.second.get<std::string>("Data"); // data is inside xml file as ascii hex values
                f.DataMD5 = v.second.get<std::string>("DataMD5");
                //std::cout << strChild << " database TOAST record entry : " << f.TransGUID << "\n";

                std::string strMD5(CMD5(f.Data.c_str()).GetMD5s());
                if(strMD5 != f.DataMD5) {
                    std::cout << "ERROR: data area in file have changed without having the MD5 checsum changed -- Warning data could be compromised ; " << strChild << "\n";
                    return false;  // data area in file have changed without having the MD5 checsum changed -- Warning data could be compromised
                }
//TODO: INVESTIGATE byte size here -- size should be smaller after unhex, since hex take 2 bytes per byte
                // take the hex converted data and unhex it before DED will decode it
                unsigned int sizeofCompressedDataInHex = (unsigned int)f.DataSize;
                unsigned int sizeof_data_in_unhexed_buf = 4*sizeofCompressedDataInHex + 1;
                unsigned char* data_in_unhexed_buf = (unsigned char*) malloc (sizeof_data_in_unhexed_buf);
                ZeroMemory(data_in_unhexed_buf,sizeof_data_in_unhexed_buf); // make sure no garbage is inside the newly allocated space
                const std::vector<unsigned char> iterator_data_in_hexed_buf(&f.Data[0],&f.Data[sizeofCompressedDataInHex]);
                boost::algorithm::unhex(iterator_data_in_hexed_buf.begin(),iterator_data_in_hexed_buf.end(), data_in_unhexed_buf);// convert the hex array to an array containing byte values
                // According to what the protocol prescribes for DED entries in TOAST, then a loop of decode, according to specs, of entries is needed
                // fetch the data area and unpack it with DED to check it
                EntityChunkDataInfo chunk;
                DED_PUT_DATA_IN_DECODER(decoder_ptr,data_in_unhexed_buf,sizeof_data_in_unhexed_buf);
                assert(decoder_ptr != 0);
                std::string strEntityChunkId = boost::to_lower_copy(strChild) + "_chunk_id"; // eg. profile_chunk_id
                std::string strEntityChunkSeq = boost::to_lower_copy(strChild) + "_chunk_seq"; // eg. profile_chunk_seq
                std::string strEntityChunkData = boost::to_lower_copy(strChild) + "_chunk_data"; // eg. profile_chunk_data
                // decode data ...
                DED_GET_STRUCT_START( decoder_ptr, "record" );
                    DED_GET_STDSTRING	( decoder_ptr, strEntityChunkId, chunk.entity_chunk_id ); // key of particular item
                    DED_GET_ULONG   	( decoder_ptr, "aiid", chunk.aiid ); // this number is continuesly increasing all thruout the entries in this table
                    DED_GET_ULONG   	( decoder_ptr, strEntityChunkSeq, chunk.entity_chunk_seq ); // sequence number of particular item
                    DED_GET_STDVECTOR	( decoder_ptr, strEntityChunkData, chunk.entity_chunk_data ); //
                DED_GET_STRUCT_END( decoder_ptr, "record" );
                //toastDED.push_back(chunk);

                if(prevchunkid=="nothing" || prevchunkid != chunk.entity_chunk_id)
                {
                    if(prevchunkid!="nothing" && prevchunkid != chunk.entity_chunk_id){
                        //EntityTOASTDataResult.vecElements.push_back(Element);
                        records_elements.push_back(Element);
                        pushed=true;
                        /// new Element
                        Element.strElementID = chunk.entity_chunk_id;
                        Element.ElementData.clear();
                        prevchunkid = chunk.entity_chunk_id;
                    }
                    else
                    {
                        pushed=false;
                        /// new Element
                        Element.strElementID = chunk.entity_chunk_id;
                        Element.ElementData.clear();
                        prevchunkid = chunk.entity_chunk_id;
                    }
                }
                /// this will, chunk by chunk, assemble the elementfile data
                std::copy(chunk.entity_chunk_data.begin(), chunk.entity_chunk_data.end(), std::back_inserter(Element.ElementData));
                pushed=false;
                free(data_in_unhexed_buf);
            }
        }
        ///should only add to vecElements if chunks have been assembled and next element is to be processed
        if(pushed==false){
            //EntityTOASTDataResult.vecElements.push_back(Element);
            records_elements.push_back(Element);
        }
    }
    catch (std::exception& e)  {    std::cout << "[ReadTOASTXmlFile] Throw Error: " << e.what () << "\n"; bResult=false; }

    if(is)
        is.close();

    return bResult;
}



/** \brief WriteXmlFile
 *
 * \param std::ostream filename
 * \param unsigned char* pointer to compressed binary data area
 * \param unsigned int size of compressed data
 * \param std::string EntityName
 * \return true / false
 *
 */
//TODO: make WriteXmlFile thread safe, meaning it must timeout if not capable of writting to file
bool CDatabaseControl::WriteXmlFile(std::ostream & os,unsigned char* pCompressedData, unsigned int sizeofCompressedData, std::string EntityName )
{
    bool bResult=true;
    using boost::property_tree::ptree;
    ptree pt;
    int nSizeOfHex=0;
    /*
    <Profile>
    <version></version>
    <ProfileRecord>
        <TransGUID></TransGUID> -- each record has a unique transaction id to be used with security, when detecting inconsistencies
        <Protocol>DED</Protocol>
        <ProtocolVersion>1.0.0.0</ProtocolVersion>
        <DataSize></DataSize>
        <Data></Data> -- here is the pCompressedData from above protocol encoding, compression, encrypting algoritm -- currently DED (dataencoderdecoder)
        <DataMD5><DataMD5> -- this md5 checksum is used as an extra verification -- Data must be verified with this value before being used.
    </ProfileRecord>
    </Profile>
    */


    try
    {
        char* data_in_hex_buf = (char*) malloc (4*sizeofCompressedData + 1);
        ZeroMemory(data_in_hex_buf,4*sizeofCompressedData+1); // make sure no garbage is inside the newly allocated space
        const std::vector<unsigned char> iterator_data_in_hex_buf(&pCompressedData[0],&pCompressedData[sizeofCompressedData]);
        boost::algorithm::hex(iterator_data_in_hex_buf.begin(),iterator_data_in_hex_buf.end(), data_in_hex_buf);// convert the byte array to an array containing hex values in ascii

        std::string strMD5(CMD5((const char*)data_in_hex_buf).GetMD5s());

        std::string strtmp((const char*)data_in_hex_buf);
        nSizeOfHex = strtmp.size();

        std::string strEntity = EntityName;
        std::string strVersion = strEntity + ".version";
        std::string strEntityRecord = EntityName +"." + EntityName + "Record";
        pt.add(strVersion, 1);
        ptree & node = pt.add(strEntityRecord, ""); /// ex. Profile.ProfileRecord

        node.put("TransGUID", "581dd417e98c4e11b3b6aa15435cd929"); //TODO:change GUID for every write
        node.put("Protocol","DED");
        node.put("ProtocolVersion","1.0.0.0");
        node.put("DataSize",nSizeOfHex);
        node.put("Data",data_in_hex_buf);
        node.put("DataMD5",strMD5);

        write_xml( os, pt ); // write xml file
        free(data_in_hex_buf); // free temporary conversion buffer
    }
    catch (std::exception& e)
    {
        std::cout << "[WriteXmlFile] Error: " << e.what () << "\n";
        bResult=false;
    }

    return bResult;
}

/** \brief AppendXmlFile
 *
 * \param std::string filename
 * \param unsigned char* pointer to compressed binary data area
 * \param unsigned int size of compressed data
 * \param std::string EntityName
 * \return true / false
 *
 */
//bool CDatabaseControl::AppendXmlFile(std::ostream & os,unsigned char* pCompressedData, unsigned int sizeofCompressedData, std::string EntityName )
bool CDatabaseControl::AppendXmlFile(std::string filename,unsigned char* pCompressedData, unsigned int sizeofCompressedData, std::string EntityName )
{
    bool bResult=true;
    using boost::property_tree::ptree;
    ptree pt, node;
    int nSizeOfHex=0;
    /*
    <Profile>
    <version></version>
    <ProfileRecord>
        <TransGUID></TransGUID> -- each record has a unique transaction id to be used with security, when detecting inconsistencies
        <Protocol>DED</Protocol>
        <ProtocolVersion>1.0.0.0</ProtocolVersion>
        <DataSize></DataSize>
        <Data></Data> -- here is the pCompressedData from above protocol encoding, compression, encrypting algoritm -- currently DED (dataencoderdecoder)
        <DataMD5><DataMD5> -- this md5 checksum is used as an extra verification -- Data must be verified with this value before being used.
    </ProfileRecord>
    </Profile>
    */


    try
    {
        char* data_in_hex_buf = (char*) malloc (4*sizeofCompressedData + 1);
        ZeroMemory(data_in_hex_buf,4*sizeofCompressedData+1); // make sure no garbage is inside the newly allocated space
        const std::vector<unsigned char> iterator_data_in_hex_buf(&pCompressedData[0],&pCompressedData[sizeofCompressedData]);
        boost::algorithm::hex(iterator_data_in_hex_buf.begin(),iterator_data_in_hex_buf.end(), data_in_hex_buf);// convert the byte array to an array containing hex values in ascii
        std::string strMD5(CMD5((const char*)data_in_hex_buf).GetMD5s());
        std::string strtmp((const char*)data_in_hex_buf);
        nSizeOfHex = strtmp.size();

        std::string strversion = EntityName + ".version"; // eg. Profile.version
        std::string strrecord  = EntityName + "." + EntityName + "Record"; // eg. Profile.ProfileRecord

        if(!boost::filesystem::exists(filename))
        {
            /// File does NOT exist, thus handle it as a new creation
            pt.clear(); // http://www.boost.org/doc/libs/1_41_0/doc/html/boost/property_tree/basic_ptree.html#id973361-bb
            pt.add(strversion, 1);
            ptree &node = pt.add(strrecord, "");
            node.put("TransGUID", strMD5);
            node.put("Protocol","DED");
            node.put("ProtocolVersion","1.0.0.0");
            node.put("DataSize",nSizeOfHex);
            node.put("Data",data_in_hex_buf);
            node.put("DataMD5",strMD5);
        }
        else
        {
            /// First read file to be appended to
            read_xml( filename, pt );

            /// Then create a new record at the end
            ptree &newnode = pt.add(strrecord, "");
            newnode.put("TransGUID", strMD5);
            newnode.put("Protocol","DED");
            newnode.put("ProtocolVersion","1.0.0.0");
            newnode.put("DataSize",nSizeOfHex);
            newnode.put("Data",data_in_hex_buf);
            newnode.put("DataMD5",strMD5);

            /// Then insert at the end of record
            //std::string strChild = EntityName;
            //pt.insert(pt.get_child(strChild).end(),newnode.front()); /// ex. will add TransGUID node at the end


        }

        /// Then write file
        //boost::property_tree::xml_writer_settings<std::string> set(' ', 4);
        xml_writer_settings set(' ', 4);
        write_xml( filename, pt, std::locale(), set);// write xml file

        free(data_in_hex_buf); // free temporary conversion buffer
    }
    catch (std::exception& e)
    {
        std::cout << "[AppendXmlFile] Error: " << e.what () << "\n";
        bResult=false;
    }



///
///example:
    /*
    example 1:
    <xml version="1.0" encoding="utf-8"?>
    <list>
        <object name="ob1">
            <property1>foo</property1>
        </object>
        <object name="ob2">
            <property1>bar</property1>
        </object>
        ...
    </list>

    using namespace boost::property_tree;
    ptree pt, t;

    read_xml("test.xml", pt);

    // create new "tree":
    t.put("object.<xmlattr>.name","ob3");
    t.put("object.property1","sth");

    // insert t at the end of the list
    pt.insert(pt.get_child("list").end(),t.front());

    boost::property_tree::xml_writer_settings<char> set(' ', 4);
    write_xml("test.xml", pt, std::locale(), set);
    -------------------------------------------------
    example 2:
    using namespace boost::property_tree;
    ptree tree;

    read_xml("data.xml", tree);
    ptree &dataTree = tree.get_child("Data");
    dataTree.put("Parameter2", "2");

    xml_writer_settings<char> w(' ', 4);
    write_xml("test.xml", tree, std::locale(), w);
    */

    return bResult;
}

//http://akrzemi1.wordpress.com/2011/07/13/parsing-xml-with-boost/
EntityRealm CDatabaseControl::readDDEntityRealm( std::istream & is, std::string EntityName )
{
    // populate tree structure pt
    using boost::property_tree::ptree;
    ptree pt;
    //read_xml(is, pt);
    // traverse pt
    EntityRealm ans;
    std::string strChild = EntityName+"Realm";
    try
    {
        read_xml(is, pt);
        BOOST_FOREACH( ptree::value_type const& v, pt.get_child(strChild) )
        {
            if( v.first == "DDEntry" )
                {
                    DDEntityEntry f;
                    f.DDGuid = v.second.get<std::string>("DDGuid");
                    f.PhysicalDataElementName = v.second.get<std::string>("PhysicalDataElementName");
                    f.Location = v.second.get<std::string>("Location");
                    f.Category = v.second.get<std::string>("Category");
                    f.DataType = v.second.get<std::string>("DataType");
                    f.MaxLength = v.second.get<std::string>("MaxLength");
                    f.Description = v.second.get<std::string>("Description");
                    f.characteristics = v.second.get<std::string>("characteristics");
                    f.relationship = v.second.get<std::string>("relationship");
                    f.Mandatory = v.second.get<std::string>("Mandatory");
                    f.accessrights = v.second.get<std::string>("accessrights");
                    ans.push_back(f);
                    //std::cout << "DataDictionary entry : " << f.PhysicalDataElementName << "\n";
                }
        }
    }
    catch (std::exception& e)  {    std::cout << "[test] Error: " << e.what () << "\n"; }

    return ans;
}

bool CDatabaseControl::readDDEntityRealm( std::istream & is, std::string EntityName, std::vector<Elements>& DEDElements )
{
    bool bResult=false;

    // populate tree structure pt
    using boost::property_tree::ptree;
    ptree pt;
    //read_xml(is, pt);
    // traverse pt
    EntityRealm ans;
    std::string strChild = EntityName+"Realm";
    try
    {
        read_xml(is, pt);
        BOOST_FOREACH( ptree::value_type const& v, pt.get_child(strChild) )
        {
            if(bBusy==false) return false;
            if( v.first == "DDEntry" )
                {
                    DDEntityEntry f;
                    Elements elm;
                    //f.DDGuid = v.second.get<std::string>("DDGuid");
                    f.PhysicalDataElementName = v.second.get<std::string>("PhysicalDataElementName");
                    elm.strElementID = f.PhysicalDataElementName;
                    DEDElements.push_back(elm);
                    /*
                    f.Location = v.second.get<std::string>("Location");
                    f.Category = v.second.get<std::string>("Category");
                    f.DataType = v.second.get<std::string>("DataType");
                    f.MaxLength = v.second.get<std::string>("MaxLength");
                    f.Description = v.second.get<std::string>("Description");
                    f.characteristics = v.second.get<std::string>("characteristics");
                    f.relationship = v.second.get<std::string>("relationship");
                    f.Mandatory = v.second.get<std::string>("Mandatory");
                    f.accessrights = v.second.get<std::string>("accessrights");
                    */
                    ans.push_back(f);
                    //std::cout << "[readDDEntityRealm] DataDictionary entry : " << f.PhysicalDataElementName << "\n";
                    bResult=true;
                }
        }
    }
    catch (std::exception& e)  {    std::cout << "[readDDEntityRealm] Error: " << e.what () << "\n"; bResult=false; }

    return bResult;
}

bool CDatabaseControl::readDDEntityRealm( std::string EntityName, std::vector<Elements>& DEDElements )
{
    bool bResult = false;
    std::string uppercaseEntityName = boost::to_upper_copy(EntityName);
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::string strFilepathEntity = full_path.string() + relative_DATADICTIONARY_ENTITIES_PLACE + (std::string) uppercaseEntityName + ".xml"; // Datadictionary file spec for entity


/*
    TODO: WARNING!
    Be careful of race conditions: if the file disappears between the "exists" check and the time you open it, your program will fail unexpectedly.
    It's better to go and open the file, check for failure and if all is good then do something with the file. It's even more important with security-critical code.
*/
    if(!boost::filesystem::exists(strFilepathEntity))
    {
        std::cout << "[readDDEntityRealm] Error: Can't find file : " << strFilepathEntity << std::endl;
        boost::filesystem::path full_path( boost::filesystem::current_path() );
        std::cout << "[readDDEntityRealm] Error: Current path is : " << full_path << std::endl;

    }
    else
    {
        std::ifstream is (strFilepathEntity); // POSIX equivalent path
        bResult = readDDEntityRealm(is,(std::string)EntityName, DEDElements);
    }
    return bResult;
}

bool CDatabaseControl::readDDTOASTEntityRealm( std::string EntityName, std::vector<Elements>& DEDElements )
{
    bool bResult=false;
    std::string uppercaseEntityName = boost::to_upper_copy(EntityName);
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::string strFilepathEntity = full_path.string() + relative_DATADICTIONARY_ENTITIES_PLACE + (std::string) uppercaseEntityName + "_TOAST_ATTRIBUTES.xml"; // Datadictionary file spec for entity TOAST area
    if(!boost::filesystem::exists(strFilepathEntity))
    {
        std::cout << "[readDDTOASTEntityRealm] Error: Can't find file : " << strFilepathEntity << std::endl;
        boost::filesystem::path full_path( boost::filesystem::current_path() );
        std::cout << "[readDDTOASTEntityRealm] Error: Current path is : " << full_path << std::endl;

    }
    else
    {
        std::ifstream is (strFilepathEntity); // POSIX equivalent path
        try{
        bResult = readDDEntityRealm(is,(std::string)EntityName, DEDElements);
        }
        catch (std::exception& e)  {    std::cout << "[readDDTOASTEntityRealm] Error: " << e.what () << "\n"; bResult=false; }
    }
    return bResult;
}

bool CDatabaseControl::CHECK_REALM( EntityRealm realm, std::string realmEntries[], std::string EntityName )
{
    bool bResult=true;
    ptree pt;
    int n=0;
    BOOST_FOREACH( DDEntityEntry f, realm )
    {
        if(bBusy==false) return false;

        //std::cout << "DataDictionary entry : " << f.PhysicalDataElementName << "\n";
        if(f.PhysicalDataElementName != (std::string)realmEntries[n]) // last item
        {
            std::cout << "DataDictionary does NOT correlate with current version @ " << realmEntries[n] << "\n";
            bResult = false;
            return bResult;
        }
        n++;
    }
    return bResult;
}


bool CDatabaseControl::ReadEntityFile(std::string EntityName, std::string EntityFileName, std::vector<Elements>& DEDElements)
{
    bool bResult=true;
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::string strFilepath = full_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
    std::string uppercaseEntityName = boost::to_upper_copy(EntityName);
    std::string strFilepathEntity = full_path.string() + relative_DATADICTIONARY_ENTITIES_PLACE + (std::string) uppercaseEntityName + ".xml"; // Datadictionary file spec for entity

    if ( !boost::filesystem::exists( strFilepath ) && !boost::filesystem::exists( strFilepathEntity ))
    {
        //std::cout << "Can't find my file!" << std::endl;
        bResult=false;
    }
    else
    {
        /// Fetch all attributes from datadictionary file and create DED elements list
        std::ifstream isEntity (strFilepathEntity); // POSIX equivalent path
        bResult = readDDEntityRealm(isEntity,(std::string)EntityName, DEDElements);
        if(bResult==false)
        {
            std::cout << "[ReadEntityFile] ERROR : File can not be read - please check access rights : " << strFilepathEntity << std::endl;    /// no need to go further, something is wrong with the file
            return bResult;
        }

        /// Read data from file
        DatabaseEntityRecord EntityRecordResult;
        {
            std::ifstream is (strFilepath);
            if ( !boost::filesystem::exists( strFilepath ) )
            {
                //std::cout << "[ReadEntityFile] File NOT found : " << strFilepath << std::endl;
                bResult = false;
            }
            else
                bResult = ReadXmlFile(is,EntityRecordResult, EntityName);
        }
        if(bResult==false)
        {
            //std::cout << "[ReadEntityFile] ERROR : File can not be read : " << strFilepath << std::endl;    /// no need to go further, something is wrong with the file
            return bResult;
        }

        bResult=false;
        BOOST_FOREACH( DatabaseEntityRecordEntry f, EntityRecordResult )
        {
            if(bBusy==false) return false;
            if(f.DataSize > (unsigned int) 0)
            {
                // take the hex converted data and unhex it before DED will decode it
                unsigned int sizeofCompressedDataInHex = (unsigned int)f.DataSize;
                unsigned char* data_in_unhexed_buf = (unsigned char*) malloc (4*sizeofCompressedDataInHex + 1);
                ZeroMemory(data_in_unhexed_buf,4*sizeofCompressedDataInHex+1); // make sure no garbage is inside the newly allocated space
                const std::vector<unsigned char> iterator_data_in_unhexed_buf(&f.Data[0],&f.Data[sizeofCompressedDataInHex]);
                boost::algorithm::unhex(iterator_data_in_unhexed_buf.begin(),iterator_data_in_unhexed_buf.end(), data_in_unhexed_buf);// convert the hex array to an array containing byte values

                // fetch the data area and unpack it with DED to check it
                DED_PUT_DATA_IN_DECODER(decoder_ptr,data_in_unhexed_buf,sizeofCompressedDataInHex);
                assert(decoder_ptr != 0);

                bool bDecoded=true;

                // decode data ...
                if( DED_GET_STRUCT_START( decoder_ptr, "record" ) )
                {
                    int n=0;
                    std::string strtmp="";
                    BOOST_FOREACH( Elements f, DEDElements )
                    {
                        if(bBusy==false) return false;
                        strtmp=""; // clear
                        /// put attribute values from entity file into DEDElements
                        if(DED_GET_STDSTRING( decoder_ptr, (std::string)f.strElementID, strtmp ) == false)
                        {
                            std::cout << "[ReadEntityFile] WARNING : DECODING ERROR: elementID : " << f.strElementID << std::endl;
                            bDecoded=false;
                            break;
                        }
                        else
                        {
                            DEDElements[n].ElementData.clear();
                            std::copy(strtmp.begin(), strtmp.end(), std::back_inserter(DEDElements[n].ElementData));
                            n++;
                        }
                    }
                }
                if( bDecoded == true && DED_GET_STRUCT_END( decoder_ptr, "record" ) )
                {
                    //std::cout << "[ReadEntityFile] INFO : end of record " << std::endl;
                }
                else
                {
                    bDecoded=false;
                }
                bResult = bDecoded;
                free(data_in_unhexed_buf);
            }
        }
    }

    return bResult;
}

/** \brief AppendRecordToTOASTXmlFile
 *
 * \param std::ostream filename
 * \param unsigned char* pointer to compressed binary data area
 * \param unsigned int size of compressed data
 * \return true / false
 *
 */
bool CDatabaseControl::AppendRecordToTOASTXmlFile(std::string EntityName, bool bFlush, std::ostream & os,unsigned char* pCompressedData, unsigned int sizeofCompressedData )
{
    bool bResult=true;
    static int iFirst=1; // first time entered into this function
    using boost::property_tree::ptree;
    static ptree pt;
    int nSizeOfHex=0;

/* example of format (following datadictionary standard defined in file DD_TOAST.xml ):
<Profile>
<version></version>
<ProfileRecord>
    <TransGUID></TransGUID> -- each record has a unique transaction id to be used with security, when detecting inconsistencies
    <Protocol>DED</Protocol>
    <ProtocolVersion>1.0.0.0</ProtocolVersion>
    <DataSize></DataSize>
    <Data></Data> -- here is the pCompressedData from above protocol encoding, compression, encrypting algoritm -- currently DED (dataencoderdecoder)
    <DataMD5><DataMD5> -- this md5 checksum is used as an extra verification -- Data must be verified with this value before being used.
</ProfileRecord>
<ProfileRecord>
    <TransGUID></TransGUID> -- each record has a unique transaction id to be used with security, when detecting inconsistencies
    <Protocol>DED</Protocol>
    <ProtocolVersion>1.0.0.0</ProtocolVersion>
    <DataSize></DataSize>
    <Data></Data> -- here is the pCompressedData from above protocol encoding, compression, encrypting algoritm -- currently DED (dataencoderdecoder)
    <DataMD5><DataMD5> -- this md5 checksum is used as an extra verification -- Data must be verified with this value before being used.
</ProfileRecord>
<ProfileRecord>
    <TransGUID></TransGUID> -- each record has a unique transaction id to be used with security, when detecting inconsistencies
    <Protocol>DED</Protocol>
    <ProtocolVersion>1.0.0.0</ProtocolVersion>
    <DataSize></DataSize>
    <Data></Data> -- here is the pCompressedData from above protocol encoding, compression, encrypting algoritm -- currently DED (dataencoderdecoder)
    <DataMD5><DataMD5> -- this md5 checksum is used as an extra verification -- Data must be verified with this value before being used.
</ProfileRecord>
.
.
.
</Profile>
*/

///TODO: have this function read from datadictionary the definitions

    try{
        char* data_in_hex_buf = (char*) malloc (4*sizeofCompressedData + 1);
        ZeroMemory(data_in_hex_buf,4*sizeofCompressedData+1); // make sure no garbage is inside the newly allocated space
        const std::vector<unsigned char> iterator_data_in_hex_buf(&pCompressedData[0],&pCompressedData[sizeofCompressedData]);
        boost::algorithm::hex(iterator_data_in_hex_buf.begin(),iterator_data_in_hex_buf.end(), data_in_hex_buf);// convert the byte array to an array containing hex values in ascii
        std::string strMD5(CMD5((const char*)data_in_hex_buf).GetMD5s());
        std::string strtmp((const char*)data_in_hex_buf);
        nSizeOfHex = strtmp.size();

        std::string strversion = EntityName + ".version"; // eg. Profile.version
        std::string strrecord  = EntityName + "." + EntityName + "Record"; // eg. Profile.ProfileRecord

        if(iFirst==1){
                iFirst=-1;
            pt.clear(); // http://www.boost.org/doc/libs/1_41_0/doc/html/boost/property_tree/basic_ptree.html#id973361-bb
            pt.add(strversion, 1);
            ptree &node = pt.add(strrecord, "");
//            node.put("TransGUID", "581dd417e98c4e11b3b6aa15435cd929"); //TODO:change GUID for every write
            node.put("TransGUID", strMD5);
            node.put("Protocol","DED");
            node.put("ProtocolVersion","1.0.0.0");
            node.put("DataSize",nSizeOfHex);
            node.put("Data",data_in_hex_buf);
            node.put("DataMD5",strMD5);
        }
        else
        {
            ptree &node = pt.add(strrecord, "");
//            node.put("TransGUID", "581dd417e98c4e11b3b6aa15435cd929"); //TODO:change GUID for every write
            node.put("TransGUID", strMD5);
            node.put("Protocol","DED");
            node.put("ProtocolVersion","1.0.0.0");
            node.put("DataSize",nSizeOfHex);
            node.put("Data",data_in_hex_buf);
            node.put("DataMD5",strMD5);
        }


        if(bFlush==true) {
            write_xml( os, pt ); // write xml file
            iFirst=1; // reset
            os.flush();
        }
        free(data_in_hex_buf); // free temporary conversion buffer
    }catch (std::exception& e)  { std::cout << "[WriteXmlFile] Error: " << e.what () << "\n"; bResult=false; iFirst=1;}

    return bResult;
}


bool CDatabaseControl::AppendElementToToastXmlFile(std::string EntityName, bool bFlush, unsigned long& aiid, std::ostream & os, std::string entity_chunk_id, std::vector<unsigned char>& iterator_data_in_buf )
{
//    int iMaxChunkSize=1400;
//    int iMaxChunkSize=14000; /// TEST WITH LARGE SIZE -- virker ikke
//    int iMaxChunkSize=140; /// TEST WITH SMALL SIZE  -- virker, men er lidt for lille
//    int iMaxChunkSize=1400; /// TEST WITH SMALL SIZE -- virker ikke
//    int iMaxChunkSize=800; /// TEST WITH SMALL SIZE -- virker ikke
    int iMaxChunkSize=300; /// TEST WITH SMALL SIZE
    int iTotalSize=iterator_data_in_buf.size();
    int iBytesLeft=iTotalSize;
    int n=0;
    std::vector<unsigned char> chunkdata;
    unsigned long entity_chunk_seq= (unsigned long)0;
    bool bResult=false;
    bool bInternalFlush=false;
    std::string strentity_chunk_id   = boost::to_lower_copy(EntityName) + "_chunk_id";
    std::string strentity_chunk_seq  = boost::to_lower_copy(EntityName) + "_chunk_seq";
    std::string strentity_chunk_data = boost::to_lower_copy(EntityName) + "_chunk_data";
    do
    {
        chunkdata.clear();  // clean previous use

        if(iTotalSize>iMaxChunkSize){
            if(iMaxChunkSize>iBytesLeft){
                std::copy(iterator_data_in_buf.begin()+(n*iMaxChunkSize), iterator_data_in_buf.begin()+(n*iMaxChunkSize)+iBytesLeft, std::back_inserter(chunkdata));
                if(bFlush==true)
                    bInternalFlush=true;
            }
            else {
                std::copy(iterator_data_in_buf.begin()+(n*iMaxChunkSize), iterator_data_in_buf.begin()+(n*iMaxChunkSize)+iMaxChunkSize, std::back_inserter(chunkdata));
            }
        }
        else {
            std::copy(iterator_data_in_buf.begin(), iterator_data_in_buf.end(), std::back_inserter(chunkdata));
            if(bFlush==true)
                bInternalFlush=true;
        }

        if(chunkdata.size()<=0){
            std::cout << "[AppendElementToToastXmlFile] ERROR: NO data inserted in chunkdata, default <empty> added " << "\n";
            std::string strtmp="<empty>";
            std::copy(strtmp.begin(), strtmp.end(), std::back_inserter(chunkdata));
            //return false;
        }

        n++;
        aiid++;
        entity_chunk_seq++;
        { /// defined in DD_TOAST_DED.xml in datadictionary
        DED_START_ENCODER(encoder_ptr);
        DED_PUT_STRUCT_START( encoder_ptr, "record" );
            DED_PUT_STDSTRING	( encoder_ptr, strentity_chunk_id, entity_chunk_id ); // key of particular item
            DED_PUT_ULONG   	( encoder_ptr, "aiid", (unsigned long)aiid ); // this number is continuesly increasing all thruout the entries in this table
            DED_PUT_ULONG   	( encoder_ptr, strentity_chunk_seq, (unsigned long)entity_chunk_seq ); // sequence number of particular item
            DED_PUT_STDVECTOR	( encoder_ptr, strentity_chunk_data, chunkdata );
        DED_PUT_STRUCT_END( encoder_ptr, "record" );
        DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);
        if(sizeofCompressedData==0) sizeofCompressedData = iLengthOfTotalData; // if sizeofcompresseddata is 0 then compression was not possible and size is the same as for uncompressed
        iBytesLeft = iTotalSize-iMaxChunkSize*n;
        bResult = AppendRecordToTOASTXmlFile(EntityName, bInternalFlush, os, pCompressedData, sizeofCompressedData);
        }
    }while(iBytesLeft>0 || bResult==false);

    return bResult;
}


bool CDatabaseControl::WriteEntityTOASTFile(std::string EntityName, std::string EntityToastFileName,std::vector<Elements>& TOASTelements)
{
    bool bResult=false;

    boost::filesystem::path full_path( boost::filesystem::current_path() );
    //std::cout << "[put][WriteEntityTOASTFile] Current path is : " << full_path << std::endl;

    std::string strFilepath = full_path.string() + relativeENTITIES_DATABASE_TOAST_PLACE + EntityToastFileName + ".xml"; // database file for entity
    std::ofstream os (strFilepath);

    /// Put elements in structure
    bool bFlush=false;
    unsigned long aiid=(unsigned long)0;
    std::string customer_chunk_id=(std::string)"";
    int iAmount = TOASTelements.size();
    if(iAmount>0)
    {
        ///+ do until all elements are put into the toast area as multiple records, each element has its own unique customer_chunk_id
        BOOST_FOREACH( Elements f, TOASTelements )
        {
            if(bBusy==false) return false;
            customer_chunk_id=(std::string)f.strElementID;
            aiid++;
            iAmount--;
            if(iAmount==0)
                bFlush=true;  // last element, so flush

            try
            {
                bResult = AppendElementToToastXmlFile(EntityName, bFlush, aiid, os ,customer_chunk_id , f.ElementData);
            }
            catch (std::exception& e)
            {
                std::cout << "[AppendElementToToastXmlFile] Error: " << e.what () << "\n";
                bResult=false;
            }
            if(bResult == false)
                break;
        }
        ///-
    }
    else
        std::cout << "[WriteEntityTOASTFile] Warning: NO TOAST elements, NO TOAST data stored " << "\n";
    return bResult;
}

/** \brief WriteEntityFile - will write all elements into entity file. It will look in datadictionary and find out if element belongs in primary file or in TOAST file
 *
 * \param std::string EntityName
 * \param std::string EntityFileName
 * \param std::vector<Elements>& DEDElements
 * \return true / false
 *
 */
bool CDatabaseControl::WriteEntityFile(std::string EntityName, std::string EntityFileName, std::vector<Elements>& DEDElements)
{
    bool bResult=true;
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    //std::cout << "[put][WriteEntityFile] Current path is : " << full_path << std::endl;

    std::string strFilepath = full_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
    std::string uppercaseEntityName = boost::to_upper_copy(EntityName);
    std::string strchunkid_value = "";

    /// Fetch all attributes from datadictionary file and create DED elements list
    std::vector<Elements> _DEDElements;
    std::vector<Elements> _TOASTDEDElements;
    int iAmountOfToastElementsUpdated = 0;
    bResult = readDDEntityRealm((std::string)EntityName, _DEDElements);
    if(bResult==false)
    {
        //std::cout << "[WriteEntityFile] ERROR : File can not be read : Entity name : " << EntityName << std::endl;    /// no need to go further, something is wrong with the file
        return bResult;
    }
    else
    {
        bResult = readDDTOASTEntityRealm((std::string)EntityName, _TOASTDEDElements);
        if(bResult==false)
        {
            std::cout << "[WriteEntityFile] WARNING : NO TOAST File read : Entity name : " << EntityName << std::endl;
        }

        /// find the TOAST chunk id from main entity file attribut
        bResult=false;
        std::vector<Elements> DEDElementsInDBFile;
        std::string strChunkID = boost::to_lower_copy(EntityName) + "_chunk_id";
        strchunkid_value = "";
        bResult = find_element_value(DEDElements, strChunkID, strchunkid_value);
        if(bResult==false)
        {
            /// Since DEDElements did not contain chunk id field, then try and find it in the file - this scenario is when only one or a few entities are being updated, not the whole file
  //          bResult = ReadEntityFile(EntityName, EntityFileName, DEDElementsInDBFile);
  //          bResult = find_element_value(DEDElementsInDBFile, strChunkID, strchunkid_value);
  //          if(bResult==false)
  //          {
                std::cout << "[WriteEntityFile] ERROR : NO TOAST chunk id found - following attribut MUST be in file : <entity>_chunk_id : Entity name : " << EntityName << " Entity filename: " << strFilepath << std::endl;
                std::cout << "[WriteEntityFile] ERROR : INORDER TO WRITE INTO AN ENTITY FILE, THEN WHOLE FILESTRUCTURE MUST FOLLOW DATADICTIONARY, MEANING ALL ELEMENTS MUST BE PRESENT " << std::endl;
                return bResult; /// no need to go further, since DEDElements is NOT following datadictionary standard
  //          }
        }


        /// Merge value from DEDElements parameter with the value placeholder in _DEDElements (datadictionary present version) -- if some values can not be merged, then dictionary is NOT alligned with code and that could pose problems
        BOOST_FOREACH( Elements f, DEDElements )
        {
            if(bBusy==false) return false;
            /// find same element as in datadictionary definition
            bResult = update_element_value(_DEDElements, f.strElementID, f.ElementData);
            if(bResult == false)
            {
                /// check if element is allowed in TOAST area, if so then add it there
                /// find same element as in datadictionary definition
                bResult = update_element_value(_TOASTDEDElements, f.strElementID, f.ElementData);
                if(bResult==false)
                    std::cout << "[ReadEntityFile] ERROR : NO TOAST ENTRY FOUND FOR ELEMENT - perhaps datadictionary has been changed? : Entity name : " << EntityName << std::endl;
                else
                    iAmountOfToastElementsUpdated++;
            }
        }
    }

    if(bResult==false)
        return bResult; /// no need to go further

    std::string EntityToastFileName = strchunkid_value;

    /// setup DED object for the entity to write
    std::string tmp="";
    DED_START_ENCODER(encoder_ptr);
    DED_PUT_STRUCT_START( encoder_ptr, "record" );
    BOOST_FOREACH( Elements f, _DEDElements )
    {
        if(bBusy==false) return false;
        tmp="";
        std::copy(f.ElementData.begin(), f.ElementData.end(), std::back_inserter(tmp));
        DED_PUT_STDSTRING	( encoder_ptr, f.strElementID, tmp );
    }
    DED_PUT_STRUCT_END( encoder_ptr, "record" );
    DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);

    {
    std::ofstream os (strFilepath);
    bResult = WriteXmlFile(os, pCompressedData, sizeofCompressedData, (std::string) EntityName);
    if(bResult==false)
        std::cout << "[WriteEntityFile] ERROR : WriteXmlFile failed : " << strFilepath << std::endl;
    }

    if(bResult==false)
        return bResult; /// no need to go further

    /// setup TOAST and write it
    if(iAmountOfToastElementsUpdated>0)
    {
        bResult = WriteEntityTOASTFile(EntityName, EntityToastFileName, _TOASTDEDElements);
        if(bResult==false)
            std::cout << "[WriteEntityFile] ERROR : WriteEntityTOASTFile failed : " << EntityToastFileName << " Amount of elements tried to be updated: [" << iAmountOfToastElementsUpdated << "] " << std::endl;
    }
    else
        std::cout << "[WriteEntityFile] WARNING : WriteEntityTOASTFile NO ELEMENTS UPDATED!! : " << EntityToastFileName << std::endl;

    return bResult;
}

/** \brief AppendEntityFile - will append elements into an existing entity file. It will look in datadictionary and find out if element belongs in primary file or in TOAST file
 *
 * \param std::string EntityName
 * \param std::string EntityFileName
 * \param std::vector<Elements>& DEDElements
 * \return true / false
 *
 */
bool CDatabaseControl::AppendEntityFile(std::string EntityName, std::string EntityFileName, std::vector<Elements>& DEDElements)
{
    bool bResult=true;
    boost::filesystem::path full_path( boost::filesystem::current_path() );

    std::string strFilepath = full_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
    std::string uppercaseEntityName = boost::to_upper_copy(EntityName);
    std::string strchunkid_value = "";

    /// Fetch all attributes from datadictionary file and create DED elements list
    std::vector<Elements> _DEDElements;
    std::vector<Elements> _TOASTDEDElements;
    int iAmountOfToastElementsUpdated = 0;
    bResult = readDDEntityRealm((std::string)EntityName, _DEDElements);
    if(bResult==false)
    {
        std::cout << "[AppendEntityFile] ERROR : File can not be read : Entity name : " << EntityName << std::endl;    /// no need to go further, something is wrong with the file
        return bResult;
    }
    else
    {
        bResult = readDDTOASTEntityRealm((std::string)EntityName, _TOASTDEDElements);
        if(bResult==false)
        {
            std::cout << "[AppendEntityFile] WARNING : NO TOAST File read : Entity name : " << EntityName << std::endl;
        }

        /// find the TOAST chunk id from main entity file attribut
        bResult=false;
        std::string strChunkID = boost::to_lower_copy(EntityName) + "_chunk_id";
        strchunkid_value = "";
        bResult = find_element_value(DEDElements, strChunkID, strchunkid_value);
        if(bResult==false)
        {
            std::cout << "[AppendEntityFile] ERROR : NO TOAST chunk id found - following attribut MUST be in file : <entity>_chunk_id : Entity name : " << EntityName << std::endl;
            return bResult; /// no need to go further, since DEDElements is NOT following datadictionary standard
        }

        /// Merge value from DEDElements parameter with the value placeholder in _DEDElements (datadictionary present version) -- if some values can not be merged, then dictionary is NOT alligned with code and that could pose problems
        BOOST_FOREACH( Elements f, DEDElements )
        {
            if(bBusy==false) return false;
            /// find same element as in datadictionary definition
            bResult = update_element_value(_DEDElements, f.strElementID, f.ElementData);
            if(bResult == false)
            {
                /// check if element is allowed in TOAST area, if so then add it there
                /// find same element as in datadictionary definition
                bResult = update_element_value(_TOASTDEDElements, f.strElementID, f.ElementData);
                if(bResult==false)
                    std::cout << "[AppendEntityFile] ERROR : NO TOAST ENTRY FOUND FOR ELEMENT - perhaps datadictionary has been changed? : Entity name : " << EntityName << std::endl;
                else
                    iAmountOfToastElementsUpdated++;
            }
        }
    }

    if(bResult==false)
        return bResult; /// no need to go further

    std::string EntityToastFileName = strchunkid_value;

    /// setup DED object for the entity to write
    std::string tmp="";
    DED_START_ENCODER(encoder_ptr);
    DED_PUT_STRUCT_START( encoder_ptr, "record" );
    BOOST_FOREACH( Elements f, _DEDElements )
    {
        if(bBusy==false) return false;
        tmp="";
        std::copy(f.ElementData.begin(), f.ElementData.end(), std::back_inserter(tmp));
        DED_PUT_STDSTRING	( encoder_ptr, f.strElementID, tmp );
    }
    DED_PUT_STRUCT_END( encoder_ptr, "record" );
    DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);

    {
//    std::ofstream os (strFilepath);
//    bResult = AppendXmlFile(os, pCompressedData, sizeofCompressedData, (std::string) EntityName);
    bResult = AppendXmlFile(strFilepath, pCompressedData, sizeofCompressedData, (std::string) EntityName);
    if(bResult==false)
        std::cout << "[AppendEntityFile] ERROR : AppendXmlFile failed : " << strFilepath << std::endl;
    }

    if(bResult==false)
        return bResult; /// no need to go further

    if(iAmountOfToastElementsUpdated>0)
    {
        //bResult = AppendEntityTOASTFile(EntityName, EntityToastFileName, _TOASTDEDElements);
        //if(bResult==false)
            std::cout << "[AppendEntityFile] ERROR : AppendEntityTOASTFile failed : " << EntityToastFileName << " Amount of elements tried to be updated: [" << iAmountOfToastElementsUpdated << "] " << std::endl;
    }
    //else
    //    std::cout << "[AppendEntityFile] WARNING : AppendEntityTOASTFile NO ELEMENTS UPDATED!! : " << EntityToastFileName << std::endl;

    return bResult;
}

bool CDatabaseControl::updateEntityAttribut(std::vector<Elements>& DEDElements, std::string strElementID, std::vector<unsigned char> ElementData )
{
    bool bResult=false;
// not working - hmm    /// lambda function finding the element
//    std::find_if(DEDElements.begin(), DEDElements.end(), [&](Elements & o)
//    {
//        if(o.strElementID == (std::string)strElementID)
//        {
//            /// put new value in DEDElements which should be current datadictionary entity structure
//            o.ElementData.clear();
//            std::copy(ElementData.begin(), ElementData.end(), std::back_inserter(o.ElementData));
//            bResult=true;
//            return bResult;
//        }
//    });

    bResult = update_element_value(DEDElements, strElementID, ElementData);

    return bResult;
}

bool CDatabaseControl::updateEntityAttribut(std::string EntityName, std::string EntityFileName, std::string strElementID, std::vector<unsigned char> ElementData )
{
    bool bResult=true;
    /// Fetch all attributes from datadictionary file and create DED elements list
    std::vector<Elements> _DEDElements;
    bResult = readDDEntityRealm((std::string)EntityName, _DEDElements);
    if(bResult==false)
    {
        std::cout << "[readDDEntityRealm] ERROR : File can not be read : Entity name : " << EntityName << std::endl;    /// no need to go further, something is wrong with the file
        return bResult;
    }
    else
    {
        /// find same element as in datadictionary definition - only elements existing in datadictionary will be updated in entity database file
        bResult = find_element(_DEDElements, strElementID);
        if(bResult == true)
        {
            /// Element was found, now read entity file and find it also there and make an update
            std::vector<Elements> wDEDElements;
            bResult = ReadEntityFile((std::string)EntityName,(std::string)EntityFileName,wDEDElements);
            if(bResult==false)
            {
                std::cout << "[ReadEntityFile] ERROR : File can not be read : Entity name : " << EntityName << std::endl;    /// no need to go further, something is wrong with the file
                return bResult;
            }
            else
            {
                /// database file has been read, now update elements attribut value and write file again
                /// function finding the element in read entity file
                bResult = find_element(wDEDElements, strElementID);
                if(bResult==true)
                {
                    /// put new value in wDEDElements which should be current datadictionary entity structure
                    bResult = update_element_value(wDEDElements,strElementID,ElementData);
                    if(bResult==true)
                    {
                        /// now write back into the entity file
                        bResult = WriteEntityFile((std::string)EntityName,(std::string)EntityFileName,wDEDElements);
                    }
                }
            }
        }
    }
    return bResult;
}


bool CDatabaseControl::FetchFileAsVector(std::string strfilepath, std::vector<unsigned char> &FileDataBytesInVector)
{
    /// http://www.cplusplus.com/reference/istream/istream/read/
    long length = 0;

    bool bResult=false;


/////+tst - works
//  std::ifstream infile ("hugo1.gif",std::ifstream::binary);
//  std::ofstream outfile ("outhugo.gif",std::ofstream::binary);
//  // get size of file
//  infile.seekg (0,infile.end);
//  long size = infile.tellg();
//  infile.seekg (0);
//  // allocate memory for file content
//  char* buffer = new char[size];
//  // read content of infile
//  infile.read (buffer,size);
//  // write to outfile
//  outfile.write (buffer,size);
//  // release dynamically-allocated memory
//  delete[] buffer;
//  outfile.close();
//  infile.close();
/////-tst


    std::ifstream is (strfilepath, ios::binary);
    if (is)
    {
        // get length of file:
        is.seekg (0, is.end);
        length = is.tellg();
        is.seekg (0);

        std::cout << "[FetchFileAsVector] Reading file: " << strfilepath << " ; amount " << length << " characters... \n";

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//    std::istream_iterator<unsigned char> start(is), end(is.seekg (0, is.end));
//    FileDataBytesInVector.resize(length,0); // Make sure receipient has room
//    std::copy(start, end, std::back_inserter(FileDataBytesInVector)); // read file into receipient vector
///////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
        // allocate memory for file content
        char* buffer = new char[length];
        // read content of infile
        is.read (buffer,length);
        std::vector<unsigned char> vec(&buffer[0],&buffer[length]);
        // transfer to vector
        std::copy(vec.begin(), vec.end(), std::back_inserter(FileDataBytesInVector)); // read file into receipient vector
        // release dynamically-allocated memory
        delete[] buffer;
*/

        FileDataBytesInVector.resize(length,0); // Make sure receipient has room
        // read content of infile
        is.read ((char*)&FileDataBytesInVector[0],length);

        std::cout << "[FetchFileAsVector] size: " << (int) FileDataBytesInVector.size() << '\n';
        std::cout << "[FetchFileAsVector] capacity: " << (int) FileDataBytesInVector.capacity() << '\n';
        std::cout << "[FetchFileAsVector] max_size: " << (int) FileDataBytesInVector.max_size() << '\n';

        is.close();

        /// wait some time for file to close
        boost::posix_time::seconds workTime(3);
        boost::this_thread::sleep(workTime);

        bResult=true;
    }
    else
        std::cout << "[FetchFileAsVector] Reading file was not possible: " << strfilepath << "\n";


    return bResult;
}


/** \brief ftgt -- pronounsed FetchGet
 *
 * \param realm_name
 * \param index_name - name of index in realm, also this is a file, since all indexes in a realm each have their own file and toast file
 * \param record_value - returns all elements including elements lying in the TOAST area
 * \return true/false
 *
 */
bool CDatabaseControl::ftgt( std::string realm_name, std::string index_name, std::vector<Elements>&record_value )
{
    bool bResult=true;
    std::string EntityName      = realm_name;
    std::string EntityFileName  = index_name;

    boost::filesystem::path full_path( boost::filesystem::current_path() );
    //std::cout << "[ftgt] Error: Current path is : " << full_path << std::endl;

   /// Fetch all attributes from datadictionary file and create DED elements list
    std::vector<Elements> _DEDElements;
    bResult = readDDEntityRealm((std::string)EntityName, _DEDElements);
    if(bResult==false)
    {
        std::cout << "[ftgt] ERROR : File can not be read : Entity name : " << EntityName << std::endl;    /// no need to go further, something is wrong with the file
        return bResult;
    }
    else
    {
        /// now read the actual entity file and make sure it follows current datadictionary configuration
        bResult = ReadEntityFile((std::string)EntityName,(std::string)EntityFileName,record_value);
        if(bResult==false)
        {
            std::cout << "[ftgt] ERROR : reading file failed : Entity name : " << EntityName << " filename: " << EntityFileName << std::endl;    /// no need to go further, something is wrong with the file
            return bResult;
        }
        else
        {//TODO: Consider putting below fetch from TOAST into ReadEntityFile, since toast is a part of an entity it should reside there
            bResult=false;
            /// now fetch ALL elements with their attribute values  -- all incl. TOAST attributes
            /// this means that now we should fetch all attributes from the TOAST entity file
            std::string strChunkID = boost::to_lower_copy(EntityName) + "_chunk_id";
            std::string strchunkid_value = "";
            /// function finding the element in entity file
            bResult = find_element_value(record_value,strChunkID,strchunkid_value);
            if(bResult==true)
            {
                bResult=false;
                std::string strTOASTFilepath = full_path.string() + relativeENTITIES_DATABASE_TOAST_PLACE + strchunkid_value + ".xml"; // database file for entity
                /// now open its toast file and put all attributes and values on record_value
                if ( boost::filesystem::exists( strTOASTFilepath ) )
                {
                    std::ifstream istoast (strTOASTFilepath);
                    bResult = ReadTOASTXmlFile(istoast, record_value, EntityName);
                    if(bResult==false)
                    {
                        std::cout << "[ftgt] ERROR : File can not be read : file name : " << strTOASTFilepath << std::endl;    /// no need to go further, something is wrong with the file
                        return bResult;
                    }
                    /// now all elements from Entity should have been read, including the ones in TOAST (they have also been merged, so no chunks exists)
                }
                else
                {
                    /// warning there could not be found a TOAST file, this is not necessary an error, however strange
                    std::cout << "[ftgt] WARNING: there could not be found a TOAST file, this is not necessary an error, however strange : " << strTOASTFilepath << std::endl;
                    bResult=true;
                }
            }
        }
    }
    return bResult;
}

/** \brief put
 *
 * \param realm_name
 * \param index_name - name of index in realm, also this is a file, since all indexes in a realm each have their own file and toast file
 * \param record_value - all elements including elements lying in the TOAST area - will be put in file(s)
 * \return true/false
 *
 */
bool CDatabaseControl::put(std::string realm_name, std::string index_name, std::vector<Elements> record_value)
{
    bool bResult=false;
    bResult = WriteEntityFile((std::string)realm_name,(std::string)index_name,record_value);
    return bResult;
}

/** \brief append
 *
 * \param realm_name
 * \param index_name - name of index in realm, also this is a file, since all indexes in a realm each have their own file and toast file
 * \param record_value - all elements including elements lying in the TOAST area - will be put in file(s)
 * \return true/false
 *
 * Here is the actual data, stored in various xml files, structured according to DataDictionary description

Example of how database files are organised; NB! ALL values are stored in DED blocks !!!!
////////////////////////////////////////////
// DATABASE ENTITY FILE STRUCTURES - XML: //
////////////////////////////////////////////
PROFILE =
<Profile>
<version></version>
<ProfileRecord>
    <TransGUID></TransGUID> -- each record has a unique transaction id to be used with security, when detecting inconsistencies
    <Protocol>DED</Protocol>
    <ProtocolVersion>1.0.0.0</ProtocolVersion>
    <DataSize></DataSize>
    <Data></Data> -- Data = elements defined as DDEntries in DD_PROFILE.xml  -- will be put in a DED object structure
    <DataMD5></DataMD5> -- this md5 checksum is used as an extra verification -- Data must be verified with this value before being used.
</ProfileRecord>
...
</Profile>

 *
 *
 */
bool CDatabaseControl::append(std::string realm_name, std::string index_name, std::vector<Elements> record_value)
{
    bool bResult=false;
    bResult = AppendEntityFile((std::string)realm_name,(std::string)index_name,record_value);
    return bResult;
}

std::string CDatabaseControl::removeExtension(const std::string &filename)
{
    size_t lastdot = filename.find_last_of(".");
    if (lastdot == std::string::npos) return filename;
    return filename.substr(0, lastdot);
}

/** \brief list_realm_files
 *
 * \param realm_name - name of area where files are
 * \param realmFileNames - returns names of all realm files
 * \return true/false
 *
 */
//bool CDatabaseControl::list_realm_files( std::string realm_name, std::multimap<std::time_t, boost::filesystem::path> &result_set )
bool CDatabaseControl::list_realm_files( std::string realm_name, std::vector<std::string> &result_set )
{
    bool bResult=false;
    bool bReturnResult=false;

    boost::filesystem::path full_path( boost::filesystem::current_path() );

    namespace fs = boost::filesystem;
    std::string pathtosomewhere = full_path.string() + relativeENTITIES_DATABASE_PLACE;
    fs::path someDir(pathtosomewhere);
    fs::directory_iterator end_iter;

    typedef std::multimap<std::time_t, fs::path> result_set_t;
///    result_set_t result_set;

    if ( fs::exists(someDir) && fs::is_directory(someDir))
    {
      for( fs::directory_iterator dir_iter(someDir) ; dir_iter != end_iter ; ++dir_iter)
      {

        if (fs::is_regular_file(dir_iter->status()) )
        {
            /// check if file belongs to realm - this is done by looking up in its corresponding dictionary file

            // Fetch all attributes from datadictionary file and create DED elements list
            std::vector<Elements> _DEDElements;
            bResult = readDDEntityRealm((std::string)realm_name, _DEDElements);
            if(bResult==false)
            {
                //std::cout << "[list_realm_files] ERROR : File can not be read : DataDictionary Entity name : " << realm_name << std::endl;    /// no need to go further, something is wrong with the file
                //return bResult;
                /// Ignore file - move on to next
            }
            else
            {
                std::string filename = dir_iter->path().filename().string();
                std::string filenamepath = dir_iter->path().string();
                std::string EntityFileName = removeExtension(filename);
                //std::string strFilepath = full_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName; // database file for entity
                std::vector<Elements> record_value;

                /// now read the actual entity file and make sure it follows current datadictionary configuration
                bResult = ReadEntityFile((std::string)realm_name,(std::string)EntityFileName,record_value);
                if(bResult==false)
                {
                    //std::cout << "[list_realm_files] ERROR : File can not be read : Entity filename : " << EntityFileName << std::endl;    /// no need to go further, something is wrong with the file
                    //return bResult;
                    /// Ignore file - move on to next
                }
                else
                {
                    //result_set.insert(result_set_t::value_type(fs::last_write_time(dir_iter->path()), *dir_iter));
                    result_set.push_back(filenamepath);
                    bReturnResult = true;
                }
            }
        }
      }
    }

//    // showing contents:
//    std::cout << "mymultimap result_set contains:\n";
//    for (it=result_set.begin(); it!=result_set.end(); ++it)
//        std::cout << (*it).first << " => " << (*it).second << '\n';

    return bReturnResult;
}

/** \brief list_realm_entities
 *
 * \param realm_name - name of area where files are
 * \param realmFileNames - returns names of all realm entities
 * \return true/false
 *
 */
bool CDatabaseControl::list_realm_entities( std::string realm_name, std::vector<std::string> &result_set )
{
    bool bResult=false;
    bool bReturnResult=false;

    boost::filesystem::path full_path( boost::filesystem::current_path() );

    namespace fs = boost::filesystem;
    std::string pathtosomewhere = full_path.string() + relativeENTITIES_DATABASE_PLACE;
    fs::path someDir(pathtosomewhere);
    fs::directory_iterator end_iter;

    if ( fs::exists(someDir) && fs::is_directory(someDir))
    {
      for( fs::directory_iterator dir_iter(someDir) ; dir_iter != end_iter ; ++dir_iter)
      {

        if (fs::is_regular_file(dir_iter->status()) )
        {
            /// check if file belongs to realm - this is done by looking up in its corresponding dictionary file

            // Fetch all attributes from datadictionary file and create DED elements list
            std::vector<Elements> _DEDElements;
            bResult = readDDEntityRealm((std::string)realm_name, _DEDElements);
            if(bResult==false)
            {
                /// Ignore file - move on to next
            }
            else
            {
                std::string filename = dir_iter->path().filename().string();
                std::string filenamepath = dir_iter->path().string();
                std::string EntityName = removeExtension(filename);
                std::vector<Elements> record_value;

                /// now read the actual entity file and make sure it follows current datadictionary configuration
                bResult = ReadEntityFile((std::string)realm_name,(std::string)EntityName,record_value);
                if(bResult==false)
                {
                    /// Ignore file - move on to next
                }
                else
                {
                    result_set.push_back(EntityName);
                    bReturnResult = true;
                }
            }
        }
      }
    }

    return bReturnResult;
}

