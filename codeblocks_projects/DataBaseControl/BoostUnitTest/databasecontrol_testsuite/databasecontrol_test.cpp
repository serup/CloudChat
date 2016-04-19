#define BOOST_TEST_MODULE databasecontroltest
#include <boost/test/included/unit_test.hpp>
#include <boost/test/results_reporter.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <iostream>
#include <fstream>      // std::ifstream
#include "../../databasecontrol.hpp"
//#include "../../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h"
//#include "../../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h"
using namespace std;
using namespace boost::unit_test;

//////////////////////////////////////////
// TESTCASE
//////////////////////////////////////////
// howto compile :
// use makefile_cygwin.mak from inside cygwin terminal :
// use makefile.mak from linux prompt
// command line in linux:
// g++ -g -o bin/Debug/databasecontrol_test databasecontrol_test.cpp ../../databasecontrol.hpp  -L"/usr/local/lib/"  -D__MSABI_LONG=long -lboost_system -lboost_signals -lboost_thread -lpthread -lrt  -std=gnu++11
//
// ex.
// make -f makefile_cygwin.mak
// make -f makefile.mak
// NB! makefile will not only build and link, it will also run the testcase
//     and convert the generated result output into the file test_result.html
//////////////////////////////////////////
// how to test:
// make -f makefile_cygwin.mak test
// This will convert test_results.xml file to test_results.html, based on test_results.xslt file
//////////////////////////////////////////

#define BOOST_AUTO_TEST_MAIN
#ifndef NOTESTRESULTFILE
#ifdef BOOST_AUTO_TEST_MAIN
std::ofstream out;

struct ReportRedirector
{
    ReportRedirector()
    {
        out.open("test_results.xml");
        assert( out.is_open() );
        boost::unit_test::results_reporter::set_stream(out);
    }
};

BOOST_GLOBAL_FIXTURE(ReportRedirector)
#endif
#endif

/**
	 * C++ version 0.4 std::string style "itoa":
	 * Contributions from Stuart Lowe, Ray-Yuan Sheu,
	 * Rodrigo de Salvo Braz, Luc Gallant, John Maloney
	 * and Brian Hunt
	 *
	 * or use : http://stackoverflow.com/questions/4668760/converting-an-int-to-stdstring
	 * boost::lexical_cast<std::string>(yourint) from boost/lexical_cast.hpp
	 */
std::string itoa(int value, int base=10)
{
		std::string buf;

		// check that the base if valid
		if (base < 2 || base > 16) return buf;

		enum { kMaxDigits = 35 };
		buf.reserve( kMaxDigits ); // Pre-allocate enough space.
		int quotient = value;

		// Translating number to string with base:
		do {
			buf += "0123456789abcdef"[ std::abs( quotient % base ) ];
			quotient /= base;
		} while ( quotient );

		// Append the negative sign
		if ( value < 0) buf += '-';

		std::reverse( buf.begin(), buf.end() );
		return buf;
}

static bool bDirWasSet=false;
void SetDir()
{
    if(bDirWasSet==false) {
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::cout << "[validateReadProfileEntity] : Current path is : " << full_path << std::endl;
    chdir("../../../");
    boost::filesystem::path new_full_path( boost::filesystem::current_path() );
    std::cout << "[validateReadProfileEntity] : Current path is : " << new_full_path << std::endl;
    bDirWasSet = true;
    }
}

BOOST_AUTO_TEST_SUITE ( databasecontroltest ) // name of the test suite
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(databasecontrol_instantiated)
{
    CDatabaseControl *ptestDatabaseControl = new CDatabaseControl();
    BOOST_CHECK(ptestDatabaseControl != 0);
    delete ptestDatabaseControl;
}

BOOST_AUTO_TEST_CASE(datadictionary_profile)
{
    /**< Test if variables needed are present in data dictionary */
    /**< http://akrzemi1.wordpress.com/2011/07/13/parsing-xml-with-boost/ */
    std::string realmEntries[] = {
    "profileID",
    "profileName",
    "protocolTypeID",
    "sizeofProfileData",
    "profile_chunk_id",
    //"organizationID",
    "AccountStatus",
    "SubscriptionExpireDate",
    "ProfileStatus"};


    SetDir();
    CDatabaseControl *ptestDatabaseControl = new CDatabaseControl();
    BOOST_CHECK(ptestDatabaseControl != 0);

//    std::ifstream is ("/DataDictionary/Entities/DD_PROFILE.xml"); // POSIX equivalent path
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::string strFilepathEntity = full_path.string() + relative_DATADICTIONARY_ENTITIES_PLACE + (std::string) "PROFILE" + ".xml"; // Datadictionary file spec for entity
    if(!boost::filesystem::exists(strFilepathEntity))
    {
        std::cout << "[datadictionary_profile] Error: Can't find file : " << strFilepathEntity << std::endl;
    }
    else
    {
        std::ifstream is (strFilepathEntity); // POSIX equivalent path
        EntityRealm EntityRealmResult = ptestDatabaseControl->readDDEntityRealm(is,(std::string)"Profile");
        BOOST_CHECK(ptestDatabaseControl->CHECK_REALM(EntityRealmResult, realmEntries, (std::string)"Profile" )==true);
    }

    delete ptestDatabaseControl;
}

BOOST_AUTO_TEST_CASE(datadictionary_profile_toast)
{
    /**< Test if variables needed are present in data dictionary */
    /**< http://akrzemi1.wordpress.com/2011/07/13/parsing-xml-with-boost/ */
    std::string realmEntries[] = {"profile_chunk_id",
    "aiid",
    "profile_chunk_seq",
    "profile_chunk_data"};

    SetDir();
    CDatabaseControl *ptestDatabaseControl = new CDatabaseControl();
    BOOST_CHECK(ptestDatabaseControl != 0);

    //std::ifstream is ("/DataDictionary/Entities/DD_PROFILE_TOAST.xml"); // POSIX equivalent path
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::string strFilepathEntity = full_path.string() + relative_DATADICTIONARY_ENTITIES_PLACE + (std::string) "PROFILE_TOAST" + ".xml"; // Datadictionary file spec for entity
    if(!boost::filesystem::exists(strFilepathEntity))
    {
        std::cout << "[datadictionary_profile_toast] Error: Can't find file : " << strFilepathEntity << std::endl;
    }
    else
    {
        std::ifstream is (strFilepathEntity); // POSIX equivalent path
        EntityRealm EntityRealmResult = ptestDatabaseControl->readDDEntityRealm(is,(std::string)"Profile");
        BOOST_CHECK(ptestDatabaseControl->CHECK_REALM(EntityRealmResult, realmEntries, (std::string)"Profile" )==true);
    }
    delete ptestDatabaseControl;
}

BOOST_AUTO_TEST_CASE(datadictionary_profile_toast_attributes)
{
    /**< Test if variables needed are present in data dictionary */
    /**< http://akrzemi1.wordpress.com/2011/07/13/parsing-xml-with-boost/ */
    std::string realmEntries[] = {
        "lifecyclestate",
        "username",
        "password",
        "devicelist",
        "firstname",
        "lastname",
        "streetname",
        "streetno",
        "postalcode",
        "city",
        "state",
        "country",
        "email",
        "mobilephone",
        "title",
        "about",
        "foto",
        "type",
        "status",
        "expiredate",
        "creditor",
        "terms",
        "subscriptions",
        "paymentdetails",
        "notificationlevel",
        "bankaccount",
        "memberships",
        "friendships",
        "presence",
        "location",
        "context",
        "schedule",
        "deviceID",
        "experience",
        "skills",
        "messagesboard",
        "eventlog",
        "supervisor"};

    SetDir();
    CDatabaseControl *ptestDatabaseControl = new CDatabaseControl();
    BOOST_CHECK(ptestDatabaseControl != 0);

    //std::ifstream is ("/DataDictionary/Entities/DD_PROFILE_TOAST_ATTRIBUTES.xml"); // POSIX equivalent path
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::string strFilepathEntity = full_path.string() + relative_DATADICTIONARY_ENTITIES_PLACE + (std::string) "PROFILE_TOAST_ATTRIBUTES" + ".xml"; // Datadictionary file spec for entity
    if(!boost::filesystem::exists(strFilepathEntity))
    {
        std::cout << "[datadictionary_profile_toast] Error: Can't find file : " << strFilepathEntity << std::endl;
    }
    else
    {
        std::ifstream is (strFilepathEntity); // POSIX equivalent path
        EntityRealm EntityRealmResult = ptestDatabaseControl->readDDEntityRealm(is,(std::string)"Profile");
        BOOST_CHECK(ptestDatabaseControl->CHECK_REALM(EntityRealmResult, realmEntries, (std::string)"Profile" )==true);
    }
    delete ptestDatabaseControl;
}

BOOST_AUTO_TEST_CASE(validateReadProfileEntity)
{
    bool bResult=false;
    std::string strProfileID                =(std::string)"TestProfile1";
    std::string strProfileName              =(std::string)"TestName";
    std::string strprotocolTypeID           =(std::string)"DED1.00.00";
    std::string strsizeofProfileData        =(std::string)"0";
    std::string strProfile_chunk_id         =(std::string)"0";
  //  std::string strorganizationID           =(std::string)"888";
    std::string strAccountStatus            =(std::string)"2";
    std::string strSubscriptionExpireDate   =(std::string)"20160101";
    std::string strProfileStatus            =(std::string)"2";

//    std::string strFilepath = (std::string)"../../../DataDictionary/Database/ENTITIEs/ProfileTest.xml";

    SetDir();

    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::string strFilepath = full_path.string() + relativeENTITIES_DATABASE_PLACE + (std::string)"ProfileTest.xml";


     /**< The Data stored in ProfileRecord data area must follow XML structure defined in datadictionary for Profile */
    DED_START_ENCODER(encoder_ptr);
    DED_PUT_STRUCT_START( encoder_ptr, "record" );
        DED_PUT_STDSTRING	( encoder_ptr, "profileID", strProfileID );
        DED_PUT_STDSTRING	( encoder_ptr, "profileName", strProfileName );
        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", strprotocolTypeID );
        DED_PUT_STDSTRING	( encoder_ptr, "sizeofProfileData", strsizeofProfileData );
        DED_PUT_STDSTRING	( encoder_ptr, "profile_chunk_id", strProfile_chunk_id );
  //      DED_PUT_STDSTRING	( encoder_ptr, "organizationID", strorganizationID );
        DED_PUT_STDSTRING	( encoder_ptr, "AccountStatus", strAccountStatus );
        DED_PUT_STDSTRING	( encoder_ptr, "SubscriptionExpireDate", strSubscriptionExpireDate );
        DED_PUT_STDSTRING	( encoder_ptr, "ProfileStatus", strProfileStatus );
    DED_PUT_STRUCT_END( encoder_ptr, "record" );
    DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);

/*  validate that the record file for Profile is following this structure
<Database>
<ProfileRecord>
    <TransGUID></TransGUID> -- each record has a unique transaction id to be used with security, when detecting inconsistencies
    <Protocol>DED</Protocol>
    <ProtocolVersion>1.0.0.0</ProtocolVersion>
    <DataSize></DataSize> -- here the size of the HEX string which is the data area ( converted into a string of hex values )
    <Data></Data> -- here is the pCompressedData from above protocol encoding, compression, encrypting algoritm -- currently DED (dataencoderdecoder)
    <DataMD5><DataMD5> -- this md5 checksum is used as an extra verification -- Data must be verified with this value before being used.
</ProfileRecord>
</Database>

*/

    // First create a file
    {
    CDatabaseControl CDbCtrl;
    std::ofstream os (strFilepath);
    bResult = CDbCtrl.WriteXmlFile(os, pCompressedData, sizeofCompressedData, (std::string) "Profile");
    // check that file is written
    BOOST_CHECK(bResult == true);
    }

    // Read entity file using general read for database entity files
    std::vector<Elements> DEDElements;
    {
    CDatabaseControl CDbCtrl;
    bResult = CDbCtrl.ReadEntityFile((std::string)"Profile",(std::string)"ProfileTest",DEDElements);
    BOOST_CHECK(bResult == true);
    }

    bResult = true;
    // Validate fetched data with above written
    std::string strAttr[] = {"profileID",
    "profileName",
    "protocolTypeID",
    "sizeofProfileData",
    "profile_chunk_id",
  //  "organizationID",
    "AccountStatus",
    "SubscriptionExpireDate",
    "ProfileStatus"};

    std::vector<std::string> strVecAttrValue = {strProfileID,
                                  strProfileName,
                                  strprotocolTypeID,
                                  strsizeofProfileData,
                                  strProfile_chunk_id,
                                  //strorganizationID,
                                  strAccountStatus,
                                  strSubscriptionExpireDate,
                                  strProfileStatus};
    int n=0;
    unsigned int nn = strVecAttrValue.size();
    int nTrue=0;
    int nFalse=0;
    BOOST_FOREACH( Elements f, DEDElements )
    {
        // iterate thru fetched data and validate against above written data
        if( f.strElementID == (std::string)strAttr[n] )
        {
            std::string strValue(f.ElementData.begin(),f.ElementData.end());
             if( strValue != strVecAttrValue[n] )
                nFalse++;
             else
                nTrue++;
        }
        n++;
        if(n>=nn)
            break;
    }

    if(nFalse>0) bResult=false;
    if(nTrue==0) bResult=false;

     /// cleanup after this test
    boost::filesystem::remove( strFilepath );
    BOOST_CHECK(bResult == true);
}

BOOST_AUTO_TEST_CASE(validateWriteProfileEntity)
{
    std::vector<Elements> DEDElements;
    std::string EntityName = "Profile";
    std::string EntityFileName = "ProfileTest";

    SetDir();
    CDatabaseControl CDbCtrl;
    CDbCtrl.readDDEntityRealm((std::string)EntityName, DEDElements); /// Fetch current datadictionary specs for this entity

    ///put values to elements
    std::string strAttr[] = {"profileID",
    "profileName",
    "protocolTypeID",
    "sizeofProfileData",
    "profile_chunk_id"};
  //  "organizationID"};
    std::vector<std::string> strVecAttrValue = {"TestProfile1",
                                  "TestName",
                                  "DED1.00.00",
                                  "0",
                                  "0"};
                                  //"888"};

// this is how DED will be build inside WriteEntityFile
//    DED_START_ENCODER(encoder_ptr);
//    DED_PUT_STRUCT_START( encoder_ptr, "record" );
//        DED_PUT_STDSTRING	( encoder_ptr, "ProfileID", "TestProfile1" );
//        DED_PUT_STDSTRING	( encoder_ptr, "ProfileName", "TestName" );
//        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", "DED1.00.00" );
//        DED_PUT_STDSTRING	( encoder_ptr, "sizeofProfileData", "0" );
//        DED_PUT_STDSTRING	( encoder_ptr, "Profile_chunk_id", "0" );
//        DED_PUT_STDSTRING	( encoder_ptr, "organizationID", "888" );
//    DED_PUT_STRUCT_END( encoder_ptr, "record" );

    int n=0;
    unsigned int nn = strVecAttrValue.size();
    while(DEDElements.size() > nn)
    {
        DEDElements.pop_back(); /// remove elements that are not being updated
    }
    BOOST_FOREACH( Elements f, DEDElements )
    {
        // iterate thru elements and add value to current specs of above given entity
        if( f.strElementID == (std::string)strAttr[n] )
        {
            std::copy(strVecAttrValue[n].begin(), strVecAttrValue[n].end(), std::back_inserter(DEDElements[n].ElementData));
        }
        n++;
        if(n>=nn)
            break;
    }

 /*   std::find_if(DEDElements.begin(), DEDElements.end(), [&](Elements & o)
    {
        bool bResult=false;
        if(o.strElementID == (std::string)"profileID")
        {
            /// put value in _DEDElements which is current datadictionary entity structure
            o.ElementData.clear();
            std::string tmp = "TestProfile1";
            std::copy(tmp.begin(), tmp.end(), std::back_inserter(o.ElementData));
            bResult=true;
        }
        return bResult;
    });
*/

    /// Write values in entity file
    bool bResult = CDbCtrl.WriteEntityFile((std::string)EntityName,(std::string)EntityFileName,DEDElements);
    BOOST_CHECK(bResult == true);

    /// Validate that file was written with the correct values
    // Read entity file using general read for database entity files
    std::vector<Elements> wDEDElements;
    bResult = CDbCtrl.ReadEntityFile((std::string)EntityName,(std::string)EntityFileName,wDEDElements);
    BOOST_CHECK(bResult == true);

    n=0;
    int nTrue=0;
    int nFalse=0;
    BOOST_FOREACH( Elements f, wDEDElements )
    {
        // iterate thru fetched data and validate against above written data
        if( f.strElementID == (std::string)strAttr[n] )
        {
            std::string strValue(f.ElementData.begin(),f.ElementData.end());
             if( strValue != strVecAttrValue[n] )
                nFalse++;
             else
                nTrue++;
        }
        n++;
        if(n>=nn)
            break;
    }

    if(nFalse>0) bResult=false;
    if(nTrue==0) bResult=false;
    BOOST_CHECK(bResult == true);

    /// cleanup after this test
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::string strFilepath = full_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
    boost::filesystem::remove( strFilepath );
}

BOOST_AUTO_TEST_CASE(validateUpdateProfileEntity)
{
    std::vector<Elements> DEDElements;
    std::string EntityName = "Profile";
    std::string EntityFileName = "ProfileTest";

    SetDir();
    CDatabaseControl CDbCtrl;
    CDbCtrl.readDDEntityRealm((std::string)EntityName, DEDElements); /// Fetch current datadictionary specs for this entity

    ///TODO: put values to elements
    std::string strAttr[] = {"profileID",
    "profileName",
    "protocolTypeID",
    "sizeofProfileData",
    "profile_chunk_id",
    //"organizationID",
    "AccountStatus",
    "SubscriptionExpireDate",
    "ProfileStatus"};
    std::vector<std::string> strVecAttrValue = {"TestProfile1",
                                  "TestName",
                                  "DED1.00.00",
                                  "0",
                                  "0",
                                  //"888",
                                  "0",
    				              "20160101",
    				              "2"};

    int n=0;
    unsigned int nn = strVecAttrValue.size();
    while(DEDElements.size() > nn)
    {
        DEDElements.pop_back(); /// remove elements that are not being updated
    }
    BOOST_FOREACH( Elements f, DEDElements )
    {
        // iterate thru elements and add value to current specs of above given entity
        if( f.strElementID == (std::string)strAttr[n] )
        {
            std::copy(strVecAttrValue[n].begin(), strVecAttrValue[n].end(), std::back_inserter(DEDElements[n].ElementData));
        }
        n++;
        if(n>=nn)
            break;
    }

    /// Write values in entity file
    bool bResult = CDbCtrl.WriteEntityFile((std::string)EntityName,(std::string)EntityFileName,DEDElements);
    BOOST_CHECK(bResult == true);

    /// Validate that file was written with the correct values
    // Read entity file using general read for database entity files
    std::vector<Elements> wDEDElements;
    bResult = CDbCtrl.ReadEntityFile((std::string)EntityName,(std::string)EntityFileName,wDEDElements);
    BOOST_CHECK(bResult == true);

    n=0;
    int nTrue=0;
    int nFalse=0;
    BOOST_FOREACH( Elements f, wDEDElements )
    {
        // iterate thru fetched data and validate against above written data
        if( f.strElementID == (std::string)strAttr[n] )
        {
            std::string strValue(f.ElementData.begin(),f.ElementData.end());
             if( strValue != strVecAttrValue[n] )
                nFalse++;
             else
                nTrue++;
        }
        n++;
    }

    if(nFalse>0) bResult=false;
    if(nTrue==0) bResult=false;
    BOOST_CHECK(bResult == true);

    /// update entity attribut in entity file
    std::vector<unsigned char> newElementData;
    std::string strElementID = "profileID";
    std::string newValue = "TestProfile2"; // old value was TestProfile1
    std::copy(newValue.begin(), newValue.end(), std::back_inserter(newElementData));
    CDbCtrl.updateEntityAttribut(wDEDElements, (std::string) strElementID, newElementData );

    bResult=false;
//    /// lambda function finding the element
//    std::find_if(wDEDElements.begin(), wDEDElements.end(), [&](Elements & o)
//    {
//        if(o.strElementID == (std::string)strElementID)
//        {
//            if(o.ElementData == newElementData)
//            {
//                bResult=true;
//                return bResult;
//            }
//        }
//    });
    bResult = CDbCtrl.compare_element(wDEDElements, strElementID, newElementData);
    BOOST_CHECK(bResult == true);

    /// cleanup after this test
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::string strFilepath = full_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
    boost::filesystem::remove( strFilepath );
}

BOOST_AUTO_TEST_CASE(updateEntityAttribut)
{
    bool bResult=false;
    std::vector<Elements> DEDElements;
    std::string EntityName = "Profile";
    std::string EntityFileName = "ProfileTest";
    std::string strElementID = "profileID";
    std::vector<unsigned char> newElementData;
    std::string newValue = "TestProfile2";
    std::copy(newValue.begin(), newValue.end(), std::back_inserter(newElementData));

    SetDir();
    CDatabaseControl CDbCtrl;
    CDbCtrl.readDDEntityRealm((std::string)EntityName, DEDElements); /// Fetch current datadictionary specs for this entity

    /// put values to elements
    std::string strAttr[] = {"profileID",
    "profileName",
    "protocolTypeID",
    "sizeofProfileData",
    "profile_chunk_id",
    //"organizationID",
    "AccountStatus",
    "SubscriptionExpireDate",
    "ProfileStatus"};
    std::vector<std::string> strVecAttrValue = {"TestProfile1",
                                  "TestName",
                                  "DED1.00.00",
                                  "0",
                                  "0",
                                  //"888",
                                  "0",
                                  "20160101",
                                  "2"};

    int n=0;
    unsigned int nn = strVecAttrValue.size();
    while(DEDElements.size() > nn)
    {
        DEDElements.pop_back(); /// remove elements that are not being updated
    }
    BOOST_FOREACH( Elements f, DEDElements )
    {
        // iterate thru elements and add value to current specs of above given entity
        if( f.strElementID == (std::string)strAttr[n] )
        {
            std::copy(strVecAttrValue[n].begin(), strVecAttrValue[n].end(), std::back_inserter(DEDElements[n].ElementData));
        }
        n++;
        if(n>=nn)
            break;
    }

    /// Write pre - values in entity file
    bResult = CDbCtrl.WriteEntityFile((std::string)EntityName,(std::string)EntityFileName,DEDElements);
    BOOST_CHECK(bResult == true);

    /// Update value in above written file
    bResult = CDbCtrl.updateEntityAttribut(EntityName, EntityFileName, strElementID, newElementData );
    BOOST_CHECK(bResult == true);

    /// Validate that file was written with the correct values
    // Read entity file using general read for database entity files
    std::vector<Elements> wDEDElements;
    bResult = CDbCtrl.ReadEntityFile((std::string)EntityName,(std::string)EntityFileName,wDEDElements);
    BOOST_CHECK(bResult == true);
    while(wDEDElements.size() > nn)
    {
        wDEDElements.pop_back(); /// remove elements that are not being updated
    }

    n=0;
    int nTrue=0;
    int nFalse=0;
    BOOST_FOREACH( Elements f, wDEDElements )
    {
        // iterate thru fetched data and validate against above written data
        if( f.strElementID == (std::string)strAttr[n] )
        {
            std::string strValue(f.ElementData.begin(),f.ElementData.end());
             if( strValue != strVecAttrValue[n] )
             {
                /// check if value is the changed value, if so then this is not false :-)
                if(strValue == newValue && f.strElementID == strElementID)
                {
                    nTrue++;
                }
                else
                    nFalse++;
             }
             else
                nTrue++;
        }
        n++;
    }

    if(nFalse>0) bResult=false;
    if(nTrue==0) bResult=false;
    BOOST_CHECK(bResult == true);

    /// cleanup after this test
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::string strFilepath = full_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
    boost::filesystem::remove( strFilepath );
}

BOOST_AUTO_TEST_CASE(no_toast_ftgt)
{
    bool bResult=false;
    std::vector<Elements> DEDElements;
    std::string EntityName = "Profile";
    std::string EntityFileName = "ProfileTest";
    std::string strElementID = "profileID";
    std::vector<unsigned char> newElementData;
    std::string newValue = "TestProfile2";
    std::copy(newValue.begin(), newValue.end(), std::back_inserter(newElementData));

    SetDir();
    CDatabaseControl CDbCtrl;
    CDbCtrl.readDDEntityRealm((std::string)EntityName, DEDElements); /// Fetch current datadictionary specs for this entity

    /// put values to elements
    std::string strAttr[] = {"profileID",
    "profileName",
    "protocolTypeID",
    "sizeofProfileData",
    "profile_chunk_id",
    //"organizationID",
    "AccountStatus",
    "SubscriptionExpireDate",
    "ProfileStatus"};
    std::vector<std::string> strVecAttrValue = {"TestProfile1",
                                  "TestName",
                                  "DED1.00.00",
                                  "0",
                                  "0",
                                  //"888",
                                  "0",
                                  "20160101",
                                  "2"};



    int n=0;
    unsigned int nn = strVecAttrValue.size();
    while(DEDElements.size() > nn)
    {
        DEDElements.pop_back(); /// remove elements that are not being updated
    }
    BOOST_FOREACH( Elements f, DEDElements )
    {
        // iterate thru elements and add value to current specs of above given entity
        if( f.strElementID == (std::string)strAttr[n] )
        {
            std::copy(strVecAttrValue[n].begin(), strVecAttrValue[n].end(), std::back_inserter(DEDElements[n].ElementData));
        }
        n++;
        if(n>=nn)
            break;
    }


    /// Write pre - values in entity file
    bResult = CDbCtrl.WriteEntityFile((std::string)EntityName,(std::string)EntityFileName,DEDElements);
    BOOST_CHECK(bResult == true);

    /// use ftgt to fetch the entity file
    std::vector<Elements> record_value;
    bResult = CDbCtrl.ftgt( (std::string)EntityName, (std::string)EntityFileName, record_value );
    BOOST_CHECK(bResult == true);

    /// cleanup after this test
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::string strFilepath = full_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
    boost::filesystem::remove( strFilepath );
}

BOOST_AUTO_TEST_CASE(with_empty_toast_ftgt)
{
    bool bResult=false;
    std::vector<Elements> DEDElements;
    std::string EntityName = "Profile";
    std::string EntityFileName = "ProfileTest";
    std::string strElementID = "profileID";
    std::vector<unsigned char> newElementData;
    std::string newValue = "TestProfile2";
    std::copy(newValue.begin(), newValue.end(), std::back_inserter(newElementData));

    SetDir();
    CDatabaseControl CDbCtrl;
    CDbCtrl.readDDEntityRealm((std::string)EntityName, DEDElements); /// Fetch current datadictionary specs for this entity

    /// put values to elements
    std::string strAttr[] = {"profileID",
    "profileName",
    "protocolTypeID",
    "sizeofProfileData",
    "profile_chunk_id",
    //"organizationID",
    "AccountStatus",
    "SubscriptionExpireDate",
    "ProfileStatus"};
    std::vector<std::string> strVecAttrValue = {"TestProfile1",
                                  "TestName",
                                  "DED1.00.00",
                                  "0",
                                  "22980574", // file does NOT exists, so method will consider it empty, could be considered an error, however it really is more a warning
                                  //"888",
                                  "0",
                                  "20160101",
                                  "2"};

    int n=0;
    unsigned int nn = strVecAttrValue.size();
    while(DEDElements.size() > nn)
    {
        DEDElements.pop_back(); /// remove elements that are not being updated
    }
    BOOST_FOREACH( Elements f, DEDElements )
    {
        // iterate thru elements and add value to current specs of above given entity
        if( f.strElementID == (std::string)strAttr[n] )
        {
            std::copy(strVecAttrValue[n].begin(), strVecAttrValue[n].end(), std::back_inserter(DEDElements[n].ElementData));
        }
        n++;
        if(n>=nn)
            break;
    }

    /// Write pre - values in entity file
    bResult = CDbCtrl.WriteEntityFile((std::string)EntityName,(std::string)EntityFileName,DEDElements);
    BOOST_CHECK(bResult == true);

    /// use ftgt to fetch the entity file
    std::vector<Elements> record_value;
    bResult = CDbCtrl.ftgt( (std::string)EntityName, (std::string)EntityFileName, record_value );
    BOOST_CHECK(bResult == true);

    /// cleanup after this test
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::string strFilepath = full_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
    std::string strFilepathToast = full_path.string() + relativeENTITIES_DATABASE_TOAST_PLACE + (std::string)"22980574" + ".xml"; // database file for entity TOAST
    boost::filesystem::remove( strFilepath );
    boost::filesystem::remove( strFilepathToast );
}

BOOST_AUTO_TEST_CASE(validateWriteEntityFile_and_ftgt_with_toast)
{
    bool bResult=false;
    std::vector<Elements> DEDElements;
    std::string EntityName = "Profile";
    std::string EntityFileName = "ProfileTest";
    std::string strElementID = "profileID";
    std::vector<unsigned char> newElementData;
    std::string newValue = "TestProfile2";
    std::copy(newValue.begin(), newValue.end(), std::back_inserter(newElementData));

    SetDir();
    CDatabaseControl CDbCtrl;
    CDbCtrl.readDDEntityRealm((std::string)EntityName, DEDElements); /// Fetch current datadictionary specs for this entity
    CDbCtrl.readDDTOASTEntityRealm((std::string)EntityName, DEDElements); /// Fetch current datadictionary specs for this entity TOAST and add it to list

    /// put values to elements -- NB this test is not taking into account that attributes could be moved around - it assumes that username is first in toast file attributes
    std::string strAttr[] = {"profileID",
    "profileName",
    "protocolTypeID",
    "sizeofProfileData",
    "profile_chunk_id",
    //"organizationID",
    "AccountStatus",
    "SubscriptionExpireDate",
    "ProfileStatus",
    "lifecyclestate",
    "username"};  /// last element belongs in TOAST area ; WriteEntityFile - should find out by itself and update value inside TOAST file
    std::vector<std::string> strVecAttrValue = {"TestProfile1",
                                  "TestName",
                                  "DED1.00.00",
                                  "0",
                                  "22980574", // file does NOT exists, so method will consider it empty, could be considered an error, however it really is more a warning
                                 // "888",
                                  "0",
                                  "20160101",
                                  "2",
                                  "0", // lifecycle
                                  "SERUP"};

    unsigned int n=0;
    unsigned int nn = strVecAttrValue.size();
    while(DEDElements.size() > nn)
    {
        DEDElements.pop_back(); /// remove elements that are not being updated
    }
    BOOST_FOREACH( Elements f, DEDElements )
    {
        // iterate thru elements and add value to current specs of above given entity
        if( f.strElementID == (std::string)strAttr[n] )
        {
            std::copy(strVecAttrValue[n].begin(), strVecAttrValue[n].end(), std::back_inserter(DEDElements[n].ElementData));
        }
        n++;
        if(n>=nn)
            break;
    }

    /// Write pre - values in entity file
    bResult = CDbCtrl.WriteEntityFile((std::string)EntityName,(std::string)EntityFileName,DEDElements);
    BOOST_CHECK(bResult == true);

    std::vector<Elements> record_value;
    if(bResult==true)
    {
        /// use ftgt to fetch the entity file
        bResult = CDbCtrl.ftgt( (std::string)EntityName, (std::string)EntityFileName, record_value );
        BOOST_CHECK(bResult == true);
    }


    /// validate data read
    bResult=true;
    n=0;
    /// ftgt should have read all attributes into record_value, now compare with DEDElements, which is elements written
    BOOST_FOREACH( Elements v, record_value )
    {
        // iterate thru elements and compare
        if(v.strElementID != (std::string)DEDElements[n].strElementID) bResult=false;
        if(bResult == false)
        {
            std::cout << "v.strElementID [ " << v.strElementID << (std::string)" ] != DEDElements[n].strElementID [ " << DEDElements[n].strElementID << " ] \n";
        }
        else
            bResult=true;

        if(v.ElementData != DEDElements[n].ElementData) bResult=false;
        if(bResult == false)
        {
            std::cout << "v.ElementDataElementData [ "  << (std::string)" ] != DEDElements[n].ElementData [ "  << " ] \n";
        }
        else
            bResult=true;

        n++;
        if(n>=DEDElements.size())
            break;
    }

    BOOST_CHECK(bResult == true);

    /// cleanup after this test
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::string strFilepath = full_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
    std::string strFilepathToast = full_path.string() + relativeENTITIES_DATABASE_TOAST_PLACE + (std::string)"22980574" + ".xml"; // database file for entity TOAST
    boost::filesystem::remove( strFilepath );
    boost::filesystem::remove( strFilepathToast );
}

BOOST_AUTO_TEST_CASE(validate_put_and_ftgt_with_toast)
{
    bool bResult=false;
    std::vector<Elements> DEDElements;
    std::string EntityName = "Profile";
    std::string EntityFileName = "ProfileTest";
    std::string strElementID = "profileID";
    std::vector<unsigned char> newElementData;
    std::string newValue = "TestProfile2";
    std::copy(newValue.begin(), newValue.end(), std::back_inserter(newElementData));

    SetDir();
    CDatabaseControl CDbCtrl;
    CDbCtrl.readDDEntityRealm((std::string)EntityName, DEDElements); /// Fetch current datadictionary specs for this entity
    CDbCtrl.readDDTOASTEntityRealm((std::string)EntityName, DEDElements); /// Fetch current datadictionary specs for this entity TOAST and add it to list

    /// put values to elements -- NB this test is not taking into account that attributes could be moved around - it assumes that username is first in toast file attributes
    std::string strAttr[] = {"profileID",
    "profileName",
    "protocolTypeID",
    "sizeofProfileData",
    "profile_chunk_id",
    //"organizationID",
    "AccountStatus",
    "SubscriptionExpireDate",
    "ProfileStatus",
    "lifecyclestate",
    "username",
    "password"};  /// last element belongs in TOAST area ; WriteEntityFile - should find out by itself and update value inside TOAST file
    std::vector<std::string> strVecAttrValue = {"MyProfile",
                                  "MyName",
                                  "DED1.00.00",
                                  "0",
                                  "22980574", // file does NOT exists, so method will consider it empty, could be considered an error, however it really is more a warning
                                  //"888",
                                  "0",
                                  "20160101",
                                  "2",
                                  "0",
                                  "SERUP",
                                  "secret101"};

    unsigned int n=0;
    unsigned int nn = strVecAttrValue.size();
    while(DEDElements.size() > nn)
    {
        DEDElements.pop_back(); /// remove elements that are not being updated
    }
    BOOST_FOREACH( Elements f, DEDElements )
    {
        // iterate thru elements and add value to current specs of above given entity
        if( f.strElementID == (std::string)strAttr[n] )
        {
            std::copy(strVecAttrValue[n].begin(), strVecAttrValue[n].end(), std::back_inserter(DEDElements[n].ElementData));
        }
        n++;
        if(n>=nn)
            break;
    }

    /// Write pre - values in entity file
    bResult = CDbCtrl.put((std::string)EntityName,(std::string)EntityFileName,DEDElements);
    BOOST_CHECK(bResult == true);

    std::vector<Elements> record_value;
    if(bResult==true)
    {
        /// use ftgt to fetch the entity file
        bResult = CDbCtrl.ftgt( (std::string)EntityName, (std::string)EntityFileName, record_value );
        BOOST_CHECK(bResult == true);
    }


    /// validate data read
    bResult=true;
    n=0;
    /// ftgt should have read all attributes into record_value, now compare with DEDElements, which is elements written
    BOOST_FOREACH( Elements v, record_value )
    {
        // iterate thru elements and compare
        if(v.strElementID != (std::string)DEDElements[n].strElementID) bResult=false;
        if(bResult == false)
        {
            std::cout << "v.strElementID [ " << v.strElementID << (std::string)" ] != DEDElements[n].strElementID [ " << DEDElements[n].strElementID << " ] \n";
        }
        else
            bResult=true;

        if(v.ElementData != DEDElements[n].ElementData) bResult=false;
        if(bResult == false)
        {
            std::cout << "v.ElementDataElementData [ "  << (std::string)" ] != DEDElements[n].ElementData [ "  << " ] \n";
        }
        else
            bResult=true;

        n++;
        if(n>=DEDElements.size())
            break;
    }

    BOOST_CHECK(bResult == true);

    /// cleanup after this test
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::string strFilepath = full_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
    std::string strFilepathToast = full_path.string() + relativeENTITIES_DATABASE_TOAST_PLACE + (std::string)"22980574" + ".xml"; // database file for entity TOAST
    boost::filesystem::remove( strFilepath );
    boost::filesystem::remove( strFilepathToast );

}


BOOST_AUTO_TEST_CASE(validate_put_and_ftgt_with_imagefile_in_toast)
{
    bool bResult=false;
    std::vector<Elements> DEDElements;
    std::string EntityName = "Profile";
    std::string EntityFileName = "ProfileTest";

    SetDir();
    CDatabaseControl CDbCtrl;
    CDbCtrl.readDDEntityRealm((std::string)EntityName, DEDElements); /// Fetch current datadictionary specs for this entity
    CDbCtrl.readDDTOASTEntityRealm((std::string)EntityName, DEDElements); /// Fetch current datadictionary specs for this entity TOAST and add it to list

    /// Helpful script to cut attributes out of DD files
    /// $cat DD_PROFILE_TOAST_ATTRIBUTES.xml | grep -e '<PhysicalDataElementName>' | sed -e 's/<PhysicalDataElementName>/\"/' | sed -e 's/<\/PhysicalDataElementName>/\",/'
    ///
    /// put values to elements -- NB this test is not taking into account that attributes could be moved around - it assumes that lifecyclestate is first in toast file attributes
    std::string strAttr[] = {"profileID",
    "profileName",
    "protocolTypeID",
    "sizeofProfileData",
    "profile_chunk_id",
    "AccountStatus",
    "SubscriptionExpireDate",
    "ProfileStatus",
    "lifecyclestate", /// From here attributes lie in toast area ; WriteEntityFile - should find out by itself and update value inside TOAST file
        "username",
        "password",
        "devicelist",
        "firstname",
        "lastname",
        "streetname",
        "streetno",
        "postalcode",
        "city",
        "state",
        "country",
        "email",
        "mobilephone",
        "title",
        "about",
        "foto", /// test puts image in this attribute
/*        "type",
        "status",
        "expiredate",
        "creditor",
        "terms",
        "subscriptions",
        "paymentdetails",
        "notificationlevel",
        "bankaccount",
        "memberships",
        "friendships",
        "presence",
        "location",
        "context",
        "schedule",
        "deviceID",
        "experience",
        "skills",
        "messagesboard",
        "eventlog",
        "supervisor" */
         };
    std::vector<std::string> strVecAttrValue = {"MyProfile",
                                  "MyName",
                                  "DED1.00.00",
                                  "0",
                                  "22980574", // file does NOT exists, so method will consider it empty, could be considered an error, however it really is more a warning
				                  "0",
                                  "20160101",
                                  "2",
                                  "0", /// lifecyglestate
                                  "SERUP", /// username
                                  "secret101", /// password
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<foto>"}; /// foto ; replace <foto> with foto

    unsigned int n=0;
    unsigned int nn = strVecAttrValue.size();
    while(DEDElements.size() > nn)
    {
        DEDElements.pop_back(); /// remove elements that are not being updated
    }
    BOOST_FOREACH( Elements f, DEDElements )
    {
        // iterate thru elements and add value to current specs of above given entity
        if( f.strElementID == (std::string)strAttr[n] )
        {
            std::copy(strVecAttrValue[n].begin(), strVecAttrValue[n].end(), std::back_inserter(DEDElements[n].ElementData));
        }
        n++;
        if(n>=nn)
            break;
    }
/*
//
//    /// insert an image in foto placeholder
//    std::vector<unsigned char> fotoElementData;
//    std::string strPicturefilepath = "Hugo1.gif";
//    bResult = CDbCtrl.FetchFileAsVector(strPicturefilepath,fotoElementData);
//    BOOST_CHECK(bResult == true);
//    if(bResult==true)
//    {
//        bResult = false;
//        bResult = CDbCtrl.update_element_value(DEDElements, (std::string) "foto", fotoElementData);
//        BOOST_CHECK(bResult == true);
//
//
//        ///+tst - working
//        std::ofstream outfile ("out.gif", std::ios::out | std::ofstream::binary);
//        std::ofstream outfile2 ("out2.gif", std::ios::out | std::ofstream::binary);
//        const std::vector<unsigned char> iterator_data_in_image_buf(&fotoElementData[0],&fotoElementData[fotoElementData.size()]);
//        // write outfile
//        std::copy(iterator_data_in_image_buf.begin(), iterator_data_in_image_buf.end(), std::ostream_iterator<unsigned char>(outfile));
//        outfile.close();
//        // write outfile2
//        outfile2.write((char*)&fotoElementData[0],fotoElementData.size());
//        outfile2.close();
//
//        std::ofstream outfile3 ("out3.gif", std::ios::out | std::ofstream::binary);
//        // write outfile3
//        std::copy(fotoElementData.begin(), fotoElementData.end(), std::ostream_iterator<unsigned char>(outfile3));
//        outfile3.close();
//        ///-
//
//
//    }
*/
    /// insert an image in foto placeholder
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    bResult = CDbCtrl.update_element_value_with_file(DEDElements, (std::string) "foto", (std::string) full_path.string() + (std::string) "/DataBaseControl/BoostUnitTest/databasecontrol_testsuite/Hugo1.gif");
    BOOST_CHECK(bResult == true);


    /// Write pre - values in entity file
    bResult=false;
    bResult = CDbCtrl.put((std::string)EntityName,(std::string)EntityFileName,DEDElements);
    BOOST_CHECK(bResult == true);

    std::vector<Elements> record_value;
    if(bResult==true)
    {
        /// use ftgt to fetch the entity file
        bResult = CDbCtrl.ftgt( (std::string)EntityName, (std::string)EntityFileName, record_value );
        BOOST_CHECK(bResult == true);
    }


    /// validate data read
    bResult=true;
    n=0;
    /// ftgt should have read all attributes into record_value, now compare with DEDElements, which is elements written
    BOOST_FOREACH( Elements v, record_value )
    {
        // iterate thru elements and compare
        if(v.strElementID != (std::string)DEDElements[n].strElementID) bResult=false;
        if(bResult == false)
        {
            std::cout << "v.strElementID [ " << v.strElementID << (std::string)" ] != DEDElements[n].strElementID [ " << DEDElements[n].strElementID << " ] \n";
        }


        if(v.ElementData != DEDElements[n].ElementData) bResult=false;
        if(bResult == false)
        {
            std::cout << "v.ElementDataElementData [ "  << (std::string)" ] != DEDElements[n].ElementData [ "  << " ] \n";
        }

//        ///+tst - working
//        if(v.strElementID == "foto" && bResult != false)
//        {///should write a valid .gif file same as "hugo1.gif""
//            std::ofstream outfile4 ("Hugo_out_from_within_xml.gif", std::ios::out | std::ofstream::binary);
//            // write outfile4
//            std::copy(v.ElementData.begin(), v.ElementData.end(), std::ostream_iterator<unsigned char>(outfile4));
//            outfile4.close();
//        }
//        ///-tst

        n++;
        if(n>=DEDElements.size())
            break;
    }

    BOOST_CHECK(bResult == true);

    /// cleanup after this test
    std::string strFilepath = full_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
    std::string strFilepathToast = full_path.string() + relativeENTITIES_DATABASE_TOAST_PLACE + (std::string)"22980574" + ".xml"; // database file for entity TOAST
    boost::filesystem::remove( strFilepath );
    boost::filesystem::remove( strFilepathToast );
}

BOOST_AUTO_TEST_CASE(validate_put_and_ftgt_with_image_in_toast)
{
    bool bResult=false;
    std::vector<Elements> DEDElements;
    std::string EntityName = "Profile";
    std::string EntityFileName = "ProfileTest";

    SetDir();
    CDatabaseControl CDbCtrl;
    CDbCtrl.readDDEntityRealm((std::string)EntityName, DEDElements); /// Fetch current datadictionary specs for this entity
    CDbCtrl.readDDTOASTEntityRealm((std::string)EntityName, DEDElements); /// Fetch current datadictionary specs for this entity TOAST and add it to list

    /// Helpful script to cut attributes out of DD files
    /// $cat DD_PROFILE_TOAST_ATTRIBUTES.xml | grep -e '<PhysicalDataElementName>' | sed -e 's/<PhysicalDataElementName>/\"/' | sed -e 's/<\/PhysicalDataElementName>/\",/'
    ///
    /// put values to elements -- NB this test is not taking into account that attributes could be moved around - it assumes that lifecyclestate is first in toast file attributes
    std::string strAttr[] = {"profileID",
    "profileName",
    "protocolTypeID",
    "sizeofProfileData",
    "profile_chunk_id",
    "AccountStatus",
    "SubscriptionExpireDate",
    "ProfileStatus",
    "lifecyclestate", /// From here attributes lie in toast area ; WriteEntityFile - should find out by itself and update value inside TOAST file
        "username",
        "password",
        "devicelist",
        "firstname",
        "lastname",
        "streetname",
        "streetno",
        "postalcode",
        "city",
        "state",
        "country",
        "email",
        "mobilephone",
        "title",
        "about",
        "foto", /// test puts image in this attribute
/*        "type",
        "status",
        "expiredate",
        "creditor",
        "terms",
        "subscriptions",
        "paymentdetails",
        "notificationlevel",
        "bankaccount",
        "memberships",
        "friendships",
        "presence",
        "location",
        "context",
        "schedule",
        "deviceID",
        "experience",
        "skills",
        "messagesboard",
        "eventlog",
        "supervisor" */
         };
    std::vector<std::string> strVecAttrValue = {"MyProfile",
                                  "MyName",
                                  "DED1.00.00",
                                  "0",
                                  "22980574", // file does NOT exists, so method will consider it empty, could be considered an error, however it really is more a warning
				                  "0",
                                  "20160101",
                                  "2",
                                  "0", /// lifecyglestate
                                  "SERUP", /// username
                                  "secret101", /// password
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<foto>"}; /// foto ; replace <foto> with foto - embedded image

    unsigned int n=0;
    unsigned int nn = strVecAttrValue.size();
    while(DEDElements.size() > nn)
    {
        DEDElements.pop_back(); /// remove elements that are not being updated
    }
    BOOST_FOREACH( Elements f, DEDElements )
    {
        // iterate thru elements and add value to current specs of above given entity
        if( f.strElementID == (std::string)strAttr[n] )
        {
            std::copy(strVecAttrValue[n].begin(), strVecAttrValue[n].end(), std::back_inserter(DEDElements[n].ElementData));
        }
        n++;
        if(n>=nn)
            break;
    }

    /// insert an image in foto placeholder
    std::vector<unsigned char> newElementData;
  //-working:  std::string newValue = "data:image/jpeg;base64,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";
    
    std::string newValue = "data:image/png;base64,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3UxEh36pJTKjoKclu9sepqO4u152qArLYWU7GTqdS1woZu6olR4add66uw4gP3Pflb975sEffeStv/7CB1758gd/8fl3/eFLHzz4+0//9rPv++azD373nU/97DMf+MFnP3B6WD232ju/vVTNhEMKK7EUsPSwmF8kwAdBSZwrEZEUCUEcsyBKwBmLiOB1XQ6N3ToLTTgelaq1SB0abE4OxahgjBJUG+s3qSkUyhu3j7eyy8cLmRrL+bWpCpOsEoMN1ZPS8hGtmkURdk5rfyMEVjupFVX0EIZZVZWvlVONSrLfzA6bme2V5tVz62996t57bj/psMyYjUfm5m49NnPrHMQDq+lGa70B8gbL/8Sp05r/3/UfOPUWu1YnY+jdJ07u1Bs9v7qbyz174sSTu1tP7m/duza+tti7MGzuNwo7lcx6PjmOBpoevia6i24iQyFJfIozhjpgyMUIJIq7wLkEETtsI7gLOnCcwpIEWnDTLUWusu4CihQRZChKt7c7H7p65fnjew9Nxnd022cr+ZOF9E4mNlblcUgGigtBaT2u7qYj+5nYxVL2zlb1wUHn6bXJe84c/9T1C1957J5/efsTL33inb/70of/8g+fePmvX3jtqx/8y1c+/OtPvfPlT77zH5994JUvfeIP3/zSQlo9tza8dnKzlAyEvZzMgTQxL0/4BFxwO3nG4RHQSIDzKzhLm132GRLX5zIqtFwSNzlgghJmQJvJKguTwnCcxylNOMHIqh2GottnVGKuUJaIFZnlvWKhJbm9861xsDqQdi7mwmVLsokLUYMRfYMRuZUQjQ56Llv1H8pm4m43GlKlQjZSLcbGvdLKsLY+rt977bTTNEOixtmjrzca5m+97Q12p+WNt7wBZuT/myWA/M82e6Om6vxfPdas106vVqUt5jv3ju+3O301cLpafWpn5/HtjYe3Vu5aHl6ddC+NWqfb5ZPN4vFqYTObmETUgeppSmzZTRQpIk9PK8cQeTcJszBN4SkSS8KOm8zxTJF3Vzi+41EWQ6G+JDdIuolTK4rvwf7o75986oMXLrzv7OknVpfuG/YeWBjspSIrIWUl4l0MyrAFkKfyyXPF9O2N8sOj3lPL4+e3Vt9/9sRn7770908/+IP3PvvrT7/3T1/+8B++9N6Db37s159+6x8/964fvOvx77/7qV997v0H//qNX37jb5Ks4/hi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";
    std::copy(newValue.begin(), newValue.end(), std::back_inserter(newElementData));
    bResult = CDbCtrl.update_element_value(DEDElements, (std::string) "foto", newElementData);
    BOOST_CHECK(bResult == true);

    /// Write pre - values in entity file
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    bResult=false;
    bResult = CDbCtrl.put((std::string)EntityName,(std::string)EntityFileName,DEDElements);
    BOOST_CHECK(bResult == true);

    std::vector<Elements> record_value;
    if(bResult==true)
    {
        /// use ftgt to fetch the entity file
        bResult = CDbCtrl.ftgt( (std::string)EntityName, (std::string)EntityFileName, record_value );
        BOOST_CHECK(bResult == true);
    }


    /// validate data read
    bResult=true;
    n=0;
    /// ftgt should have read all attributes into record_value, now compare with DEDElements, which is elements written
    BOOST_FOREACH( Elements v, record_value )
    {
        // iterate thru elements and compare
        if(v.strElementID != (std::string)DEDElements[n].strElementID) bResult=false;
        if(bResult == false)
        {
            std::cout << "v.strElementID [ " << v.strElementID << (std::string)" ] != DEDElements[n].strElementID [ " << DEDElements[n].strElementID << " ] \n";
        }


        if(v.ElementData != DEDElements[n].ElementData) bResult=false;
        if(bResult == false)
        {
            std::cout << "v.ElementDataElementData [ "  << (std::string)" ] != DEDElements[n].ElementData [ "  << " ] \n";
        }

        n++;
        if(n>=DEDElements.size())
            break;
    }

    BOOST_CHECK(bResult == true);

    /// cleanup after this test
    std::string strFilepath = full_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
    std::string strFilepathToast = full_path.string() + relativeENTITIES_DATABASE_TOAST_PLACE + (std::string)"22980574" + ".xml"; // database file for entity TOAST
    boost::filesystem::remove( strFilepath );
    boost::filesystem::remove( strFilepathToast );
}

BOOST_AUTO_TEST_CASE(validate_put_and_ftgt_with_xlargeimage_in_toast)
{
    bool bResult=false;
    std::vector<Elements> DEDElements;
    std::string EntityName = "Profile";
    std::string EntityFileName = "ProfileTest";

    SetDir();
    CDatabaseControl CDbCtrl;
    CDbCtrl.readDDEntityRealm((std::string)EntityName, DEDElements); /// Fetch current datadictionary specs for this entity
    CDbCtrl.readDDTOASTEntityRealm((std::string)EntityName, DEDElements); /// Fetch current datadictionary specs for this entity TOAST and add it to list

    /// Helpful script to cut attributes out of DD files
    /// $cat DD_PROFILE_TOAST_ATTRIBUTES.xml | grep -e '<PhysicalDataElementName>' | sed -e 's/<PhysicalDataElementName>/\"/' | sed -e 's/<\/PhysicalDataElementName>/\",/'
    ///
    /// put values to elements -- NB this test is not taking into account that attributes could be moved around - it assumes that lifecyclestate is first in toast file attributes
    std::string strAttr[] = {"profileID",
    "profileName",
    "protocolTypeID",
    "sizeofProfileData",
    "profile_chunk_id",
    "AccountStatus",
    "SubscriptionExpireDate",
    "ProfileStatus",
    "lifecyclestate", /// From here attributes lie in toast area ; WriteEntityFile - should find out by itself and update value inside TOAST file
        "username",
        "password",
        "devicelist",
        "firstname",
        "lastname",
        "streetname",
        "streetno",
        "postalcode",
        "city",
        "state",
        "country",
        "email",
        "mobilephone",
        "title",
        "about",
        "foto", /// test puts image in this attribute
/*        "type",
        "status",
        "expiredate",
        "creditor",
        "terms",
        "subscriptions",
        "paymentdetails",
        "notificationlevel",
        "bankaccount",
        "memberships",
        "friendships",
        "presence",
        "location",
        "context",
        "schedule",
        "deviceID",
        "experience",
        "skills",
        "messagesboard",
        "eventlog",
        "supervisor" */
         };
    std::vector<std::string> strVecAttrValue = {"MyProfile",
                                  "MyName",
                                  "DED1.00.00",
                                  "0",
                                  "22980574", // file does NOT exists, so method will consider it empty, could be considered an error, however it really is more a warning
				                  "0",
                                  "20160101",
                                  "2",
                                  "0", /// lifecyglestate
                                  "SERUP", /// username
                                  "secret101", /// password
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<Test>",
                                  "<foto>"}; /// foto ; replace <foto> with foto - embedded image

    unsigned int n=0;
    unsigned int nn = strVecAttrValue.size();
    while(DEDElements.size() > nn)
    {
        DEDElements.pop_back(); /// remove elements that are not being updated
    }
    BOOST_FOREACH( Elements f, DEDElements )
    {
        // iterate thru elements and add value to current specs of above given entity
        if( f.strElementID == (std::string)strAttr[n] )
        {
            std::copy(strVecAttrValue[n].begin(), strVecAttrValue[n].end(), std::back_inserter(DEDElements[n].ElementData));
        }
        n++;
        if(n>=nn)
            break;
    }

    /// insert an image in foto placeholder
    std::vector<unsigned char> newElementData;
  //-working:  std::string newValue = "data:image/jpeg;base64,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";
    
  //-working:  std::string newValue = "data:image/png;base64,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3UxEh36pJTKjoKclu9sepqO4u152qArLYWU7GTqdS1woZu6olR4add66uw4gP3Pflb975sEffeStv/7CB1758gd/8fl3/eFLHzz4+0//9rPv++azD373nU/97DMf+MFnP3B6WD232ju/vVTNhEMKK7EUsPSwmF8kwAdBSZwrEZEUCUEcsyBKwBmLiOB1XQ6N3ToLTTgelaq1SB0abE4OxahgjBJUG+s3qSkUyhu3j7eyy8cLmRrL+bWpCpOsEoMN1ZPS8hGtmkURdk5rfyMEVjupFVX0EIZZVZWvlVONSrLfzA6bme2V5tVz62996t57bj/psMyYjUfm5m49NnPrHMQDq+lGa70B8gbL/8Sp05r/3/UfOPUWu1YnY+jdJ07u1Bs9v7qbyz174sSTu1tP7m/duza+tti7MGzuNwo7lcx6PjmOBpoevia6i24iQyFJfIozhjpgyMUIJIq7wLkEETtsI7gLOnCcwpIEWnDTLUWusu4CihQRZChKt7c7H7p65fnjew9Nxnd022cr+ZOF9E4mNlblcUgGigtBaT2u7qYj+5nYxVL2zlb1wUHn6bXJe84c/9T1C1957J5/efsTL33inb/70of/8g+fePmvX3jtqx/8y1c+/OtPvfPlT77zH5994JUvfeIP3/zSQlo9tza8dnKzlAyEvZzMgTQxL0/4BFxwO3nG4RHQSIDzKzhLm132GRLX5zIqtFwSNzlgghJmQJvJKguTwnCcxylNOMHIqh2GottnVGKuUJaIFZnlvWKhJbm9861xsDqQdi7mwmVLsokLUYMRfYMRuZUQjQ56Llv1H8pm4m43GlKlQjZSLcbGvdLKsLY+rt977bTTNEOixtmjrzca5m+97Q12p+WNt7wBZuT/myWA/M82e6Om6vxfPdas106vVqUt5jv3ju+3O301cLpafWpn5/HtjYe3Vu5aHl6ddC+NWqfb5ZPN4vFqYTObmETUgeppSmzZTRQpIk9PK8cQeTcJszBN4SkSS8KOm8zxTJF3Vzi+41EWQ6G+JDdIuolTK4rvwf7o75986oMXLrzv7OknVpfuG/YeWBjspSIrIWUl4l0MyrAFkKfyyXPF9O2N8sOj3lPL4+e3Vt9/9sRn7770908/+IP3PvvrT7/3T1/+8B++9N6Db37s159+6x8/964fvOvx77/7qV997v0H//qNX37jb5Ks4/hi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";

    std::copy(newValue.begin(), newValue.end(), std::back_inserter(newElementData));
    bResult = CDbCtrl.update_element_value(DEDElements, (std::string) "foto", newElementData);
    BOOST_CHECK(bResult == true);

    /// Write pre - values in entity file
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    bResult=false;
    bResult = CDbCtrl.put((std::string)EntityName,(std::string)EntityFileName,DEDElements);
    BOOST_CHECK(bResult == true);

    std::vector<Elements> record_value;
    if(bResult==true)
    {
        /// use ftgt to fetch the entity file
        bResult = CDbCtrl.ftgt( (std::string)EntityName, (std::string)EntityFileName, record_value );
        BOOST_CHECK(bResult == true);
    }


    /// validate data read
    bResult=true;
    n=0;
    /// ftgt should have read all attributes into record_value, now compare with DEDElements, which is elements written
    BOOST_FOREACH( Elements v, record_value )
    {
        // iterate thru elements and compare
        if(v.strElementID != (std::string)DEDElements[n].strElementID) bResult=false;
        if(bResult == false)
        {
            std::cout << "v.strElementID [ " << v.strElementID << (std::string)" ] != DEDElements[n].strElementID [ " << DEDElements[n].strElementID << " ] \n";
        }


        if(v.ElementData != DEDElements[n].ElementData) bResult=false;
        if(bResult == false)
        {
            std::cout << "v.ElementDataElementData [ "  << (std::string)" ] != DEDElements[n].ElementData [ "  << " ] \n";
        }

        n++;
        if(n>=DEDElements.size())
            break;
    }

    BOOST_CHECK(bResult == true);

    /// cleanup after this test
    std::string strFilepath = full_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
    std::string strFilepathToast = full_path.string() + relativeENTITIES_DATABASE_TOAST_PLACE + (std::string)"22980574" + ".xml"; // database file for entity TOAST
    boost::filesystem::remove( strFilepath );
    boost::filesystem::remove( strFilepathToast );
}


BOOST_AUTO_TEST_CASE(list_realm_files)
{
    bool bResult=false;
    std::string realm_name = "Profile";
    //namespace fs = boost::filesystem;
    //typedef std::multimap<std::time_t, fs::path > result_set_t;
    //result_set_t result_set;
    //result_set_t::iterator it;
    std::vector<std::string> result_set;

    SetDir();
    CDatabaseControl CDbCtrl;
    bResult = CDbCtrl.list_realm_files( realm_name, result_set );

    BOOST_FOREACH( std::string v, result_set )
    {
            std::cout << " realm filename: " << v << "\n";
    }

//    // showing contents:
//    std::cout << "mymultimap result_set contains:\n";
//    for (it=result_set.begin(); it!=result_set.end(); ++it)
//        std::cout << (*it).first << " => " << (*it).second << '\n';

    BOOST_CHECK(bResult == true);
}