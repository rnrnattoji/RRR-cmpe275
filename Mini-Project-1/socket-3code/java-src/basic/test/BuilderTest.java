
import org.junit.Test;
import org.junit.jupiter.api.Assertions;

import gash.payload.BasicBuilder;
import gash.payload.Message;

public class BuilderTest {
	static final String n = "fred", g = "dogs", t = "hello";
	static final String msg = "dogs,fred,hello";

	@Test
	public void testBuilder() throws Exception {
		BasicBuilder builder = new BasicBuilder();

		String s = builder.encode(new Message(n,g,t));
		Assertions.assertEquals(msg,s);

		System.out.println("msg: " + s);

		var m = builder.decode(s.getBytes());
		Assertions.assertEquals(m.getName(),n);
		Assertions.assertEquals(m.getGroup(), g);
		Assertions.assertEquals(m.getText(),t);
	}
}