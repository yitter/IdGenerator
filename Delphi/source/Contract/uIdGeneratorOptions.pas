unit uIdGeneratorOptions;

interface

uses
  System.DateUtils, System.SysUtils;

type
  TIdGeneratorOptions = class
  private
    FMethod: SmallInt;
    // FBaseTime: TDateTime;
    FBaseTime: Int64;
    FWorkerId: Word;
    FWorkerIdBitLength: Byte;
    FSeqBitLength: Byte;
    FMaxSeqNumber: Integer;
    FMinSeqNumber: Word;
    FTopOverCostCount: Integer;
    FDataCenterId: Cardinal;
    FDataCenterIdBitLength: Byte;
    FTimestampType: Byte;
  public
    /// <summary>
    /// 雪花计算方法
    /// （1-漂移算法|2-传统算法），默认1
    /// </summary>
    property Method: SmallInt read FMethod write FMethod default 1;
    /// <summary>
    /// 基础时间（UTC格式）
    /// 不能超过当前系统时间
    /// </summary>
    // property BaseTime: TDateTime read FBaseTime write FBaseTime;
    property BaseTime: Int64 read FBaseTime write FBaseTime;
    /// <summary>
    /// 机器码
    /// 必须由外部设定，最大值 2^WorkerIdBitLength-1
    /// </summary>
    property WorkerId: Word read FWorkerId write FWorkerId default 0;
    /// <summary>
    /// 机器码位长
    /// 默认值6，取值范围 [1, 15]（要求：序列数位长+机器码位长不超过22）
    /// </summary>
    property WorkerIdBitLength: Byte read FWorkerIdBitLength write FWorkerIdBitLength default 6; // 10;
    /// <summary>
    /// 序列数位长
    /// 默认值6，取值范围 [3, 21]（要求：序列数位长+机器码位长不超过22）
    /// </summary>
    property SeqBitLength: Byte read FSeqBitLength write FSeqBitLength default 6; // 10;
    /// <summary>
    /// 最大序列数（含）
    /// 设置范围 [MinSeqNumber, 2^SeqBitLength-1]，默认值0，表示最大序列数取最大值（2^SeqBitLength-1]）
    /// </summary>
    property MaxSeqNumber: Integer read FMaxSeqNumber write FMaxSeqNumber default 0;
    /// <summary>
    /// 最小序列数（含）
    /// 默认值5，取值范围 [5, MaxSeqNumber]，每毫秒的前5个序列数对应编号0-4是保留位，其中1-4是时间回拨相应预留位，0是手工新值预留位
    /// </summary>
    property MinSeqNumber: Word read FMinSeqNumber write FMinSeqNumber default 5;
    /// <summary>
    /// 最大漂移次数（含），
    /// 默认2000，推荐范围500-10000（与计算能力有关）
    /// </summary>
    property TopOverCostCount: Integer read FTopOverCostCount write FTopOverCostCount default 2000;
    /// <summary>
    /// 数据中心ID（默认0）
    /// </summary>
    property DataCenterId: Cardinal read FDataCenterId write FDataCenterId default 0;
    /// <summary>
    /// 数据中心ID长度（默认0）
    /// </summary>
    property DataCenterIdBitLength: Byte read FDataCenterIdBitLength write FDataCenterIdBitLength default 0;
    /// <summary>
    /// 时间戳类型（0-毫秒，1-秒），默认0
    /// </summary>
    property TimestampType: Byte read FTimestampType write FTimestampType default 0;

    constructor Create(); overload;
    constructor Create(WorkerId: Word); overload;
  end;

implementation

{ TIdGeneratorOptions }

constructor TIdGeneratorOptions.Create(WorkerId: Word);
begin
  FBaseTime := 1582136402000; // EncodeDateTime(2020, 2, 20, 2, 20, 2, 20);
  FWorkerId := WorkerId;
end;

constructor TIdGeneratorOptions.Create();
begin
  FBaseTime := 1582136402000; // EncodeDateTime(2020, 2, 20, 2, 20, 2, 20);
end;

end.
