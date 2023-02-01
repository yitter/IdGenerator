unit uDefaultIdGenerator;

interface

uses
  uIIdGenerator, uISnowWorker, uIdGeneratorOptions, System.DateUtils, System.SysUtils;

type
  TDefaultIdGenerator = class(TInterfacedObject, IIdGenerator)
  private
    SnowWorker: ISnowWorker;
  public
    constructor Create(options: TIdGeneratorOptions); overload;
    function NewLong(): Int64;
  end;

implementation

uses
  uSnowWorkerM1, uSnowWorkerM2, uSnowWorkerM3;

{ TDefaultIdGenerator }

function GetMillisecondTimeStamp(ET: TDateTime): Int64;
var
  ST: TDateTime;
begin
  ST := EncodeDateTime(1970, 1, 1, 0, 0, 0, 0);
  Result := MilliSecondsBetween(ET, ST) - 28800000; // 8*60*60*1000;
end;

constructor TDefaultIdGenerator.Create(options: TIdGeneratorOptions);
var
  MaxLength, MaxWorkerIdNumber, MaxDataCenterIdNumber, MaxSeqNumber: Integer;
begin
  if (options = nil) then
  begin
    raise Exception.Create('options error.');
  end;

  // 1.BaseTime
  if (options.BaseTime < GetMillisecondTimeStamp(IncYear(Now(), -50))) or
    (options.BaseTime > GetMillisecondTimeStamp(Now())) then
  begin
    raise Exception.Create('BaseTime error.');
  end;

  // 2.WorkerIdBitLength
  if (options.TimestampType = 0) then
    MaxLength := 22
  else
    MaxLength := 31; // （秒级时间戳时放大到31位）
  if (options.WorkerIdBitLength <= 0) then
  begin
    raise Exception.Create('WorkerIdBitLength error.(range:[1, 21])');
  end;
  if (options.DataCenterIdBitLength + options.WorkerIdBitLength + options.SeqBitLength > MaxLength) then
  begin
    raise Exception.Create('error：DataCenterIdBitLength + WorkerIdBitLength + SeqBitLength <= ' + IntToStr(MaxLength));
  end;

  // 3.WorkerId
  MaxWorkerIdNumber := (1 shl options.WorkerIdBitLength) - 1;
  if (MaxWorkerIdNumber = 0) then
  begin
    MaxWorkerIdNumber := 63;
  end;
  if ((options.WorkerId < 0) or (options.WorkerId > MaxWorkerIdNumber)) then
  begin
    raise Exception.Create('WorkerId error. (range:[0, ' + IntToStr(MaxWorkerIdNumber) + ']');
  end;

  MaxDataCenterIdNumber := (1 shl options.DataCenterIdBitLength) - 1;
  if (options.DataCenterId < 0) or (options.DataCenterId > MaxDataCenterIdNumber) then
  begin
    raise Exception.Create('DataCenterId error. (range:[0, ' + IntToStr(MaxDataCenterIdNumber) + ']');
  end;

  // 4.SeqBitLength
  if ((options.SeqBitLength < 2) or (options.SeqBitLength > 21)) then
  begin
    raise Exception.Create('SeqBitLength error. (range:[2, 21])');
  end;

  // 5.MaxSeqNumber
  MaxSeqNumber := (1 shl options.SeqBitLength) - 1;
  if (MaxSeqNumber = 0) then
  begin
    MaxSeqNumber := 63;
  end;
  if ((options.MaxSeqNumber < 0) or (options.MaxSeqNumber > MaxSeqNumber)) then
  begin
    raise Exception.Create('MaxSeqNumber error. (range:[1, ' + IntToStr(MaxSeqNumber) + ']');
  end;

  // 6.MinSeqNumber
  if ((options.MinSeqNumber < 5) or (options.MinSeqNumber > MaxSeqNumber)) then
  begin
    raise Exception.Create('MinSeqNumber error. (range:[5, ' + IntToStr(MaxSeqNumber) + ']');
  end;

  // 7.TopOverCostCount
  if ((options.TopOverCostCount < 0) or (options.TopOverCostCount > 10000)) then
  begin
    raise Exception.Create('TopOverCostCount error. (range:[0, 10000]');
  end;

  case (options.Method) of
    2:
      begin
        SnowWorker := TSnowWorkerM2.Create(options);
      end;
  else
    begin
      if ((options.DataCenterIdBitLength = 0) and (options.TimestampType = 0)) then
      begin
        SnowWorker := TSnowWorkerM1.Create(options);
      end
      else
      begin
        SnowWorker := TSnowWorkerM3.Create(options);
      end;
    end;
  end;

  if (options.Method <> 2) then
  begin
    Sleep(500);
  end;
end;

function TDefaultIdGenerator.NewLong(): Int64;
begin
  Result := SnowWorker.NextId();
end;

end.
