import hadoop.hadoopMappers.WordMapper;
import org.junit.Before;
import org.junit.Test;
import org.springframework.context.ApplicationContext;
import org.springframework.context.support.ClassPathXmlApplicationContext;


/**
 * Created by serup on 19-02-16.
 */
public class hadoopTests {

    @Before
    public void setupHadoop()
    {

    }

    @Test
    public void testCreateMapperComponent()
    {
        /**
         * Creating the Mapper Component
         * A mapper is a component that divides the original problem into smaller problems that are easier to solve.
         * - create a custom mapper component by extending the Mapper<KEYIN, VALUEIN, KEYOUT, VALUEOUT> class
         *   and overriding its map() method.
         */
        WordMapper wordMapper = new WordMapper();
        ApplicationContext ctx = new ClassPathXmlApplicationContext("applicationContext.xml");

    }
}
