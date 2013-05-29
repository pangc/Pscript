#include <string>
#include <iostream>

// Extremely simple string class
class String {
public:
   String()   {s = new char [1]; s[0] = 0;}
   String(char *_s)  {s = new char [1]; s[0] = 0; Assign (_s);}
   ~String()  {delete[] s;}
   char *Val ()   {return s;}
   int   Len ()   {return strlen(s);}
   void  Assign (String &n)   {int l; delete[] s; l = n.Len(); s = new char[l+1]; strcpy (s, n.Val());}
   void  Assign (char *n)     {int l; delete[] s; l = strlen(n); s = new char[l+1]; strcpy (s, n);}
   void  Concatenate (String &n)   {
      int nl = Len() + n.Len();
      char *t = new char [nl + 1];
      strcpy (t, s); strcat (t, n.Val());
      delete[] s; s = t;
   }
   void  Concatenate (char *n)   {
      String t; t.Assign (n); Concatenate(t);
   }
   void Print ()   {std::cout << s;}
   void Input ()   {delete[]s; s = new char[81]; std::cin.getline(s,80);}
private:
   char *s;
};

