#include <sys/param.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/types.h>
#include <ctype.h>
#include <netinet/in.h>
//#include <netinet/ip.h>
#include <arpa/ftp.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <pwd.h>
#include <stdarg.h>
#include "g_string.h"
#include "ftpprocess.h"
#include "logprocess.h"

/*
UPDATE-LOG：
	2005-03-28:规范ftp_系列函数的返回值.

*/


/*Struct Define*/
#define FTP_BUF_SIZE 2048

/*超时处理,缺省的超时时间是20秒*/
#define FTP_TIME_OUT 30
struct RECV_BUF{
int	len;
char*	pend;
char	pbuf[FTP_BUF_SIZE];
};
/*Static Var*/
static struct sockaddr_in _ftp_origin_ctl_addr;/*origin ctl connection address*/
static struct sockaddr_in _ftp_data_addr;/*data connect address*/
static struct sockaddr_in _ftp_conn_ctl_addr;/*active ctl connect address*/
static int		_ftp_if_conn=0;/*ftp if connected*/
static FILE 	*_ftp_cin, *_ftp_cout;/*ftp socket input/output*/
static int		_ftp_data=-1;/*ftp data connection fd*/
static int		_ftp_cp_end=0;/* no pending replies */
static int		_ftp_code;/*last reply code*/
static char 	_ftp_reply_string[1024];		/* last line of previous reply */
static struct RECV_BUF _ftp_buf;/*sock recv buffer*/
static int		_ftp_cur_type=TYPE_I;/*ftp type*/
static char 	_ftp_pasv[64];		 /* passive port for proxy data connection */
static int		_ftp_passive_mode;	  /* passive mode enabled */
static int		_ftp_sendport=-1;	/* use PORT cmd for each data connection */
static int		_ftp_crflag;/* if 1, strip car. rets. on ascii gets */
/*Global Debuf Flag*/
extern int debug;
/*Static Function*/
static int command(const char *__fmt, ...);
static int getreply(int __expecteof);
static void do_settype(const char *__type );
static void lostpeer(int __ignore);
static void recvrequest(const char *cmd, 
		const char *local, const char *remote, 
		const char *lmode, int printnames);
static int empty(fd_set *mask, int hifd, int sec);	
static void abort_remote(FILE *__din);	  
static void ptransfer(const char *direction, long bytes, 
	  const struct timeval *t0, 
	  const struct timeval *t1);
static int initconn(void);
static FILE *dataconn(const char *__lmode);	  

static int safe_read( int __fd, char *__buf, unsigned __bufsize, unsigned __min);
static int safe_write( int __fd, char *__data, unsigned __data_size, unsigned __min);



struct	types {
	const char *t_name;
	const char *t_mode;
	int t_type;
	const char *t_arg;
} types[] = {
	{ "ascii",	"A",	TYPE_A,	NULL },
	{ "binary",	"I",	TYPE_I,	NULL },
	{ "image",	"I",	TYPE_I,	NULL },
	{ "ebcdic",	"E",	TYPE_E,	NULL },
	{ NULL, NULL, 0, NULL }
};

/*
 * Set transfer type.
 */
static 
void 
do_settype(const char *__type ) 
{
	struct types *p;
	int comret;

	for (p = types; p->t_name; p++)
		if (strcmp(__type, p->t_name) == 0)
			break;
	if (p->t_name == 0) {
		LOG_MSG("ftp error: %s unknown mode",__type);
		_ftp_code = -1;
		return;
	}
	if ((p->t_arg != NULL) && (*(p->t_arg) != '\0'))
		comret = command("TYPE %s %s", p->t_mode, p->t_arg);
	else
		comret = command("TYPE %s", p->t_mode);
	if (comret == COMPLETE) {
		_ftp_cur_type = p->t_type;
	}
}

/*
 * Start up passive mode interaction
 */
void
setpassive(void)
{
		_ftp_passive_mode=(_ftp_passive_mode) ? 0:1;
		LOG_MSG("Passive mode %s.\n", ( _ftp_passive_mode ? "on":"off" ) ) ;
}

static int command(const char *__fmt, ...)
{
	va_list _ap;

	va_start(_ap, __fmt);
	if (strncmp("PASS ", __fmt, 5) == 0)
		LOG_MSG("FTP INFO:---> PASS XXXX");
	else 
	{
		char _buf[256];
		memset(_buf, 0, sizeof(_buf));
		vsprintf(_buf, __fmt, _ap);
		LOG_MSG("FTP INFO:---> %s", _buf);
	}
	va_end(_ap);
	if (_ftp_cout==NULL) 
	{
		LOG_MSG("ftp error: no control connection for command");
		return -1;
	}
	/*send msg to srv*/
	_ftp_cp_end = 1;
	va_start(_ap, __fmt);
	vfprintf(_ftp_cout, __fmt, _ap);
	va_end(_ap);
	fprintf(_ftp_cout, "\r\n");
	(void) fflush(_ftp_cout);
	
	return getreply(!strcmp(__fmt, "QUIT"));
}

static void lostpeer(int __ignore)
{
	(void)__ignore;

	if (_ftp_if_conn) {
		if (_ftp_cout!=NULL) {
			shutdown(fileno(_ftp_cout), 1+1);
			fclose(_ftp_cout);
			_ftp_cout=NULL;
		}
		if ( _ftp_data>=0 ) {
			shutdown(_ftp_data, 1+1);
			close(_ftp_data);
			_ftp_data= -1;
		}
		_ftp_if_conn= 0;
	}
}

static int getreply(int __expecteof)
{
	register int c, n;
	register int dig;
	register char *cp;
	int originalcode = 0, continuation = 0;
	char* _p_buf=_ftp_buf.pbuf;
	int pflag = 0;
	size_t px = 0;
	size_t psize = sizeof(_ftp_pasv);

	for (;;) {
		dig = n = _ftp_code = 0;
		cp = _ftp_reply_string;
		/*grantguo insert for time out support*/
		while ( strchr( _ftp_buf.pbuf, '\n' )==NULL )
		{	/*set time out value*/
			int _len;
			struct timeval _t;
			fd_set _mask;
			FD_ZERO(&_mask);
			FD_SET(fileno(_ftp_cin), &_mask);
			/*time out*/
			_t.tv_sec = FTP_TIME_OUT;
			_t.tv_usec = 0;
			if((_len= select( fileno(_ftp_cin)+1, &_mask, (fd_set*)0, (fd_set*)0, &_t ))==-1 )
            {
                n='4';
                _ftp_code=421;
                LOG_MSG( "ftp error: select error:%s!",strerror(errno) );
                _ftp_buf.len = 0;
                _ftp_buf.pend = _ftp_buf.pbuf;
                memset( _ftp_buf.pbuf, 0, sizeof(_ftp_buf.pbuf) );
                lostpeer(0);
                _ftp_cp_end=0;
                return 4;
            }
            if(_len==0)
            {
            	_ftp_cp_end=0;
				lostpeer(0);
                LOG_MSG( "ftp error: select TimeOut:%d s!",FTP_TIME_OUT );
                return 0;
            }
			if ( !FD_ISSET(fileno(_ftp_cin), &_mask) )
			{
				n='4';
				_ftp_code=421;
				LOG_MSG( "ftp error: Command TimeOut!" );
				_ftp_buf.len = 0;
				_ftp_buf.pend = _ftp_buf.pbuf;
				memset( _ftp_buf.pbuf, 0, sizeof(_ftp_buf.pbuf) );
				lostpeer(0);
				_ftp_cp_end=0;
				return 4;
			}
			_len = read( fileno(_ftp_cin), _ftp_buf.pend, FTP_BUF_SIZE );
			if ( _len > 0 )
			{
				_ftp_buf.len += _len;
				_ftp_buf.pend += _len;
				memset( _ftp_buf.pend, 0, FTP_BUF_SIZE - _ftp_buf.len );
				LOG_MSG("RECEV MSG: %s",_p_buf);
			}
			if ( _len < 0 )
			{
				LOG_MSG( "ftp error: Read data from server fail, %s", strerror(errno));
				n='4';
				_ftp_code=421;
				_ftp_buf.len = 0;
				_ftp_buf.pend = _ftp_buf.pbuf;
				memset( _ftp_buf.pbuf, 0, sizeof(_ftp_buf.pbuf) );
				lostpeer(0);
				_ftp_cp_end=0;
				return 4;
			}
			if ( _len ==0 )
			{
				n='4';
				_ftp_code=421;
				LOG_MSG( "ftp error: server have close this connection!" );
				_ftp_buf.len = 0;
				_ftp_buf.pend = _ftp_buf.pbuf;
				memset( _ftp_buf.pbuf, 0, sizeof(_ftp_buf.pbuf) );
				lostpeer(0);
				_ftp_cp_end=0;
				return 4;
			 }
		}
		
		/*read cin*/
continueread:
		dig=0;
		n=0;
		pflag=0;
		continuation = 0;
		cp=_ftp_reply_string;
		while( (c=*_p_buf++) != '\n' ) {
			dig++;
			/*time out over*/
			if ( c==EOF ) 
			{
				if (__expecteof) {
					_ftp_code = 221;
					return (0);
				}
				lostpeer(0);
				LOG_MSG("ftp error: 421 Service not available, remote server has closed connection");
				_ftp_code = 421;
				return(4);
			}
			if (dig < 4 && isdigit(c))
				_ftp_code = _ftp_code * 10 + (c - '0');
			if (!pflag && _ftp_code == 227)
				pflag = 1;
			if (dig > 4 && pflag == 1 && isdigit(c))
				pflag = 2;
			if (pflag == 2) {
				if (c != '\r' && c != ')') {
					if (px < psize-1) _ftp_pasv[px++] = c;
				}
				else {
					_ftp_pasv[px] = '\0';
					pflag = 3;
				}
			}
			if (dig == 4 && (c == '-' || get_string_letter_count(_p_buf,'\n') >= 2)) {
				continuation++;
			}
			if (n == 0)
				n = c;
			if (c !='\r' && ( cp < &_ftp_reply_string[sizeof(_ftp_reply_string) - 1] ) )
				*cp++ = c;
		}
		*cp='\0';
		LOG_MSG("PARSE MSG [%s] OK.", _ftp_reply_string);
		
		if ((strchr(_p_buf, '\n')!=NULL) && continuation && _ftp_code != originalcode) {
			if (originalcode == 0)
			{
				originalcode = _ftp_code;
			}
			_ftp_code = 0;
			goto continueread;
		}		

		/*mov buffer*/
		memset( _ftp_buf.pbuf, 0, _p_buf - _ftp_buf.pbuf );
		_ftp_buf.len = _ftp_buf.pend - _p_buf;
		memmove( _ftp_buf.pbuf, _p_buf, _ftp_buf.len );
		_ftp_buf.pend = _ftp_buf.pbuf + _ftp_buf.len;
		memset( _ftp_buf.pend, 0, FTP_BUF_SIZE - _ftp_buf.len );
		_p_buf=_ftp_buf.pbuf;

		if(continuation && originalcode/10 != 2 && _ftp_code != 0) {
			if (originalcode == 0)
				originalcode = _ftp_code;
			_ftp_code = 0;
			continue;
		}	 
		LOG_MSG("DBG:continuation:%d, originalcode:%d,_ftp_code:%d.\n",continuation,originalcode,_ftp_code);

		if (n != '1')
			_ftp_cp_end = 0;
		if (_ftp_code == 421 || originalcode == 421 )
			lostpeer(0);
			
		return (n - '0');
	}
	
}

int ftp_conn( struct FTP_SERVER_INFO* __srv )
{
	int _sock;
	int _len;

	/*init*/
	_ftp_buf.len = 0;
	_ftp_buf.pend=_ftp_buf.pbuf;
	memset( _ftp_buf.pbuf, 0, FTP_BUF_SIZE );
	memset( _ftp_reply_string, 0, sizeof(_ftp_reply_string));
	setpassive();
	memset( &_ftp_origin_ctl_addr, 0, sizeof(_ftp_origin_ctl_addr));
	_ftp_origin_ctl_addr.sin_family=AF_INET;
	_ftp_origin_ctl_addr.sin_port=htons(__srv->port);
	
	if ( inet_pton(AF_INET, __srv->ip, &_ftp_origin_ctl_addr.sin_addr)<=0 )
	{
			LOG_MSG( "ftp error: inet_pton(%s) error %s .", __srv->ip, strerror(errno));
			goto bad;
	}
	
	_sock = socket(AF_INET, SOCK_STREAM, 0);
	if ( _sock< 0) {
		LOG_MSG( "ftp error: socket() %s.",strerror(errno) );
		goto bad;
	}
	
	if ( connect(_sock, (struct sockaddr*)&_ftp_origin_ctl_addr, sizeof(_ftp_origin_ctl_addr))<0 )
	{
		LOG_MSG( "ftp error: connnect() %s.", strerror(errno) );
		goto bad;
	}

	_len = sizeof (_ftp_conn_ctl_addr);
	if (getsockname(_sock, (struct sockaddr *)&_ftp_conn_ctl_addr, (socklen_t*)&_len) < 0) 
	{
		LOG_MSG("ftp error: getsockname %s", strerror(errno));
		goto bad;
	}
	
	_ftp_cin = fdopen(_sock, "r");
	_ftp_cout = fdopen(_sock, "w");
	
	if ( _ftp_cin==NULL || _ftp_cout==NULL ) 
	{
		LOG_MSG("ftp: fdopen failed." );
		if (_ftp_cin)
			(void) fclose(_ftp_cin);
		if (_ftp_cout)
			(void) fclose(_ftp_cout);
		goto bad;
	}
	LOG_MSG( "Connected to %s.", __srv->ip );
	if ( getreply(0)>2 ) {	/* read startup message from server */
		if (_ftp_cin)
			(void) fclose(_ftp_cin);
		if (_ftp_cin)
			(void) fclose(_ftp_cout);
		goto bad;
	}
	_ftp_if_conn=1;
	return 1;
	
bad:
	_ftp_code=-1;	
	(void) close(_sock);
	return _ftp_code;
}

int ftp_login( struct FTP_SERVER_INFO* __srv )
{
	int n;
	if (_ftp_if_conn<1)
		return 0;
	/*USER cmd*/
	n=command("USER %s", __srv->user);
	/*PASS cmd*/
	if (n == CONTINUE) {
		n = command("PASS %s", __srv->pass);
	}
	/*if fail*/
	if (n != COMPLETE) {
		LOG_MSG( "ftp error: %s,%s,%s,login failed.", __srv->ip, __srv->user, __srv->pass);
		return 0;
	}
	LOG_MSG( "ftp %s,%s,%s,login success.", __srv->ip, __srv->user, __srv->pass);
	return 1;
}

static void recvrequest(const char *cmd, 
		const char *local, const char *remote, 
		const char *lmode, int printnames)
{
	FILE *volatile fout, *volatile din = 0;
	int (*volatile closefunc)(FILE *);
	volatile int tcrflag, bare_lfs = 0;
	static unsigned bufsize;
	static char *buf;
	volatile long bytes = 0;
	register int c, d,ret=0;
	struct timeval start, stop;
	struct stat64 st;

	tcrflag = !_ftp_crflag;
	closefunc = NULL;

	do_settype("binary");

	if (initconn()) {
		while (_ftp_cp_end) {
			(void) getreply(0);
		}
		if (_ftp_data >= 0) {
			(void) close(_ftp_data);
			_ftp_data = -1;
		}
		_ftp_code = -1;
		return;
	}
	if (remote) {
		if ((ret=command("%s %s", cmd, remote)) != PRELIM) {
			LOG_MSG("ftp error: cmd: %s remote: %s ret:%d, PRELIM=%d\n",cmd, remote,ret,PRELIM);
			_ftp_code = -1;
			return;
		}
	} 
	else {
		if ((ret=command("%s", cmd)) != PRELIM) {
			LOG_MSG("ftp error: cmd: %s ret:%d, PRELIM=%d\n",cmd,ret, PRELIM);
			_ftp_code = -1;
			return;
		}
	}
	din = dataconn("r");
	if (din == NULL)
	{
		LOG_MSG("ftp error: dataconn err:%d\n",strerror(errno));
		goto abort;
	}
	if (strcmp(local, "-") == 0)
		fout = stdout;
	else {
		fout = fopen64(local, lmode);
		if (fout == NULL) {
			LOG_MSG("ftp error: local: %s: %s\n", local, strerror(errno) );
			goto abort;
		}
		closefunc = fclose;
	}
	if (fstat64(fileno(fout), &st) < 0 || st.st_blksize == 0)
		st.st_blksize = BUFSIZ;
	if (st.st_blksize > (int)bufsize) {
		if (buf)
			(void) free(buf);
		buf = (char*)malloc((unsigned)st.st_blksize);
		if (buf == NULL) {
			LOG_MSG("ftp error:malloc %s", strerror(errno));
			bufsize = 0;
			goto abort;
		}
		bufsize = st.st_blksize;
	}
	(void) gettimeofday(&start, (struct timezone *)0);
	switch (_ftp_cur_type) {
	case TYPE_I:
	case TYPE_L:
		errno = d = 0;
		while ((c = safe_read(fileno(din), buf, bufsize,FTP_TIME_OUT)) > 0) {
			if ((d = write(fileno(fout), buf, c)) != c)
				break;
			bytes += c;
		}
		if (c < 0) {
			if (errno != EPIPE)
				LOG_MSG("ftp error: netin %s", strerror(errno));
			bytes = -1;
		}
		if (d < c) {
			if (d < 0)
				LOG_MSG("ftp error: local: %s: %s", local,
					strerror(errno));
			else
				LOG_MSG("ftp error: %s: short write", local);
		}
		break;

	case TYPE_A:
		while ((c = getc(din)) != EOF) {
			if (c == '\n')
				bare_lfs++;
			while (c == '\r') {
				bytes++;
				if ((c = getc(din)) != '\n' || tcrflag) {
					if (ferror(fout))
						goto break2;
					(void) putc('\r', fout);
					if (c == '\0') {
						bytes++;
						goto contin2;
					}
					if (c == EOF)
						goto contin2;
				}
			}
			(void) putc(c, fout);
			bytes++;
	contin2:	;
		}
break2:

		if (bare_lfs) {
			LOG_MSG("WARNING! %d bare linefeeds received in ASCII mode", bare_lfs);
			LOG_MSG("File may not have transferred correctly.");
		}
		if (ferror(din)) {
			if (errno != EPIPE)
				LOG_MSG("ftp error:netin %s", strerror(errno));
			bytes = -1;
		}
		if (ferror(fout))
			LOG_MSG("ftp error: local: %s: %s\n", local,
				strerror(errno));
		break;
	}
	if (closefunc != NULL)
		(*closefunc)(fout);
	(void) gettimeofday(&stop, (struct timezone *)0);
	(void) fclose(din);
	/* closes data as well, so discard it */
	_ftp_data = -1;
	(void) getreply(0);
	if (bytes > 0 )
		ptransfer("received", bytes, &start, &stop);
	else
	{
		LOG_MSG("ftp: get remote file:%s size is %d",remote, bytes);
		//_ftp_code = -1;
	}
	return;
abort:

/* abort using RFC959 recommended IP,SYNC sequence	*/

	(void) gettimeofday(&stop, (struct timezone *)0);
	if (!_ftp_cp_end) {
		_ftp_code = -1;
		return;
	}
	abort_remote(din);
	_ftp_code = -1;
	if (closefunc != NULL && fout != NULL)
		(*closefunc)(fout);
	if (din) {
		(void) fclose(din);
	}
	if (_ftp_data >= 0) {
		/* if it just got closed with din, again won't hurt */
		(void) close(_ftp_data);
		_ftp_data = -1;
	}
	if (bytes > 0)
		ptransfer("received", bytes, &start, &stop);
}

static int
empty(fd_set *mask, int hifd, int sec)
{
	struct timeval t;

	t.tv_sec = (long) sec;
	t.tv_usec = 0;
	return(select(hifd+1, mask, (fd_set *) 0, (fd_set *) 0, &t));
}


static void
abort_remote(FILE *__din)
{
	char buf[BUFSIZ];
	int nfnd, hifd;
	fd_set mask;

	/*
	 * send IAC in urgent mode instead of DM because 4.3BSD places oob mark
	 * after urgent byte rather than before as is protocol now
	 */
	snprintf(buf, sizeof(buf), "%c%c%c", IAC, IP, IAC);
	if (send(fileno(_ftp_cout), buf, 3, MSG_OOB) != 3)
		LOG_MSG("ftp error: abort %s", strerror(errno));
	fprintf(_ftp_cout,"%cABOR\r\n", DM);
	(void) fflush(_ftp_cout);
	FD_ZERO(&mask);
	FD_SET(fileno(_ftp_cin), &mask);
	hifd = fileno(_ftp_cin);
	if (__din) { 
		FD_SET(fileno(__din), &mask);
		if (hifd < fileno(__din)) hifd = fileno(__din);
	}
	if ((nfnd = empty(&mask, hifd, FTP_TIME_OUT)) <= 0) {
		if (nfnd < 0) {
			LOG_MSG("abort %s", strerror(errno));
		}
		lostpeer(0);
	}
	if (__din && FD_ISSET(fileno(__din), &mask)) {
		while (read(fileno(__din), buf, BUFSIZ) > 0)
			/* LOOP */;
	}
	if (getreply(0) == ERROR && _ftp_code == 552) {
		/* 552 needed for nic style abort */
		(void) getreply(0);
	}
	(void) getreply(0);
}


static void
tvsub(struct timeval *tdiff, 
	  const struct timeval *t1, 
	  const struct timeval *t0)
{

	tdiff->tv_sec = t1->tv_sec - t0->tv_sec;
	tdiff->tv_usec = t1->tv_usec - t0->tv_usec;
	if (tdiff->tv_usec < 0)
		tdiff->tv_sec--, tdiff->tv_usec += 1000000;
}

static void
ptransfer(const char *direction, long bytes, 
	  const struct timeval *t0, 
	  const struct timeval *t1)
{
	struct timeval td;
	float s, bs;

	if (1) {
		tvsub(&td, t1, t0);
		s = td.tv_sec + (td.tv_usec / 1000000.);
#define	nz(x)	((x) == 0 ? 1 : (x))
		bs = bytes / nz(s);
		LOG_MSG("%ld bytes %s in %.3g secs (%.2g Kbytes/sec)\n",
			bytes, direction, s, bs / 1024.0);
	}
}

/*
 * Need to start a listen on the data channel before we send the command,
 * otherwise the server's connect may fail.
 */
static int
initconn(void)
{
	register char *p, *a;
	int result, tmpno = 0;
	socklen_t _len;
	int on = 1;
	u_long a1,a2,a3,a4,p1,p2;

	if (_ftp_passive_mode) {
		_ftp_data = socket(AF_INET, SOCK_STREAM, 0);
		if (_ftp_data< 0) {
			LOG_MSG("ftp error: socket %s", strerror(errno));
			return 1;
		}
		
		if (command("PASV") != COMPLETE) {
			LOG_MSG("ftp error: Passive mode refused.");
			return(1);
		}

		/*
		 * What we've got at this point is a string of comma separated
		 * one-byte unsigned integer values, separated by commas.
		 * The first four are the an IP address. The fifth is the MSB
		 * of the port number, the sixth is the LSB. From that we'll
		 * prepare a sockaddr_in.
		 */

		if (sscanf(_ftp_pasv,"%ld,%ld,%ld,%ld,%ld,%ld",
			   &a1,&a2,&a3,&a4,&p1,&p2)
			!= 6) 
		{
			LOG_MSG("Passive mode address [%s] scan failure. Shouldn't happen!\n",_ftp_pasv);
			return(1);
		}

		_ftp_data_addr.sin_family = AF_INET;
		_ftp_data_addr.sin_addr.s_addr = htonl((a1 << 24) | (a2 << 16) |
						  (a3 << 8) | a4);
		_ftp_data_addr.sin_port = htons((p1 << 8) | p2);

		if (connect(_ftp_data, (struct sockaddr *) &_ftp_data_addr,
			sizeof(_ftp_data_addr))<0) 
		{
			LOG_MSG("ftp error: data link connect %s",strerror(errno));
			return(1);
		}
		return(0);
	}
noport:
	_ftp_data_addr = _ftp_conn_ctl_addr;
	if (_ftp_sendport)
		_ftp_data_addr.sin_port = 0;	/* let system pick one */ 
	if ( _ftp_data != -1)
		(void) close(_ftp_data);
	_ftp_data = socket(AF_INET, SOCK_STREAM, 0);
	if (_ftp_data < 0) {
		LOG_MSG("ftp error: socket %s", strerror(errno));
		if (tmpno)
			_ftp_sendport = 1;
		return (1);
	}
	if (!_ftp_sendport)
		if (setsockopt(_ftp_data, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof (on)) < 0) {
			LOG_MSG("ftp error: setsockopt (reuse address) %s", strerror(errno));
			goto bad;
		}
	if (bind(_ftp_data, (struct sockaddr *)&_ftp_data_addr, sizeof (_ftp_data_addr)) < 0) {
		LOG_MSG("ftp error: bind %s", strerror(errno) );
		goto bad;
	}

	_len = sizeof (_ftp_data_addr);
	if (getsockname(_ftp_data, (struct sockaddr *)&_ftp_data_addr, &_len) < 0) {
		LOG_MSG("ftp error: getsockname %s", strerror(errno));
		goto bad;
	}
	if (listen(_ftp_data, 1) < 0)
		LOG_MSG("ftp error: listen %s", strerror(errno) );
	if (_ftp_sendport) {
		a = (char *)&_ftp_data_addr.sin_addr;
		p = (char *)&_ftp_data_addr.sin_port;
#define	UC(b)	(((int)b)&0xff)
		result =
			command("PORT %d,%d,%d,%d,%d,%d",
			  UC(a[0]), UC(a[1]), UC(a[2]), UC(a[3]),
			  UC(p[0]), UC(p[1]));
		if (result == ERROR && _ftp_sendport == -1) {
			_ftp_sendport = 0;
			tmpno = 1;
			goto noport;
		}
		return (result != COMPLETE);
	}
	if (tmpno)
		_ftp_sendport = 1;
	return (0);
bad:
	(void) close(_ftp_data), _ftp_data = -1;
	if (tmpno)
		_ftp_sendport = 1;
	return (1);
}

static FILE *
dataconn(const char *__lmode)
{
	struct sockaddr_in from;
	int s;
	socklen_t fromlen = sizeof(from);

	if ( _ftp_passive_mode)
		return (fdopen(_ftp_data, __lmode));

	s = accept(_ftp_data, (struct sockaddr *) &from, &fromlen);
	if (s < 0) {
		LOG_MSG("ftp error: accept %s", strerror(errno));
		(void) close(_ftp_data), _ftp_data = -1;
		return (NULL);
	}
	(void) close(_ftp_data);
	_ftp_data = s;
	return (fdopen(_ftp_data, __lmode));
}

static
void
sendrequest(const char *cmd, const char *local, const char *remote, int printnames)
{
	struct stat64 st;
	struct timeval start, stop;
	int (*volatile closefunc)(FILE *);
	register int c, d;
	FILE *volatile fin, *volatile dout = 0;
	volatile long bytes = 0;
	char buf[BUFSIZ], *bufp;
	const char *volatile lmode;

	closefunc = NULL;
	lmode = "w";
	
	fin = fopen64(local, "r");
	if (fin == NULL) 
	{
		LOG_MSG( "ftp local: %s: %s\n", local,
			strerror(errno));
			_ftp_code = -1;
			return;
	}
	closefunc = fclose;
	if (fstat64(fileno(fin), &st) < 0 ||
			(st.st_mode&S_IFMT) != S_IFREG) 
	{
		LOG_MSG("%s: not a plain file.\n", local);
		fclose(fin);
		_ftp_code = -1;
		return;
	}
	if (initconn()) 
	{
		_ftp_code = -1;
		if (closefunc != NULL)
			(*closefunc)(fin);
		return;
	}
	if (strcmp(cmd, "STOR") == 0 || strcmp(cmd, "APPE") == 0)
	{
		lmode = "r+w";
	}
	if (remote) {
		if (command("%s %s", cmd, remote) != PRELIM) {
				(*closefunc)(fin);
			return;
		}
	} 
	else
	if (command("%s", cmd) != PRELIM) {
			if (closefunc != NULL)
				(*closefunc)(fin);
			return;
		}
	dout = dataconn(lmode);
	if (dout == NULL)
		goto abort;
	(void) gettimeofday(&start, (struct timezone *)0);
	switch (_ftp_cur_type) {
	case TYPE_I:
	case TYPE_L:
		errno = d = 0;
		while ((c = read(fileno(fin), buf, sizeof (buf))) > 0) 
		{
			bytes += c;
			for (bufp = buf; c > 0; c -= d, bufp += d)
				if ((d = safe_write(fileno(dout), bufp, c, FTP_TIME_OUT)) <= 0)
					break;
		}
		if (bytes )
				LOG_MSG("\rBytes transferred: %ld", bytes);
		if (c < 0)
			LOG_MSG("local: %s: %s", local, strerror(errno));
		if (d < 0) {
			if (errno != EPIPE) 
				LOG_MSG("netout %s", strerror(errno));
			bytes = -1;
		}
		break;
	case TYPE_A:
		while ((c = getc(fin)) != EOF)
		{
			if (c == '\n') {
				if (ferror(dout))
					break;
				(void) putc('\r', dout);
				bytes++;
			}
			(void) putc(c, dout);
			bytes++;							
		}
		if (bytes)
			LOG_MSG("Bytes transferred: %ld", bytes);
		if (ferror(fin))
			LOG_MSG("local: %s: %s", local, strerror(errno));
		if (ferror(dout)) {
			if (errno != EPIPE)
				LOG_MSG("netout %s", strerror(errno));
			bytes = -1;
		}
		break;
	}
	(void) gettimeofday(&stop, (struct timezone *)0);
	if (closefunc != NULL)
		(*closefunc)(fin);
	(void) fclose(dout);
	/* closes data as well, so discard it */
	_ftp_data = -1;
	(void) getreply(0);
	if (bytes > 0)
		ptransfer("sent", bytes, &start, &stop);
	return;
abort:
	(void) gettimeofday(&stop, (struct timezone *)0);
	if (!_ftp_cp_end) {
		_ftp_code = -1;
		return;
	}
	if (dout) {
		(void) fclose(dout);
	}
	if (_ftp_data >= 0) {
		/* if it just got closed with dout, again won't hurt */
		(void) close(_ftp_data);
		_ftp_data = -1;
	}
	(void) getreply(0);
	_ftp_code = -1;
	if (closefunc != NULL && fin != NULL)
		(*closefunc)(fin);
	if (bytes > 0)
		ptransfer("sent", bytes, &start, &stop);
}

int ftp_list( const char*  __path,	const char* __local )
/*
	0失败，
	1成功
*/
{
	if ( !_ftp_if_conn )
		return 0;
	recvrequest( "LIST", __local, __path, "w", 0);
	
	if ( (_ftp_code/100)==2 )
		return 1;
		
	return 0;
}
int ftp_lcd( const char* __path )
/*
	0失败，
	1成功
*/
{
	char buf[256];

	if (chdir(__path) < 0) {
		LOG_MSG("ftp local: %s: %s\n", __path, strerror(errno));
		_ftp_code = -1;
		return 0;
	}
	if (!getcwd(buf, sizeof(buf))) {
		if (errno==ERANGE) strcpy(buf, "<too long>");
			else strcpy(buf, "???");
	}

	LOG_MSG("Local directory now %s\n", buf);
	_ftp_code = 0;
	return 1;
}
int ftp_cd( const char* __path )
/*
	0失败，
	1成功
*/
{
	if ( !_ftp_if_conn )
		return 0;
	command("CWD %s", __path);
	if ( (_ftp_code/100)==2 )
		return 1;
	return 0;
}


int ftp_mkdir(char* __path)
{
   char* token = NULL;
   int len = 0;
   token = strtok( __path, "/" );
   char path[4096]={0};

   while( token != NULL )
   {
	  sprintf(path+len,"%s%s","/",token);
	  len += strlen(token)+1;
      if ( !_ftp_if_conn )
		return 0;
      LOG_MSG("FTP DIR %s\n", path);
      command("CWD %s", path);
      if ( (_ftp_code/100)==2 )
      {
	  	  token = strtok( NULL,"/" );
		  continue;
      }
	  else
	  {
	      command("XMKD %s", path);
		  if((_ftp_code/100)==2 )
		  {
		    token = strtok( NULL,"/" );
		    continue;
		  }
		  else return 0;
	  }
   }
   
   if( (_ftp_code/100)==2 ) return 1;
   return 0;
}

int ftp_get( const char* __local, const char *__remote)
/*
	0失败，
	1成功
*/
{
	if ( !_ftp_if_conn )
		return 0;
	recvrequest( "RETR", __local, __remote, "w", 0);	
	if ( (_ftp_code/100)==2 )
		return 1;
	return 0;

}
int ftp_put( const char* __local, const char *__remote)
/*
	0失败，
	1成功
*/
{
	if ( !_ftp_if_conn )
		return 0;
	sendrequest( "STOR", __local, __remote, 0 );
	if ( (_ftp_code/100)==2 )
		return 1;	
	return 0;
}
int ftp_del( const char* __remote)
/*
	0失败，
	1成功
*/
{
	if ( !_ftp_if_conn )
		return 0;
		command("DELE %s", __remote);
	if ( (_ftp_code/100)==2 )
			return 1;
	return 0;
}
int ftp_quit()
/*
	0失败，
	1成功
*/
{
	if ( !_ftp_if_conn )
		return 0;
	command("QUIT");
	return 1;
}
/*
 *	* Rename a remote file.
 *	 */
int
ftp_rename(const char *__src, const char *__dest)
/*
	0失败，
	1成功
*/
{
	if (command("RNFR %s", __src) == CONTINUE)
					(void) command("RNTO %s", __dest);
	if ( (_ftp_code/100)==2 )
		return 1;
	return 0;
}


int safe_read( int __fd, char *__buf, unsigned __bufsize, unsigned __second)
/*
用SELECT加强READ，能够超时返回,
返回-1表示超时或者错误。
*/
{

	int 	_ret;
	fd_set	_fds;
	struct timeval	_timeout;

	_timeout.tv_sec	= __second;
	_timeout.tv_usec=0;

	FD_ZERO(&_fds);
	FD_SET((unsigned int)__fd, &_fds);

	switch( select(__fd+1, &_fds, NULL, NULL, &_timeout) )
	{
		case -1:
				_ret=-1;
				break;
		case 0:
				_ret=0;
				break;
		default:
				if ( FD_ISSET(__fd, &_fds) )
				{
						_ret = read(__fd, __buf, __bufsize);
						break;
				}
				_ret=0;
	}
	
	return _ret;	
}


int safe_write( int __fd, char *__data, unsigned __data_size, unsigned __second)
/*
用SELECT加强write，能够超时返回,
返回-1表示超时或者错误。
*/
{

	int 	_ret;
	fd_set	_fds;
	struct timeval	_timeout;

	_timeout.tv_sec	= __second;
	_timeout.tv_usec= 0;

	FD_ZERO(&_fds);
	FD_SET((unsigned int)__fd, &_fds);

	switch( select(__fd+1, NULL, &_fds,  NULL, &_timeout) )
	{
		case -1:
				_ret=-1;
				break;
		case 0:
				_ret=0;
				break;
		default:
				if ( FD_ISSET(__fd, &_fds) )
				{
						_ret = write(__fd, __data, __data_size);
						break;
				}
				_ret=0;
	}
	
	return _ret;	
}
