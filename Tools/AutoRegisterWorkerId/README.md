# idgenerator

## 关于AutoRegisterWorkerId


## 自动注册WorkerId

🔍 唯一ID生成器，依赖WorkerId，当业务服务需要水平无差别复制（自动扩容）时，这就要求能自动注册全局唯一WorkerId，然后才能生产唯一ID。

🔍 本算法提供开源动态库（go语言实现），能在容器 k8s 等容器环境下，通过 redis 自动注册 WorkerId。

🔍 通过redis注册WorkerId，并非唯一方法。你还可以开发中心化的配置服务，各端点服务启动时，通过中心服务获取唯一 WorkerId。

🔍 当然，如果你的服务无需自动扩容，那就不必自动注册WorkerId，而是为它们分别设置全局唯一值。

🔍 发挥你的想象力，方法还有很多。此处抛砖引玉：开发中心化的ID生成服务，由它为各端点服务（单个或批量）生成可用ID。


#### 自动注册流程图

图片链接：https://github.com/yitter/IdGenerator/blob/master/Tools/AutoRegisterWorkerId/regprocess.jpg

源码路径：/Go/source/regworkerid/reghelper.go

#### 动态库下载

下载链接：https://github.com/yitter/IdGenerator/releases/download/reg_v1.0/regworkerid_lib_v1.0.zip

#### 动态库接口定义
```
// 注册一个 WorkerId，会先注销所有本机已注册的记录
// ip: redis 服务器地址
// port: redis 端口
// password: redis 访问密码，可为空字符串“”
// maxWorkerId: 最大 WorkerId
extern GoInt32 RegisterOne(char* ip, GoInt32 port, char* password, GoInt32 maxWorkerId);

// 注销本机已注册的 WorkerId
extern void UnRegister();

// 检查本地WorkerId是否有效（0-有效，其它-无效）
extern GoInt32 Validate(GoInt32 workerId);
```

