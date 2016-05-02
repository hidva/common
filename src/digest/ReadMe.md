
## 模块介绍

*   摘要计算

## API Reference: digest.h

### IDigest

*   一次摘要计算过程分为若干次(至少 1 次)`Update()`调用与 1 次`Final()`组成.
*   在一次摘要计算过程之后又可以开始下一次摘要计算过程,各个摘要计算过程之间相互独立
*   在任何时候都可以调用`Reset()`开启一段新的摘要计算过程.


#### Update()

```cpp
void Update(const void *ptr,size_t s);
```

#### Final()

```cpp
void Final(void *dst,size_t max_size);
```

*   计算已经通过`Update()`添加的数据的摘要.并将摘要值存放到`dst,max_size`确定的缓冲区中.
*   若`max_size < GetDigestSize()`,则截取摘要值的前`max_size`存放到`dst`中.

#### Reset()

```cpp
void Reset();
```

#### GetDigestSize()

```cpp
int GetDigestSize() const;
```

*   返回摘要值的长度,以字节为单位.


