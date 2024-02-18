package gash.payload;

public class Message {
	private String name, group, text;

	public Message(String name, String group, String text) {
		this.name = name;
		this.group = group;
		this.text = text;
	}

	@Override
	public String toString() {
		var sb = new StringBuilder();
		sb.append("from ").append(name);
		sb.append(", to group: ").append(group);
		sb.append(", text: ").append(text);

		return sb.toString();
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getGroup() {
		return group;
	}

	public void setGroup(String group) {
		this.group = group;
	}

	public String getText() {
		return text;
	}

	public void setText(String text) {
		this.text = text;
	}
}
