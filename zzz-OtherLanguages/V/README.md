#  ❄ idgenerator-V

## 介绍

项目更多介绍参照：https://github.com/yitter/idgenerator

## 运行环境

```sh
$ v --enable-globals run test.v
```

## 调用示例（V）

第1步，**全局** 初始化（应用程序启动时执行一次）：
```v ignore
// 定义全局变量
__global ( idgen gen.YitIdHelper ) //定义全局变量

// 使用默认参数配置
idgen = gen.YitIdHelper{
	id_gen: gen.make_generator(&contract.IdGeneratorOptions{})
}

// 更改配置参数
idgen.set_id_generator(&contract.IdGeneratorOptions{
	method: 1
	base_time: 1582136402000
	workerid_bitlength: 6
	seq_bitlength: 10
})

//以上配置全局一次
```

第2步，生成ID：
```v ignore
// 初始化以后，即可在任何需要生成ID的地方，调用以下方法：
newId := idgen.next_id()
```
