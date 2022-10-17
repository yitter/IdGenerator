object fTest: TfTest
  Left = 0
  Top = 0
  Caption = #38634#33457#31639#27861#27979#35797
  ClientHeight = 509
  ClientWidth = 668
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Microsoft YaHei UI'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 20
  object mmo1: TMemo
    Left = 0
    Top = 100
    Width = 668
    Height = 409
    Align = alClient
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 1
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 668
    Height = 100
    Align = alTop
    Caption = 'Panel1'
    ShowCaption = False
    TabOrder = 0
    object Button1: TButton
      Left = 32
      Top = 62
      Width = 90
      Height = 30
      Caption = #29983#25104'100'#20010
      TabOrder = 1
      OnClick = Button1Click
    end
    object btnTimeStamp: TButton
      Left = 568
      Top = 62
      Width = 90
      Height = 30
      Caption = #27979#35797#26102#38388#25139
      TabOrder = 3
      OnClick = btnTimeStampClick
    end
    object Button2: TButton
      Left = 168
      Top = 62
      Width = 90
      Height = 30
      Caption = '500W'#26102#38388
      TabOrder = 2
      OnClick = Button2Click
    end
    object rgSelect: TRadioGroup
      Left = 1
      Top = 1
      Width = 666
      Height = 55
      Align = alTop
      Caption = #38634#33457#31639#27861
      Columns = 3
      ItemIndex = 0
      Items.Strings = (
        #28418#31227#31639#27861
        #20256#32479#31639#27861)
      TabOrder = 0
    end
  end
end
