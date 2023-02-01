program IdGenTest;

uses
  Vcl.Forms,
  uTest in 'uTest.pas' {fTest} ,
  uISnowWorker in 'Contract\uISnowWorker.pas',
  uIIdGenerator in 'Contract\uIIdGenerator.pas',
  uTOverCostActionArg in 'Contract\uTOverCostActionArg.pas',
  uIdGeneratorOptions in 'Contract\uIdGeneratorOptions.pas',
  uSnowWorkerM1 in 'Core\uSnowWorkerM1.pas',
  uSnowWorkerM2 in 'Core\uSnowWorkerM2.pas',
  uSnowWorkerM3 in 'Core\uSnowWorkerM3.pas',
  uDefaultIdGenerator in 'uDefaultIdGenerator.pas',
  uYitIdHelper in 'uYitIdHelper.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  ReportMemoryLeaksOnShutdown := (DebugHook <> 0);
  Application.CreateForm(TfTest, fTest);
  Application.Run;

end.
