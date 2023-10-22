#ifndef IDT_H
#define IDT_H



#define EXCEPTION_STATUS 256

extern void handler_DE(void);
extern void handler_DB(void);
extern void handler_NMI(void);
extern void handler_BP(void);
extern void handler_OF(void);
extern void handler_BR(void);
extern void handler_UD(void);
extern void handler_NM(void);
extern void handler_DF(void);
extern void handler_CSO(void);
extern void handler_TS(void);
extern void handler_NP(void);
extern void handler_SS(void);
extern void handler_GP(void);
extern void handler_PF(void);
extern void handler_MF(void);
extern void handler_AC(void);
extern void handler_MC(void);
extern void handler_XF(void);
extern void IDT_init(void);






#endif
