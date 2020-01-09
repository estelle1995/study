Collection 的集合保存数据的目的是为了输出， Map 集合保存数据的目的是为了进行 key 的查找

# Map 接口
public interface Map<K,V>
1. V put(K key, V value)     返回重复的时候的旧数据，如果key不存在的话返回 null
2. V get(Object key)
3. Set<Map.Entry<K, V>> entrySet()   将 Map 集合转为 Set 集合
4. public boolean containsKey(Object key)
5. public boolean containsValue(Object value)
6. Set<K> keySet()  将 Map 集合中的 key 转为 Set 集合
7. public V remove(Object key) 

最常用的子类： HashMap, HashTable, TreeMap, LinkedHashMap

## HashMap
该类的主要特点是无序存储

public class HashMap<K, V> extends AbstractMap<k, V> implements Map<K, V>, Cloneable, Serializable

源码如下：
```java
static final int TREEIFY_THRESHOLD = 8;
static final int DEFAULT_INITIAL_CAPACITY = 1 << 4;
static final float DEFAULT_LOAD_FACTOR = 0.75f;
public HashMap() {
    this.loadFactor = DEFAULT_LOAD_FACTOR; // all other fields defaulted
}
public V put(K key, V value) {
    return putVal(hash(key), key, value, false, true);
}
final V putVal(int hash, K key, V value, boolean onlyIfAbsent,
                   boolean evict) {
        Node<K,V>[] tab; Node<K,V> p; int n, i;
        if ((tab = table) == null || (n = tab.length) == 0)
            n = (tab = resize()).length;
        if ((p = tab[i = (n - 1) & hash]) == null)
            tab[i] = newNode(hash, key, value, null);
        else {
            Node<K,V> e; K k;
            if (p.hash == hash &&
                ((k = p.key) == key || (key != null && key.equals(k))))
                e = p;
            else if (p instanceof TreeNode)
                e = ((TreeNode<K,V>)p).putTreeVal(this, tab, hash, key, value);
            else {
                for (int binCount = 0; ; ++binCount) {
                    if ((e = p.next) == null) {
                        p.next = newNode(hash, key, value, null);
                        if (binCount >= TREEIFY_THRESHOLD - 1) // -1 for 1st
                            treeifyBin(tab, hash);
                        break;
                    }
                    if (e.hash == hash &&
                        ((k = e.key) == key || (key != null && key.equals(k))))
                        break;
                    p = e;
                }
            }
            if (e != null) { // existing mapping for key
                V oldValue = e.value;
                if (!onlyIfAbsent || oldValue == null)
                    e.value = value;
                afterNodeAccess(e);
                return oldValue;
            }
        }
        ++modCount;
        if (++size > threshold)
            resize();
        afterNodeInsertion(evict);
        return null;
    }
```
在使用 put方法进行数据保存的时候会调用一个 putVal() 方法， 同时会将 key 进行 hash 处理（生成一个 hash 码），而对于 putVal 方法里面提供一个 Node<K, V> 进行数据保存，而在使用 putVal() 方法操作的过程之中会调用一个 resize() 方法可以进行容量的扩充

基于Map接口实现、允许null键/值、非同步(synchronized)、不保证有序(比如插入的顺序)、也不保证序不随时间变化


## LinkedHashMap
public class LinkedHashMap<K,V> extends HashMap<K,V> implements Map<K,V>   

LinkedhashMap底层是散列表+双向链表实现的，在保存（每次调用put函数时）数据时，新数据会被插入到链表尾部。

一般在使用 LinkedHashMap 类的时候往往数据量都不要特别大，因为会造成时间复杂度攀升

当使用了 LinkedHashMap 存储之后所有的数据保存数据为添加顺序。

## Hashtable 子类
Hashtable 类是从 JDK1.0 的时候提供的，与 Vector、 Enumeration 属于最早的一批动态数组的实现类，后来为了将其继续实现下来，让其实现了 Map 接口

public class Hashtable<K, V> extends Dictionary<K, V> implements Map<K, V>, Cloeable, Serializable

Hashtable 中不允许 key， value值为空，否则会出现 NullPointerException 的异常


## Map.Entry 接口
HashMap 中的静态内部类 Node 实现了 Map.Entry 接口  static class Node <K, V> implements Map.Entry<K, V> {}
![](assets/Map_Entry继承结构.png)
public static interface Map.Entry<K,V> 并且在这个接口里面提供有两个重要的方法：
1. 获取 key： public K getKey()
2. 获取value： public V getValue()

在 JDK 1.9 以前的开发之中，使用者基本上都不会去考虑创建 Map.Entry 的对象，实际上在正常的开发过程之中，使用者也不需要关心 Map.Entry 的创建，可是从 jdk1.9 之后 Map 接口中追加有 public static <K, V> Map.Entry<K, V> entry(K k, V, v)

Map.Entry 的主要作用就是作为一个 Key 和 Value 的包装类型使用

### 遍历输出Map集合
1. HashMap
```java
   public static void main(String[] args) {
		Map<Integer, String> map = new HashMap<Integer, String>();
		map.put(1, "hello");
		map.put(null, "world");
		map.put(0, "mldn");
		map.put(3, null);
		map.put(2, "cn");
		Set<Map.Entry<Integer, String>> set = map.entrySet();
		for (Map.Entry<Integer, String> entry : set) {
			System.out.println(entry.getKey() + ", " + entry.getValue());
		}
		System.out.println();
		set.forEach((entry)->{
			System.out.println(entry.getKey() + ", " + entry.getValue());
		});
		System.out.println();
		Iterator<Map.Entry<Integer, String>> iter = set.iterator();
		while (iter.hasNext()) {
			Map.Entry<Integer, String> entry = iter.next();
			System.out.println(entry.getKey() + ", " + entry.getValue());
		}
	}
```

2. HashTable
```java
	public static void main(String[] args) {
		Hashtable<Integer, String> map = new Hashtable<Integer, String>();
		map.put(1, "hello");
//		map.put(null, "world");
		map.put(0, "mldn");
//		map.put(3, null);
		map.put(2, "cn");
		Set<Map.Entry<Integer, String>> set = map.entrySet();
		for (Map.Entry<Integer, String> entry : set) {
			System.out.println(entry.getKey() + ", " + entry.getValue());
		}
		System.out.println();
		set.forEach((entry)->{
			System.out.println(entry.getKey() + ", " + entry.getValue());
		});
		System.out.println();
		Iterator<Map.Entry<Integer, String>> iter = set.iterator();
		while (iter.hasNext()) {
			Map.Entry<Integer, String> entry = iter.next();
			System.out.println(entry.getKey() + ", " + entry.getValue());
		}
		System.out.println();
		Enumeration<Integer> keys = map.keys();
		Enumeration<String> values = map.elements();
		while (keys.hasMoreElements()) {
			System.out.print(keys.nextElement() + ",");
		}
		System.out.println();
		while (values.hasMoreElements()) {
			System.out.print(values.nextElement() + ",");
		}
	}
```

