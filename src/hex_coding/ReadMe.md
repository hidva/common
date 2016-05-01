
## 模块介绍

*   Hex编解码;用于将二进制内容编码为16进制字符串的形式;以及将16进制字符串的形式解码为2进制形式.

## API Reference: hex_coding.h

### HexEncode

```c++
size_t HexEncode(char *buf,size_t max_size,const void *data,size_t size);
```

*   将`[data,data + size)`确定的二进制内容编码成16进制字符串之后存放在`[buf,buf + max_size)`
    确定的缓冲区中.
*   `RETURN`;返回写入到`buf`缓冲区的字节数.


```c++
template <typename StringType>
StringType& HexEncode(StringType &str,const void *data,size_t size);
```

*   将`[data,data + size)`确定的二进制内容编码成16进制字符串之后,追加到`str`之后.

```c++
template <typename StringType>
inline StringType HexEncode(const void *data,size_t size);
```

*   将`[data,data + size)`确定的二进制内容编码成16进制字符串之后,并据此创建一个`StringType`
    对象存放转化后的字符串.


### HexDecode

```c++
size_t HexDecode(void *data,size_t max_size,const char *buf,size_t size);
```

*   将`[buf,buf + size)`确定的16进制字符串解码成二进制之后存放在`[data,data + max_size)`
    确定的缓冲区中.
*   `RETURN`;写入到`data`指向缓冲区的字节数.


```c++
template <typename StringType>
StringType& HexDecode(StringType &str,const char *buf,size_t size);
```

*   将`[buf,buf + size)`确定的16进制字符串解码成二进制之后追加到`str`中.

```c++
template <typename StringType>
StringType HexDecode(const char *buf,size_t size);
```
*   将`[buf,buf + size)`确定的16进制字符串解码成二进制,并据此创建一个`StringType`对象来存放
    结果.

```cpp
template <typename StringType>
StringType HexDecode(const char *buf);
```    
*   等同于`HexDecode(buf,strlen(buf))`;
    
