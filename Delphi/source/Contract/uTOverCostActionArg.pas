unit uTOverCostActionArg;

interface

type
  TOverCostActionArg = class
  private
    FActionType: Integer;
    FTimeTick: Int64;
    FWorkerId: Word;
    FOverCostCountInOneTerm: Integer;
    FGenCountInOneTerm: Integer;
    FTermIndex: Integer;
  public
    /// <summary>
    /// 事件类型
    /// 1-开始，2-结束，8-漂移
    /// </summary>
    property ActionType: Integer read FActionType write FActionType default 0;
    /// <summary>
    /// 时间戳
    /// </summary>
    property TimeTick: Int64 read FTimeTick write FTimeTick;
    /// <summary>
    /// 机器码
    /// </summary>
    property WorkerId: Word read FWorkerId write FWorkerId;
    /// <summary>
    /// 漂移计算次数
    /// </summary>
    property OverCostCountInOneTerm: Integer read FOverCostCountInOneTerm write FOverCostCountInOneTerm default 0;
    /// <summary>
    /// 漂移期间生产ID个数
    /// </summary>
    property GenCountInOneTerm: Integer read FGenCountInOneTerm write FGenCountInOneTerm default 0;
    /// <summary>
    /// 漂移周期
    /// </summary>
    property TermIndex: Integer read FTermIndex write FTermIndex default 0;

    constructor Create(const WorkerId: Word; const TimeTick: Int64; const ActionType: Integer = 0;
      const OverCostCountInOneTerm: Integer = 0; const GenCountWhenOverCost: Integer = 0;
      const Index: Integer = 0); overload;
  end;

implementation

{ TOverCostActionArg }

constructor TOverCostActionArg.Create(const WorkerId: Word; const TimeTick: Int64; const ActionType: Integer;
  const OverCostCountInOneTerm: Integer; const GenCountWhenOverCost: Integer; const Index: Integer);
begin
  FWorkerId := WorkerId;
  FTimeTick := TimeTick;
  FActionType := ActionType;
  FOverCostCountInOneTerm := OverCostCountInOneTerm;
  FGenCountInOneTerm := GenCountWhenOverCost;
  FTermIndex := Index;
end;

end.
