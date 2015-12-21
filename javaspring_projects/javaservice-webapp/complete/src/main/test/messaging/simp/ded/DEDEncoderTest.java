package messaging.simp.ded;

import org.junit.Test;
import org.springframework.util.Assert;

import static org.junit.Assert.*;

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
     *
     *   if( DED.GET_STRUCT_START( "event" ) &&
     *          DED.GET_METHOD ( "name", strValue ) &&
     *          DED.GET_USHORT ( "trans_id", iValue) &&
     *          DED.GET_BOOL   ( "startstop", bValue ) &&
     *       DED.GET_STRUCT_END( "event" ))
     *   {
     *     bDecoded=true;
     *   }
     *   else
     *   {
     *     bDecoded=false;
     *   }

     * @throws Exception
     */
    @Test
    public void testEncode() throws Exception
    {

        //TODO: make functions for encddoding data - example

        short trans_id = 1;
        boolean action = true;

        DEDEncoder DED = new DEDEncoder();
        DED.PUT_STRUCT_START( "event" );
        DED.PUT_METHOD  ( "name",  "MusicPlayer" );
        DED.PUT_USHORT  ( "trans_id",  trans_id);
        DED.PUT_BOOL    ( "startstop", action );
        DED.PUT_STRUCT_END( "event" );

        DEDobject DEDobject = new DEDobject();
        DED.GET_ENCODED_DATA(DEDobject);

        //..transmitting :-) simulation

        //..receiving :-) simulation

        boolean bDecoded=false;
        String strValue="";
        short iValue=0;
        boolean bValue=false;

        // decode data ...
        DEDDecoder DED2 = new DEDDecoder();
        DED2.PUT_DATA_IN_DECODER( DEDobject.pCompressedData, DEDobject.sizeofCompressedData);
        if( DED2.GET_STRUCT_START( "event" )==1 &&
                DED2.GET_METHOD ( "name", strValue )==1 &&
                DED2.GET_USHORT ( "trans_id", iValue)==1 &&
                DED2.GET_BOOL   ( "startstop", bValue )==1 &&
            DED2.GET_STRUCT_END( "event" )==0)
        {
          bDecoded=true;
        }
        else
        {
          bDecoded=false;
        }


    }

}