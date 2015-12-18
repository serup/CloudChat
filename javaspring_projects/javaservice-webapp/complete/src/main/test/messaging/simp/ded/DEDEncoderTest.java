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
     *   DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
     *   DED.PUT_STRUCT_START( DED, "event" );
     *      DED.PUT_METHOD  ( DED, "name",  "MusicPlayer" );
     *      DED.PUT_USHORT  ( DED, "trans_id",  trans_id);
     *      DED.PUT_BOOL    ( DED, "startstop", action );
     *   DED.PUT_STRUCT_END( DED, "event" );
     *   DED.GET_ENCODED_DATA(DEDobject);
     *
     *   //..transmitting :-) simulation
     *
     *   //..receiving :-) simulation
     *   DEDDecoder DED = DEDDecoder.DED_START_DECODER();
     *   DED.PUT_DATA_IN_DECODER( DEDobject.pCompressedData, DEDobject.sizeofCompressedData);
     *
     *   boolean bDecoded=false;
     *   String strValue;
     *   short iValue;
     *   boolean bValue;
     *
     *   // decode data ...
     *
     *   if( DED.GET_STRUCT_START( DED, "event" ) &&
     *          DED.GET_METHOD ( DED, "name", strValue ) &&
     *          DED.GET_USHORT ( DED, "trans_id", iValue) &&
     *          DED.GET_BOOL   ( DED, "startstop", bValue ) &&
     *       DED.GET_STRUCT_END( DED, "event" ))
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

        DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
        DED.PUT_STRUCT_START( DED, "event" );
        DED.PUT_METHOD  ( DED, "name",  "MusicPlayer" );
        DED.PUT_USHORT  ( DED, "trans_id",  trans_id);
        DED.PUT_BOOL    ( DED, "startstop", action );
        DED.PUT_STRUCT_END( DED, "event" );

        DEDobject DEDobject = new DEDobject();
        DED.GET_ENCODED_DATA(DEDobject);




    }

}