void TerminalLcd (void);
void TerminalRs  (void);
int xgetsU4 (		/* 0:��� ������ ��� ���������, 1:������ �������� */
	char* buff,	/* ��������� �� ����� */
	int len		/* ������ ������ */
);
int xatoi (			/* 0:Failed, 1:Successful */
	char **str,		/* Pointer to pointer to the string */
	long *res		/* Pointer to the valiable to store the value */
);
typedef struct COMMANDTYP{
   const char *Command;      //������� (� ��������� ����)
   const char Name;             //��� � ����� ������
}CommandType;

extern UINT8   TempDirectRs;
extern UINT8   TempCommandRs;
extern char   LcdBufferData[128];

