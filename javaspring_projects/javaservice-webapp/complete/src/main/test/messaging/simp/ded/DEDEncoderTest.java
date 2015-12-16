package messaging.simp.ded;

import org.junit.Test;
import org.springframework.util.Assert;

import static org.junit.Assert.*;

/**
 * Created by serup on 15-12-15.
 */
public class DEDEncoderTest {

    @Test
    public void testEncode() throws Exception {
        DEDEncoder DED = DEDEncoder.DED_START_ENCODER();


//TODO: make functions for encoding data - example
//    unsigned short trans_id = 1;
//    bool action = true;
//
//    DED_START_ENCODER(encoder_ptr);
//    DED_PUT_STRUCT_START( encoder_ptr, "event" );
//          DED_PUT_METHOD  ( encoder_ptr, "name",  "MusicPlayer" );
//          DED_PUT_USHORT  ( encoder_ptr, "trans_id",      trans_id);
//          DED_PUT_BOOL    ( encoder_ptr, "startstop", action );
//    DED_PUT_STRUCT_END( encoder_ptr, "event" );


    }

}