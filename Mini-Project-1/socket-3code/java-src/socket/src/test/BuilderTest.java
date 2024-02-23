package test;

import gash.payload.BasicBuilder;
import gash.payload.Message;
// Use the correct JUnit version consistently; here, JUnit 5 is used
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Assertions;

public class BuilderTest {

    static final String n = "fred", g = "dogs", t = "hello";
    static final String expectedStart = "0014,dogs,fred,hello"; 

    @Test
    public void testBuilder() throws Exception {
        BasicBuilder builder = new BasicBuilder();

        String encodedMessage = builder.encode(new Message(n, g, t));

        String lengthStr = encodedMessage.substring(0, 4);
        int length = Integer.parseInt(lengthStr);

        Assertions.assertEquals(length, encodedMessage.length() - 5 );

        Assertions.assertTrue(encodedMessage.startsWith(lengthStr + ","));
        Assertions.assertTrue(encodedMessage.contains(g + ","));
        Assertions.assertTrue(encodedMessage.contains(n + ","));
        Assertions.assertTrue(encodedMessage.endsWith(t + "\0"));

        var m = builder.decode(encodedMessage.getBytes());

        Assertions.assertEquals(m.getName(), n);
        Assertions.assertEquals(m.getGroup(), g);
        Assertions.assertEquals(m.getText(), t + "\0");
    }
}