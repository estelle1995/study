import java.util.*;
public class Main {
    private static int n;
    private static int w;
    private static int [] volumn;
    private static List<Integer> bucket;
    private static int bucket_w;
    
    public static void main(String [] args) {
        Scanner scanner = new Scanner(System.in);
        n = scanner.nextInt();
        w = scanner.nextInt();
        volumn = new int[n];
        for (int i = 0; i < n; i++) volumn[i] = scanner.nextInt();
        bucket = new ArrayList<Integer>();
        System.out.println(1 + compute(0));
    }
    
    public static int compute(int index) {
        if (bucket_w > w || index >= volumn.length) return 0;
        int res = 0;
        for (int i = index; i < volumn.length; i++) {
            if (bucket_w + volumn[i] <= w) {
                bucket_w = bucket_w + volumn[i];
                res += 1 + compute(i + 1);
                bucket_w = bucket_w - volumn[i];
            }
        }
        return res;
    }
}