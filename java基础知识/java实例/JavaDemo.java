class Channel {
    private String info = "www.mldn.cn";
    public String getInfo() {
        return "parent";
    }
}
class DatabaseChannel extends Channel {
    private String info = "HelloMLDN";
    public String getInfo() {
        return "children";
    }
    public void fun() {
        System.out.println(this.getInfo());
        System.out.println(super.getInfo());
    }
}
public class JavaDemo {
    public static void main(String args[]) {
        DatabaseChannel channel = new DatabaseChannel();
        channel.fun();
    }
}