
## 模块介绍

*   rrid;Resource Release Is Destruction 资源释放即析构;其主要思想就是在析构函数中完成对资
    源的释放.

*   rrid 模块,很大程度上参考了[facebook/folly](https://github.com/facebook/folly);

## 模块使用

### 在析构函数中释放资源

#### API Reference

##### ScopeGuard

###### Dismiss()

```c++
void Dismiss();
```

*   将当前`ScopeGuard`对象设置为 dismiss!即在对象被析构时不会调用回调函数.
*   该函数只需调用一次即可,并且多次调用与一次调用效用相同!

##### MakeScopeGuard()

```c++
template <typename FunctionType>
ScopeGuard
MakeScopeGuard(FunctionType&& fn);
```

*   `ScopeGuard` Helper 方法,很方便地可以构造一个`ScopeGuard`对象. 
*   `PARAM:fn`;回调函数,其调用形式为下:

    ```c++
    void fn();
    ```
*   `RETURN`;使用`auto`来保存返回值,如:`auto scope_object = MakeScopeGuard(...);`

##### ON_SCOPE_EXIT()

```c++
#define ON_SCOPE_EXIT(object_name)

ON_SCOPE_EXIT(object_name) {
    回调函数体;
}; // 不要忘记这里的分号!
```

*   通过与 lambda 表达式的协同作战可以更加方便的创建`ScopeGuard`对象.
*   `PARAM:object_name`;指定了`ScopeGuard`对象的对象标识符.

#### Demo

```c++
void OpenAlogFile()
{
    int fd = open(path1,O_RDONLY);
    ON_SCOPE_EXIT (close_file1) {
        close(fd);
    };
    if (fd < 0) {
        close_file1.Dismiss(); // 此时不需要调用回调.
        return ;
    }
    
    int fd1 = open(path2,O_RDWR);
    ON_SCOPE_EXIT (close_file2) {
        close(fd1);
    };
    if (fd1 < 0) {
        close_file2.Dismiss();
        return ;
    }
    
    int fd2 = open(path3,O_RDONLY);
    auto close_file3 = MakeScopeGuard([&] () -> void { close(fd2); });
    if (fd2 < 0) {
        close_file3.Dismiss();
        return ;
    }
    return ;
}
```
