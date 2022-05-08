int ft_strlen(char const *str){
  int c = 0;
  while(*(str+c)!= 0) c++;
  return (c);
}
int ft_strlen(char const **str){
  int c = 0;
  while(str[c] != NULL) c++;
  return (c);
}

/*
 * Ищет первое вхождение строки(thing) в строке(str).
 * addend заставляет вернуть не вхождение, а выход строки(ака когда строка закончилась)
 * */
int ft_find(char const *str, char const *thing, bool addend, int start, int stop){
  int lenStr = ft_strlen(str);
  int lenThing = ft_strlen(thing);
  if (stop > lenStr) stop = lenStr;
  if (lenStr-start-(lenStr-stop) < lenThing || start > lenStr) return (-1);
  for(int j = start, c; j < stop; j++)
    if(*(str + j) == *thing && stop - j > lenThing)
    {
      c = 0;
      for(int i = 0; i < lenThing; i++)
        if(*(str + j + i) == *(thing + i)) c++;
      if (c == lenThing)
      {
        if (addend)
          return (j + lenThing);
        else
          return (j);
      }
    }
  return (-1);
}
int ft_find(char const *str, char const *thing){return (ft_find(str,thing,false,0, ft_strlen(str) + 1));}
int ft_find(char const *str, char const *thing, bool addend){return (ft_find(str,thing, addend ,0, ft_strlen(str) + 1));}
int ft_find(char const *str, char const *thing, bool addend,int start){return (ft_find(str,thing, addend , start, ft_strlen(str) + 1));}

/*
 * Обрезает строку
 * */
char* ft_cut(char const *str, int start, int stop)
{
  char *s;
  int lenStr = ft_strlen(str);
  s = (char *)malloc(sizeof(char) * (lenStr-start-(lenStr-stop)));
  if (lenStr-start-(lenStr-stop) < 0 || s == NULL) return (NULL);
  for(int i = 0; i <= lenStr-start-(lenStr-stop); i++)
    *(s+i) = *(str+start+i);
  *(s+(lenStr-start-(lenStr-stop))) = 0;
  return (s);
}
char* ft_cut(char const *str, int start){return (ft_cut(str,start,ft_strlen(str)));}

/*
 * Поиск количества слов/символов в строке
 * */
int ft_count(char const *str, char *thing)
{
  int c = 0;
  int i = 0;
  int li = 0;
  while(i!= -1)
  {
    i = ft_find(str, thing, true, li);
    li = i + 1;
    c++;
  }
  return (c);
}
/*
char** ft_split(char *str, char *thing)
{
  int words = ft_count(str,thing);
  int lenThing = ft_strlen(thing);
  char **mass = (char **)malloc(sizeof(char *) * (words + 2));
  if (mass == NULL) return NULL;
  for(int i = 0,c=0,t=0; i <= words; i++)
  {
    c = ft_find(str,thing,false,c);
    if(c != -1)
    {
      mass[i] = (char *)malloc(sizeof(char) * (c - t + 1));
      if (mass[i] == NULL) return NULL;
      mass[i] = ft_cut(str,t,c);
      t = c+1;
      c = c + lenThing + 1;
    }
  }
  mass[words] = NULL;
  return(mass);
}
*/
int ft_strlcpy(char *dst, const char *src, int size)
{
  int i = 0;
  if (!dst || !src) return (0);
  while (1 < size && src[i]){
    dst[i] = src[i];
    i++;
    size--;
  }
  if (size > 0) dst[i] = '\0';
  return (ft_strlen(src));
}
int ft_strcpy(char *dst, const char *src){return (ft_strlcpy(dst,src,ft_strlen(src)+1));}
char  *ft_strdup(const char *str)
{
  char *arr=(char *)malloc(ft_strlen(str) + 1);
  if (arr == NULL) return (NULL);
  ft_strlcpy(arr, str, ft_strlen(str) + 1);
  return (arr);
}
char  *ft_substr(char const *s, int start, int len)
{
  char    *arr;
  int     i = 0;
  if (s == NULL) return (NULL);
  if (start >= ft_strlen(s)) return (ft_strdup(""));
  if (len > ft_strlen(s) - start) len = ft_strlen(s) - start;
  if (!(arr = (char *)malloc(len + 1))) return (NULL);
  while (s[i] && i < len){
    arr[i] = s[start + i];
    i++;
  }
  arr[i] = '\0';
  return (arr);
}

static char     **ft_clear(char **tab)
{
  unsigned int  i = 0;
  while (tab[i]) free(tab[i++]);
  free(tab);
  return (NULL);
}

static int    ft_strlen_c(const char *str, char c)
{
  int i = 0;
  while (str[i] && str[i] != c) i++;
  return (i);
}

static int    ft_count_words(const char *str, char c)
{
  int i = 0,count = 0;
  while (str[i]){
    if ((str[i] != c && str[i + 1] && str[i + 1] == c) || (str[i] != c && !str[i + 1])) count++;
    i++;
  }
  return (count);
}

char        **ft_split_c(char const *s, char c)
{
  int     i = 0;
  int     start = 0;
  char      **array;
  if (!s) return (NULL);
  array = (char **)malloc((ft_count_words(s, c) + 1) * sizeof(char *));
  if (array == NULL) return (NULL);
  while (s[i++] == c) start++;
  i = -1;
  while (++i < ft_count_words(s, c)){
    if (!(array[i] = ft_substr(s, start, ft_strlen_c(&s[start], c)))) return (ft_clear(array));
    start += ft_strlen_c(&s[start], c);
    while (s[start] == c) start++;
  }
  array[i] = NULL;
  return (array);
}

char *strtochar(String s)
{
  //int len = 0;
  //while(s[len] != 0)
  //  len++;
  int len = s.length();
  char *str = (char *)malloc(sizeof(char) * (len+1));
  for(int i = 0;i < len; i++)
    str[i] = s[i];
  str[len] = 0;
  return (str);
}
