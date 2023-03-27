void PrintEventUSB(void) {
    
    InitUSBTerminal(115200);//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
    
    if (BufferReadEvent.TypeEvent != 255) {
        xprintf("%u-%02u-%02u %02u:%02u:%02u ",
                BufferReadEvent.Year, BufferReadEvent.Month, BufferReadEvent.Day,
                BufferReadEvent.Hour, BufferReadEvent.Minute, BufferReadEvent.Second);
        switch (BufferReadEvent.TypeEvent) {
            case 100://��������� ������� ��
                xprintf("��������� ������� ��\r");
                break;
            case 101://�������� ����� � ����
                xprintf("�������� ����� � ����\r");
                break;
            case 102://�������� ��
                xprintf("������ %u ", BufferReadEvent.Direct);
                xprintf("�������� %s %lu\r", (BufferReadEvent.SerialNumber%1000) < 501?"���":"���",BufferReadEvent.SerialNumber);
                break;
            case 103://������ ��
                xprintf("������ %u ", BufferReadEvent.Direct);
                xprintf("������ %s %lu\r",(BufferReadEvent.SerialNumber%1000) < 501?"���":"���", BufferReadEvent.SerialNumber);
                break;
            case 104://���. ���������� ������� ��
                xprintf("���. ���������� ������� �� ");
                xprintf("PWR1 %u,%u� ", BufferReadEvent.IdNet/100,BufferReadEvent.IdNet%100);
                xprintf("PWR2 %u,%u�\r", BufferReadEvent.QuantityBos/100,BufferReadEvent.QuantityBos%100);
                break;
            case 106:
                xprintf("��������������� ���-%u %lu\r",BufferReadEvent.reserv,BufferReadEvent.SerialNumber);
                break;
            case 1://���. ����� � ��
                xprintf("���. ����� � %s %lu ",(BufferReadEvent.SerialNumber%1000) < 501?"���":"���", BufferReadEvent.SerialNumber);
                xprintf("����������� %u ", BufferReadEvent.Direct);
                xprintf("����������������� %u ���\r", BufferReadEvent.QuantityBos);
                break;
            case 2://�������� ����� � ��
                xprintf("�������� ����� � %s %lu ", (BufferReadEvent.SerialNumber%1000) < 501?"���":"���",BufferReadEvent.SerialNumber);
                xprintf("����������� %u\r", BufferReadEvent.Direct);
                break;
            case 3://��������� ����������
                //                xprintf("��������� ���������� ");
                switch (BufferReadEvent.Situation) {
                    case 0:
                        sprintf(lcdline,"%s","�����");
                        break;
                    case 1:
                        sprintf(lcdline,"%s","��������");
                        break;
                    case 2:
                        sprintf(lcdline,"%s","�����");
                        break;
                    case 3:
                        sprintf(lcdline,"%s","����");
                        break;
                }
                xprintf("%s %lu �������. %u ���������� %s\r",(BufferReadEvent.SerialNumber%1000) < 501?"���":"���", BufferReadEvent.SerialNumber, BufferReadEvent.Direct,&lcdline[0]);
                break;
            case 4://��������� ��������� ������������
                xprintf("��������� ��������� ");
                xprintf("%s %lu �������. %u\r",(BufferReadEvent.SerialNumber%1000) < 501?"���":"���", BufferReadEvent.SerialNumber, BufferReadEvent.Direct);
                TempErrByte.ErrByte = BufferReadEvent.ErrByte;
                if(TempErrByte.ErrByte || BufferReadEvent.reserv){
                    if (TempErrByte.ConnectBos) {
                        xprintf("�������� ����� � ���\r");
                    }
                    if (TempErrByte.Bos) {
                        xprintf("������������� ���\r");
                    }
                    if (TempErrByte.IPR) {
                        xprintf("������������� �� ��\r");
                    }
                    if (TempErrByte.SMK) {
                        xprintf("������������� �� ������\r");
                    }
                    if (TempErrByte.noteALARM) {
                        xprintf("������������� �� �� �������\r");
                    }
                    if (TempErrByte.noteAUTO) {
                        xprintf("������������� �� �� ����������\r");
                    }
                    if (TempErrByte.Pwr1) {
                        xprintf("������������� ��������� ������� ���\r");
                    }
// ���. 28.03.22                    
                    if(BufferReadEvent.SerialNumber > 2111000){
                        if(BufferReadEvent.reserv & 0x01){
                            xprintf("������������� �� ���\r"); // ������������� �� ���
                        }
                        if(BufferReadEvent.reserv & 0x02){
                            xprintf("������������� �� ����\r");// ������������� �� ����
                        }
                    }
//================                    
                }else{
                     xprintf("������������ �����\r");
                }
                break;
            case 5:// ���������� ��������� �����������
                TempAutoByte.StatusAutoByte = BufferReadEvent.StatusAutoByte;
                if(!TempAutoByte.Delay2minOn && !TempAutoByte.StopStart){
                    sprintf(lcdline,"%s","���������� ���������, ����� �������");
                    // ���������� ��������, ����� �������
                }else{
                    if(TempAutoByte.StopStart){
                        sprintf(lcdline,"%s","���������� �������������");
                        // ���������� ��������������
                    }
                    if(TempAutoByte.Delay2minOn){
                        sprintf(lcdline,"%s","������� �����");
                        // ����� �������
                    }
                }
                xprintf("%s %lu ����������� %u %s\r",(BufferReadEvent.SerialNumber%1000) < 501?"���":"���",
                            BufferReadEvent.SerialNumber,BufferReadEvent.Direct,&lcdline[0]);
                break;
        case 6:// ������� �����
            xprintf("����������� %u, �������� ������� �����, ����������� ���� N%u\r",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 7:// ������� ���� �� �����������
            xprintf("����������� %u, �������� ������� �����, ����������� ���� N%u\r",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 8:// �� ������� ����
            xprintf("��� ����, �������� ������� �����, ����������� ���� N%u\r",BufferReadEvent.reserv);
            break;
        case 14: //  ������ ������ ���������� �����������
            // ���������� �������������� ������� �� ��
            xprintf("����������� %u, ���������� ��������������, ����������� ���� N%u\r",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 15:
            // ���������� ������������� ������� �� ��
            xprintf("����������� %u, ���������� �������������, ����������� ���� N%u\r",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        default:
            Nop();
            xprintf("������� %u �� ��������\r", BufferReadEvent.TypeEvent);
            break;
        }
    }
}