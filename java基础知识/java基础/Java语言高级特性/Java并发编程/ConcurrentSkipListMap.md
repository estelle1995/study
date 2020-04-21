跳跃表总结
1. 跳跃表是通过多层链表来实现的，每层都是一个有序的链表；
2. 跳跃表的最底层包含了所有的元素，其他层保存的是元素的索引节点
3. 跳跃表搜索，删除，插入元素的时间复杂度是 O(logN)
4. 跳跃表通过随机的抛硬币方式来决定新插入的节点是否提到上层作为索引节点
5. 跳跃表提高搜索效率的方式是通过空间换时间得到的。

接下来我们就来学习ConcurrentSkipListMap这个类。ConcurrentSkipListMap是一个基于skip list实现的线程安全的有序存储的Map，默认情况下根据key的自然顺序进行排序，或者根据在Map进行创建时提供的比较器进行排序。同样，该类不允许key或者value为null。我们来看一下它的实现。

# 继承结构
```java
public class ConcurrentSkipListMap<K, V> extends AbstractMap<K, V> 
    implements ConcurrentNavigableMap<K, V>, Cloneable, Serializable {

}
```
# 内部类
ConcurrentSkipListMap 中有三个比较重要的内部类，分别是Node，Index， HeadIndex 这三个类。Node表示最底层的链表节点，Index类表示基于Node类的索引层，而HeadIndex则是用来维护索引的层次。

# put方法
1. 根据 key 从跳跃表的左上方开始，向右或者向下查找到需要插入位置的前驱节点，查找过程中会删除一些已经标记为删除状态的节点。
2. 然后判断跳跃表中是否已经存在了该 key，如果存在，则CAS替换value值返回value值。
3. 生成节点，插入到最底层链表中。
4. 然后根据随机值来判断是否生成索引层以及生成索引层的层次
5. 如果需要创建索引曾，则判断索引层的层次是否超过最大的level，如果大于，需要创建 HeadIndex 索引层；否则只需要创建 Index 索引层即可。
6. 从 head 开始遍历，将每一层的新添加的 Index 插入到对应的位置。

# get方法
先找到对应的前驱节点，然后一直向右查找即可，如果查找过程中，有节点标记删除，则调用 helpDelete 删除该节点，链表结构发生了改变，重新查找。

# remove 方法
1. 先查找到对应的前驱节点，并且查找过程中会删除一些已经标记为删除状态的节点；
2. 查询到要删除的节点，通过 CAS 操作把 value 设置为 null （这样其他线程可以感知到这个节点状态，协助完成删除工作）。然后在该节点后面添加一个 marker 节点作为删除标志位，若添加成功，则将该结点的前驱的后继设置位该结点之前的后继（也就是删除该结点操作），这样可以避免丢失数据；
3. 如果该层已经没有了其他结点，调用 tryReduceLevel方法移除该层
