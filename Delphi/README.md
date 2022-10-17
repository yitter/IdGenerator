# ❄ idgenerator-Delphi

## 编译环境、运行环境

Delphi XE10.3.3

## 调用示例（Delphi）

第1步，**全局**初始化（创建只执行一次）：

```objectpascal
// 添加引用
uses uIdGeneratorOptions, uIIdGenerator, uDefaultIdGenerator, uYitIdHelper;

// 声明IdGeneratorOptions、YitIdHelper对象为全局变量
var
  IdGeneratorOption: TIdGeneratorOptions;
  YitIdHelper: TYitIdHelper;

// 创建并配置雪花算法参数：
begin
  IdGeneratorOption := TIdGeneratorOptions.Create;
  // 参数参考IdGeneratorOptions定义。
  with IdGeneratorOptiondo
  begin
    //以下全部为默认参数
    Method := 1;
    // BaseTime := DateTime.Now.AddYears(-10);
    WorkerId := 1;

    WorkerIdBitLength := 6;
    SeqBitLength := 6;

    MaxSeqNumber := 0;
    MinSeqNumber := 5;

    TopOverCostCount := 2000;

    DataCenterId := 0;
    DataCenterIdBitLength := 0;

    TimestampType := 0;
  end;

  YitIdHelper := TYitIdHelper.Create;
  // 保存参数（务必调用，否则参数设置不生效）：
  YitIdHelper.SetIdGenerator(options);

  // 以上过程只需全局一次，且应在生成Id之前完成。
end;
```

第2步，生成ID：

```objectpascal
// 初始化后，在任何需要生成Id的地方，调用以下方法：
var Id: Int64 := YitIdHelper.NextId();
```

第3步，释放内存（销毁只执行一次）：

```objectpascal
YitIdHelper.Free;
IdGeneratorOption.Free;
```

## 代码贡献者
塵封追憶(Delphi-asdf, 82257695)


