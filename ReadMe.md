
## 简介

*   公共库.存放着一些库之类的东西

## 组织

*   内部采用模块化设计,每一个模块具有以下属性:

    -   名字;指定了模块的名字.
    -   路径,文件列表;路径为模块路径相对于 common 项目的相对路径;若模块的文件直接放在于 common
        根目录,则文件列表中记录了模块所包括的文件.
    -   依赖;存放着模块依赖的模块列表.

*   如何使用头文件;`#include "PATH"`;`PATH` 是模块文件相对于 common 项目的相对路径.这样有很
    多好处,慢慢体会 T_t.

### 关于参数检查

*   在模块最外层的接口总是执行参数检查;不过可以在接口后追加'_on_trusted'或'OnTrusted'来调用
    不进行检查的版本.

*   在模块内层的接口不执行参数检查;因为这里由外层接口来保障.

## 如何使用

*   确定所使用模块之后,根据模块的路径属性;将模块包括的文件拷贝到自己项目中,再根据模块的依赖属性将递
    归执行上述操作.

*   module_describe.json;记录了当前已经存在的模块,以及她们的一些信息.

## 在 Android 中使用

*   这里,我感觉构建系统还不是很成熟,不过可以用.

### 实现

1.  每一个 module 都有一个对应的 Android.mk,描述了如何将当前 module 构建为一个静态文件.

    -   若上层应用需要将 module 构建为'.so',则可以写一个 wrapper 将 module 生成的静态文件
        再生成一个 so;

2.  module 对应的 Android.mk 具有如下定式:

    ```makefile
    # 用来保存 LOCAL_PATH 的值不变.
    LOCAL_PATH_BAK := $(LOCAL_PATH)
    LOCAL_PATH := $(call my-dir)

    include $(CLEAR_VARS)

    # module 名称.
    LOCAL_MODULE := pp_qq_common_digest

    # 这里若上层应用可以配置编译 module 时一些设置.
    ifdef $(LOCAL_MODULE)_c_includes
    LOCAL_C_INCLUDES := $($(LOCAL_MODULE)_c_includes)
    endif

    ifdef $(LOCAL_MODULE)_cflags
    LOCAL_CFLAGS := $($(LOCAL_MODULE)_cflags)
    endif

    ifdef $(LOCAL_MODULE)_cppflags
    LOCAL_CPPFLAGS := $($(LOCAL_MODULE)_cppflags)
    endif

    ifdef $(LOCAL_MODULE)_export_cflags
    LOCAL_EXPORT_CFLAGS := $($(LOCAL_MODULE)_export_cflags)
    endif

    ifdef $(LOCAL_MODULE)_export_cppflags
    LOCAL_EXPORT_CPPFLAGS := $($(LOCAL_MODULE)_export_cppflags)
    endif

    ifdef $(LOCAL_MODULE)_export_c_includes
    LOCAL_EXPORT_C_INCLUDES := $($(LOCAL_MODULE)_export_c_includes)
    endif

    # Android NDK 构建系统允许 LOCAL_SRC_FILES 为空,所以若一个 module 全为头文件,则这里
    # 为空就行.
    LOCAL_SRC_FILES :=
    LOCAL_CPP_FEATURES := rtti exceptions

    # 指定了 module 依赖于哪些其他的 module.
    LOCAL_STATIC_LIBRARIES := pp_qq_common_exception

    include $(BUILD_STATIC_LIBRARY)


    LOCAL_PATH := $(LOCAL_PATH_BAK)
    ```

### 使用

*   首先这里有一个[CommonInAndroidDemo][0],演示如何在 Android NDK 中使用 common.不过常
    规步骤为下:

1.  将所需要的 module,以及该 module 依赖的 module 的 Android.mk include 进来;当然在
    include 之前应该配置好编译环境(如:pp_qq_common_common_c_includes 之类).

2.  然后就可以通过`LOCAL_STATIC_LIBRARIES`,`LOCAL_SHARED_LIBRARIES`来引用 module 了.

    -   不过注意了,当一个 static library 在多个 so 中都存在副本,则行为未定义.



[0]: <https://github.com/pp-qq/CommonInAndroidDemo>
