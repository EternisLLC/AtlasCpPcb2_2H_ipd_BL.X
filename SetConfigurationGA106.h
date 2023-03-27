//------ Биты конфигурации -------------
//Биты расписаны в c:\Program Files (x86)\Microchip\xc16\v1.33\docs\/config_index.html или стр.245
#pragma config JTAGEN   =   OFF     //Вкл-выкл JTAG порта
#pragma config GCP      =   OFF     //Защита кода памяти программ
#pragma config GWRP     =   OFF     //Разрешение записи в память программ
#pragma config BKBUG    =   OFF     //Фоновая отладка
#pragma config ICS      =   PGx1    //Выбор порта отладки
#pragma config FWDTEN   =   OFF     //Вкл-выкл WDT
#pragma config WINDIS   =   OFF     //Вкл-выкл оконного режима WDT
#pragma config FWPSA    =   PR128   //Предделитель WDT(1:128)
#pragma config WDTPS    =   PS32768 //Постделитель WDT(1:32,768)
#pragma config IESO     =   OFF     //Переключатель режима вутреннего-внешнего тактирования
#pragma config FNOSC    =   PRIPLL  //Выбор генератора (FRC-внутренний) FRCPLL,PRI,PRIPLL,SOSC,LPRC,FRCDIV
#pragma config FCKSM    =   CSDCMD  //Мониторинг отказа тактового генератора
#pragma config OSCIOFNC =   OFF     //Назначение ножки RC15 (ножка в/в или выход FOSC/2)
#pragma config IOL1WAY  =   OFF     //Разрешение однократной записи в регистры RPxx (OFF - многократная запись RPxx)
#pragma config POSCMOD  =   XT      //Выбор режима основного генератора EC,XT,HS,NONE
#pragma config WPEND    =   WPSTARTMEM//Защита памяти программ от записи по сегментам или всей памяти
#pragma config WPCFG    =   WPCFGDIS//Защита битов CONFIG
#pragma config WPDIS    =   WPDIS   //Выключение сегментной защиты
#pragma config WPFP     =   WPFP0   //Выбор страницы при вкл. сегментной защите

