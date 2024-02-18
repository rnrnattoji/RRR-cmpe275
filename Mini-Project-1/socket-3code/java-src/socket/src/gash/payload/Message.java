package gash.payload;

public class Message {
	private String text;

	public Message( String text) {
		this.text = text;
	}

	@Override
	public String toString() {
		var sb = new StringBuilder();
		sb.append("text: ").append(text);

		return sb.toString();
	}

	// public String getName() {
	// 	return name;
	// }

	// public void setName(String name) {
	// 	this.name = name;
	// }

	// public String getGroup() {
	// 	return group;
	// }

	// public void setGroup(String group) {
	// 	this.group = group;
	// }

	public String getText() {
		return text;
	}

	public void setText(String text) {
		this.text = text;
	}
}
