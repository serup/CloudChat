package JavaServicesApp;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication
public class Application {

    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);  // https://spring.io/guides/gs/spring-boot/

        //TODO: connect to DOPS and login to DFD as HadoopJavaServiceApp
        
    }
}
