#if 0

/*
                Формат модуля Pro Tracker v3.6x

   Указаны  смещения  до  областей  в модуле и их длина в байтах
(десятичные числа).

+0 (13) "ProTracker 3." - идентификационная строка.
+13 (17) "6" (или "5", "4", или даже "3" ) - номер подверсии.
   Следует заметить, что для модулей PTv3.4x и ниже используется
другая, "несимметричная" таблица  громкости, а в модулях PTv3.3x
используется  альтернативная частотная таблица "Pro Tracker", не
совпадающая с одноименной современной!
+14 (16) " compilation of " (необязательное - любой текст этой
 длины).
+30 (32) название  модуля (ASCII, lat, неиспользованные  символы
 забиты пробелами).
+62 (4) " by " (необязательное - любые 4 символа)
+66 (32) имя автора (ASCII, lat, неиспользованные символы забиты
 пробелами).
+98 (1) один пробел (любой символ).
 (то есть первые 99 байт модуля образуют соответствующую строку)

+99 (1) номер частотной таблицы: 0=Pro Tracker, 1=Sound Tracker,
 2=ASM or PSC, 3=RealSound. Табличка занимает 192 байта и содер-
 жит значения периодов для 96 нот,начиная с C-1 (ДО первой окта-
 вы). Период - значение, обратное частоте ноты, помещаемое в со-
 ответствующие регистры AY. Младшие байты (здесь и ниже,за одним
 исключением,которое будет указано) хранятся первыми. Компилятор
 PT сохраняет таблицу, соответствующую модулю, в тело плейера по
 относительному адресу 512. Таблицу громкости он сохраняет в том
 же теле плейера по относительному адресу 256.
+100 (1) значение темпа.
+101 (1) song end (1=в модуле всего одна позиция) - в плейере не
 используется.
+102 (1) song loop (0=зацикливание на начало).
+103 (2) Psa_chn=смещение от начала модуля до таблицы паттернов.
+105 (32*2) смещения  от начала модуля до сэмплов, начиная с ну-
 левого  сэмпла. По два байта  на сэмпл. Нулевой сэмпл в текущих
 версиях  редактора  не используется. Для  всех неиспользованных
 сэмплов смещение равно нулю.
+169 (16*2) смещения  от начала  модуля до орнаментов, начиная с
 нулевого. По два байта на орнамент.Нулевой орнамент - это отсу-
 тствие орнамента,поэтому данные этого орнамента (см.ниже) соде-
 ржат 0,1,0 (можно использовать этот орнамент по своему усмотре-
 нию,НО тогда в сонге нельзя будет использовать сэмплы без орна-
 ментов).Для всех неиспользованных орнаментов смещение равно ну-
 лю.
+201 (?) список  позиций  (ордер).   Содержит  номера  паттернов
 (0...84), умноженные  на 3. Таблица завершается  кодом #ff. Pro
 Tracker v3.3x-v3.5x не поддерживает больше 42 паттернов.
 Pro Tracker v3.6x не поддерживает больше 46 паттернов.
+Psa_chn (?*6) указатель паттернов. Содержит для каждого из име-
 ющихся паттернов смещения:
   ++0 (2) до блока данных канала (трека) A
   ++2 (2) до блока данных канала (трека) B
   ++4 (2) до блока данных канала (трека) C.
 Данные  по смещению шума (отдельная колонка в редакторе) компи-
 лируются в канал B.
+? (?*?) блоки данных каналов, то есть треки.

 Трек содержит следующие данные:
================================================================
#00 - конец трека.
#01, delay, Lsl, Hsl - эффект Gliss(Slide) вверх или вниз. Delay
 - время  в  пятидесятых  долях секунды, по истечении которого к
 периоду  ноты  будет  прибавлена величина Lsl+256*Hsl. Смещение
 накапливается плейером в соответствующей переменной и прибавля-
 ется после формирования частоты ноты, т.е.сначала обрабатывает-
 ся строчка орнамента, а уже потом...
#02, delay, Lmax, Hmax, Lsl, Hsl - эффект  Tone Portamento вверх
 или вниз.Delay - время в пятидесятых долях секунды,по истечении
 которого  к периоду ноты будет прибавлена величина Lsl+256*Hsl.
 Lmax+256*Hmax - максимальное  смещение (беззнаковое), после на-
 копления  которого  следует  прекратить Portamento (в PT3.6x не
 используется, т.к. возможно неправильное указание направления).
#03, offset - sample offset. Сэмпл начинает играть не сначала.
#04, offset - ornament offset. Орнамент  начинает играть не сна-
 чала.
#05, YEStime, NOtime - vibrate. нота то звучит, то не звучит.
#08, delay, Lsl, Hsl - эффект slide envelope. К значению периода
 огибающей время от времени прибавляется Lsl+256*Hsl.
#09, tempo - указание темпа (в прерываниях на строку). Стандарт-
 ный плейер меньше tempo=2 не играет.
NB: параменты  эффектов (#0x)  лежат  не сразу после кода номера
эффекта, а ПОСЛЕ КОНЦА СТРОКИ!!! Если используется несколько эф-
фектов на одну ноту (на самом деле так не бывает),то сначала ле-
жат параменты последнего эффекта, потом предпоследнего и так да-
лее...

#10, smp*2 - выключить огибающую, поставить 0-й орнамент и
 изменить номер сэмпла.
#1x, Henv, Lenv, smp*2 - изменить номер сэмпла, поставить 0-й
 орнамент и включить огибающую типа x-1 с периодом
 Lenv+256*Henv. При включении огибающей она инициализируется,
 т.е. начинается новый период!
#20-#3f - указать смещение шума (бывает только в канале B)
#4x - указать орнамент номер x.
 (огибающая не выключается)
#50-#Af - указать  высоту ноты и ЗАКОНЧИТЬ анализ текущей строки
 канала.
#B0 - выключить Envelope.
#B1, lines - не анализировать канал в течение lines строк.
 (lines=1 соответствует одной строке). Действует не только на
 промежуток между этой и следующей нотой, но и далее, пока не
 указано другое значение lines!
#Bx, Henv, Lenv - то же, что #1x, но  без сэмпла. То есть просто
 включить огибающую типа x-1 с указанным периодом.
#Cv - указать громкость.(v=0 - пауза и ЗАКОНЧИТЬ анализ строки.)
#D0 - ЗАКОНЧИТЬ анализ строки.
#D1-#Ef - указать номер сэмпла.
#Fx, smp*2 - указать номер орнамента (x) и номер сэмпла.
 (огибающая выключается)
================================================================

+? (?*(?*4+2)) - сэмплы.
   ++0 (1) - loop
   ++1 (1) - end (1=сэмпл из одной строчки)
   ++2 (?*4) - данные:
      +++0 (1) sv +- N4 N3 N2 N1 N0 Em
      +++1 (1) Nm ts ns Tm V3 V2 V1 V0
 sv=1 - признак съезжания громкости, +- =1 соответствует её уве-
 личению; N4-0 - частота шума ИЛИ смещение огибающей (зависит от
 наличия  маски  шума): смещение  огибающей 0-15 - вниз, 16-31 -
 вверх (N4 интерпретируется как знак); V3-0 - громкость; Tn, Nm,
 Em - маски тона, шума и огибающей соотв., причём если маска ра-
 вна единице,то соответствующий элемент звука выключен; ts, ns=1
 - признаки того, что  текущее  смещение тона или шума/огибающей
 будет запомнено.
      +++2 (2) смещение периода тона (положительное - вверх, от-
 рицательное - вниз).

+? (?*(?+2)) - орнаменты.
   ++0 (1) - loop
   ++1 (1) - end (1=орнамент из одной строчки)
   ++2 (?) - данные: смещения в полутонах (0=нет смещения, поло-
 жительное смещение - вверх, отрицательное - вниз).

жду дополнений!

Дополнения от 10.xi.02:
1. (Sergey Bulba): Таблица ASM or PSC не имеет отношения к соот-
ветствующим  редакторам. Модули из этих редакторов следует импо-
ртировать с таблицей Sound Tracker. Таблица ASM or PSC рассчита-
на так,чтобы при тактовой частоте AY 1.7744 MHz ноты в редакторе
совпадали  с одноименными нотами фортепиано. (Таблица Real Sound
- аналогично, но для 1.75 MHz.)
2. В модуле  может  использоваться  и 0-й сэмпл, если в треке он
хранится вместе с орнаментом (можно нулевым). Итого 32 сэмпла.
3. Проверена информация по поводу Portamento, Loop и #B1.
4. Исправлена информация по поводу сэмплов.
Дополнение от 24.ii.03:
5. Дополнено по поводу выключения огибающей.
Дополнение от 25.iii.03:
6. Стандарт PTv3.6x - 46 паттернов.
7. При переполнении смещения орнамента вниз (точнее, при отрица-
тельном номере ноты) подставляется самая низкая нота C-1. При
переполнении вверх результат не определён.
Дополнение от 26.vii.03:
8. Выше уточнены изменения для PT3.6x.
9. Максимальный  размер модуля, который можно загрузить в редак-
тор, равен #3300 байт, или #4000 байт для "cjf" версии.
Дополнение от 20.iii.04:
10. PT3+67 имеет 48 паттернов, но не гарантируется, что их будет
столько же во всех следующих версиях.
Дополнение от 29.vi.04:
11. см.про #10 и #B1.
12. в PT3.69 можно указывать номер орнамента без влияния на
огибающую. в VTII пока нельзя. все плейеры играют такое.
Дополнение от 26.viii.04:
13. на строчках сэмпла, где включен шум, накопленное смещение
огибающей игнорируется.

Alone Coder
*/

6: (PT3_MusicName:array[0..$62]of char;
    PT3_TonTableId:byte;
    PT3_Delay:byte;
    PT3_NumberOfPositions:byte;
    PT3_LoopPosition:byte;
    PT3_PatternsPointer:word;
    PT3_SamplesPointers:array[0..31]of word;
    PT3_OrnamentsPointers:array[0..15]of word;
    PT3_PositionList:array[0..65535-201]of byte);

 PT3_Channel_Parameters = record
  Address_In_Pattern,
  OrnamentPointer,
  SamplePointer,
  Ton:word;
  Loop_Ornament_Position,
  Ornament_Length,
  Position_In_Ornament,
  Loop_Sample_Position,
  Sample_Length,
  Position_In_Sample,
  Volume,
  Number_Of_Notes_To_Skip,
  Note,
  Slide_To_Note,
  Amplitude:byte;
  Envelope_Enabled,
  Enabled,
  SimpleGliss:boolean;
  Current_Amplitude_Sliding,
  Ton_Slide_Count,
  Current_OnOff,
  OnOff_Delay,
  OffOn_Delay,
  Ton_Slide_Delay,
  Current_Ton_Sliding,
  Ton_Accumulator,
  Ton_Slide_Step,
  Ton_Delta:smallint;
  Note_Skip_Counter:shortint;
  Current_Noise_Sliding,
  Current_Envelope_Sliding:byte;
 end;

 PPT3_Parameters = ^PT3_Parameters;
 PT3_Parameters = record
  Env_Base:packed record
  case Boolean of
  True: (wrd:smallint);
  False:(lo:byte;
         hi:byte);
  end;
  Cur_Env_Slide,
  Env_Slide_Add:smallint;
  Cur_Env_Delay,
  Env_Delay:shortint;
  Noise_Base,
  Delay,
  AddToNoise,
  DelayCounter,
  CurrentPosition:byte;
 end;

const
{Table #0 of Pro Tracker 3.3x - 3.4r}
 PT3NoteTable_PT_33_34r:PT3ToneTable = (
  $0C21,$0B73,$0ACE,$0A33,$09A0,$0916,$0893,$0818,$07A4,$0736,$06CE,$066D,
  $0610,$05B9,$0567,$0519,$04D0,$048B,$0449,$040C,$03D2,$039B,$0367,$0336,
  $0308,$02DC,$02B3,$028C,$0268,$0245,$0224,$0206,$01E9,$01CD,$01B3,$019B,
  $0184,$016E,$0159,$0146,$0134,$0122,$0112,$0103,$00F4,$00E6,$00D9,$00CD,
  $00C2,$00B7,$00AC,$00A3,$009A,$0091,$0089,$0081,$007A,$0073,$006C,$0066,
  $0061,$005B,$0056,$0051,$004D,$0048,$0044,$0040,$003D,$0039,$0036,$0033,
  $0030,$002D,$002B,$0028,$0026,$0024,$0022,$0020,$001E,$001C,$001B,$0019,
  $0018,$0016,$0015,$0014,$0013,$0012,$0011,$0010,$000F,$000E,$000D,$000C);

{Table #0 of Pro Tracker 3.4x - 3.5x}
 PT3NoteTable_PT_34_35:PT3ToneTable = (
  $0C22,$0B73,$0ACF,$0A33,$09A1,$0917,$0894,$0819,$07A4,$0737,$06CF,$066D,
  $0611,$05BA,$0567,$051A,$04D0,$048B,$044A,$040C,$03D2,$039B,$0367,$0337,
  $0308,$02DD,$02B4,$028D,$0268,$0246,$0225,$0206,$01E9,$01CE,$01B4,$019B,
  $0184,$016E,$015A,$0146,$0134,$0123,$0112,$0103,$00F5,$00E7,$00DA,$00CE,
  $00C2,$00B7,$00AD,$00A3,$009A,$0091,$0089,$0082,$007A,$0073,$006D,$0067,
  $0061,$005C,$0056,$0052,$004D,$0049,$0045,$0041,$003D,$003A,$0036,$0033,
  $0031,$002E,$002B,$0029,$0027,$0024,$0022,$0020,$001F,$001D,$001B,$001A,
  $0018,$0017,$0016,$0014,$0013,$0012,$0011,$0010,$000F,$000E,$000D,$000C);

{Table #1 of Pro Tracker 3.3x - 3.5x)}
 PT3NoteTable_ST:PT3ToneTable = (
  $0EF8,$0E10,$0D60,$0C80,$0BD8,$0B28,$0A88,$09F0,$0960,$08E0,$0858,$07E0,
  $077C,$0708,$06B0,$0640,$05EC,$0594,$0544,$04F8,$04B0,$0470,$042C,$03FD,
  $03BE,$0384,$0358,$0320,$02F6,$02CA,$02A2,$027C,$0258,$0238,$0216,$01F8,
  $01DF,$01C2,$01AC,$0190,$017B,$0165,$0151,$013E,$012C,$011C,$010A,$00FC,
  $00EF,$00E1,$00D6,$00C8,$00BD,$00B2,$00A8,$009F,$0096,$008E,$0085,$007E,
  $0077,$0070,$006B,$0064,$005E,$0059,$0054,$004F,$004B,$0047,$0042,$003F,
  $003B,$0038,$0035,$0032,$002F,$002C,$002A,$0027,$0025,$0023,$0021,$001F,
  $001D,$001C,$001A,$0019,$0017,$0016,$0015,$0013,$0012,$0011,$0010,$000F);

{Table #2 of Pro Tracker 3.4r}
 PT3NoteTable_ASM_34r:PT3ToneTable = (
  $0D3E,$0C80,$0BCC,$0B22,$0A82,$09EC,$095C,$08D6,$0858,$07E0,$076E,$0704,
  $069F,$0640,$05E6,$0591,$0541,$04F6,$04AE,$046B,$042C,$03F0,$03B7,$0382,
  $034F,$0320,$02F3,$02C8,$02A1,$027B,$0257,$0236,$0216,$01F8,$01DC,$01C1,
  $01A8,$0190,$0179,$0164,$0150,$013D,$012C,$011B,$010B,$00FC,$00EE,$00E0,
  $00D4,$00C8,$00BD,$00B2,$00A8,$009F,$0096,$008D,$0085,$007E,$0077,$0070,
  $006A,$0064,$005E,$0059,$0054,$0050,$004B,$0047,$0043,$003F,$003C,$0038,
  $0035,$0032,$002F,$002D,$002A,$0028,$0026,$0024,$0022,$0020,$001E,$001D,
  $001B,$001A,$0019,$0018,$0015,$0014,$0013,$0012,$0011,$0010,$000F,$000E);

{Table #2 of Pro Tracker 3.4x - 3.5x}
 PT3NoteTable_ASM_34_35:PT3ToneTable = (
  $0D10,$0C55,$0BA4,$0AFC,$0A5F,$09CA,$093D,$08B8,$083B,$07C5,$0755,$06EC,
  $0688,$062A,$05D2,$057E,$052F,$04E5,$049E,$045C,$041D,$03E2,$03AB,$0376,
  $0344,$0315,$02E9,$02BF,$0298,$0272,$024F,$022E,$020F,$01F1,$01D5,$01BB,
  $01A2,$018B,$0174,$0160,$014C,$0139,$0128,$0117,$0107,$00F9,$00EB,$00DD,
  $00D1,$00C5,$00BA,$00B0,$00A6,$009D,$0094,$008C,$0084,$007C,$0075,$006F,
  $0069,$0063,$005D,$0058,$0053,$004E,$004A,$0046,$0042,$003E,$003B,$0037,
  $0034,$0031,$002F,$002C,$0029,$0027,$0025,$0023,$0021,$001F,$001D,$001C,
  $001A,$0019,$0017,$0016,$0015,$0014,$0012,$0011,$0010,$000F,$000E,$000D);

{Table #3 of Pro Tracker 3.4r}
 PT3NoteTable_REAL_34r:PT3ToneTable = (
  $0CDA,$0C22,$0B73,$0ACF,$0A33,$09A1,$0917,$0894,$0819,$07A4,$0737,$06CF,
  $066D,$0611,$05BA,$0567,$051A,$04D0,$048B,$044A,$040C,$03D2,$039B,$0367,
  $0337,$0308,$02DD,$02B4,$028D,$0268,$0246,$0225,$0206,$01E9,$01CE,$01B4,
  $019B,$0184,$016E,$015A,$0146,$0134,$0123,$0113,$0103,$00F5,$00E7,$00DA,
  $00CE,$00C2,$00B7,$00AD,$00A3,$009A,$0091,$0089,$0082,$007A,$0073,$006D,
  $0067,$0061,$005C,$0056,$0052,$004D,$0049,$0045,$0041,$003D,$003A,$0036,
  $0033,$0031,$002E,$002B,$0029,$0027,$0024,$0022,$0020,$001F,$001D,$001B,
  $001A,$0018,$0017,$0016,$0014,$0013,$0012,$0011,$0010,$000F,$000E,$000D);

{Table #3 of Pro Tracker 3.4x - 3.5x}
 PT3NoteTable_REAL_34_35:PT3ToneTable = (
  $0CDA,$0C22,$0B73,$0ACF,$0A33,$09A1,$0917,$0894,$0819,$07A4,$0737,$06CF,
  $066D,$0611,$05BA,$0567,$051A,$04D0,$048B,$044A,$040C,$03D2,$039B,$0367,
  $0337,$0308,$02DD,$02B4,$028D,$0268,$0246,$0225,$0206,$01E9,$01CE,$01B4,
  $019B,$0184,$016E,$015A,$0146,$0134,$0123,$0112,$0103,$00F5,$00E7,$00DA,
  $00CE,$00C2,$00B7,$00AD,$00A3,$009A,$0091,$0089,$0082,$007A,$0073,$006D,
  $0067,$0061,$005C,$0056,$0052,$004D,$0049,$0045,$0041,$003D,$003A,$0036,
  $0033,$0031,$002E,$002B,$0029,$0027,$0024,$0022,$0020,$001F,$001D,$001B,
  $001A,$0018,$0017,$0016,$0014,$0013,$0012,$0011,$0010,$000F,$000E,$000D);

{Volume table of Pro Tracker 3.3x - 3.4x}
 PT3VolumeTable_33_34:PT3VolTable = (
  ($00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00),
  ($00,$00,$00,$00,$00,$00,$00,$00,$01,$01,$01,$01,$01,$01,$01,$01),
  ($00,$00,$00,$00,$00,$00,$01,$01,$01,$01,$01,$02,$02,$02,$02,$02),
  ($00,$00,$00,$00,$01,$01,$01,$01,$02,$02,$02,$02,$03,$03,$03,$03),
  ($00,$00,$00,$00,$01,$01,$01,$02,$02,$02,$03,$03,$03,$04,$04,$04),
  ($00,$00,$00,$01,$01,$01,$02,$02,$03,$03,$03,$04,$04,$04,$05,$05),
  ($00,$00,$00,$01,$01,$02,$02,$03,$03,$03,$04,$04,$05,$05,$06,$06),
  ($00,$00,$01,$01,$02,$02,$03,$03,$04,$04,$05,$05,$06,$06,$07,$07),
  ($00,$00,$01,$01,$02,$02,$03,$03,$04,$05,$05,$06,$06,$07,$07,$08),
  ($00,$00,$01,$01,$02,$03,$03,$04,$05,$05,$06,$06,$07,$08,$08,$09),
  ($00,$00,$01,$02,$02,$03,$04,$04,$05,$06,$06,$07,$08,$08,$09,$0A),
  ($00,$00,$01,$02,$03,$03,$04,$05,$06,$06,$07,$08,$09,$09,$0A,$0B),
  ($00,$00,$01,$02,$03,$04,$04,$05,$06,$07,$08,$08,$09,$0A,$0B,$0C),
  ($00,$00,$01,$02,$03,$04,$05,$06,$07,$07,$08,$09,$0A,$0B,$0C,$0D),
  ($00,$00,$01,$02,$03,$04,$05,$06,$07,$08,$09,$0A,$0B,$0C,$0D,$0E),
  ($00,$01,$02,$03,$04,$05,$06,$07,$08,$09,$0A,$0B,$0C,$0D,$0E,$0F));

{Volume table of Pro Tracker 3.5x}
 PT3VolumeTable_35:PT3VolTable = (
  ($00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00),
  ($00,$00,$00,$00,$00,$00,$00,$00,$01,$01,$01,$01,$01,$01,$01,$01),
  ($00,$00,$00,$00,$01,$01,$01,$01,$01,$01,$01,$01,$02,$02,$02,$02),
  ($00,$00,$00,$01,$01,$01,$01,$01,$02,$02,$02,$02,$02,$03,$03,$03),
  ($00,$00,$01,$01,$01,$01,$02,$02,$02,$02,$03,$03,$03,$03,$04,$04),
  ($00,$00,$01,$01,$01,$02,$02,$02,$03,$03,$03,$04,$04,$04,$05,$05),
  ($00,$00,$01,$01,$02,$02,$02,$03,$03,$04,$04,$04,$05,$05,$06,$06),
  ($00,$00,$01,$01,$02,$02,$03,$03,$04,$04,$05,$05,$06,$06,$07,$07),
  ($00,$01,$01,$02,$02,$03,$03,$04,$04,$05,$05,$06,$06,$07,$07,$08),
  ($00,$01,$01,$02,$02,$03,$04,$04,$05,$05,$06,$07,$07,$08,$08,$09),
  ($00,$01,$01,$02,$03,$03,$04,$05,$05,$06,$07,$07,$08,$09,$09,$0A),
  ($00,$01,$01,$02,$03,$04,$04,$05,$06,$07,$07,$08,$09,$0A,$0A,$0B),
  ($00,$01,$02,$02,$03,$04,$05,$06,$06,$07,$08,$09,$0A,$0A,$0B,$0C),
  ($00,$01,$02,$03,$03,$04,$05,$06,$07,$08,$09,$0A,$0A,$0B,$0C,$0D),
  ($00,$01,$02,$03,$04,$05,$06,$07,$07,$08,$09,$0A,$0B,$0C,$0D,$0E),
  ($00,$01,$02,$03,$04,$05,$06,$07,$08,$09,$0A,$0B,$0C,$0D,$0E,$0F));

function LoadTrackerModule(var Module:ModTypes;PPLItem:pointer;Index,MAddr,MLen:integer;Mem:pointer;FType:Available_Types):boolean;
   if FType = FT.PT3 then
    begin
      if ((PPLItem <> nil) and (LowerCase(ExtractFileExt(PPlaylistItem(PPLItem)^.FileName)) <> '.pt3')) or
         (PPLItem = nil) then
       begin
        i := 0; i1 := 0;
        with Module do
         begin
          while (i < 65535 - 201) and (PT3_PositionList[i] <> 255) do
           begin
            if longword(i1) < PT3_PositionList[i] then i1 := PT3_PositionList[i];
            Inc(i)
           end;
          if i >= 65535 - 201 then RaiseBadFileStructure;
          if i > 255 then
           PT3_NumberOfPositions := 255
          else
           PT3_NumberOfPositions := i;
          j := MAddr;
          if j <> 0 then
           begin
            if j >= 65536 then RaiseBadFileStructure;
            if PT3_PatternsPointer < j then RaiseBadFileStructure;
            Dec(PT3_PatternsPointer,j);
            for i := 0 to 31 do
             begin
              if PT3_SamplesPointers[i] < j then RaiseBadFileStructure;
              Dec(PT3_SamplesPointers[i],j)
             end;
            for i := 0 to 15 do
             begin
              if PT3_OrnamentsPointers[i] < j then RaiseBadFileStructure;
              Dec(PT3_OrnamentsPointers[i],j)
             end;
            pwrd := @Index[PT3_PatternsPointer];
            for i := 0 to i1 + 2 do
             begin
              if (PT3_PatternsPointer+i*2 >= 65535) or (pwrd^ < j) then RaiseBadFileStructure;
              Dec(pwrd^,j);
              inc(pwrd);
             end;
           end;
         end;
       end;
    end

 procedure CaseTrModules(FType:Available_Types;n:integer);
 begin
// case FType of
//    PT3File:
 if FType = FT.PT3 then
       begin
        PlConsts[n].Version := 6;
        if PLConsts[n].RAM^.PT3_MusicName[13] in ['0'..'9'] then
         PlConsts[n].Version := Ord(PLConsts[n].RAM^.PT3_MusicName[13]) - $30;
        All_GetRegisters[n] := @PT3_Get_Registers;
       end

      if (CurFileType = FT.PT3{PT3File}) and (PlConsts[0].Version >= 7) then
       begin
        TS := Ord(ZRAM.PT3_MusicName[98]);
        if (TS <> $20) then
         begin
          TSMode := True;
          PLConsts[1] := PLConsts[0];
          PLConsts[1].TS := TS;
          CurFileType1 := CurFileType;//PT3File;
          All_GetRegisters[1] := @PT3_Get_Registers;
         end;
       end;


procedure InitTrackerModule(FType:Available_Types;n:integer);
//PT3File:
else if FType = FT.PT3 then
 begin
  with PlParams[n].PT3,PLConsts[n].RAM^ do
   begin
    DelayCounter := 1;
    Delay := PT3_Delay;
{"FillChared"    CurrentPosition := 0;
    Noise_Base := 0;
    AddToNoise := 0;
    Cur_Env_Slide := 0;
    Cur_Env_Delay := 0;
    Env_Base.wrd := 0}
   end;

   with PLConsts[n].RAM^ do
    begin
     i := PT3_PositionList[0];
     b := PLConsts[n].TS;
     if b <> $20 then i := b * 3 - 3 - i;
     PlParams[n].PT3_A.Address_In_Pattern :=
        PWord(@Index[PT3_PatternsPointer + i * 2])^;
     PlParams[n].PT3_B.Address_In_Pattern :=
        PWord(@Index[PT3_PatternsPointer + i * 2 + 2])^;
     PlParams[n].PT3_C.Address_In_Pattern :=
        PWord(@Index[PT3_PatternsPointer + i * 2 + 4])^
    end;

   with PlParams[n].PT3_A,PLConsts[n].RAM^ do
    begin
     OrnamentPointer := PT3_OrnamentsPointers[0];
     Loop_Ornament_Position := Index[OrnamentPointer];
     inc(OrnamentPointer);
     Ornament_Length := Index[OrnamentPointer];
     inc(OrnamentPointer);
     //extra code begin (pt3 has no default sample anyway)
      SamplePointer := PT3_SamplesPointers[1];
      Loop_Sample_Position := Index[SamplePointer];
      inc(SamplePointer);
      Sample_Length := Index[SamplePointer];
      inc(SamplePointer);
     //extra code end
     Volume := 15;
//"FillChared"     Current_Ton_Sliding := 0;
     Note_Skip_Counter := 1;
{"FillChared"     Current_OnOff := 0;
     Enabled := False;
     Envelope_Enabled := False;
     Note := 0;
     Ton := 0}
    end;

   with PlParams[n].PT3_B do
    begin
     OrnamentPointer := PlParams[n].PT3_A.OrnamentPointer;
     Loop_Ornament_Position := PlParams[n].PT3_A.Loop_Ornament_Position;
     Ornament_Length := PlParams[n].PT3_A.Ornament_Length;
     //extra code begin (pt3 has no default sample anyway)
      SamplePointer := PlParams[n].PT3_A.SamplePointer;
      Loop_Sample_Position := PlParams[n].PT3_A.Loop_Sample_Position;
      Sample_Length := PlParams[n].PT3_A.Sample_Length;
     //extra code end
     Volume := 15;
//"FillChared"     Current_Ton_Sliding := 0;
     Note_Skip_Counter := 1;
{"FillChared"     Current_OnOff := 0;
     Enabled := False;
     Envelope_Enabled := False;
     Note := 0;
     Ton := 0}
    end;

   with PlParams[n].PT3_C do
    begin
     OrnamentPointer := PlParams[n].PT3_A.OrnamentPointer;
     Loop_Ornament_Position := PlParams[n].PT3_A.Loop_Ornament_Position;
     Ornament_Length := PlParams[n].PT3_A.Ornament_Length;
     //extra code begin (pt3 has no default sample anyway)
      SamplePointer := PlParams[n].PT3_A.SamplePointer;
      Loop_Sample_Position := PlParams[n].PT3_A.Loop_Sample_Position;
      Sample_Length := PlParams[n].PT3_A.Sample_Length;
     //extra code end
     Volume := 15;
//"FillChared"     Current_Ton_Sliding := 0;
     Note_Skip_Counter := 1;
{"FillChared"     Current_OnOff := 0;
     Enabled := False;
     Envelope_Enabled := False;
     Note := 0;
     Ton := 0}
    end;

 end

function FoundPT3(IntegrityCheck:boolean;var TimeLength,LoopPoint:integer):boolean;
var
 j,j1,j2,j4,j5,j6,adr:integer;
 pw:PWord;
 M:PModTypes;
begin
TimeLength := 0;
LoopPoint := 0;
FoundPT3 := False;
if Readen1 < 202 then exit;
adr := F_Frame^.PT3_SamplesPointers[0];
j4 := F_Frame^.PT3_PatternsPointer - adr; if (j4 < 202) or (j4 > Readen1) then exit;
if F_Frame^.Index[j4 - 1] <> 255 then exit;
j := F_Frame^.PT3_OrnamentsPointers[0] - adr; if (j < 202) or (j + 2 > Readen1) then exit;

move(F_Frame^.Index[j],j,3); if j <> 256 then exit;

j5 := 0;
j1 := 0;
while j5 < 256 do
 begin
  if j5 + 201 > Readen1 then exit;
  j2 := F_Frame^.PT3_PositionList[j5]; if j2 = 255 then break;
  if j2 mod 3 <> 0 then exit;
  if j1 < j2 then j1 := j2;
  inc(j5);
 end;

pw := @F_Frame^.Index[j4];
j := 65535;
for j2 := 0 to j1 + 2 do
 begin
  if j4 + j2*2 >= Readen1 then exit;
  j6 := pw^ - adr; if (j6 < 202) or (j6 > Readen1) then exit;
  if j > j6 then j := j6;
  inc(pw);
 end;

dec(j,j4); if j <= 0 then exit;
if j  mod 6 <> 0 then exit;
if j div 6 <> j1 div 3 + 1 then exit;

j := 15;
while (j > 0) and (F_Frame^.PT3_OrnamentsPointers[j] = adr) do dec(j);
j := F_Frame^.PT3_OrnamentsPointers[j] - adr; if (j < 202) or (j + 2 > Readen1) then exit;

F_Length := j + F_Frame^.Index[j + 1] + 2;
if F_Length > Readen1 + 1 then exit;

if IntegrityCheck then
 try
  New(M);
  try
   if not LoadTrackerModule(M^,nil,-1,adr,F_Length,F_Frame,FT.PT3{PT3File}) then exit;
   GetTimePT3(M,TimeLength,LoopPoint);
   if TimeLength = 0 then exit;
  finally
   Dispose(M);
  end;
 except
  exit;
 end;

F_Frame^.PT3_NumberOfPositions := j5;
F_Address := adr;
F_Frame^.PT3_PatternsPointer := j4;
for j := 0 to 31 do Dec(F_Frame^.PT3_SamplesPointers[j],adr);
for j := 0 to 15 do Dec(F_Frame^.PT3_OrnamentsPointers[j],adr);
pw := @F_Frame^.Index[j4];
for j := 0 to j1 + 2 do
 begin
  Dec(pw^,adr);
  Inc(pw);
 end;
FoundPT3 := True;
end;

else if FType = FT.PT3 then
            begin
             SetLength(PLItem.Title,32);
             SetLength(PLItem.Author,32);
             if not Loaded then
              begin
               UniFileSeek(URHandle,$1E + PLItem.Offset);
               UniRead(URHandle,@PLItem.Title[1],32);
               UniFileSeek(URHandle,$42 + PLItem.Offset);
               UniRead(URHandle,@PLItem.Author[1],32);
              end
             else
              begin
               Move(F_Frame^.PT3_MusicName[$1E],PLItem.Title[1],32);
               Move(F_Frame^.PT3_MusicName[$42],PLItem.Author[1],32);
              end;
            end

procedure PT3_Get_Registers(CNum:integer);

 function GetNoteFreq(j:integer):integer;
 begin
  case PLConsts[CNum].RAM^.PT3_TonTableId of
  0:if PlConsts[CNum].Version <= 3 then
     Result := PT3NoteTable_PT_33_34r[j]
    else
     Result := PT3NoteTable_PT_34_35[j];
  1:Result := PT3NoteTable_ST[j];
  2:if PlConsts[CNum].Version <= 3 then
     Result := PT3NoteTable_ASM_34r[j]
    else
     Result := PT3NoteTable_ASM_34_35[j];
  else if PlConsts[CNum].Version <= 3 then
        Result := PT3NoteTable_REAL_34r[j]
       else
        Result := PT3NoteTable_REAL_34_35[j]
  end
 end;

 procedure PatternInterpreter(var Chan:PT3_Channel_Parameters);
 var
  quit:boolean;
  Flag9,Flag8,Flag5,Flag4,
  Flag3,Flag2,Flag1:byte;
  counter,b:byte;
  PrNote,PrSliding:integer;
 begin
  PrNote := Chan.Note;
  PrSliding := Chan.Current_Ton_Sliding;
  quit := False;
  counter := 0;
  Flag9 := 0; Flag8 := 0; Flag5 := 0; Flag4 := 0;
  Flag3 := 0; Flag2 := 0; Flag1 := 0;
  with Chan,PLConsts[CNum].RAM^ do
   begin
    repeat
     case Index[Address_In_Pattern] of
     $f0..$ff:
       begin
        OrnamentPointer :=
          PT3_OrnamentsPointers[Index[Address_In_Pattern] - $f0];
        Loop_Ornament_Position := Index[OrnamentPointer];
        Inc(OrnamentPointer);
        Ornament_Length := Index[OrnamentPointer];
        Inc(OrnamentPointer);
        Inc(Address_In_Pattern);
        SamplePointer := PT3_SamplesPointers[Index[Address_In_Pattern] div 2];
        Loop_Sample_Position := Index[SamplePointer];
        Inc(SamplePointer);
        Sample_Length := Index[SamplePointer];
        Inc(SamplePointer);
        Envelope_Enabled := False;
        Position_In_Ornament := 0
       end;
     $d1..$ef:
       begin
        SamplePointer := PT3_SamplesPointers[Index[Address_In_Pattern] - $d0];
        Loop_Sample_Position := Index[SamplePointer];
        Inc(SamplePointer);
        Sample_Length := Index[SamplePointer];
        Inc(SamplePointer)
       end;
     $d0:
       quit := true;
     $c1..$cf:
       Volume := Index[Address_In_Pattern] - $c0;
     $c0:
       begin
        Position_In_Sample := 0;
        Current_Amplitude_Sliding := 0;
        Current_Noise_Sliding := 0;
        Current_Envelope_Sliding := 0;
        Position_In_Ornament := 0;
        Ton_Slide_Count := 0;
        Current_Ton_Sliding := 0;
        Ton_Accumulator := 0;
        Current_OnOff := 0;
        Enabled := False;
        quit := True;
       end;
     $b2..$bf:
       begin
        Envelope_Enabled := True;
        SoundChip[CNum].SetEnvelopeRegister(Index[Address_In_Pattern] - $b1);
        Inc(Address_In_Pattern);
        with PlParams[CNum].PT3 do
         begin
          Env_Base.hi := Index[Address_In_Pattern];
          Inc(Address_In_Pattern);
          Env_Base.lo := Index[Address_In_Pattern];
          Position_In_Ornament := 0;
          Cur_Env_Slide := 0;
          Cur_Env_Delay := 0
         end
       end;
     $b1:
       begin
        inc(Address_In_Pattern);
        Number_Of_Notes_To_Skip := Index[Address_In_Pattern]
       end;
     $b0:
       begin
        Envelope_Enabled := False;
        Position_In_Ornament := 0
       end;
     $50..$af:
       begin
        Note := Index[Address_In_Pattern] - $50;
        Position_In_Sample := 0;
        Current_Amplitude_Sliding := 0;
        Current_Noise_Sliding := 0;
        Current_Envelope_Sliding := 0;
        Position_In_Ornament := 0;
        Ton_Slide_Count := 0;
        Current_Ton_Sliding := 0;
        Ton_Accumulator := 0;
        Current_OnOff := 0;
        Enabled := True;
        quit := True
       end;
     $40..$4f:
       begin
        OrnamentPointer :=
          PT3_OrnamentsPointers[Index[Address_In_Pattern] - $40];
        Loop_Ornament_Position := Index[Chan.OrnamentPointer];
        Inc(OrnamentPointer);
        Ornament_Length := Index[OrnamentPointer];
        Inc(OrnamentPointer);
        Position_In_Ornament := 0
       end;
     $20..$3f:
       PlParams[CNum].PT3.Noise_Base := Index[Address_In_Pattern] - $20;
     $10..$1f:
       begin
        if Index[Address_In_Pattern] = $10 then
         Envelope_Enabled := False
        else
         begin
          SoundChip[CNum].SetEnvelopeRegister(Index[Address_In_Pattern] - $10);
          Inc(Address_In_Pattern);
          with PlParams[CNum].PT3 do
           begin
            Env_Base.hi := Index[Address_In_Pattern];
            Inc(Address_In_Pattern);
            Env_Base.lo := Index[Address_In_Pattern];
            Envelope_Enabled := True;
            Cur_Env_Slide := 0;
            Cur_Env_Delay := 0
           end
         end;
        Inc(Address_In_Pattern);
        SamplePointer := PT3_SamplesPointers[Index[Address_In_Pattern] div 2];
        Loop_Sample_Position := Index[SamplePointer];
        Inc(SamplePointer);
        Sample_Length := Index[SamplePointer];
        Inc(SamplePointer);
        Position_In_Ornament := 0
       end;
     $9:
       begin
        Inc(counter);
        Flag9 := counter
       end;
     $8:
       begin
        Inc(counter);
        Flag8 := counter
       end;
     $5:
       begin
        Inc(counter);
        Flag5 := counter
       end;
     $4:
       begin
        Inc(counter);
        Flag4 := counter
       end;
     $3:
       begin
        Inc(counter);
        Flag3 := counter
       end;
     $2:
       begin
        Inc(counter);
        Flag2 := counter
       end;
     $1:
       begin
        Inc(counter);
        Flag1 := counter
       end
     end;
     inc(Address_In_Pattern)
    until quit;
    while counter > 0 do
     begin
      if (counter = Flag1) then
       begin
        Ton_Slide_Delay := Index[Address_In_Pattern];
        Ton_Slide_Count := Ton_Slide_Delay;
        Inc(Address_In_Pattern);
        Ton_Slide_Step := PWord(@Index[Address_In_Pattern])^;
        Inc(Address_In_Pattern,2);
        SimpleGliss := True;
        Current_OnOff := 0;
        if (Ton_Slide_Count = 0) and (PlConsts[CNum].Version >= 7) then
         Inc(Ton_Slide_Count);
       end
      else if (counter = Flag2) then
       begin
        SimpleGliss := False;
        Current_OnOff := 0;
        Ton_Slide_Delay := Index[Address_In_Pattern];
        Ton_Slide_Count := Ton_Slide_Delay;
        Inc(Address_In_Pattern,3);
        Ton_Slide_Step := Abs(SmallInt(PWord(@Index[Address_In_Pattern])^));
        Inc(Address_In_Pattern,2);
        Ton_Delta := GetNoteFreq(Note) - GetNoteFreq(PrNote);
        Slide_To_Note := Note;
        Note := PrNote;
        if PlConsts[CNum].Version >= 6 then
         Current_Ton_Sliding := PrSliding;
        if Ton_Delta - Current_Ton_Sliding < 0 then
         Ton_Slide_Step := -Ton_Slide_Step
       end
      else if counter = Flag3 then
       begin
        Position_in_Sample := Index[Address_In_Pattern];
        Inc(Address_In_Pattern)
       end
      else if counter = Flag4 then
       begin
        Position_in_Ornament := Index[Address_In_Pattern];
        inc(Address_In_Pattern)
       end
      else if counter = Flag5 then
       begin
        OnOff_Delay := Index[Address_In_Pattern];
        Inc(Address_In_Pattern);
        OffOn_Delay := Index[Address_In_Pattern];
        Current_OnOff := OnOff_Delay;
        Inc(Address_In_Pattern);
        Ton_Slide_Count := 0;
        Current_Ton_Sliding := 0
       end
      else if counter = Flag8 then
       begin
        with PlParams[CNum].PT3 do
         begin
          Env_Delay := Index[Address_In_Pattern];
          Cur_Env_Delay := Env_Delay;
          Inc(Address_In_Pattern);
          Env_Slide_Add := PWord(@Index[Address_In_Pattern])^;
         end;
        Inc(Address_In_Pattern,2)
       end
      else if counter = Flag9 then
       begin
        b := Index[Address_In_Pattern];
        PlParams[CNum].PT3.Delay := b;
        if TSMode and (PLConsts[1].TS <> $20) then
         begin
          PlParams[0].PT3.Delay := b;
          PlParams[0].PT3.DelayCounter := b;
          PlParams[1].PT3.Delay := b;
         end;
        Inc(Address_In_Pattern)
       end;
      Dec(counter)
     end;
    Note_Skip_Counter := Number_Of_Notes_To_Skip
   end
 end;

var
 TempMixer:byte;
 AddToEnv:shortint;

 procedure ChangeRegisters(var Chan:PT3_Channel_Parameters);
 var
  j,b1,b0:byte;
  w:word;
 begin
  with Chan,PLConsts[CNum].RAM^ do
   begin
    if Enabled then
     begin
      Ton := PWord(@Index[SamplePointer + Position_In_Sample * 4 + 2])^;
      Inc(Ton,Ton_Accumulator);
      b0 := Index[SamplePointer + Position_In_Sample * 4];
      b1 := Index[SamplePointer + Position_In_Sample * 4 + 1];
      if b1 and $40 <> 0 then
       Ton_Accumulator := Ton;
      j := Note + Index[OrnamentPointer + Position_In_Ornament];
      if shortint(j) < 0 then j := 0 else if j > 95 then j := 95;
      w := GetNoteFreq(j);
      Ton := (Ton + Current_Ton_Sliding + w) and $fff;
      if Ton_Slide_Count > 0 then
       begin
        Dec(Ton_Slide_Count);
        if Ton_Slide_Count = 0 then
         begin
          Inc(Current_Ton_Sliding,Ton_Slide_Step);
          Ton_Slide_Count := Ton_Slide_Delay;
          if not SimpleGliss then
           if ((Ton_Slide_Step < 0) and (Current_Ton_Sliding <= Ton_Delta)) or
              ((Ton_Slide_Step >= 0) and (Current_Ton_Sliding >= Ton_Delta)) then
            begin
             Note := Slide_To_Note;
             Ton_Slide_Count := 0;
             Current_Ton_Sliding := 0
            end
         end
       end;
      Amplitude := b1 and $f;
      if b0 and $80 <> 0 then
      if b0 and $40 <> 0 then
       begin
        if Current_Amplitude_Sliding < 15 then
         inc(Current_Amplitude_Sliding)
       end
      else if Current_Amplitude_Sliding > -15 then
       dec(Current_Amplitude_Sliding);
      inc(Amplitude,Current_Amplitude_Sliding);
      if shortint(Amplitude) < 0 then Amplitude := 0
      else if Amplitude > 15 then Amplitude := 15;
      if PlConsts[CNum].Version <= 4 then
       Amplitude := PT3VolumeTable_33_34[Volume,Amplitude]
      else
       Amplitude := PT3VolumeTable_35[Volume,Amplitude];
      if (b0 and 1 = 0) and Envelope_Enabled then
       Amplitude := Amplitude or 16;
      if b1 and $80 <> 0 then
       begin
        if b0 and $20 <> 0 then
         j := (b0 shr 1) or $F0 + Current_Envelope_Sliding
        else
         j := (b0 shr 1) and $F + Current_Envelope_Sliding;
        if b1 and $20 <> 0 then Current_Envelope_Sliding := j;
        Inc(AddToEnv,j);
       end
      else
       begin
        PlParams[CNum].PT3.AddToNoise := b0 shr 1 + Current_Noise_Sliding;
        if b1 and $20 <> 0 then
         Current_Noise_Sliding := PlParams[CNum].PT3.AddToNoise
       end;
      TempMixer := b1 shr 1 and $48 or TempMixer;
      Inc(Position_In_Sample);
      if Position_In_Sample >= Sample_Length then
       Position_In_Sample := Loop_Sample_Position;
      Inc(Position_In_Ornament);
      if Position_In_Ornament >= Ornament_Length then
       Position_In_Ornament := Loop_Ornament_Position
     end
    else
     Amplitude := 0;
    TempMixer := TempMixer shr 1;
    if Current_OnOff > 0 then
     begin
      dec(Current_OnOff);
      if Current_OnOff = 0 then
       begin
        Enabled := not Enabled;
        if Enabled then Current_OnOff := OnOff_Delay
        else Current_OnOff := OffOn_Delay
       end;
     end
   end
 end;

var
 i,b:integer;
begin
if PlConsts[CNum].Global_Tick_Counter >= PlConsts[CNum].Global_Tick_Max then
 if Do_Loop then
  PlConsts[CNum].Global_Tick_Counter := PlConsts[CNum].Global_Tick_Max
 else
  begin
   Real_End[CNum] := True;
   exit
  end;
with PlParams[CNum].PT3 do
 begin
  Dec(DelayCounter);
  if DelayCounter = 0 then
   begin
    with PlParams[CNum].PT3_A do
     begin
      Dec(Note_Skip_Counter);
      if Note_Skip_Counter = 0 then
       with PLConsts[CNum].RAM^ do
        begin
         if (Index[Address_In_Pattern] = 0) then
          begin
           inc(CurrentPosition);
           if CurrentPosition = PT3_NumberOfPositions then
            CurrentPosition := PT3_LoopPosition;
           i := PT3_PositionList[CurrentPosition];
           b := PLConsts[CNum].TS;
           if b <> $20 then i := b * 3 - 3 - i;
           Address_In_Pattern :=
             PWord(@Index[PT3_PatternsPointer +  i * 2])^;
           PlParams[CNum].PT3_B.Address_In_Pattern :=
             PWord(@Index[PT3_PatternsPointer +  i * 2 + 2])^;
           PlParams[CNum].PT3_C.Address_In_Pattern :=
             PWord(@Index[PT3_PatternsPointer +  i * 2 + 4])^;
           Noise_Base := 0;
          end;
         PatternInterpreter(PlParams[CNum].PT3_A);
        end;
     end;
    with PlParams[CNum].PT3_B do
     begin
      Dec(Note_Skip_Counter);
      if Note_Skip_Counter = 0 then
       PatternInterpreter(PlParams[CNum].PT3_B);
     end;
    with PlParams[CNum].PT3_C do
     begin
      Dec(Note_Skip_Counter);
      if Note_Skip_Counter = 0 then
       PatternInterpreter(PlParams[CNum].PT3_C);
     end;
    DelayCounter := Delay;
   end;

  AddToEnv := 0;
  TempMixer := 0;
  ChangeRegisters(PlParams[CNum].PT3_A);
  ChangeRegisters(PlParams[CNum].PT3_B);
  ChangeRegisters(PlParams[CNum].PT3_C);

  SoundChip[CNum].SetMixerRegister(TempMixer);

  SoundChip[CNum].RegisterAY.TonA := PlParams[CNum].PT3_A.Ton;
  SoundChip[CNum].RegisterAY.TonB := PlParams[CNum].PT3_B.Ton;
  SoundChip[CNum].RegisterAY.TonC := PlParams[CNum].PT3_C.Ton;

  SoundChip[CNum].SetAmplA(PlParams[CNum].PT3_A.Amplitude);
  SoundChip[CNum].SetAmplB(PlParams[CNum].PT3_B.Amplitude);
  SoundChip[CNum].SetAmplC(PlParams[CNum].PT3_C.Amplitude);

  SoundChip[CNum].RegisterAY.Noise := (Noise_Base + AddToNoise) and 31;

  SoundChip[CNum].RegisterAY.Envelope := Env_Base.wrd + AddToEnv + Cur_Env_Slide;

  if Cur_Env_Delay > 0 then
   begin
    Dec(Cur_Env_Delay);
    if Cur_Env_Delay = 0 then
     begin
      Cur_Env_Delay := Env_Delay;
      Inc(Cur_Env_Slide,Env_Slide_Add)
     end
   end
end;

Inc(PlConsts[CNum].Global_Tick_Counter);
end;

procedure GetTimePT3(Module:PModTypes;var Tm,Lp:integer);
var
 b:byte;
 TS:integer;
 vars:array[0..1] of record
  a1,a2,a3,a11,a22,a33:shortint;
  j1,j2,j3:longword;
 end;

 procedure GetPatPtrs(n,i:integer);
 begin
  if (i < 0) or (i > 84 * 3) or (i mod 3 <> 0) then RaiseBadFileStructure;
  with vars[n],Module^ do
   begin
    j1 := PWord(@Index[PT3_PatternsPointer + i * 2])^;
    j2 := PWord(@Index[PT3_PatternsPointer + i * 2 + 2])^;
    j3 := PWord(@Index[PT3_PatternsPointer + i * 2 + 4])^;
   end;
 end;

 function PatInt(n:integer):boolean;
 var
  j,c1,c2,c3,c4,c5,c8:integer;
 begin
 Result := False;
  with vars[n],Module^ do
   begin
        dec(a1);
        if a1 = 0 then
         begin
          if Index[j1] = 0 then exit;
          j := 0; c1 := 0; c2 := 0; c3 := 0; c4 := 0; c5 := 0; c8 := 0;
          repeat
           case Index[j1] of
           $d0,$c0,$50..$af:
            begin
             a1 := a11;
             incr(j1);
             break
            end;
           $10,$f0..$ff:
            inc(j1);
           $b2..$bf:
            inc(j1,2);
           $b1:
            begin
             incr(j1);
             a11 := Index[j1]
            end;
           $11..$1f:
            inc(j1,3);
           1:
            begin
             inc(j);
             c1 := j
            end;
           2:
            begin
             inc(j);
             c2 := j
            end;
           3:
            begin
             inc(j);
             c3 := j
            end;
           4:
            begin
             inc(j);
             c4 := j
            end;
           5:
            begin
             inc(j);
             c5 := j
            end;
           8:
            begin
             inc(j);
             c8 := j
            end;
           9:
            inc(j)
           end;
           incr(j1)
          until False;
          while j > 0 do
           begin
            if (j = c1) or (j = c8) then
             inc(j1,3)
            else if (j = c2) then
             inc(j1,5)
            else if (j = c3) or (j = c4) then
             inc(j1)
            else if (j = c5) then
             inc(j1,2)
            else
             begin
              b := Index[j1];
              inc(j1)
             end;
            if j1 >= 65536 then RaiseBadFileStructure;
            dec(j)
           end
         end;
        dec(a2);
        if a2 = 0 then
         begin
          j := 0; c1 := 0; c2 := 0; c3 := 0; c4 := 0; c5 := 0; c8 := 0;
          repeat
           case Index[j2] of
           $d0,$c0,$50..$af:
            begin
             a2 := a22;
             incr(j2);
             break
            end;
           $10,$f0..$ff:
            inc(j2);
           $b2..$bf:
            inc(j2,2);
           $b1:
            begin
             incr(j2);
             a22 := Index[j2]
            end;
           $11..$1f:
            inc(j2,3);
           1:
            begin
             inc(j);
             c1 := j
            end;
           2:
            begin
             inc(j);
             c2 := j
            end;
           3:
            begin
             inc(j);
             c3 := j
            end;
           4:
            begin
             inc(j);
             c4 := j
            end;
           5:
            begin
             inc(j);
             c5 := j
            end;
           8:
            begin
             inc(j);
             c8 := j
            end;
           9:
            inc(j)
           end;
           incr(j2)
          until False;
          while j > 0 do
           begin
            if (j = c1) or (j = c8) then
             inc(j2,3)
            else if (j = c2) then
             inc(j2,5)
            else if (j = c3) or (j = c4) then
             inc(j2)
            else if (j = c5) then
             inc(j2,2)
            else
             begin
              b := Index[j2];
              inc(j2)
             end;
            if j2 >= 65536 then RaiseBadFileStructure;
            dec(j)
           end
         end;
        dec(a3);
        if a3 = 0 then
         begin
          j := 0; c1 := 0; c2 := 0; c3 := 0; c4 := 0; c5 := 0; c8 := 0;
          repeat
           case Module^.Index[j3] of
           $d0,$c0,$50..$af:
            begin
             a3 := a33;
             incr(j3);
             break
            end;
           $10,$f0..$ff:
            inc(j3);
           $b2..$bf:
            inc(j3,2);
           $b1:
            begin
             incr(j3);
             a33 := Index[j3]
            end;
           $11..$1f:
            inc(j3,3);
           1:
            begin
             inc(j);
             c1 := j
            end;
           2:
            begin
             inc(j);
             c2 := j
            end;
           3:
            begin
             inc(j);
             c3 := j
            end;
           4:
            begin
             inc(j);
             c4 := j
            end;
           5:
            begin
             inc(j);
             c5 := j
            end;
           8:
            begin
             inc(j);
             c8 := j
            end;
           9:
            inc(j)
           end;
           incr(j3)
          until False;
          while j > 0 do
           begin
            if (j = c1) or (j = c8) then
             inc(j3,3)
            else if (j = c2) then
             inc(j3,5)
            else if (j = c3) or (j = c4) then
             inc(j3)
            else if (j = c5) then
             inc(j3,2)
            else
             begin
              b := Index[j3];
              inc(j3)
             end;
            if j3 >= 65536 then RaiseBadFileStructure;
            dec(j);
           end;
         end;
   end;
 Result := True;  
 end;

var
 i:integer;
 DLCatcher:integer;

begin
   with Module^ do
    begin
     b := PT3_Delay; TS := $20;
     if PT3_MusicName[13] in ['7'..'9'] then
      TS := Ord(PT3_MusicName[98]);
     for i := 0 to 1 do
      with vars[i] do
       begin
        a11 := 1; a22 := 1; a33 := 1;
        DLCatcher := 256*256; //max 256 patterns 256 lines per pattern
       end;
     for i := 0 to PT3_NumberOfPositions - 1 do
      begin
       if i = PT3_LoopPosition then Lp := tm;
       GetPatPtrs(0,PT3_PositionList[i]);
       with vars[0] do
        begin
         a1 := 1; a2 := 1; a3 := 1;
        end;
       if TS <> $20 then
        begin
         GetPatPtrs(1,TS * 3 - 3 - PT3_PositionList[i]);
         with vars[1] do
          begin
           a1 := 1; a2 := 1; a3 := 1;
          end;
        end;
       repeat
        if not PatInt(0) then break;
        if TS <> $20 then
         if not PatInt(1) then break;
        Inc(tm,b);
        Dec(DLCatcher);
        if DLCatcher < 0 then RaiseBadFileStructure;
       until False;
      end;
    end;
end;

#endif
