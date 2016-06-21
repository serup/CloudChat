package JavaServicesApp;

import ProtocolHandlings.DOPsCommunication;
import dops.utils.utils;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import static org.junit.Assert.assertEquals;

@SpringBootApplication
public class javaServiceApp {

    public static void main(String[] args) {
        SpringApplication.run(javaServiceApp.class, args);  // https://spring.io/guides/gs/spring-boot/

        utils util = new utils();
        DOPsCommunication dopsCommunications = new DOPsCommunication();
        if(util.isEnvironmentOK()) {
            String uniqueId = "HadoopJavaServiceApp";
            //String uniqueId = "985998707DF048B2A796B44C89345494";
            String username = "johndoe@email.com";
            String password = "12345";

            if(dopsCommunications.connectToDOPs(uniqueId, username, password)) {

            }
       }
    }
}
