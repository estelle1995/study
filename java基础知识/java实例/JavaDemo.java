
public class JavaDemo {
    
    public int aMethod() {
        static int i=0;
        i++;
        return i;
 }
 public static void main(String args[]) {
        JavaDemo test = new JavaDemo();
        test.aMethod();
        int j = test.aMethod();
        System.out.println(j);
 }
}