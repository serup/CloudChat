package dops.protocol.ded;

import org.junit.Test;

/**
 * Created by serup on 15-12-15.
 */
public class DEDEncoderTest {

    /**
     *
     *   short trans_id = 1;
     *   boolean action = true;
     *
     *   DEDEncoder DED = DEDEncoder();
     *   DED.PUT_STRUCT_START( "event" );
     *      DED.PUT_METHOD  ( "name",  "MusicPlayer" );
     *      DED.PUT_USHORT  ( "trans_id",  trans_id);
     *      DED.PUT_BOOL    ( "startstop", action );
     *   DED.PUT_STRUCT_END( "event" );
     *   DED.GET_ENCODED_DATA(DEDobject);
     *
     *   //..transmitting :-) simulation
     *
     *   //..receiving :-) simulation
     *   DEDDecoder DED = DEDDecoder();
     *   DED.PUT_DATA_IN_DECODER( DEDobject.pCompressedData, DEDobject.sizeofCompressedData);
     *
     *   boolean bDecoded=false;
     *   String strValue;
     *   short iValue;
     *   boolean bValue;
     *
     *   // decode data ...
     *   DEDDecoder DED2 = new DEDDecoder();
     *   DED2.PUT_DATA_IN_DECODER( DEDobject.pCompressedData, DEDobject.sizeofCompressedData);
     *   if( DED2.GET_STRUCT_START( "event" )==1 &&
     *           (strValue = DED2.GET_METHOD ( "name" )).length()>0 &&
     *           (iValue   = DED2.GET_USHORT ( "trans_id")) !=-1 &&
     *           (bValue   = DED2.GET_BOOL   ( "startstop")) &&
     *           (strText  = DED2.GET_STDSTRING ( "text")).length()>0 &&
     *       DED2.GET_STRUCT_END( "event" )==1)
     *   {
     *     bDecoded=true;
     *   }
     *   else
     *   {
     *     bDecoded=false;
     *   }
     *
     * @throws Exception
     */
    @Test
    public void testEncodeDecode() throws Exception
    {
        short trans_id = 1;
        boolean action = true;

        DEDEncoder DED = new DEDEncoder();
        DED.PUT_STRUCT_START( "event" );
            DED.PUT_METHOD  ( "name",  "MusicPlayer" );
            DED.PUT_USHORT  ( "trans_id",  trans_id);
            DED.PUT_BOOL    ( "startstop", action );
            DED.PUT_STDSTRING("text", "JavaServicesApp world");
        DED.PUT_STRUCT_END( "event" );

        DEDobject DEDobject = new DEDobject();
        DED.GET_ENCODED_DATA(DEDobject); // data should be compressed using LZSS algorithm

        //..transmitting :-) simulation

        //..receiving :-) simulation

        boolean bDecoded=false;
        String strValue="";
        short iValue=0;
        boolean bValue=false;
        String strText="";

        // decode data ...
        DEDDecoder DED2 = new DEDDecoder();
        DED2.PUT_DATA_IN_DECODER( DEDobject.pCompressedData, DEDobject.sizeofCompressedData); // data should be decompressed using LZSS algorithm
        if( DED2.GET_STRUCT_START( "event" )==1 &&
                (strValue = DED2.GET_METHOD ( "name" )).length()>0 &&
                (iValue   = DED2.GET_USHORT ( "trans_id")) !=-1 &&
                (bValue   = DED2.GET_BOOL   ( "startstop")) &&
                (strText  = DED2.GET_STDSTRING ( "text")).length()>0 &&
            DED2.GET_STRUCT_END( "event" )==1)
        {
          bDecoded=true;
        }
        else
        {
          bDecoded=false;
        }


    }

}