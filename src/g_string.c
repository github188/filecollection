#include "g_string.h"

#define IN_WARD  1 /*在单词内*/
#define OUT_WARD 2 /*在单词外*/

 int get_string_wards_count( char* __str )
/*
功能:得到字符串中的单词个数
输入参数:__str,以NULL结尾的c string
返回参数:单词个数*/
{
	int _state, _num;
	char *_tmp, _ch;
	
	_num=0;
	_state=OUT_WARD;
	_tmp=__str;
	
	while( (_ch=*_tmp)!=0 )
	{
		if ( _ch==' ' || _ch=='\n' || _ch=='\t' )
			_state=OUT_WARD;
		else
		if ( _state==OUT_WARD )
		{
			_state=IN_WARD;
			_num++;
		}
		_tmp++;
	}
    return _num;
}


 int get_string_wards_by_pos( char* __str, int __pos, char* __buf, int __buf_size )
/*
功能:得到字符串中某个位置的单词
输入参数:	__str 字符串
			__pos 单词的位置,已1开始计数
			__buf 存放单词的缓冲区
			__buf_size 缓冲区大小
返回值: 返回单词的长度,如果每找到则返回0*/
{
	int _state, _num, _cp_size;
	char *_tmp, _ch;
	
	_num=0,_cp_size=0;
	_state=OUT_WARD;
	_tmp=__str;
	
    memset( __buf, 0, __buf_size );
	while( (_ch=*_tmp)!=0 )
	{
		if ( _ch==' ' || _ch=='\n' || _ch=='\t' )
			_state=OUT_WARD;
		else
		if ( _state==OUT_WARD )
		{
			_state=IN_WARD;
			_num++;
			if ( _num==__pos )
			{
				while ( (_ch=*_tmp)!=0 )
				{
					if ( _ch==' ' || _ch=='\n' || _ch=='\t' )
						return _cp_size;
					if ( _cp_size>=__buf_size )
						return __buf_size;
					__buf[_cp_size++]=_ch;
					_tmp++;		
				}
			}
		}
		_tmp++;
	}
	return 0;
}

int string_replace( char* __str, char* __find, char* __replace, char* __buf, int __size )
/*
功能:在一个字符串__str中用__replace代替__find,并填充到__buf中
输入参数:	__str源子符串
			__find在源字符串中降要被代替的旧字符串
			__replace在源字符串中将要被代替的新字符串
			__buf输出缓冲区
			__size缓冲区大小
返回值:		新字符串长度.
警告:如果__str很大,性能将有所影响.
*/
{
	char *_cur,*_prev;
	char *_son_cur;
	int   _left;

	_left = __size;
	memset( __buf, 0, __size );
	_cur = _prev = __str;
	/*循环匹配*/
	while(*_cur)
	{
		_son_cur=__find;

		while( *_cur&&*_son_cur )
		{
			if ( *_cur!=*_son_cur )
				break;
			_cur++;
			_son_cur++;
		};
		/*匹配到了结尾,发现成功*/
		if ( *_son_cur==0 )
		{
			if ( _left>(int)strlen(__replace) )
			{
				strcpy( (__buf+__size-_left), __replace );
				_left -= strlen(__replace);
			}
			else
			{
				/*缓冲区用完退出*/
				strncpy( (__buf+__size-_left), __replace, _left-1);
				break;
			}
			_prev = _cur;
			continue;
		}
	
		/*没有匹配*/
		while( _left&&(_prev<=_cur) )
		{
			*(__buf+__size-_left)=*_prev;
			_left--;
			_prev++;
		}
		_cur++;
	}

	return strlen( __buf );
}


int sub_string( const char* __src, int __direction, int __start, int __len, char *__buf, int __size )
/*
 功能：取一个字符串的子字符串
	__src:源字符串
	__direction:搜索方向，0从头开始，1从尾开始。
	__start:开始位置
	__len:复制长度
	__buf:目标字符串缓冲区
	__size:缓冲区大小
*/
{
	char *_start;
	memset( __buf, 0, __size );
	
	if ( (int)strlen(__src)<__start || (int)strlen(__src)<__len ) 
		return -1;
	if ( __direction==0 )
	{
		if ( (int)strlen(__src) < __start+__len )
			return -1;
	}
	else
	if ( __direction==1 )
	{
		if ( __len>__start )
			return -1;
	}	
	
	/*确定_start位置*/
	if ( __direction==0 )
	{
		_start=(char*)__src+__start-1;
	}
	else
	if ( __direction==1 )
	{
		_start=(char*)__src+strlen(__src)-__start;
	}
	
	strncpy(__buf, _start, (__len>=__size ? __size:__len) );
	
	return 0;
	
}


int get_string_letter_count(char *_str, char c)
{
	int nCount = 0;
	while(*_str != '\0')
	{
		if(*_str == c)
			nCount += 1;
		_str++;
	}
	return nCount;
}

