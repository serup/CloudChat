/*
 * DatabaseControl.hpp
 * - contains main database class with methods for handling database files
 * - also integrity tests made using datadictionary xml description files
 */
#include <string>
#include <cctype>       // std::toupper
#include <boost/algorithm/string.hpp> // boost::to_upper
#include <boost/algorithm/hex.hpp>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// The signature of boost::property_tree::xml_parser::write_xml() changed in Boost 1.56
// See https://github.com/PointCloudLibrary/pcl/issues/864
#include <boost/version.hpp>
#if (BOOST_VERSION >= 105600)
  typedef boost::property_tree::xml_writer_settings<std::string> xml_writer_settings;
#else
  typedef boost::property_tree::xml_writer_settings<char> xml_writer_settings;
#endif

#include <boost/foreach.hpp>
#include <boost/range.hpp> // used when calculating amount of std::string array
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#include "md5.h"
#include <iostream>
#include <fstream>      // std::ifstream
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include "../websocket_server/dataframe.hpp"
#include "../websocket_server/wsclient/wsclient.hpp"
using namespace websocket;
using namespace std;
using boost::property_tree::ptree;
#include "../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h"
#include "../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h"

#ifndef DATABASECONTROL
#define DATABASECONTROL


/// HOWTO REMOVE UNWANTED CRLF
//sed -i 's/\r\n/\n/' databasecontrol.hpp


//#ifndef Elements
//struct Elements
//{
//    std::string strElementID;
//    std::vector<unsigned char> ElementData;
//};
//#endif

struct DEDElementFinder
{
    DEDElementFinder(std::string id)
        : mID(id)
    {
    }

    bool operator()( Elements *test )
    {
        bool bResult=false;
        if(test->strElementID == mID)
            bResult=true;
        return bResult;
    }

    std::string mID;
};


///////////////////////////
// DDEntry               //
///////////////////////////
struct DDEntry
{
    std::string  DDGuid;
    std::string  PhysicalDataElementName;
    std::string  Location;
    std::string  Category;
    std::string  DataType;
    std::string  MaxLength;
    std::string  Description;
    std::string  characteristics;
    std::string  relationship;
    std::string  Mandatory;
    std::string  accessrights;
};
struct DDStream
{
    std::string datastream_name;
    std::vector<DDEntry> Entry;
};

///////////////////////////
// Profile datastream    //
///////////////////////////

typedef std::vector<DDStream> ProfileDatastreams;

/// TODO: streamline stream section with names in systemspecs datadictionary !!!!!!
///////////////////////////
// STREAM SECTION        //
///////////////////////////
// incomming ======>
struct CreateNewProfileInfo
{
    /// Helpful script to cut attributes out of DD_PROFILE file
    /// $cat DD_PROFILE.xml | grep -e '<PhysicalDataElementName>' | sed -e 's/<PhysicalDataElementName>/\"/' | sed -e 's/<\/PhysicalDataElementName>/\",/'
    unsigned short iTransID; // trans id is used for as handshaking between client/"server", to make sure communication is aligned
    std::string strSource; // source is name of "client/function" which is making a request for creating a new Profile -- this is needed to beable to send a response back
    std::string strProfileID;
    std::string strProfileName;
    std::string strProtocolTypeID;
    std::string strSizeofProfileData;
    std::string strProfile_chunk_id;
    std::string strAccountStatus;
    std::string strSubscriptionExpireDate;
    std::string strProfileStatus;
    std::vector<Elements> vecElements; // here is TOAST data, in the form of elements
};
struct LoginProfileRequest
{
    unsigned short iTransID; // trans id is used for as handshaking between client/"server", to make sure communication is aligned
    std::string strSource; // source is needed to beable to send a response back
    std::string strProfileID;
    //std::string strProfileName; // username
    std::string strProfileUsername; // username
    std::string strProfilePassword;

};
struct ProfilePasswordMatchRequest
{
    unsigned short iTransID; // trans id is used for as handshaking between client/"server", to make sure communication is aligned
    std::string strSource; // source is needed to beable to send a response back
    std::string strProfileID;
    std::string strProfileUsername; // username
    std::string strProfilePassword;
};
struct AccountValidationRequest
{
    unsigned short iTransID; // trans id is used for as handshaking between client/"server", to make sure communication is aligned
    std::string strSource; // source is needed to beable to send a response back
    std::string strProfileID;
    enum enumAccountStatus { AccountError, AccountValid, AccountNotValid, AccountSuspended } eAccountStatus;
    std::string strSubscriptionExpireDate;
};
struct UpdateProfileStatusRequest
{
    unsigned short iTransID; // trans id is used for as handshaking between client/"server", to make sure communication is aligned
    std::string strProfileID;
    std::string strProfileUsername; // username
    enum enumAccountStatus { AccountError, AccountValid, AccountNotValid, AccountSuspended } eAccountStatus;
};
struct WriteLogRequest
{

};

/// Helpful script to cut attributes out of DD files
/// $cat DD_HISTORY.xml | grep -e '<PhysicalDataElementName>' | sed -e 's/<PhysicalDataElementName>/\"/' | sed -e 's/<\/PhysicalDataElementName>/\",/'
struct ProfileLogMessage
{
    std::string LogEntryCounter;
    std::string protocolTypeID;
    std::string profileID;
    std::string src;
    std::string dest;
    std::string srcUsername;
    std::string destUsername;
    std::string showdelivery;
    std::string timeReceived;
    std::string timeSend;
    std::string delivered;
    std::string message;
    std::string acceptedIndex;
    std::string history_chunk_id;
};

struct ProfileLogRequest
{
    unsigned short iTransID; // trans id is used for as handshaking between client/"server", to make sure communication is aligned
    std::string strProfileID; // unique id of guest that wants to write chat in profile log
    enum enumLogRequest { AddToLog, RemoveFromLog, FetchFromLog } eLogRequest;
    ProfileLogMessage profileLogMsg;
    enum enumLogStatus { LogError, LogWriteSuccess, LogWriteFailed, LogRemoveSuccess, LogRemoveFailed, LogFetchSuccess, LogFetchFailed } eProfileLogStatus;
};

struct FetchProfileInfo
{
    unsigned short iTransID; // trans id is used for as handshaking between client/"server", to make sure communication is aligned
    std::string strSource; // source is name of "client/function" which is making a request for fetching a Profile -- this is needed to beable to send a response back
    std::string strProfileID;
    std::string strProfileName;
    std::string strPassword;
    std::string strProtocolTypeID;
    std::string strSizeofProfileData;
    std::string strProfile_chunk_id;
    std::string strOrganizationID;
    std::vector<Elements> vecElements; // here is TOAST data, in the form of elements
};

struct ProfileInfo
{
    std::string strProfileID;
    std::string strProfileName;
};

// outgoing <=======
struct ProfileAlreadyInDatabase
{

};
struct ProfileSavedInDatabase
{

};
struct CreateProfileRequestResponse
{
    enum enumResponse { error, ProfileAlreadyInDatabase, ProfileSavedInDatabase } enumresp;
    wsclient::dataframe frame;
};
struct LoginProfileRequestResponse
{
    enum enumResponse { error, ProfileNotFoundInDatabase, ProfileLoggingIn } enumresp;
    wsclient::dataframe frame;
};
struct AuthenticationResponse
{
    enum enumResponse { error, AuthentificationSuccess, AuthentificationFailed } enumresp;
    enum enumAccountStatus { AccountError, AccountValid, AccountNotValid, AccountSuspended } eAccountStatus;
    std::string strSubscriptionExpireDate;
    wsclient::dataframe frame;
};
struct FetchProfileResponse
{
    enum enumResponse { error, ProfileExists, ProfileDoNotExists } enumresp;
    enum enumAccountStatus { AccountError, AccountValid, AccountNotValid, AccountSuspended } eAccountStatus;
    std::string strSubscriptionExpireDate;
    wsclient::dataframe frame;
};
struct MatchPasswordResponse
{
    enum enumResponse { error, AuthentificationSuccess, PasswordDoNotMatch } enumresp;
    wsclient::dataframe frame;
};
struct AccountValidationResponse
{
    enum enumResponse { error, AccountValidationSuccess, AccountValidationFailed } enumresp;
    wsclient::dataframe frame;
};
struct UpdateProfileStatusResponse
{
    enum enumResponse { error, UpdateProfileStatusSuccess, UpdateProfileStatusFailed } enumresp;
    enum enumProfileStatus { ProfileError, ProfileLoggedIn, ProfileLoggedOut, ProfileBusy } eProfileStatus;
    wsclient::dataframe frame;
};
struct WriteLogResponse
{
    enum enumResponse { error, WriteLogSuccess, WriteLogFailed } enumresp;
    wsclient::dataframe frame;
};
struct FetchProfileRequestResponse
{
    enum enumResponse { error, ProfileNotFoundInDatabase, ProfileFoundInDatabase } enumresp;
    enum enumAccountStatus { AccountError, AccountValid, AccountNotValid, AccountSuspended } eAccountStatus;
    std::string strSubscriptionExpireDate;
    wsclient::dataframe frame;
};
struct ProfileLogResponse
{
    enum enumResponse { error, WriteLogSuccess, WriteLogFailed } enumresp;
    wsclient::dataframe frame;
};

/////////////////////////////
// internal stream section //
/////////////////////////////
//+
struct ProfileToastInfo
{
    unsigned short iTransID; // trans id is used for as handshaking between client/"server", to make sure communication is aligned
    std::string strSource; // source is name of "client/function" which is making a request for fetching a Profile -- this is needed to beable to send a response back
    std::string strToastFilename; //
    //TODO: add more info to Profile - since toast area is where most data of Profile will be, then here will be the description -- it will however be stored in chunks inside a DED structure
    std::string strPicturefilepath;
    std::vector<unsigned char> pictureDataInVector;
};
struct ProfileToastData  // same as Elements
{
    //std::string Profile_chunk_id;
    //std::vector<unsigned char> Profile_chunk_data;
    std::vector<Elements> vecElements; // here is TOAST data, in the form of elements
};
struct ProfileChunkDataInfo
{
    std::string Profile_chunk_id;
    unsigned long aiid;
    unsigned long Profile_chunk_seq;
    std::vector<unsigned char> Profile_chunk_data;
};
typedef std::vector<ProfileChunkDataInfo> ProfileTOASTDEDRecord;



//-

///////////////////////////
// ENTITY SECTION        //
///////////////////////////
/*
 <ProfileRealm>
    <DDEntry>
        <DDGuid></DDGuid>
        <PhysicalDataElementName>ProfileID</PhysicalDataElementName>
        <Location></Location>
        <Category>GUID</Category>
        <DataType>std::string</DataType>
        <MaxLength>32</MaxLength>
        <Description></Description>
        <characteristics></characteristics>
        <relationship></relationship>
        <Mandatory>YES</Mandatory>
        <accessrights></accessrights>
    </DDEntry>
    .
    .
    .
 </ProfileRealm>

*/

struct ProfileEntry
{
    std::string  DDGuid;
    std::string  PhysicalDataElementName;
    std::string  Location;
    std::string  Category;
    std::string  DataType;
    std::string  MaxLength;
    std::string  Description;
    std::string  characteristics;
    std::string  relationship;
    std::string  Mandatory;
    std::string  accessrights;
};

typedef std::vector<ProfileEntry> ProfileRealm;

/*
<Profile>
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
struct ProfileRecordEntry
{
    std::string TransGUID;
    std::string Protocol;
    std::string ProtocolVersion;
    unsigned int DataSize;
    std::string Data; // Data is in HEX format, std::string
//    std::vector<unsigned char> Data;
    std::string DataMD5;
};
typedef std::vector<ProfileRecordEntry> ProfileRecord;

struct ProfileTOASTRecordEntry
{
    std::string TransGUID;
    std::string Protocol;
    std::string ProtocolVersion;
    unsigned int DataSize;
    std::string Data; // inside here lies data following ProfileChunkDataInfo structure, // Data is in HEX format, std::string
//    std::vector<unsigned char> Data; // inside here lies data following ProfileChunkDataInfo structure
    std::string DataMD5;
};
typedef std::vector<ProfileTOASTRecordEntry> ProfileTOASTRecord;
//TODO: Above should perhaps be changed to a more general approach, instead of hardcode to individual streams and entities -- think about it -- datadictionary files should be the master !!!
//Below is beginning of this change

/////////////////////////////////////////
// GENERAL Database file structure    ///
/////////////////////////////////////////
//+
/* example (this is how datadictionary is describing attributes in entity database files):
 <ProfileRealm>
    <DDEntry>
        <DDGuid></DDGuid>
        <PhysicalDataElementName>ProfileID</PhysicalDataElementName>
        <Location></Location>
        <Category>GUID</Category>
        <DataType>std::string</DataType>
        <MaxLength>32</MaxLength>
        <Description></Description>
        <characteristics></characteristics>
        <relationship></relationship>
        <Mandatory>YES</Mandatory>
        <accessrights></accessrights>
    </DDEntry>
    .
    .
    .
 </ProfileRealm>
 Above is showing that Profile entity realm file has one attribute called ProfileID

*/
struct DDEntityEntry
{
    std::string  DDGuid;
    std::string  PhysicalDataElementName;
    std::string  Location;
    std::string  Category;
    std::string  DataType;
    std::string  MaxLength;
    std::string  Description;
    std::string  characteristics;
    std::string  relationship;
    std::string  Mandatory;
    std::string  accessrights;
};
typedef std::vector<DDEntityEntry> EntityRealm;

struct DatabaseEntityRecordEntry
{
    std::string TransGUID;
    std::string Protocol;
    std::string ProtocolVersion;
    unsigned int DataSize;
    std::string Data;
    std::string DataMD5;
};
typedef std::vector<DatabaseEntityRecordEntry> DatabaseEntityRecord;

struct DatabaseEntityTOASTRecordEntry
{
    std::string TransGUID;
    std::string Protocol;
    std::string ProtocolVersion;
    unsigned int DataSize;
    std::string Data;
    std::string DataMD5;
};
typedef std::vector<DatabaseEntityTOASTRecordEntry> DatabaseEntityTOASTRecord;

struct EntityChunkDataInfo{
    std::string entity_chunk_id;
    unsigned long aiid;
    unsigned long entity_chunk_seq;
    std::vector<unsigned char> entity_chunk_data;
};
typedef std::vector<EntityChunkDataInfo> EntityTOASTDEDRecord;


struct EntityToastData{ // same as Elements
    //std::string profile_chunk_id;
    //std::vector<unsigned char> profile_chunk_data;
    std::vector<Elements> vecElements; // here is TOAST data, in the form of elements
};

//-

#define ENTITIES_DATABASE_PLACE "../../../DataDictionary/Database/ENTITIEs/"
#define relativeENTITIES_DATABASE_PLACE "/DataDictionary/Database/ENTITIEs/"
#define relative_DATABASE_PLACE "/DataDictionary/Database/"
#define ENTITIES_DATABASE_TOAST_PLACE "../../../DataDictionary/Database/TOASTs/"
#define relativeENTITIES_DATABASE_TOAST_PLACE "/DataDictionary/Database/TOASTs/"
#define DATADICTIONARY_ENTITIES_PLACE  "../../../DataDictionary/Entities/DD_"
#define relative_DATADICTIONARY_ENTITIES_PLACE  "/DataDictionary/Entities/DD_"

class CDatabaseControl
{

public:
	bool bBusy;
	void Init()
	{
		bBusy=true;
		/// setup basic control - run some basic file integrity test

	}
	void Destroy()
	{
		/// close files and make basic cleanup

	}
	CDatabaseControl() { Init(); }
	~CDatabaseControl() { Destroy(); }

    /// fetch and get the record in a specific realm with a specific index value - ex. fetch Profile(realm_name) with index_name = 22980574 (this is also name of entity file, since each index in a realm has its own file, and inside that file is a name for a TOAST file)
	bool ftgt( std::string realm_name, std::string index_name, std::vector<Elements>&record_value );
	/// put / update the record in a specific realm with a specific index value - ex. fetch Profile(realm_name) with index_name = 22980574 (this is also name of entity file, since each index in a realm has its own file, and inside that file is a name for a TOAST file)
	bool put(std::string realm_name, std::string index_name, std::vector<Elements> record_value);
	/// append the record in a specific realm with a specific index value - ex. fetch Profile(realm_name) with index_name = 22980574 (this is also name of entity file, since each index in a realm has its own file, and inside that file is a name for a TOAST file)
    bool append(std::string realm_name, std::string index_name, std::vector<Elements> record_value);
    /// find all realm files and put their names on a list
    bool list_realm_files( std::string realm_name, std::vector<std::string> &result_set );
    bool list_realm_entities( std::string realm_name, std::vector<std::string> &result_set );

//private:
    /// find and update element value, attached to the element ID, with ElementData,
    bool update_element_value(std::vector<Elements>& _DEDElements, std::string strElementID, std::vector<unsigned char> ElementData);
    /// find and update element value, attached to the element ID, with a files binary content
    bool update_element_value_with_file(std::vector<Elements>& _DEDElements, std::string strElementID, std::string strfilepath);
    /// find and fetch the value attached to the element ID
    bool find_element_value(std::vector<Elements>& _DEDElements, std::string strElementID,std::string& strElementIDvalue);
    /// find and fetch the element attached to the element ID
    bool fetch_element(std::vector<Elements>& _DEDElements, std::string strElementID,Elements& ElementIDvalue);
    /// find and compare value
    bool compare_element(std::vector<Elements> _DEDElements, std::string strElementID, std::vector<unsigned char> compareElementData);
    /// find element and return true if found
    bool find_element(std::vector<Elements>& _DEDElements, std::string strElementID);
	///  will read whole entity file
	bool ReadXmlFile( std::ifstream & is, DatabaseEntityRecord & records, std::string EntityName );
	/// will read whole TOAST file
	bool ReadTOASTXmlFile(std::ifstream & is, std::vector<Elements>& records_elements, std::string EntityName);
	/// will write whole entity value to entity file
	bool WriteXmlFile(std::ostream & os,unsigned char* pCompressedData, unsigned int sizeofCompressedData, std::string EntityName );
	/// will append to an existing entity file
	bool AppendXmlFile(std::string filename,unsigned char* pCompressedData, unsigned int sizeofCompressedData, std::string EntityName );
    /// read datadictionary standard description of attributes for specific entity
    EntityRealm readDDEntityRealm( std::istream & is, std::string EntityName );
    /// read datadictionary standard description of attributes for specific entity - only returning physicaldataelementname as a list of elements with name of attribute and value placeholder for later usage
    bool readDDEntityRealm( std::istream & is, std::string EntityName, std::vector<Elements>& DEDElements );
    /// read datadictionary standard description of attributes for specific entity - only returning physicaldataelementname as a list of elements with name of attribute and value placeholder for later usage
    bool readDDEntityRealm( std::string EntityName, std::vector<Elements>& DEDElements );
    /// read datadictionary standard description of attributes for specific entity TOAST area - only returning physicaldataelementname as a list of elements with name of attribute and value placeholder for later usage
    bool readDDTOASTEntityRealm( std::string EntityName, std::vector<Elements>& DEDElements );
    /// check current datadictionary spec for entity against current software version
    bool CHECK_REALM( EntityRealm realm, std::string realmEntries[], std::string EntityName );
    /// reads the database entity file
    bool ReadEntityFile(std::string EntityName, std::string EntityFileName, std::vector<Elements>& DEDElements);
    /// writes a standard datadictionary format xml record into TOAST file -- AppendElementToToastXmlFile is using this methos
    bool AppendRecordToTOASTXmlFile(std::string EntityName, bool bFlush, std::ostream & os,unsigned char* pCompressedData, unsigned int sizeofCompressedData );
    /// write a binary data block as chunks in TOAST file, following datadictionary format for xml toast files
    bool AppendElementToToastXmlFile(std::string EntityName, bool bFlush,unsigned long& aiid, std::ostream & os, std::string entity_chunk_id, std::vector<unsigned char>& iterator_data_in_buf );
    /// write to the database entity TOAST file
    bool WriteEntityTOASTFile(std::string EntityName, std::string EntityToastFileName,std::vector<Elements>& TOASTelements);
    /// write to the database entity file
    bool WriteEntityFile(std::string EntityName, std::string EntityFileName, std::vector<Elements>& DEDElements);
    /// append to the database entity file
    bool AppendEntityFile(std::string EntityName, std::string EntityFileName, std::vector<Elements>& DEDElements);
    /// update the entity files attribut value
    bool updateEntityAttribut(std::vector<Elements>& DEDElements, std::string strElementID, std::vector<unsigned char> ElementData );
    /// update the entity files attribut value
    bool updateEntityAttribut(std::string EntityName, std::string EntityFileName, std::string strElementID, std::vector<unsigned char> ElementData );
    /// read data from a file into byte array
    bool FetchFileAsVector(std::string strfilepath, std::vector<unsigned char> &FileDataBytesInVector);

    /// Remove extension
    std::string removeExtension(const std::string &filename);

};


#endif // DATABASECONTROL

