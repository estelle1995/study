java.lang.String：
```java
public final class String implements java.io.Serializable, Comparable<String>, CharSequence {
    /** String的属性值 */  
    private final char value[];

    /** The offset is the first index of the storage that is used. */
    /**数组被使用的开始位置**/
    private final int offset;

    /** The count is the number of characters in the String. */
    /**String中元素的个数**/
    private final int count;

    /** Cache the hash code for the string */
   /**String类型的hash值**/
    private int hash; // Default to 0

    /** use serialVersionUID from JDK 1.0.2 for interoperability */
    private static final long serialVersionUID = -6849794470754667710L;
    /**
     * Class String is special cased within the Serialization Stream         Protocol.
     *
     * A String instance is written into an ObjectOutputStream according to
     * <a href="{@docRoot}/../platform/serialization/spec/output.html">
     * Object Serialization Specification, Section 6.2, "Stream Elements"</a>
     */

　　private static final ObjectStreamField[] serialPersistentFields =
        new ObjectStreamField[0];
}
```