DataDictionary for DOPS system

The folder Enitites have sub data dictionaries - one file for each E/R database entity
these sub DD are put together in the final datadictionary :

DataDictionaryDOPS.xml  -- contains everything

NB! it is vital to continuesly make test with regards to sub DD's, so that they correlates the total file

example of an element in data dictionary :
    <customer_chunk_id>
        <DDGuid></DDGuid>
        <PhysicalDataElementName>customer_chunk_id</PhysicalDataElementName>
        <Location></Location>
        <Category>GUID</Category>
        <DataType>std::string</DataType>
        <MaxLength>32</MaxLength>
        <Description></Description>
        <characteristics></characteristics>
        <relationship></relationship>
        <Mandatory>YES</Mandatory>
        <accessrights></accessrights>      
    </customer_chunk_id>

read more in systemspecs or in DFD_DataDictionary.txt
