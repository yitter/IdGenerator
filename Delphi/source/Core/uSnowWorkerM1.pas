unit uSnowWorkerM1;

interface

uses
  uISnowWorker, uIdGeneratorOptions, System.SyncObjs, uTOverCostActionArg, System.DateUtils, System.SysUtils;

/// <summary>
/// 雪花漂移算法
/// </summary>
type
  TSnowWorkerM1 = class(TInterfacedObject, ISnowWorker)
  private
    // private static long _StartTimeTick = 0;
    // private static long _BaseTimeTick = 0;
  protected
    SyncLock: TCriticalSection;
  protected
    // FBaseTime: TDateTime;
    FBaseTime: Int64;
    FWorkerId: Word;
    FWorkerIdBitLength: Byte;
    FSeqBitLength: Byte;
    FMaxSeqNumber: Integer;
    FMinSeqNumber: Word;
    FTopOverCostCount: Integer;
    //
    FTimestampShift: Byte;
    FCurrentSeqNumber: Word;
    FLastTimeTick: Int64;
    FTurnBackTimeTick: Int64;
    FTurnBackIndex: Byte;
    FIsOverCost: Boolean;
    FOverCostCountInOneTerm: Integer;
    //
    FGenCountInOneTerm: Integer;
    FTermIndex: Integer;
  protected
    /// <summary>
    /// 基础时间
    /// </summary>
    // property BaseTime: TDateTime read FBaseTime write FBaseTime;
    property BaseTime: Int64 read FBaseTime;
    /// <summary>
    /// 机器码
    /// </summary>
    property WorkerId: Word read FWorkerId default 0;
    /// <summary>
    /// 机器码位长
    /// </summary>
    property WorkerIdBitLength: Byte read FWorkerIdBitLength default 0;
    /// <summary>
    /// 自增序列数位长
    /// </summary>
    property SeqBitLength: Byte read FSeqBitLength default 0;
    /// <summary>
    /// 最大序列数（含）
    /// </summary>
    property MaxSeqNumber: Integer read FMaxSeqNumber default 0;
    /// <summary>
    /// 最小序列数（含）
    /// </summary>
    property MinSeqNumber: Word read FMinSeqNumber default 0;
    /// <summary>
    /// 最大漂移次数（含）
    /// </summary>
    property TopOverCostCount: Integer read FTopOverCostCount write FTopOverCostCount default 0;

    //
    property TimestampShift: Byte read FTimestampShift write FTimestampShift default 0;

    //
    property CurrentSeqNumber: Word read FCurrentSeqNumber write FCurrentSeqNumber;
    property LastTimeTick: Int64 read FLastTimeTick write FLastTimeTick default 0; // -1L
    property TurnBackTimeTick: Int64 read FTurnBackTimeTick write FTurnBackTimeTick default 0; // -1L;
    property TurnBackIndex: Byte read FTurnBackIndex write FTurnBackIndex default 0;
    property IsOverCost: Boolean read FIsOverCost write FIsOverCost default False;
    property OverCostCountInOneTerm: Integer read FOverCostCountInOneTerm write FOverCostCountInOneTerm default 0;
{$IFDEF DEBUG}
    property GenCountInOneTerm: Integer read FGenCountInOneTerm write FGenCountInOneTerm default 0;
    property TermIndex: Integer read FTermIndex write FTermIndex default 0;
{$ENDIF}
  protected
{$IFDEF DEBUG}
    procedure DoGenIdAction(arg: TOverCostActionArg);
    procedure BeginOverCostAction(UseTimeTick: Int64);
    procedure EndOverCostAction(UseTimeTick: Int64);
    procedure BeginTurnBackAction(UseTimeTick: Int64);
    procedure EndTurnBackAction(UseTimeTick: Int64);
{$ENDIF}
    //
    function GetSecondTimeStamp(): Int64;
    function GetMillisecondTimeStamp(): Int64;
    //
    function CalcId(UseTimeTick: Int64): Int64; virtual;
    function CalcTurnBackId(UseTimeTick: Int64): Int64; virtual;
    function NextOverCostId(): Int64;
    function GetCurrentTimeTick(): Int64; virtual;
    function GetNextTimeTick(): Int64;
  public
    // Action<OverCostActionArg> GenAction { get; set; }
    function NextId(): Int64;
    function NextNormalId(): Int64;
    constructor Create(options: TIdGeneratorOptions); overload;
    destructor Destroy(); override;
  end;

implementation

{ TSnowWorkerM1 }

function TSnowWorkerM1.GetSecondTimeStamp(): Int64;
var
  ST: TDateTime;
begin
  ST := EncodeDateTime(1970, 1, 1, 0, 0, 0, 0);
  Result := MilliSecondsBetween(Now(), ST) - 28800; // 8*60*60;
end;

function TSnowWorkerM1.GetMillisecondTimeStamp(): Int64;
var
  ST: TDateTime;
begin
  ST := EncodeDateTime(1970, 1, 1, 0, 0, 0, 0);
  Result := MilliSecondsBetween(Now(), ST) - 28800000; // 8*60*60*1000;
end;

constructor TSnowWorkerM1.Create(options: TIdGeneratorOptions);
begin
  SyncLock := TCriticalSection.Create;
  // 1.BaseTime
  if (options.BaseTime <> 0) then
    FBaseTime := options.BaseTime;

  // 2.WorkerIdBitLength
  if (options.WorkerIdBitLength <> 0) then
  begin
    FWorkerIdBitLength := 6;
  end
  else
  begin
    FWorkerIdBitLength := options.WorkerIdBitLength;
  end;

  // 3.WorkerId
  FWorkerId := options.WorkerId;

  // 4.SeqBitLength
  if (options.SeqBitLength = 0) then
  begin
    FSeqBitLength := 6;
  end
  else
  begin
    FSeqBitLength := options.SeqBitLength;
  end;

  // 5.MaxSeqNumber
  if (MaxSeqNumber = 0) then
  begin
    FMaxSeqNumber := (1 shl SeqBitLength) - 1;
  end
  else
  begin
    FMaxSeqNumber := options.MaxSeqNumber;
  end;

  // 6.MinSeqNumber
  FMinSeqNumber := options.MinSeqNumber;

  // 7.Others
  FTopOverCostCount := options.TopOverCostCount;
  // if (TopOverCostCount = 0) then
  // begin
  // FTopOverCostCount := 2000;
  // end;

  FTimestampShift := Byte(WorkerIdBitLength + SeqBitLength);
  FCurrentSeqNumber := options.MinSeqNumber;

  // FBaseTimeTick = BaseTime.Ticks;
  // FStartTimeTick = (long)(DateTime.UtcNow.Subtract(BaseTime).TotalMilliseconds) - Environment.TickCount;
end;

destructor TSnowWorkerM1.Destroy();
begin
  SyncLock.Free;

  inherited;
end;

{$IFDEF DEBUG}

procedure TSnowWorkerM1.DoGenIdAction(arg: TOverCostActionArg);
begin
  // //return;
  // Task.Run(() =>
  // {
  // GenAction(arg);
  // });
end;

procedure TSnowWorkerM1.BeginOverCostAction(UseTimeTick: Int64);
begin

end;

procedure TSnowWorkerM1.EndOverCostAction(UseTimeTick: Int64);
begin

end;

procedure TSnowWorkerM1.BeginTurnBackAction(UseTimeTick: Int64);
begin

end;

procedure TSnowWorkerM1.EndTurnBackAction(UseTimeTick: Int64);
begin

end;
{$ENDIF}

function TSnowWorkerM1.NextOverCostId(): Int64;
var
  CurrentTimeTick: Int64;
begin
  CurrentTimeTick := GetCurrentTimeTick();

  if (CurrentTimeTick > FLastTimeTick) then
  begin
{$IFDEF DEBUG}
    EndOverCostAction(CurrentTimeTick);
    FGenCountInOneTerm := 0;
{$ENDIF}
    FLastTimeTick := CurrentTimeTick;
    FCurrentSeqNumber := FMinSeqNumber;
    FIsOverCost := False;
    FOverCostCountInOneTerm := 0;

    Result := CalcId(FLastTimeTick);
    Exit;
  end;

  if (FOverCostCountInOneTerm >= FTopOverCostCount) then
  begin
{$IFDEF DEBUG}
    EndOverCostAction(CurrentTimeTick);
    FGenCountInOneTerm := 0;
{$ENDIF}
    // TODO: 在漂移终止，等待时间对齐时，如果发生时间回拨较长，则此处可能等待较长时间。
    // 可优化为：在漂移终止时增加时间回拨应对逻辑。（该情况发生概率低，暂不处理）
    FLastTimeTick := GetNextTimeTick();
    FCurrentSeqNumber := FMinSeqNumber;
    FIsOverCost := False;
    FOverCostCountInOneTerm := 0;

    Result := CalcId(FLastTimeTick);
    Exit;
  end;

  if (FCurrentSeqNumber > FMaxSeqNumber) then
  begin
{$IFDEF DEBUG}
    Inc(FGenCountInOneTerm);
{$ENDIF}
    Inc(FLastTimeTick);
    FCurrentSeqNumber := FMinSeqNumber;
    FIsOverCost := True;
    Inc(FOverCostCountInOneTerm);

    Result := CalcId(FLastTimeTick);
    Exit;
  end;

{$IFDEF DEBUG}
  Inc(FGenCountInOneTerm);
{$ENDIF}
  Result := CalcId(FLastTimeTick);
end;

function TSnowWorkerM1.NextNormalId: Int64;
var
  CurrentTimeTick: Int64;
begin
  CurrentTimeTick := GetCurrentTimeTick();

  if (CurrentTimeTick < FLastTimeTick) then
  begin
    if (FTurnBackTimeTick < 1) then
    begin
      FTurnBackTimeTick := FLastTimeTick - 1;

      Inc(FTurnBackIndex);
      // 每毫秒序列数的前5位是预留位，0用于手工新值，1-4是时间回拨次序
      // 支持4次回拨次序（避免回拨重叠导致ID重复），可无限次回拨（次序循环使用）。
      if (FTurnBackIndex > 4) then
      begin
        FTurnBackIndex := 1;
      end;

{$IFDEF DEBUG}
      BeginTurnBackAction(FTurnBackTimeTick);
{$ENDIF}
    end;

    // Sleep(1);
    Result := CalcTurnBackId(FTurnBackTimeTick);
    Exit;
  end;

  // 时间追平时，_TurnBackTimeTick清零
  if (FTurnBackTimeTick > 0) then
  begin
{$IFDEF DEBUG}
    EndTurnBackAction(FTurnBackTimeTick);
{$ENDIF}
    FTurnBackTimeTick := 0;
  end;

  if (CurrentTimeTick > FLastTimeTick) then
  begin
    FLastTimeTick := CurrentTimeTick;
    FCurrentSeqNumber := FMinSeqNumber;

    Result := CalcId(FLastTimeTick);
    Exit;
  end;

  if (FCurrentSeqNumber > FMaxSeqNumber) then
  begin
{$IFDEF DEBUG}
    BeginOverCostAction(CurrentTimeTick);
    Inc(FTermIndex);
    FGenCountInOneTerm := 1;
{$ENDIF}
    FOverCostCountInOneTerm := 1;
    Inc(FLastTimeTick);
    FCurrentSeqNumber := FMinSeqNumber;
    FIsOverCost := True;

    Result := CalcId(FLastTimeTick);
    Exit;
  end;

  Result := CalcId(FLastTimeTick);
end;

function TSnowWorkerM1.CalcId(UseTimeTick: Int64): Int64;
begin
  Result := ((UseTimeTick shl FTimestampShift) + (Int64(FWorkerId) shl FSeqBitLength) + Cardinal(FCurrentSeqNumber));

  Inc(FCurrentSeqNumber);
end;

function TSnowWorkerM1.CalcTurnBackId(UseTimeTick: Int64): Int64;
begin
  Result := ((UseTimeTick shl FTimestampShift) + (Int64(FWorkerId) shl FSeqBitLength) + FTurnBackIndex);

  Dec(FTurnBackTimeTick);
end;

function TSnowWorkerM1.GetCurrentTimeTick(): Int64;
var
  Millis: Int64;
begin
  // Millis := DateTimeToUnix(Now(), False);
  Millis := GetMillisecondTimeStamp();
  Result := Millis - FBaseTime;
end;

function TSnowWorkerM1.GetNextTimeTick(): Int64;
var
  TempTimeTicker: Int64;
begin
  TempTimeTicker := GetCurrentTimeTick();
  while (TempTimeTicker <= FLastTimeTick) do
  begin
    // Sleep(1);
    TSpinWait.SpinUntil(
      function(): Boolean
      begin
        Result := False;
      end, 1);
    TempTimeTicker := GetCurrentTimeTick();
  end;

  Result := TempTimeTicker;
end;

function TSnowWorkerM1.NextId(): Int64;
begin
  SyncLock.Enter;
  try
    if FIsOverCost then
      Result := NextOverCostId()
    else
      Result := NextNormalId();
  finally
    SyncLock.Leave;
  end;
end;

end.
