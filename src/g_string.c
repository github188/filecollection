#include "g_string.h"

#define IN_WARD  1 /*�ڵ�����*/
#define OUT_WARD 2 /*�ڵ�����*/

 int get_string_wards_count( char* __str )
/*
����:�õ��ַ����еĵ��ʸ���
�������:__str,��NULL��β��c string
���ز���:���ʸ���*/
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
����:�õ��ַ�����ĳ��λ�õĵ���
�������:	__str �ַ���
			__pos ���ʵ�λ��,��1��ʼ����
			__buf ��ŵ��ʵĻ�����
			__buf_size ��������С
����ֵ: ���ص��ʵĳ���,���ÿ�ҵ��򷵻�0*/
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
����:��һ���ַ���__str����__replace����__find,����䵽__buf��
�������:	__strԴ�ӷ���
			__find��Դ�ַ����н�Ҫ������ľ��ַ���
			__replace��Դ�ַ����н�Ҫ����������ַ���
			__buf���������
			__size��������С
����ֵ:		���ַ�������.
����:���__str�ܴ�,���ܽ�����Ӱ��.
*/
{
	char *_cur,*_prev;
	char *_son_cur;
	int   _left;

	_left = __size;
	memset( __buf, 0, __size );
	_cur = _prev = __str;
	/*ѭ��ƥ��*/
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
		/*ƥ�䵽�˽�β,���ֳɹ�*/
		if ( *_son_cur==0 )
		{
			if ( _left>(int)strlen(__replace) )
			{
				strcpy( (__buf+__size-_left), __replace );
				_left -= strlen(__replace);
			}
			else
			{
				/*�����������˳�*/
				strncpy( (__buf+__size-_left), __replace, _left-1);
				break;
			}
			_prev = _cur;
			continue;
		}
	
		/*û��ƥ��*/
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
 ���ܣ�ȡһ���ַ��������ַ���
	__src:Դ�ַ���
	__direction:��������0��ͷ��ʼ��1��β��ʼ��
	__start:��ʼλ��
	__len:���Ƴ���
	__buf:Ŀ���ַ���������
	__size:��������С
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
	
	/*ȷ��_startλ��*/
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

