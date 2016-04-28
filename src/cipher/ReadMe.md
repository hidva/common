
## 模块介绍

*   加解密

## API Reference: icipher.h

### ICipher::CipherResult

```c++
struct CipherResult {
    size_t input_size;
    size_t output_size;
};
```

*   存放着一次加密(或解密)的返回结果.


### ICipher

*   `ICihper`,所有加解密算法的基类.
*   一次加解密过程;若干次(可为 0 次)的`update()`与至少一次的`final()`调用组成一次加密或解密
    过程.
*   各个加解密过程之间相互独立.
*   在一次加解密过程结束之后,可以再一次开始新一轮的加解密过程.

#### EncryptUpdate()

```c++
virtual CipherResult EncryptUpdate(void *dst,size_t max_size,const void *ptr,size_t size);
```

*   对`[ptr,ptr + size)`中的数据进行加密,结果存放到`dst,max_size`确定的输出缓冲区中.`ptr,size`
    中的数据可能会有部分不足以让底层加密算法加密(如不足一块),此时该部分数据不会被加密.
    
*   `RETURN:input_size`;指定了使用了输入缓冲区中的多少字节数;`size - input_size`是剩余未
    被加密的内容.
*   `RETURN:output_size`;指定了使用了输出缓冲区中的多少字节数;当输出缓冲区不足时,将会抛出异
    常

*   Implemention Note: 需要考虑到`dst,max_size`与`ptr,size`可能会相交的可能性!

```c++
template <typename StringType>
size_t EncryptUpdate(StringType &dst,const void *ptr,size_t size);
```

*   等同于`EncryptUpdate(void*,size_t,const void*,size_t)`,只不过是将加密结果追加到`dst`中.
*   `RETURN`;等同于`CipherResult::input_size`,因为`CipherResult::output_size`可以通
    过`dst`来计算,所以没有必要返回了.

#### EncryptFinal()

```c++
virtual CipherResult EncryptFinal(void *dst,size_t max_size,const void *ptr,size_t size);
```

*   首先调用`EncryptUpdate(dst,max_size,ptr,size)`进行加密,然后对剩下部分执行填充后加密.
*   同时该函数也有结束一次加密过程的语义!

*   `RETURN:input_size`;指定了使用输入缓冲区中多少字节数,若内部使用了填充策略,则返回填充后
    的字节数;即:`input_size - size`为被填充的字节数.
    
*   `RETURN:output_size`;指定了使用了输出缓冲区中的多少字节数;当输出缓冲区不足时,将会抛出异
    常

*   Implemention Note: 需要考虑到`dst,max_size`与`ptr,size`可能会相交的可能性!

```c++
template <typename StringType>
size_t EncryptFinal(StringType &dst,const void *ptr,size_t size);
```


#### EncryptUpdateOutputSize()

```c++
virtual CipherResult EncryptUpdateOutputSize(size_t input_size);
```

*   返回下一次调用`EncryptUpdate(dst,max_size,ptr,input_size)`时的返回值.调用者可以根据
    返回值来分配内存.
    
*   Implemention Note: 该函数不一定要返回一个精确解;若解不确定,那么返回一个最大值即可.

#### EncryptFinalOutputSize()

```c++
virtual CipherResult EncryptFinalOutputSize(size_t input_size);
```

*   返回下一次调用`EncryptFinal(dst,max_size,ptr,input_size)`时的返回值.调用者可以根据
    返回值来分配内存.
    
*   Implemention Note: 该函数不一定要返回一个精确解;若解不确定,那么返回一个最大值即可.


#### DecryptUpdateOutputSize()

```c++
virtual CipherResult DecryptUpdateOutputSize(size_t input_size);
```

*   与`EncryptUpdateOutputSize()`语义相同,只是将加密换成解密即可

#### DecryptFinalOutputSize()

```c++
virtual CipherResult DecryptFinalOutputSize(size_t input_size);
```


*   与`EncryptFinalOutputSize()`语义相同,只是将加密换成解密即可

#### DecryptUpdate()

```c++
virtual CipherResult DecryptUpdate(void *dst,size_t max_size,const void *ptr,size_t size);

template <typename StringType>
size_t DecryptUpdate(StringType &dst,const void *ptr,size_t size);
```

*   与`EncryptUpdate()`语义相同,只是将加密换成解密即可




#### DecryptFinal()

```c++
virtual CipherResult DecryptFinal(void *dst,size_t max_size,const void *ptr,size_t size);

template <typename StringType>
size_t DecryptFinal(StringType &dst,const void *ptr,size_t size);
```

*   与`EncryptFinal()`语义相同,只是将加密换成解密即可

