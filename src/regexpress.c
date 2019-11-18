#include <stdio.h>
#include <sys/types.h>
#include <regex.h>
#include "logprocess.h"
#include "regexpress.h"

int cmp_reg_exp( const char* __pattern, const char* __str )
{
	int _z;
	regex_t _reg;
 	regmatch_t _pm[1];
	int _ret;
	char _ebuf[128];
	
	/*regcomp*/
	if ( regcomp(&_reg, __pattern, REG_EXTENDED)!=0 )
	{
		regerror(_z, &_reg, _ebuf, sizeof(_ebuf));
		log_msg(g_log_file, LOG_BY_MONTH, "Pattern %s , Error %s.", __pattern, _ebuf);
		_ret=0;
	}
	/*regexec*/
	_z=regexec(&_reg, __str, 1, _pm, 0);
	if ( _z==REG_NOMATCH )
  		_ret=0;
	else 
	if ( _z!=0 ) 
	{
 		regerror(_z, &_reg, _ebuf, sizeof(_ebuf));
    	log_msg(g_log_file, LOG_BY_MONTH, "%s: regcom('%s')\n", _ebuf, __str );
    	_ret=0;
  	}
  	else
  		_ret=1;
	/*regfree*/
	regfree(&_reg);
	return _ret;
}
