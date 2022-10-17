unit uSnowWorkerM2;

interface

uses System.SysUtils, uSnowWorkerM1, uIdGeneratorOptions, System.SyncObjs;

/// <summary>
/// 常规雪花算法
/// </summary>
type
  TSnowWorkerM2 = class(TSnowWorkerM1)
  public
    function NextId(): Int64;
    constructor Create(options: TIdGeneratorOptions); overload;
  end;

implementation

{ TSnowWorkerM2 }

function IncX(var x: Integer): Integer; inline;
begin
  Result := x;
  Inc(x);
end;

constructor TSnowWorkerM2.Create(options: TIdGeneratorOptions);
begin
  inherited Create(options);
end;

function TSnowWorkerM2.NextId(): Int64;
var
  CurrentTimeTick: Int64;
begin
  SyncLock.Enter;
  try
    CurrentTimeTick := GetCurrentTimeTick();

    if (FLastTimeTick = CurrentTimeTick) then
    begin
      // if (IncX(FCurrentSeqNumber) > FMaxSeqNumber) then
      // begin
      // FCurrentSeqNumber := FMinSeqNumber;
      // CurrentTimeTick := GetNextTimeTick();
      // end;
      if ((FCurrentSeqNumber) > FMaxSeqNumber) then
      begin
        FCurrentSeqNumber := FMinSeqNumber;
        CurrentTimeTick := GetNextTimeTick();
      end
      else
      begin
        Inc(FCurrentSeqNumber);
      end;
    end
    else
    begin
      FCurrentSeqNumber := FMinSeqNumber;
    end;

    if (CurrentTimeTick < FLastTimeTick) then
    begin
      raise Exception.Create(Format('Time error for %d milliseconds', [FLastTimeTick - CurrentTimeTick]));
    end;

    FLastTimeTick := CurrentTimeTick;
    Result := ((CurrentTimeTick shl FTimestampShift) + (Int64(FWorkerId) shl FSeqBitLength) +
      Cardinal(FCurrentSeqNumber));
  finally
    SyncLock.Leave;
  end;
end;

end.
