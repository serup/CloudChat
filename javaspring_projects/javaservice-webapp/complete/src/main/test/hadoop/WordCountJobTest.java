package hadoop;

import hadoop.hadoopMappers.WordMapper;
import hadoop.hadoopReducers.WordReducer;
import integrationTests.IntegrationEnvironmentSetup;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.mapreduce.MapReduceDriver;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.assertEquals;

/**
 * Created by serup on 04-04-16.
 */

public class WordCountJobTest {

    MapDriver<LongWritable, Text, Text, IntWritable> mapDriver;
    ReduceDriver<Text, IntWritable, Text, IntWritable> reduceDriver;
    MapReduceDriver<LongWritable, Text, Text, IntWritable, Text, IntWritable> mapReduceDriver;

    /*
    */
    IntegrationEnvironmentSetup env = new IntegrationEnvironmentSetup();

    @Before
    public void setUp() throws Exception {
        assertEquals(true,env.setupHadoopIntegrationEnvironment());
        WordMapper mapper = new WordMapper();
        WordReducer reducer = new WordReducer();
        mapDriver = MapDriver.newMapDriver(mapper);
        reduceDriver = ReduceDriver.newReduceDriver(reducer);
        mapReduceDriver = MapReduceDriver.newMapReduceDriver(mapper, reducer);
    }


    @Test
    public void testWordCountMapReduce() {
        mapReduceDriver.withInput(new LongWritable(), new Text("THE ADVENTURES OF SHERLOCK HOLMES\n" +
                "\n" +
                "by\n" +
                "\n" +
                "SIR ARTHUR CONAN DOYLE\n" +
                "\n" +
                "\n" +
                "\n" +
                "   I. A Scandal in Bohemia\n" +
                "  II. The Red-headed League\n" +
                " III. A Case of Identity\n" +
                "  IV. The Boscombe Valley Mystery\n" +
                "   V. The Five Orange Pips\n" +
                "  VI. The Man with the Twisted Lip\n" +
                " VII. The Adventure of the Blue Carbuncle\n" +
                "VIII. The Adventure of the Speckled Band\n" +
                "  IX. The Adventure of the Engineer's Thumb\n" +
                "   X. The Adventure of the Noble Bachelor\n" +
                "  XI. The Adventure of the Beryl Coronet\n" +
                " XII. The Adventure of the Copper Beeches\n" +
                "\n" +
                "\n" +
                "\n" +
                "\n" +
                "ADVENTURE I. A SCANDAL IN BOHEMIA\n" +
                "\n" +
                "I.\n" +
                "\n" +
                "To Sherlock Holmes she is always THE woman. I have seldom heard\n" +
                "him mention her under any other name. In his eyes she eclipses\n" +
                "and predominates the whole of her sex. It was not that he felt\n" +
                "any emotion akin to love for Irene Adler. All emotions, and that\n" +
                "one particularly, were abhorrent to his cold, precise but\n" +
                "admirably balanced mind. He was, I take it, the most perfect\n" +
                "reasoning and observing machine that the world has seen, but as a\n" +
                "lover he would have placed himself in a false position. He never\n" +
                "spoke of the softer passions, save with a gibe and a sneer. They\n" +
                "were admirable things for the observer--excellent for drawing the\n" +
                "veil from men's motives and actions. But for the trained reasoner\n" +
                "to admit such intrusions into his own delicate and finely\n" +
                "adjusted temperament was to introduce a distracting factor which\n" +
                "might throw a doubt upon all his mental results. Grit in a\n" +
                "sensitive instrument, or a crack in one of his own high-power\n" +
                "lenses, would not be more disturbing than a strong emotion in a\n" +
                "nature such as his. And yet there was but one woman to him, and\n" +
                "that woman was the late Irene Adler, of dubious and questionable\n" +
                "memory.\n" +
                "\n" +
                "I had seen little of Holmes lately. My marriage had drifted us\n" +
                "away from each other. My own complete happiness, and the\n" +
                "home-centred interests which rise up around the man who first\n" +
                "finds himself master of his own establishment, were sufficient to\n" +
                "absorb all my attention, while Holmes, who loathed every form of\n" +
                "society with his whole Bohemian soul, remained in our lodgings in\n" +
                "Baker Street, buried among his old books, and alternating from\n" +
                "week to week between cocaine and ambition, the drowsiness of the\n" +
                "drug, and the fierce energy of his own keen nature. He was still,\n" +
                "as ever, deeply attracted by the study of crime, and occupied his\n" +
                "immense faculties and extraordinary powers of observation in\n" +
                "following out those clues, and clearing up those mysteries which\n" +
                "had been abandoned as hopeless by the official police. From time\n" +
                "to time I heard some vague account of his doings: of his summons\n" +
                "to Odessa in the case of the Trepoff murder, of his clearing up\n" +
                "of the singular tragedy of the Atkinson brothers at Trincomalee,\n" +
                "and finally of the mission which he had accomplished so\n" +
                "delicately and successfully for the reigning family of Holland.\n" +
                "Beyond these signs of his activity, however, which I merely\n" +
                "shared with all the readers of the daily press, I knew little of\n" +
                "my former friend and companion.\n" +
                "\n" +
                "One night--it was on the twentieth of March, 1888--I was\n" +
                "returning from a journey to a patient (for I had now returned to\n" +
                "civil practice), when my way led me through Baker Street. As I\n" +
                "passed the well-remembered door, which must always be associated\n" +
                "in my mind with my wooing, and with the dark incidents of the\n" +
                "Study in Scarlet, I was seized with a keen desire to see Holmes\n" +
                "again, and to know how he was employing his extraordinary powers.\n" +
                "His rooms were brilliantly lit, and, even as I looked up, I saw\n" +
                "his tall, spare figure pass twice in a dark silhouette against\n" +
                "the blind. He was pacing the room swiftly, eagerly, with his head\n" +
                "sunk upon his chest and his hands clasped behind him. To me, who\n" +
                "knew his every mood and habit, his attitude and manner told their\n" +
                "own story. He was at work again. He had risen out of his\n" +
                "drug-created dreams and was hot upon the scent of some new\n" +
                "problem. I rang the bell and was shown up to the chamber which\n" +
                "had formerly been in part my own.\n" +
                "\n" +
                "His manner was not effusive. It seldom was; but he was glad, I\n" +
                "think, to see me. With hardly a word spoken, but with a kindly\n" +
                "eye, he waved me to an armchair, threw across his case of cigars,\n" +
                "and indicated a spirit case and a gasogene in the corner. Then he\n" +
                "stood before the fire and looked me over in his singular\n" +
                "introspective fashion.\n" +
                "\n" +
                "\"Wedlock suits you,\" he remarked. \"I think, Watson, that you have\n" +
                "put on seven and a half pounds since I saw you.\"\n" +
                "\n" +
                "\"Seven!\" I answered.\n" +
                "\n" +
                "\"Indeed, I should have thought a little more. Just a trifle more,\n" +
                "I fancy, Watson. And in practice again, I observe. You did not\n" +
                "tell me that you intended to go into harness.\"\n" +
                "\n" +
                "\"Then, how do you know?\"\n" +
                "\n" +
                "\"I see it, I deduce it. How do I know that you have been getting\n" +
                "yourself very wet lately, and that you have a most clumsy and\n" +
                "careless servant girl?\"\n" +
                "\n" +
                "\"My dear Holmes,\" said I, \"this is too much. You would certainly\n" +
                "have been burned, had you lived a few centuries ago. It is true\n" +
                "that I had a country walk on Thursday and came home in a dreadful\n" +
                "mess, but as I have changed my clothes I can't imagine how you\n" +
                "deduce it. As to Mary Jane, she is incorrigible, and my wife has\n" +
                "given her notice, but there, again, I fail to see how you work it\n" +
                "out.\"\n" +
                "\n" +
                "He chuckled to himself and rubbed his long, nervous hands\n" +
                "together." ));

        mapReduceDriver.withOutput(new Text("Watson"), new IntWritable(2));  // Expect 2 counts of word 'Watson' in above text
        mapReduceDriver.runTest();

    }
}
