#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>
#include <math.h>
#include "definitions.h"
#include "Calculator.h"

#define LEXMAXLEN 17

int cnt0 = 0, //Число элементов в стеке операторов
    cnt1 = 0; //Число элементов в стеке операндов
int stkop[100]; //Стек операторов
double stk[100], //Стек операндов
       x;

//Функция возвращает код цифры
int getCode(int i)
{
    return i + 0x30;
}

//Функция добавляет текст в окно
int AppendTextWnd(HWND hw, const char *str)
{
    int len = 0;
    char text[256];
    len = GetWindowText(hw, &text[0], 256);
    if(len + strlen(str) > 255) {
        return 0;
    }
    strcat(text, str);
    SetWindowText(hw, &text[0]);
    return 1;
}

//Проверяет является ли строка вещественным числом
int isnumber(char *str)
{
    int n = 0;
    while(str[n] != '\0') if(!isdigit(str[n]) && str[n] != '.') {
            return 0;
        } else {
            n++;
        }
    return 1;
}

int push(double f)
{
    if(cnt1 != 99) {
        stk[cnt1++] = f;
    } else {
        return 0;
    }
    return cnt1;
}

double pop()
{
    if(cnt1 != 0) {
        return stk[--cnt1];
    } else {
        return 0;
    }
}

void func(int code)
{
    int i, rez, n;
#ifdef DEBUG
    //printf("%d:%lf %d %lf\n",cnt1,stk[cnt1-2],code,stk[cnt1-1]);
#endif // DEBUG
    if(code == CL_PLUS) {
        stk[cnt1 - 2] = stk[cnt1 - 2] + stk[--cnt1];
    } else if(code == CL_SUB) {
        stk[cnt1 - 2] = stk[cnt1 - 2] - stk[--cnt1];
    } else if(code == CL_USUB) {
        stk[cnt1 - 1] = -stk[cnt1 - 1];
    } else if(code == CL_MUL) {
        stk[cnt1 - 2] = stk[cnt1 - 2] * stk[--cnt1];
    } else if(code == CL_DIVN) {
        stk[cnt1 - 2] = stk[cnt1 - 2] / stk[--cnt1];
    } else if(code == CL_DIV) {
        if(stk[cnt1 - 2] < 0) {
            stk[cnt1 - 2] = -stk[cnt1 - 2];
        }
        if(stk[cnt1 - 1] < 0) {
            stk[cnt1 - 1] = -stk[cnt1 - 1];
        }
        n = 0;
        while(stk[cnt1 - 2] >= stk[cnt1 - 1]) {
            stk[cnt1 - 2] -= stk[cnt1 - 1];
            n++;
        }
        stk[--cnt1 - 1] = n;
    } else if(code == CL_MOD) {
        if(stk[cnt1 - 2] < 0) {
            stk[cnt1 - 2] = -stk[cnt1 - 2];
        }
        if(stk[cnt1 - 1] < 0) {
            stk[cnt1 - 1] = -stk[cnt1 - 1];
        }
        while(stk[cnt1 - 2] >= stk[cnt1 - 1]) {
            stk[cnt1 - 2] -= stk[cnt1 - 1];
        }
    } else if(code == CL_POW) {
        if(stk[cnt1 - 2] != 0) {
            if(stk[cnt1 - 2] > 0) {
                stk[cnt1 - 2] = stk[cnt1 - 1] * ln(stk[cnt1 - 2]);
            } else {
                stk[cnt1 - 2] = stk[cnt1 - 1] * ln(-stk[cnt1 - 2]);
            }
            stk[cnt1 - 2] = exp(stk[cnt1 - 2]);
            while(stk[cnt1 - 1] >= 1) {
                stk[cnt1 - 2] *= -1;
                stk[cnt1 - 1]--;
            }
        }
        cnt1--;
    } else if(code == CL_SIN) {
        stk[cnt1 - 1] = sinus(stk[cnt1 - 1]);
    } else if(code == CL_COS) {
        stk[cnt1 - 1] = cosinus(stk[cnt1 - 1]);
    } else if(code == CL_TG) {
        stk[cnt1 - 1] = sinus(stk[cnt1 - 1]) / cosinus(stk[cnt1 - 1]);
    } else if(code == CL_CTG) {
        stk[cnt1 - 1] = cosinus(stk[cnt1 - 1]) / sinus(stk[cnt1 - 1]);
    } else if(code == CL_SQRT) {
        stk[cnt1 - 1] = 0.5 * ln(stk[cnt1 - 1]);
        stk[cnt1 - 1] = exp(stk[cnt1 - 1]);
    } else if(code == CL_EXP) {
        stk[cnt1 - 1] = exp(stk[cnt1 - 1]);
    } else if(code == CL_LN) {
        stk[cnt1 - 1] = ln(stk[cnt1 - 1]);
    } else if(code == CL_LG) {
        stk[cnt1 - 1] = ln(stk[cnt1 - 1]) / MTH_LND;
    } else if(code == CL_SIGN) if(stk[cnt1 - 1] < 0) {
            stk[cnt1 - 1] = -1;
        } else {
            stk[cnt1 - 1] = 1;
        } else if(code == CL_AS) {
        stk[cnt1 - 1] = arcsin(stk[cnt1 - 1]);
    } else if(code == CL_AC) {
        stk[cnt1 - 1] = MTH_PI / 2 - arcsin(stk[cnt1 - 1]);
    } else if(code == CL_ATG) {
        stk[cnt1 - 1] = arctg(stk[cnt1 - 1]);
    } else if(code == CL_ACTG) {
        stk[cnt1 - 1] = arctg(1 / stk[cnt1 - 1]);
    } else {
        stk[cnt1 - 1] = stk[cnt1 - 1];
    }
#ifdef DEBUG
    //printf("= %lf:%d\n",stk[cnt1-1],cnt1);
#endif // DEBUG
}

int pushop(int code)
{
    if(cnt0 != 99) {
        stkop[cnt0++] = code;
    } else {
        return 0;
    }
    return cnt0;
}

int popop()
{
    if(cnt0 != 0) {
        return stkop[--cnt0];
    } else {
        return 0;
    }
}

void LexAnalyzer(char *str, int reg)
{
    int i = -1, code, length = 0;
    char lex[LEXMAXLEN + 1];
    FILE* in = NULL;
    in = fopen("lex.txt", "a+");
    while(str[i + 1] != '\0') {
        while(str[i + 1] == ' ') {
            i++;
        }
        while(str[i + 1] != '\0') {
            i++;
            if(isalpha(str[i]) || isdigit(str[i]) || str[i] == '.') {
                lex[length++] = str[i];
            } else if(str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/' || str[i] == '^' || str[i] == '(' || str[i] == ')') {
                if(length != 0) {
                    i--;
                } else {
                    lex[length++] = str[i];
                }
                break;
            } else {
                break;
            }
            if(length == LEXMAXLEN) {
                break;
            }
        }
        lex[length] = '\0'; //32+17*15-sin(2*exp(5)+cos(3))
#ifdef DEBUG
        printf("%s\n", lex);
#endif // DEBUG
        if(isnumber(lex)) {
            code = CL_NUM;
        } else if(!strcmp(lex, "+")) {
            code = CL_PLUS;
        } else if(!strcmp(lex, "-")) {
            code = CL_SUB;
        } else if(!strcmp(lex, "*")) {
            code = CL_MUL;
        } else if(!strcmp(lex, "/")) {
            code = CL_DIVN;
        } else if(!strcmp(lex, "^")) {
            code = CL_POW;
        } else if(!strcmp(lex, "div")) {
            code = CL_DIV;
        } else if(!strcmp(lex, "mod")) {
            code = CL_MOD;
        } else if(!strcmp(lex, "sin")) {
            code = CL_SIN;
        } else if(!strcmp(lex, "cos")) {
            code = CL_COS;
        } else if(!strcmp(lex, "tg")) {
            code = CL_TG;
        } else if(!strcmp(lex, "ctg")) {
            code = CL_CTG;
        } else if(!strcmp(lex, "sqrt")) {
            code = CL_SQRT;
        } else if(!strcmp(lex, "exp")) {
            code = CL_EXP;
        } else if(!strcmp(lex, "ln")) {
            code = CL_LN;
        } else if(!strcmp(lex, "lg")) {
            code = CL_LG;
        } else if(!strcmp(lex, "arcsin")) {
            code = CL_AS;
        } else if(!strcmp(lex, "arccos")) {
            code = CL_AC;
        } else if(!strcmp(lex, "arctg")) {
            code = CL_ATG;
        } else if(!strcmp(lex, "arcctg")) {
            code = CL_ACTG;
        } else if(!strcmp(lex, "sign")) {
            code = CL_SIGN;
        } else if(!strcmp(lex, "(")) {
            code = CL_OBRK;
        } else if(!strcmp(lex, ")")) {
            code = CL_CBRK;
        } else if(!strcmp(lex, "x") && reg) {
            code = CL_VAR;
        } else {
            code = CL_UKWN;
        }
        fprintf(in, "%d %s\n", code, lex);
        length = 0;
    }
    fprintf(in, "%d end\n", CL_END);
    fclose(in);
}

int SyntaxAnalyzer(HWND hw, int reg)
{
    int code, state = 0, numb = 0;
    char lex[LEXMAXLEN + 1], text[64];
    FILE *in = NULL;
    in = fopen("lex.txt", "a+");
    while(fscanf(in, "%d %s", &code, lex) != EOF) {
        if(code == CL_UKWN) {
            sprintf(text, "Неизвестная лексема %s", lex);
            SetWindowText(hw, text);
            fclose(in);
            return 0;
        }
        switch(state) {
        case 0:
            if(code == CL_NUM || (code == CL_VAR && reg == 1)) {
                state = 2;
            } else if(code == CL_OBRK) {
                numb++;
            } else if(code > CL_MOD && code < CL_NUM) {
                state = 1;
            } else if(code != CL_SUB) { //Если функция
                sprintf(text, "Неожидалось %s", lex);
                SetWindowText(hw, text);
                fclose(in);
                return 0;
            }
            break;
        case 1:
            if(code == CL_OBRK) {
                state = 0;
                numb++;
            } else {
                sprintf(text, "Неожидалось %s", lex);
                SetWindowText(hw, text);
                fclose(in);
                return 0;
            }
            break;
        case 2:
            if(code < CL_SIN) {
                state = 0;    //Если простой оператор или div или mod
            } else if(code == CL_END) {
                continue;
            } else if(code == CL_CBRK) {
                if(numb > 0) {
                    numb--;
                } else {
                    SetWindowText(hw, "Лишняя скобка");
                    fclose(in);
                    return 0;
                }
            } else {
                sprintf(text, "Неожидалось %s", lex);
                SetWindowText(hw, text);
                fclose(in);
                return 0;
            }
            break;
        }
    }
    if(numb != 0) {
        SetWindowText(hw, "Отсутствует скобка");
        fclose(in);
        return 0;
    }
    fclose(in);
    return 1;
}

double Calc()
{
    int code, codeprev = -1;
    double num;
    memset(stkop, 0, sizeof(stkop));
    memset(stk, 0, sizeof(stk));
    FILE *in = NULL;
    in = fopen("lex.txt", "a+");
    while(1) {
        fscanf(in, "%d", &code);
        if(code == CL_NUM) {
            fscanf(in, " %lf\n", &num);
        } else if(code == CL_END) {
            break;
        } else {
            fscanf(in, " %*s");
        }
        if(code == CL_NUM) {
            push(num);
        } else if(code == CL_VAR) {
            push(x);
        } else if(code > CL_POW && code < CL_NUM) {
            pushop(code);
        } else if(code == CL_POW) {
            while(cnt0 != 0 && stkop[cnt0 - 1] > CL_MOD && stkop[cnt0 - 1] < CL_NUM) {
                func(popop());
            }
            pushop(code);
        } else if(code == CL_DIV || code == CL_MOD) {
            while(cnt0 != 0 && stkop[cnt0 - 1] > CL_DIVN && stkop[cnt0 - 1] < CL_NUM) {
                func(popop());
            }
            pushop(code);
        } else if(code == CL_MUL || code == CL_DIVN) {
            while(cnt0 != 0 && stkop[cnt0 - 1] > CL_SUB && stkop[cnt0 - 1] < CL_NUM) {
                func(popop());
            }
            pushop(code);
        } else if(code == CL_PLUS || code == CL_SUB) {
            if(codeprev == CL_USUB || codeprev == CL_OBRK || codeprev < CL_SIN) {
                pushop(CL_USUB);
            } else {
                while(cnt0 != 0 && stkop[cnt0 - 1] < CL_NUM) {
                    func(popop());
                }
                pushop(code);
            }
        } else if(code == CL_OBRK) {
            pushop(code);
        } else if(code == CL_CBRK) {
            while(stkop[cnt0 - 1] != CL_OBRK) {
                func(popop());
            }
            popop();
            func(popop());
        }
        codeprev = code;
    }
    while(cnt0 > 0) {
        func(popop());
    }
    fclose(in);
    cnt1 = 0;
    return stk[0];
}

double Integrate(float lcnt, float hcnt)
{
    double rez = 0.0, s0, s1, s2, h;
    x = lcnt;
    s0 = Calc();
    for(x = lcnt + 0.001; x < hcnt - 0.001; x += 0.001) {
        s1 = Calc();
        x += 0.001;
        s2 = Calc();
        rez += s0 + 4 * s1 + s2;
        s0 = s2;
    }
    h = (hcnt - (x - 0.001)) / 0.002;
    x = (hcnt + (x - 0.001)) / 2;
    s1 = Calc();
    x = hcnt;
    s2 = Calc();
    rez += (s0 + 4 * s1 + s2) * h;
    return rez * 0.001 / 3;
}

double GetLength(float lcnt, float hcnt)
{
    double length = 0.0, s0, s1;
    for(x = lcnt; x < hcnt; x += 0.01) {
        x -= 0.000001;
        s0 = Calc();
        x += 0.000002;
        s1 = Calc();
        s0 = (s1 - s0) / 0.000002; //Вычисление производной
        s0 *= s0;
        s0 += 1;
        if(s0 != 0) {
            s0 = ln(s0) / 2;
            s0 = exp(s0);
        }
        length += s0 * 0.01;
    }
    return length;
}

double GetRoot(float aprx)
{
    int i = 0;
    double s0, s1, s2;
    while(1) {
        x = aprx - 0.00000001;
        s0 = Calc();
        x = aprx + 0.00000001;
        s1 = Calc();
        s1 = (s1 - s0) / 0.00000002;
        x = aprx;
        s0 = Calc();
        if(s0 < 0.001 && s0 > -0.001) {
            break;
        } else if(i > 10000) {
            break;
        }
        aprx -= s0 / s1;
        i++;
    }
    if(i > 10000) {
        if(aprx < 0) {
            aprx = INFINITY;
        } else if(aprx > 0) {
            aprx = -INFINITY;
        }
    }
    return aprx;
}

double sinus(double x)
{
    int fs = 1; //Флаг знака
    long long n = 1;
    if(x < 0) {
        x = -x;
        fs = 0;
    }
    while(x >= 2 * MTH_PI) {
        x -= 2 * MTH_PI;
    }
    double s = x, del = x;
    while(del > 0.000001 || -del > 0.000001) {
        del *= x / ++n;
        del *= -x / ++n;
        s += del;
    }
    if(fs == 1) return s;
    else return -s;
}

double cosinus(double x)
{
    long long n = 1;
    if(x < 0) {
        x = -x;
    }
    while(x >= 2 * MTH_PI) {
        x -= 2 * MTH_PI;
    }
    double s = 1, del = 1;
    while(del > 0.000001 || -del > 0.000001) {
        del *= x / n++;
        del *= -x / n++;
        s += del;
    }
    return s;
}

double exp(double x)
{
    unsigned long long n = 1;
    double del = 1, s = 1;
    int i = 0;
    while(del > 0.000001 || -del > 0.000001) {
        del *= x / n++;
        s += del;
        i++;
    }
    return s;
}

double ln(double x)
{
    if(x <= 0) {
        return -INFINITY;
    }
    double constln;
    unsigned long long n = 1;
    int k = 0;
    if(x > 1) {
        while(x >= 10) {
            x /= 10;
            k++;
        }
        constln = MTH_LND;
    } else {
        while(x < 0.1) {
            x *= 10;
            k++;
        }
        constln = -MTH_LND;
    }
    x = (x - 1) / (x + 1);
    double del = x, s = x, sqx = x * x;
    while(del > 0.000001 || -del > 0.000001) {
        del *= sqx;
        n += 2;
        s += del / n;
    }
    return 2 * s + k * constln;
}

double arcsin(double x)
{
    if(x <= -1 || x >= 1) {
        return NAN;
    }
    unsigned long long n = 1;
    double del = x, s = x, sqx = x * x;
    while(del > 0.000001 || -del > 0.000001) {
        del *= sqx * n++ / n++;
        s += del / n;
    }
    return s;
}

double arctg(double x)
{
    int f = 0,
        fs = 1; //Флаг знака
    unsigned long long n = 3;
    double del, s;
    if(x == 1) {
        return MTH_TGO;
    } else if(x == -1) {
        return -MTH_TGO;
    } else {
        if(x < 0) {
            x = -x;
            fs = 0;
        }
        if(x > 1) {
            x = 1 / x;
            s = MTH_PI / 2 - x;
            f = 1;
        } else {
            s = x;
        }
        del = x;
        x = x * x;
        while(del > 0.000001 || -del > 0.000001) {
            del *= -x;
            if(f) {
                s -= del / n;
            } else {
                s += del / n;
            }
            n += 2;
        }
        if(fs) {
            return s;
        } else {
            return -s;
        }
    }
}
