
*   存放着 common 项目所有模块共享的文件.

## API Reference: preprocess.h

### PP_QQ_CONCAT_DIRECT

```c++
PP_QQ_CONCAT_DIRECT(x,y)
```
*   将`x`,`y`拼接在一起,若`x`,`y`是宏,则**不会**预先展开!.

### PP_QQ_CONCAT_WITH_EXPAND

```c++
PP_QQ_CONCAT_WITH_EXPAND(x,y)
```
*   将`x`,`y`拼接在一起,若`x`,`y`是宏,则**会**预先展开!.


### PP_QQ_ANONYMOUS_NAME

```c++
PP_QQ_ANONYMOUS_NAME(object_name)
```
*   根据`object_name`以及`PP_QQ_ANONYMOUS_NAME`在文本文件中的行号拼接成一个无意义的标识符.

#### DEMO
```c++
auto PP_QQ_ANONYMOUS_NAME(on_exception_obj) = MakeOnException(f);
```


