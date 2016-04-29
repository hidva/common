
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


## API Reference: counter.h

### Counter


*   计数器加密模式中,计数器的基类.
*   默认实现的计数器就是一个长度为 size 字节的无符号整数,采用大端模式存储.可以根据需要实现自己
    的计数器.


#### 构造函数

```c++
Counter();
```

*   构造一个`skip_off_`为`0`的计数器.

#### Begin()

```cpp
inline void Begin(void *ptr,size_t n) const;
```

*   当需要使用 CTR 加密时,加解密模块会为计数器分配内存,然后调用`Begin(ptr,n)`初始化这段内存,
    此后就可以直接使用初始化后的结果来加密"本次加密"的第一个数据块.
*   当然在调用`Begin()`填充计数器值之前,应该调用`Skip()`跳过已经加密过的数据块.

#### Skip()

```c++
inline void Skip(int n) noexcept;
```

*   指定跳过接下来`n`个数据块的加密.
*   首先在"一次"(逻辑上) CTR 加密中,可能会加密多个数据块,如下:

    ```
    +-----------------------------------------------------------------+
    |   0   |   1   |   2   |   3   |   4   |   5   |   6   |    7    |
    +-----------------------------------------------------------------+
    ```

    这个逻辑上是一次的加密,实际上可能会分成若干次 CTR 加密过程,每次加密一小段,如上可能会使用 2
    次加密过程,第一次加密 [0,1,2,3);第 2 次加密 [3,4,5,6,7].在第 2 次加密时,需要跳过前 3
    个数据块,即首先连续调用 3 次 increase(ctr).

*   `Skip()`指定了在开始使用计数器之前先跳过多少个数据段;如下,上述加密过程可以实现为:

    ```cpp
    Counter counter;
    // 使用 counter.begin(),counter.inc() 加密 0,1,2 三个数据块;

    // 开始第 2 次加密,首先跳过前 3 个数据块;这样 counter.begin() 返回的结果就是加密第 4
    // 个时的计数器值.
    counter.Skip(3);
    ```


#### Inc()

```cpp
inline void Inc(void *ptr,size_t n) const;
```

*   自增`[ptr,ptr + n)`表示的计数器


#### Init()

```cpp
virtual void Init(void *ptr,size_t n) const;
```

*   初始化计数器,得到的计数器用于加密逻辑上第一个数据块.

#### Increase()

```cpp
virtual void Increase(void *ptr,size_t n,int step) const;
```

*   将`ptr,n`表示的计数器加`step`步骤;若`step`为负数,则相当于减去`abs(step)`.


## API Reference: block_cipher.h

### BlockCipher

*   `BlockCipher`;用于块加解密;`BlockCipher`中并没有关于加解密的具体实现;其更像是胶水;将
    底层的块加密算法,加密模式,填充模式粘合在一起,然后提供加解密功能.如下对`BlockCipher`的使用
    案例:

    ```c++
    // 当需要使用 AES 以 CBC 工作模式来进行加解密时

    // 首先构造底层块加解密算法;
    AESCipher aes_cipher;
    aes_cipher.SetKey(key_ptr,key_size /* 32 */);

    // 再构造工作模式对象;
    CBCCipherMode cbc_mode;
    cbc_mode.SetIV(iv_ptr,iv_size);

    // 然后构造一个 BlockCipher 对象将 aes_cipher,cbc_mode 粘合起来提供加解密功能.
    BlockCipher aes_cbc_cipher(&aes_cipher,&cbc_mode);

    // 最后就可以使用 aes_cbc_cipher 进行 AES,CBC 模式进行加密
    aes_cbc_cipher.EncryptUpdate(...);
    aes_cbc_cipher.EncryptUpdate(...);
    aes_cbc_cipher.EncryptFinal(...);

    // 或者解密
    aes_cbc_cipher.DecryptUpdate(...);
    aes_cbc_cipher.DecryptUpdate(...);
    aes_cbc_cipher.DecryptFinal(...);

    // 如果需要更换密钥:
    aes_cipher.SetKey(new_key_ptr,new_key_size);

    // 或者需要更换 IV:
    cbc_mode.SetIV(iv_ptr,iv_size);

    // 然后再进行加解密时,就会使用已经更改过的密钥或者 IV 了.
    ```

    从上述例子可以看出:

    1.  BlockCipher 内部保存的只是块加密,工作模式基类的指针;然后指针指向着真正的实现;(话说
        如果这里很多地方都是虚函数的调用形式;所以如果开销过大的话,可以利用模板来实现.有空再以
        模板的形式实现一份,然后基准测试一下).

    2.  BlockCipher 并没有提供参数设置与获取接口;而是有具体的块加密算法,工作模式实现来提供的.
        毕竟不同的工作模式倒是需要不同的参数,如 CTR 工作模式可能需要计数器的实现(参见`Counter`),
        而 CBC 工作模式则需要 IV.

*   如何实现一个可以作为`BlockCipher`参数的块加密,继承`BlockCipherImpl`即可.
*   如何实现一个可以作为`BlockCipher`参数的工作模式,继承`BlockCipherMode`即可.
*   如何实现一个可以作为`BlockCipher`参数的填充模式,继承`BlockCipherPaddingMode`即可.

#### 构造函数

```c++
BlockCipher(BlockCipherImpl *cipher_impl,BlockCipherMode *mode_impl,BlockCipherPaddingMode *pading_impl);
```

*   构造一个`BlockCipher`对象.


### BlockCipherMode

*   工作模式 Wrapper;要想使一个工作模式可以被`BlockCipher`粘合,则需要继承该类并实现相应接口.

#### IsNeedPadding()

```c++
virtual bool IsNeedPadding() const noexcept;
```

*   若返回`true`,则表明当前工作模式需要填充,如 cbc,ecb 等;若返回`false`,则表明当前工作模式
    不需要填充,而 ctr,ofb,cfb 等.

#### OnCipherBegin()

```c++
virtual void OnCipherBegin(BlockCipher *block_cipher);
```

*   仅在一次加密(或解密)过程开始时调用;负责初始化工作模式.
*   此时也需要初始化对应的加密算法;如: CTR 模式中无论是加密,还是解密都需要将加密算法初始化为加
    密模式.


*   Implemention Note: 在调用该函数时,`block_cipher`对象已经被初始化

#### Update()

```c++
virtual void Update(BlockCipher *block_cipher,void *dst,const void *src,size_t size);
```

*   对`[src,src + size)`确定的缓冲区进行加密(或解密),并且将结果输出到`dst`指向的缓冲区中.

*   `PARAM:size`;
    -   若本次工作模式需要填充,则`size`一定是数据块长的整数倍;
    -   若当前工作模式不需要填充,则`size`可能是块长的整数倍;或者是不足一块

*   `PARAM: dst`;指向着输出缓冲区,长度为`size`;`dst`可能与`src`相同,指向着同一个缓冲区,但
    绝不会重叠.

*   Implemention Note: 当`size`不足一块时,此时也有一次加密(或解密)过程要结束的语义!实现可以
    据此干点优化啥的.

#### OnCipherEnd()

```cpp
virtual void OnCipherEnd(BlockCipher *block_cipher);
```

*   在一次加密(或解密)过程结束时调用.

*   Implemention Note: 在调用该函数之后,`block_cipher`本身才会执行逆初始化过程.


### BlockCipherImpl

#### SetKey

```cpp
virtual void SetKey(const void *key,size_t size);
```

*   设置密钥

#### kEncryptMode,kDecryptMode,SetKey,InitMode

```cpp
enum {
    kEncryptMode,
    kDecryptMode
};


virtual void InitMode(int mode);
```

*   将当前加解密算法的模式初始化为加密模式,或者解密模式.

*   Not important: 本来我认为加解密想下面这样使用就行:

    ```cpp
    BlockCipherImpl cipher;
    cipher.SetKey(key_ptr,key_size);
    cipher.Encrypt(out,in);
    cipher.Encrypt(out,in);
    ```

    然后看了一下,openssl,nettle 这些加解密库中对加解密算法的接口是这样的:

    ```c
    AES_key aes_ctx;
    aes_set_encrypt_key(&aes_ctx,key_ptr,key_size);
    // 然后使用 aes_ctx 进行加密;

    aes_set_decrypt_key(&aes_ctx,key_ptr,key_size);
    // 然后使用 aes_ctx 进行解密.
    ```

    也就是说在进行加解密之前还得初始化一下.

#### Encrypt,Decrypt

```cpp
virtual void Encrypt(void *dst,const void *in);
virtual void Decrypt(void *dst,const void *in);
```

*   对一块数据进行加密,或者解密.

#### GetBlockSize()

```cpp
virtual unsigned GetBlockSize() const;
```

*   返回当前加密算法支持数据块的长度.

### BlockCipherPaddingMode

*   需要现在一直没用过填充模式,所以对填充模式不熟,等有时间再添加对填充模式的支持.


