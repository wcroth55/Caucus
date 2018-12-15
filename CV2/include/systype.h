#define  NUT40 0
#define  WNT40 0
#define  AIX41 0  /* IBM's  AIX 4.1 */ 
#define  BSD21 0  /* BSDI's BSD 2.1 */ 
#define  DEC32 0  /* DEC Unix (OSF/1) v 3.2 */ 
#define  FBS22 0  /* FreeBSD 2.2, also MacOS X 1.3 */
#define  HPXA9 0  /* HP-UX A.9...*/ 
#define  IRX53 0  /* Silicon Graphics IRIX 5.3 */ 
#define  SUN41 0   
#define  SOL24 0   
#define  SOL25 0 
#define  LNX12 1 
#define  SYSMD5 1 
#define  SYSV   AIX41 | DEC32 | IRX53 | SOL25
#define  UNIX   SYSV  | BSD21 | HPXA9 | LNX12 | FBS22
