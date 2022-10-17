unit uYitIdHelper;

interface

uses System.SysUtils, uIIdGenerator, uIdGeneratorOptions;

type
  TYitIdHelper = class
  private
    IdGenInstance: IIdGenerator;
  public
    function GetIdGenInstance(): IIdGenerator;
    /// <summary>
    /// 设置参数，建议程序初始化时执行一次
    /// </summary>
    /// <param name="options"></param>
    procedure SetIdGenerator(options: TIdGeneratorOptions);
    /// <summary>
    /// 生成新的Id
    /// 调用本方法前，请确保调用了 SetIdGenerator 方法做初始化。
    /// </summary>
    /// <returns></returns>
    function NextId(): Int64;
  end;

implementation

uses
  uDefaultIdGenerator;

{ TYitIdHelper }

function TYitIdHelper.GetIdGenInstance: IIdGenerator;
begin
  Result := IdGenInstance;
end;

procedure TYitIdHelper.SetIdGenerator(options: TIdGeneratorOptions);
begin
  IdGenInstance := TDefaultIdGenerator.Create(options);
end;

function TYitIdHelper.NextId(): Int64;
begin
  if (IdGenInstance = nil) then
    raise Exception.Create('Please initialize Yitter.IdGeneratorOptions first.');

  Result := IdGenInstance.NewLong();
end;

end.
