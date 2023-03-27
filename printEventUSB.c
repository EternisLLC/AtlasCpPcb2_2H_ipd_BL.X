void PrintEventUSB(void) {
    
    InitUSBTerminal(115200);//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
    
    if (BufferReadEvent.TypeEvent != 255) {
        xprintf("%u-%02u-%02u %02u:%02u:%02u ",
                BufferReadEvent.Year, BufferReadEvent.Month, BufferReadEvent.Day,
                BufferReadEvent.Hour, BufferReadEvent.Minute, BufferReadEvent.Second);
        switch (BufferReadEvent.TypeEvent) {
            case 100://ЧЛМАЮЕОЙЕ РЙФБОЙС лр
                xprintf("включение питания КП\r");
                break;
            case 101://ЙЪНЕОЕОЩ ЧТЕНС Й ДБФБ
                xprintf("изменены время и дата\r");
                break;
            case 102://ДПВБЧМЕО вх
                xprintf("Ячейка %u ", BufferReadEvent.Direct);
                xprintf("добавлен %s %lu\r", (BufferReadEvent.SerialNumber%1000) < 501?"БУР":"РСМ",BufferReadEvent.SerialNumber);
                break;
            case 103://ХДБМЕО вх
                xprintf("Ячейка %u ", BufferReadEvent.Direct);
                xprintf("удален %s %lu\r",(BufferReadEvent.SerialNumber%1000) < 501?"БУР":"РСМ", BufferReadEvent.SerialNumber);
                break;
            case 104://ЙЪН. ОБРТСЦЕОЙЕ РЙФБОЙС лр
                xprintf("изм. напряжение питания КП ");
                xprintf("PWR1 %u,%uВ ", BufferReadEvent.IdNet/100,BufferReadEvent.IdNet%100);
                xprintf("PWR2 %u,%uВ\r", BufferReadEvent.QuantityBos/100,BufferReadEvent.QuantityBos%100);
                break;
            case 106:
                xprintf("Зарегистрирован КИД-%u %lu\r",BufferReadEvent.reserv,BufferReadEvent.SerialNumber);
                break;
            case 1://ХУФ. УЧСЪШ У вх
                xprintf("уст. связь с %s %lu ",(BufferReadEvent.SerialNumber%1000) < 501?"БУР":"РСМ", BufferReadEvent.SerialNumber);
                xprintf("направление %u ", BufferReadEvent.Direct);
                xprintf("зарегистрированно %u БОС\r", BufferReadEvent.QuantityBos);
                break;
            case 2://РПФЕТСОБ УЧСЪШ У вх
                xprintf("потеряна связь с %s %lu ", (BufferReadEvent.SerialNumber%1000) < 501?"БУР":"РСМ",BufferReadEvent.SerialNumber);
                xprintf("направление %u\r", BufferReadEvent.Direct);
                break;
            case 3://ЙЪНЕОЕОЙЕ ПВУФБОПЧЛЙ
                //                xprintf("изменение обстановки ");
                switch (BufferReadEvent.Situation) {
                    case 0:
                        sprintf(lcdline,"%s","НОРМА");
                        break;
                    case 1:
                        sprintf(lcdline,"%s","ВНИМАНИЕ");
                        break;
                    case 2:
                        sprintf(lcdline,"%s","ПОЖАР");
                        break;
                    case 3:
                        sprintf(lcdline,"%s","ПУСК");
                        break;
                }
                xprintf("%s %lu направл. %u обстановка %s\r",(BufferReadEvent.SerialNumber%1000) < 501?"БУР":"РСМ", BufferReadEvent.SerialNumber, BufferReadEvent.Direct,&lcdline[0]);
                break;
            case 4://ЙЪНЕОЕОЙЕ УПУФПСОЙС ПВПТХДПЧБОЙС
                xprintf("изменение состояния ");
                xprintf("%s %lu направл. %u\r",(BufferReadEvent.SerialNumber%1000) < 501?"БУР":"РСМ", BufferReadEvent.SerialNumber, BufferReadEvent.Direct);
                TempErrByte.ErrByte = BufferReadEvent.ErrByte;
                if(TempErrByte.ErrByte || BufferReadEvent.reserv){
                    if (TempErrByte.ConnectBos) {
                        xprintf("потеряна связь с БОС\r");
                    }
                    if (TempErrByte.Bos) {
                        xprintf("неисправность БОС\r");
                    }
                    if (TempErrByte.IPR) {
                        xprintf("неисправность ШС ИП\r");
                    }
                    if (TempErrByte.SMK) {
                        xprintf("неисправность ШС дверей\r");
                    }
                    if (TempErrByte.noteALARM) {
                        xprintf("неисправность ШС ОП ТРЕВОГА\r");
                    }
                    if (TempErrByte.noteAUTO) {
                        xprintf("неисправность ШС ОП АВТОМАТИКА\r");
                    }
                    if (TempErrByte.Pwr1) {
                        xprintf("неисправность источника питания БУР\r");
                    }
// ЙЪН. 28.03.22                    
                    if(BufferReadEvent.SerialNumber > 2111000){
                        if(BufferReadEvent.reserv & 0x01){
                            xprintf("неисправность ШС УДП\r"); // ОЕЙУРТБЧОПУФШ ыу хдр
                        }
                        if(BufferReadEvent.reserv & 0x02){
                            xprintf("неисправность ШС УВОА\r");// ОЕЙУРТБЧОПУФШ ыу хчпб
                        }
                    }
//================                    
                }else{
                     xprintf("оборудование норма\r");
                }
                break;
            case 5:// ЙЪНЕОЙМПУШ УПУФПСОЙЕ БЧФПНБШФЙЛЙ
                TempAutoByte.StatusAutoByte = BufferReadEvent.StatusAutoByte;
                if(!TempAutoByte.Delay2minOn && !TempAutoByte.StopStart){
                    sprintf(lcdline,"%s","автоматика включнена, двери закрыты");
                    // БЧФПНБФЙЛБ ЧЛМАЮЕОБ, ДЧЕТЙ ЪБЛТЩФЩ
                }else{
                    if(TempAutoByte.StopStart){
                        sprintf(lcdline,"%s","автоматика заблокирована");
                        // БЧФПНБФЙЛБ ЪБВМПЛЙТПЧБОРБ
                    }
                    if(TempAutoByte.Delay2minOn){
                        sprintf(lcdline,"%s","открыты двери");
                        // ДЧЕТЙ ПФЛТЩФЩ
                    }
                }
                xprintf("%s %lu направление %u %s\r",(BufferReadEvent.SerialNumber%1000) < 501?"БУР":"РСМ",
                            BufferReadEvent.SerialNumber,BufferReadEvent.Direct,&lcdline[0]);
                break;
        case 6:// ЛПНБОДБ УВТПУ
            xprintf("Направление %u, ПЕРЕДАНА КОМАНДА СБРОС, использован ключ N%u\r",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 7:// ЛПНБОДБ РХУЛ РП ОБРТБЧМЕОЙА
            xprintf("Направление %u, ПЕРЕДАНА КОМАНДА СТАРТ, использован ключ N%u\r",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 8:// ыч ЛПНБОДБ РХУЛ
            xprintf("ДЛЯ ВСЕХ, ПЕРЕДАНА КОМАНДА СТАРТ, использован ключ N%u\r",BufferReadEvent.reserv);
            break;
        case 14: //  ОБЦБФБ ЛОПРЛБ ХРТБЧМЕОЙС БЧФПНБФЙЛПК
            // БЧФПНБФЙЛБ ТБЪВМПЛЙТПЧБОБ ЛОПРЛПК ОБ ЛР
            xprintf("Направление %u, Автоматика РАЗблокирована, использован ключ N%u\r",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 15:
            // БЧФПНБФЙЛБ ЪБВМПЛЙТПЧБОБ ЛОПРЛПК ОБ ЛР
            xprintf("Направление %u, Автоматика ЗАблокирована, использован ключ N%u\r",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        default:
            Nop();
            xprintf("событие %u не опознано\r", BufferReadEvent.TypeEvent);
            break;
        }
    }
}