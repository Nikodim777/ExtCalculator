#ifndef CALCULATOR_H_INCLUDED
#define CALCULATOR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int AppendTextWnd(HWND hw,const char *str);

int isnumber(char *str);
int pushop(int opr);
int popop();
int push(double f);
double pop();
void func(int opr);

double sinus(double x);
double cosinus(double x);
double exp(double x);
double ln(double x);
double arcsin(double x);
double arctg(double x);

void LexAnalyzer(char *str,int reg);
int SyntaxAnalyzer(HWND hw,int reg);
double Calc();
double Integrate(float lcnt,float hcnt);
double GetLength(float lcnt,float hcnt);
double GetRoot(float aprx);

#endif
