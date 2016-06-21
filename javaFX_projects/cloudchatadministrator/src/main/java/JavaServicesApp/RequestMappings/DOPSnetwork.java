package JavaServicesApp.RequestMappings;

/**
 * Created by serup on 04-05-16.
 */
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class DOPSnetwork {

    @RequestMapping("/dopsinfo")
    public String dopsinfo() {
        return "dopsinfo"; // TODO: add info about running state of backend etc...
    }
}
