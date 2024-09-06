#include "string.h"
void string_init(string_t *string)
{
    string->length = 0;
    string->max_length = STRING_MAX_LEN;
    string->text = NULL;
}



void string_copy(string_t *string, char *text)
{
    string->length = strlen(text);
    
    if (string->length > string->max_length) {
        string->length = string->max_length;
    }
    memcpy(string->text, text, string->length);
    string->text[string->length] = '\0'; 
}

void string_empty(string_t *string)
{
    memset(string->text, 0, string->max_length);
}


/*
 * 功能: n个字符比对
 * 参数: s1     字符串1
 *      s2      字符串2
 *      s3      要比较的字符数
 * 返回: 0 表示字符串一样
 *      小于0 表示s1小于s2
 *      大于0 表示s1大于s2
 * 说明: 引导和加载完成后，就会跳到这里
 */
int strncmp (const char * s1, const char * s2, int n)
{ 
	if(!n)return(0);

	while (--n && *s1 && *s1 == *s2)
	{ 
		s1++;
		s2++;
	}
	return( *s1 - *s2 );
}

char* itoa(char ** ps, int val, int base)
{
	int m = val % base;
	int q = val / base;
	if (q) {
		itoa(ps, q, base);
	}
	*(*ps)++ = (m < 10) ? (m + '0') : (m - 10 + 'A');

	return *ps;
}

int atoi(const char *src)
{
    int s = 0;
    char is_minus = 0;
  
    while (*src == ' ') {
			src++; 
		}
  
	if (*src == '+' || *src == '-') {
        if (*src == '-') {
           is_minus = 1;
        }
        src++;
    } else if (*src < '0' || *src > '9') {
		 s = 2147483647;
        return s;
    }
  
    while (*src != '\0' && *src >= '0' && *src <= '9') {
        s = s * 10 + *src - '0';
        src++;
    }
    return s * (is_minus ? -1 : 1);
}

//判断字符c是否为数字
int isdigit( int ch )
{
    return (unsigned int)(ch - '0') < 10u;
}

int isalpha(int ch)
{
    if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z')) {
        return 1;
    }
    return 0;
}

int isupper(int ch)
{
    return (unsigned int)(ch - 'A') < 26u;
}//判断字符c是否为大写英文字母

int isspace(char c)
{
    char comp[] = {' ', '\t', '\n', '\r', '\v', '\f'};
    int i;
    const int len = 6;
    for (i = 0; i < len; i++) {
        if (c == comp[i])
            return 1;
    }
    return 0;
}

/*
 * Convert a string to a long integer.
 *
 * Ignores 'locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
long strtol(const char * nptr, char ** endptr, int base)
{
    const char * s;
    long acc, cutoff;
    int c;
    int neg, any, cutlim;

    /*
     * Skip white space and pick up leading +/- sign if any.
     * If base is 0, allow 0x for hex and 0 for octal, else
     * assume decimal; if base is already 16, allow 0x.
     */
    s = nptr;
    do {
        c = (unsigned char) *s++;
    } while (isspace(c));

    if (c == '-')
    {
        neg = 1;
        c = *s++;
    }
    else
    {
        neg = 0;
        if (c == '+')
            c = *s++;
    }

    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X'))
    {
        c = s[1];
        s += 2;
        base = 16;
    }

    if (base == 0)
        base = c == '0' ? 8 : 10;

    /*
     * Compute the cutoff value between legal numbers and illegal
     * numbers.  That is the largest legal value, divided by the
     * base.  An input number that is greater than this value, if
     * followed by a legal input character, is too big.  One that
     * is equal to this value may be valid or not; the limit
     * between valid and invalid numbers is then based on the last
     * digit.  For instance, if the range for longs is
     * [-2147483648..2147483647] and the input base is 10,
     * cutoff will be set to 214748364 and cutlim to either
     * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
     * a value > 214748364, or equal but the next digit is > 7 (or 8),
     * the number is too big, and we will return a range error.
     *
     * Set any if any 'digits' consumed; make it negative to indicate
     * overflow.
     */
    cutoff = neg ? LONG_MIN : LONG_MAX;
    cutlim = cutoff % base;
    cutoff /= base;

    if (neg)
    {
        if (cutlim > 0)
        {
            cutlim -= base;
            cutoff += 1;
        }
        cutlim = -cutlim;
    }

    for (acc = 0, any = 0;; c = (unsigned char) *s++)
    {
        if (isdigit(c))
            c -= '0';
        else if (isalpha(c))
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        else
            break;

        if (c >= base)
            break;

        if (any < 0)
            continue;

        if (neg)
        {
            if (acc < cutoff || (acc == cutoff && c > cutlim))
            {
                any = -1;
                acc = LONG_MIN;
         //       errno = ERANGE;
            }
            else
            {
                any = 1;
                acc *= base;
                acc -= c;
            }
        }
        else
        {
            if (acc > cutoff || (acc == cutoff && c > cutlim))
            {
                any = -1;
                acc = LONG_MAX;
        //        errno = ERANGE;
            }
            else
            {
                any = 1;
                acc *= base;
                acc += c;
            }
        }
    }

    if (endptr != 0)
        *endptr = (char *) (any ? s - 1 : nptr);

    return (acc);
}


char* strcpy(char* dst, const char* src) 
{  
   char* r = dst;		       
   while((*dst++ = *src++));
   return r;
}

char* strncpy(char* dst, char* src, int n) 
{
  
   char* r = dst;		      
   while((*dst++ = *src++) && n > 0) n--;
   return r;
}

UINT32 strlen(const char* str) {
  
   const char* p = str;
   while(*p++);
   return (p - str - 1);
}

char strcmp (const char* a, const char* b)
{  
   while (*a != 0 && *a == *b) {
      a++;
      b++;
   }
   return *a < *b ? -1 : *a > *b;
}

/**
 * strcoll - 需要根据本地语言做处理，为了简便，直接调用strcmp
 * 
*/
int strcoll(const char *str1, const char *str2)
{
    return strcmp(str1, str2);
}

char* strrchr(const char* str, int c)
{
   
    char* ret = NULL;
    while (*str)
    {
        if (*str == (char)c)
            ret = (char *)str;
        str++;
    }
    if ((char)c == *str)
        ret = (char *)str;

    return ret;
}

char* strcat(char* strDest , const char* strSrc)
{
    char* address = strDest;
    while(*strDest)
    {
        strDest++;
    }
    while((*strDest++=*strSrc++));
    return (char* )address;
}



int strpos(char *str, char ch)
{
	int i = 0;
	int flags = 0;
	while(*str){
		if(*str == ch){
			flags = 1;	//find ch
			break;
		}
		i++;
		str++;
	}
	if(flags){
		return i;
	}else{
		return -1;	//str over but not found
	}
}

char *strncat(char *dst, const char *src, int n)
{
	char *ret = dst;
	while(*dst != '\0'){
		dst++;
	}
	while(n && (*dst++ = *src++) != '\0'){
		n--;
	}
	dst = (char*)'\0';
	return ret;
}

char *strchr(const char *s, int c)
{
    if(s == NULL)
    {
        return NULL;
    }

    while(*s != '\0')
    {
        if(*s == (char)c )
        {
            return (char *)s;
        }
        s++;
    }
    return NULL;
}

char *itoa16_align(char * str, int num)
{
	char *	p = str;
	char	ch;
	int	i;
	//为0
	if(num == 0){
		*p++ = '0';
	}
	else{	//4位4位的分解出来
		for(i=28;i>=0;i-=4){		//从最高得4位开始
			ch = (num >> i) & 0xF;	//取得4位
			ch += '0';			//大于0就+'0'变成ASICA的数字
			if(ch > '9'){		//大于9就加上7变成ASICA的字母
				ch += 7;		
			}
			*p++ = ch;			//指针地址上记录下来。
		}
	}
	*p = 0;							//最后在指针地址后加个0用于字符串结束
	return str;
}

/**
 * strmet - 复制直到遇到某个字符串
 * @src: 要操作的字符串
 * @buf: 要保存的地方
 * @ch: 要遇到的字符串
 * 
 * 返回缓冲区中字符的长度
 */
int strmet(const char *src, char *buf, char ch)
{ 
	char *p = (char *)src;

    /* 没有遇到就一直复制直到字符串结束或者遇到 */
	while (*p && *p != ch) {
        *buf = *p++;
        buf++;
	}
    /* 最后添加结束字符 */
    *buf = '\0';
	return p - (char *)src;
}

/* 朴素的模式匹配算法 ，只用一个外层循环 */
char *strstr(const char *dest, const char *src) 
{
	char *tdest = (char *)dest;
	char *tsrc = (char *)src;
	int i = 0;//tdest 主串的元素下标位置，从下标0开始找，可以通过变量进行设置，从其他下标开始找！
	int j = 0;//tsrc 子串的元素下标位置
	while (i <= strlen(tdest) - 1 && j <= strlen(tsrc) - 1) {
		//字符相等，则继续匹配下一个字符
        if (tdest[i] == tsrc[j]) {
			i++;
			j++;
		} else { //在匹配过程中发现有一个字符和子串中的不等，马上回退到 下一个要匹配的位置
			i = i - j + 1;
			j = 0;
		}
	}
	//循环完了后j的值等于strlen(tsrc) 子串中的字符已经在主串中都连续匹配到了
	if (j == strlen(tsrc)) {
		return tdest + i - strlen(tsrc);
	}
 
	return NULL;
}

size_t strspn(const char *s, const char *accept)
{
    const char *p = s;
    const char *a;
    size_t count = 0;

    for (; *p != '\0'; ++p) {
        for (a = accept; *a != '\0'; ++a) {
            if (*p == *a)
                break;
        }
        if (*a == '\0')
            return count;
        ++count;
    }
    return count;
}

const char *strpbrk(const char *str1, const char *str2)
{
    if (str1 == NULL || str2 == NULL)
    {
        //perror("str1 or str2");
        return NULL;
    }
    const char *temp1 = str1;
    const char *temp2 = str2;

    while (*temp1 != '\0')
    {
        temp2 = str2; //将str2 指针从新指向在字符串的首地址
        while (*temp2 != '\0')
        {
            if (*temp2 == *temp1)
                return temp1;
            else
                temp2++;
        }
        temp1++;
    }
return NULL;
}

char * ___strtok = NULL; 

char * strtok(char * s,const char * ct)  
{  
    char *sbegin, *send;  
  
    sbegin  = s ? s : ___strtok;  
    if (!sbegin) {  
        return NULL;  
    }  
    sbegin += strspn(sbegin,ct);  
    if (*sbegin == '\0') {  
        ___strtok = NULL;  
        return( NULL );  
    }  
    send = (char *)strpbrk( sbegin, ct);
    if (send && *send != '\0')
        *send++ = '\0';  
    ___strtok = send;  
    return (sbegin);  
}

#if 0
void *memset(void* src, UINT8 value, UINT32 size)
{
	UINT8* s = (UINT8*)src;
	while (size > 0){
		*s++ = value;
		--size;
	}
	return src;
}
#endif

void *memset16(void* src, UINT16 value, UINT32 size)
{
	UINT16* s = (UINT16*)src;
	while (size-- > 0){
		*s++ = value;
	}
	return src;
}

void *memset32(void* src, UINT32 value, UINT32 size) 
{
	UINT32* s = (UINT32*)src;
	while (size-- > 0){
		*s++ = value;
	}
	return src;
}

#if 0
void memcpy(const void* dst, const void* src, UINT32 size)
{
    UINT8 *_dst = (UINT8 *)dst;
    UINT8 *_src = (UINT8 *)src;
    while (size-- > 0)
        *_dst++ = *_src++;
}
#endif

int memcmp(const void * s1, const void *s2, int n)
{
	if ((s1 == 0) || (s2 == 0)) { /* for robustness */
		return (s1 - s2);
	}

	const char * p1 = (const char *)s1;
	const char * p2 = (const char *)s2;
	int i;
	for (i = 0; i < n; i++,p1++,p2++) {
		if (*p1 != *p2) {
			return (*p1 - *p2);
		}
	}
	return 0;
}

void* memmove(void* dst,const void* src,UINT32 count)
{
    char* tmpdst = (char*)dst;
    char* tmpsrc = (char*)src;

    if (tmpdst <= tmpsrc || tmpdst >= tmpsrc + count)
    {
        while(count--)
        {
            *tmpdst++ = *tmpsrc++; 
        }
    }
    else
    {
        tmpdst = tmpdst + count - 1;
        tmpsrc = tmpsrc + count - 1;
        while(count--)
        {
            *tmpdst-- = *tmpsrc--;
        }
    }

    return dst; 
}
