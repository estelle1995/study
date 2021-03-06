extern 关键字可应用于全局变量、函数或模板声明。它指定该符号具有外部链接。

根据上下文，extern 关键字有四种含义
1. 在非const全局变量声明中，extern 指定在另一个翻译单元中定义变量或函数。除了定义变量的文件以外，extern必须应用于所有文件中。
2. 在 const 变量声明中，它指定变量具有外部链接。 extern 必须应用于所有文件中的所有声明。 （默认情况下，全局 const 变量具有内部链接。）
3. extern "C" 指定在其他位置定义该函数，并使用 C 语言调用约定。 extern "C" 修饰符也可以应用块中多个函数声明。
4. 在模板声明中，extern 指定已在其他位置实例化模板。extern 告知编译器可以重复使用其他实例化，而不是在当前位置创建新的实例化。 有关 extern 的这种用法的详细信息，请参阅显式实例化。