
## 模块介绍

*   formatter;字符串格式化模块;即根据指定的格式将可选的参数格式为整个字符串.
*   目前支持与`printf()`兼容的 formatter;下辈子计划兼容 facebook/folly 提供的 formatter.

## API Reference: stdio_formatter.h

### AppendStdioFormat

```c++
template <typename StringType>
StringType& AppendStdioFormat(StringType &str,const char *format,...);
```

*   根据`format`参数的指示将后续可选参数格式化为字符串,并追加到`str`之后.
*   `TPARAM:StringType`;与`std::string`兼容的字符串类.
*   `RETURN`;str 的引用.

### VAppendStdioFormat

```c++
template <typename StringType>
StringType& VAppendStdioFormat(StringType &str,const char *f,va_list ap);
```

*   同`AppendStdioFormat()`一致,只不过使用`va_list`来代替可变参数.

### CreateStringFromStdioFormat

```c++
template <typename StringType>
StringType CreateStringFromStdioFormat();
```

*   使用`StringType{}`构建一个`StringType`对象.
*   `TPARAM:StringType`;与`std::string`兼容的字符串类.

```c++
template <typename StringType>
StringType CreateStringFromStdioFormat(const char *format,...);
```

*   根据`format`的指示将后续参数格式化为一个字符串.并根据该字符串构建一个`StringType`对象,然后
    返回该对象.
*   `TPARAM:StringType`;与`std::string`兼容的字符串类.

