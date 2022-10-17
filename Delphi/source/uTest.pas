unit uTest;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics, System.DateUtils,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ComCtrls, Vcl.StdCtrls, Vcl.ExtCtrls, uIdGeneratorOptions, uIIdGenerator,
  uDefaultIdGenerator, uYitIdHelper;

type
  TfTest = class(TForm)
    mmo1: TMemo;
    Panel1: TPanel;
    Button1: TButton;
    btnTimeStamp: TButton;
    Button2: TButton;
    rgSelect: TRadioGroup;
    procedure Button1Click(Sender: TObject);
    procedure btnTimeStampClick(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
  private
    { Private declarations }
    IdGeneratorOption: TIdGeneratorOptions;
    YitIdHelper: TYitIdHelper;
  public
    { Public declarations }
  end;

var
  fTest: TfTest;

implementation

{$R *.dfm}

function GetMillisecondTimeStamp(): Int64;
var
  ST: TDateTime;
begin
  ST := EncodeDateTime(1970, 1, 1, 0, 0, 0, 0);
  Result := MilliSecondsBetween(Now(), ST) - 28800000; // 8*60*60*1000;
end;

procedure TfTest.btnTimeStampClick(Sender: TObject);
const
  T1 = 28800000;
var
  SysTime: TSystemTime;
  Time1, Time2: TDateTime;
  T2, T3: Int64;
begin
  // 秒级
  // mmo1.Lines.Add(IntToStr(DateTimeToUnix(EncodeDateTime(2020, 2, 20, 2, 20, 2, 20), False)));
  // mmo1.Lines.Add(IntToStr(DateTimeToUnix(EncodeDateTime(2020, 2, 20, 2, 20, 2, 20), True)));
  // mmo1.Lines.Add(FormatDateTime('yyyy-MM-dd HH:mm:ss.zzz',
  // UnixToDateTime(DateTimeToUnix(EncodeDateTime(2020, 2, 20, 2, 20, 2, 20), False), False)));
  mmo1.Lines.Add('处理时区后转为UTC时间：' + IntToStr(DateTimeToUnix(Now(), False)));
  mmo1.Lines.Add('直接取UTC时间：' + IntToStr(DateTimeToUnix(Now(), True)));
  // mmo1.Lines.Add(FormatDateTime('yyyy-MM-dd HH:mm:ss.zzz', UnixToDateTime(DateTimeToUnix(Now(), False), False)));
  // mmo1.Lines.Add(FormatDateTime('yyyy-MM-dd HH:mm:ss.zzz', UnixToDateTime(DateTimeToUnix(Now(), True), True)));

  // 毫秒级
  Time2 := EncodeDateTime(1970, 1, 1, 0, 0, 0, 0);

  // 未处理时区
  GetLocalTime(SysTime);
  Time1 := SystemTimeToDateTime(SysTime);
  T2 := MilliSecondsBetween(Time1, Time2);
  T3 := T2 - T1;
  mmo1.Lines.Add('LocalTime：' + IntToStr(T3));

  // 处理了时区
  GetSystemTime(SysTime);
  Time1 := SystemTimeToDateTime(SysTime);
  T2 := MilliSecondsBetween(Time1, Time2);
  T3 := T2 - T1;
  mmo1.Lines.Add('SystemTime：' + IntToStr(T3));

  // 未处理时区
  Time1 := Now();
  T2 := MilliSecondsBetween(Time1, Time2);
  T3 := T2 - T1;
  mmo1.Lines.Add('Now：' + IntToStr(T3));
end;

procedure TfTest.Button1Click(Sender: TObject);
var
  options: TIdGeneratorOptions;
  IdGen: IIdGenerator;
  i, j: Integer;
begin
  // 使用方式一：直接使用
  options := TIdGeneratorOptions.Create;
  try
    with options do
    begin
      Method := rgSelect.ItemIndex + 1;
      // BaseTime := DateTime.Now.AddYears(-10);
      WorkerId := 2;

      WorkerIdBitLength := 6;
      SeqBitLength := 6;

      MaxSeqNumber := 0;
      MinSeqNumber := 5;

      TopOverCostCount := 2000;

      DataCenterId := 0;
      DataCenterIdBitLength := 0;

      TimestampType := 0;
    end;

    IdGen := TDefaultIdGenerator.Create(options);

    j := 100;
    Screen.Cursor := crHourGlass;
    mmo1.Lines.BeginUpdate();
    try
      for i := 1 to j do
      begin
        mmo1.Lines.Add(FormatFloat('000', i) + '：' + IntToStr(IdGen.NewLong()));
      end;
      mmo1.Perform(WM_VSCROLL, SB_BOTTOM, 0)
    finally
      mmo1.Lines.EndUpdate();
      Screen.Cursor := crDefault;
    end;

  finally
    options.Free;
  end;
end;

procedure TfTest.Button2Click(Sender: TObject);
const
  Total = 5000000;
var
  i: Integer;
  Id: Int64;
  ST, ET, Elapse: Cardinal;
begin
  Screen.Cursor := crHourGlass;
  try
    ST := GetTickCount();
    for i := 1 to Total do
    begin
      Id := YitIdHelper.NextId();
    end;
    ET := GetTickCount();
  finally
    Screen.Cursor := crDefault;
  end;
  Elapse := ET - ST;
  mmo1.Lines.Add(Format('生成500万个时间：%d毫秒，平均每秒%d个。', [Elapse, Trunc((Total / Elapse) * 1000)]));

end;

procedure TfTest.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  YitIdHelper.Free;
  IdGeneratorOption.Free;
end;

procedure TfTest.FormCreate(Sender: TObject);
begin
  IdGeneratorOption := TIdGeneratorOptions.Create;
  // 默认参数
  with IdGeneratorOption do
  begin
    Method := rgSelect.ItemIndex + 1;
    // BaseTime := DateTime.Now.AddYears(-10);
    WorkerId := 2;

    WorkerIdBitLength := 6;
    SeqBitLength := 6;

    MaxSeqNumber := 0;
    MinSeqNumber := 5;

    TopOverCostCount := 2000;

    DataCenterId := 0;
    DataCenterIdBitLength := 0;

    TimestampType := 0;
  end;

  // 使用方式二：封装类使用
  YitIdHelper := TYitIdHelper.Create;
  YitIdHelper.SetIdGenerator(IdGeneratorOption);
end;

end.
