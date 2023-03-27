void TerminalLcd (void);
void TerminalRs  (void);
int xgetsU4 (		/* 0:нет данных для обработки, 1:строка доступна */
	char* buff,	/* указатель на буфер */
	int len		/* Длинна буфера */
);
int xatoi (			/* 0:Failed, 1:Successful */
	char **str,		/* Pointer to pointer to the string */
	long *res		/* Pointer to the valiable to store the value */
);
typedef struct COMMANDTYP{
   const char *Command;      //команда (в текстовом виде)
   const char Name;             //имя в листе команд
}CommandType;

extern UINT8   TempDirectRs;
extern UINT8   TempCommandRs;
extern char   LcdBufferData[64];

