#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <windowsx.h>
#include "definitions.h"
#include "Calculator.h"
#include "resources.h"

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK GetBoundsDlgProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp);
BOOL CALLBACK GetInitValueDlgProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("Calculator");
BOOL fGetMessage = 0;
BOOL fenter = 0, //Флаг фокуса окна редактирования
     fwork = 1;  //Если флаг установлен - режим ввода выражения, иначе - функции
PAINTSTRUCT ps = {0, 0, 400, 500}; //Размер поля отрисовки
HDC hdc;
HWND edit, //Хэндлер окна редактрирования формулы
     snmb0, snmb1, snmb2, //Хэндлеры надписей для сохранённых значений
     save0, save1, save2, //Хэндлеры окон для хранения сохранённых значений
     note, //Описание кнопки переключения
     opermod, operctg, opersign, funccalc, //Первый ряд операций и функций
     rbtnfunc, //Радио-кнопка переключения в режим ввода функций
     operdiv, opersqrt, operasin, funcint, //Второй ряд операций и функций
     rbtnexpr, //Радио-кнопка переключения в режим ввода выражений
     opersin, operexp, operacos, funclen,
     funcsave, opercos, operln, operatg, funcroot,
     opertg, operlg, operactg, funcclr,
     log,
     popup;
HBRUSH brush0, brush1;
HFONT font1;
COLORREF clr;
FILE *in;
int cur_y;
float hcnt = 0, //Верхняя граница вычисления
      lcnt = 0; //Нижняя граница вычисления
double rezult,
       saves[3] = {0, 0, 0}; //Сохранённые значения
char deft[5]; //Переменная для хранения части строки ввода


void setText(const char *text)
{
    if(fenter == 1) {
        if(!AppendTextWnd(edit, text)) {
            SetWindowText(log, "Переполнение!");
        }
    } else {
        SetWindowText(edit, text);
        fenter = 1;
    }
}

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = (HICON) LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(M_ICON32));
    wincl.hIconSm = (HICON) LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(M_ICON16));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = CreateSolidBrush(RGB(0x99, 0xD9, 0xEA));

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl)) {
        return 0;
    }

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               _T("Калькулятор"),       /* Title Text */
               WS_OVERLAPPED | WS_SYSMENU, /* default window without resize*/
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               400,                 /* The programs width */
               485,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while ((fGetMessage = GetMessage (&messages, NULL, 0, 0))) {
        if(fGetMessage == -1) {
            break;
        }
        switch (messages.message) {
            case WM_KEYDOWN:
                if(messages.wParam == VK_RETURN && messages.hwnd == edit) {
                    SendMessage(funccalc, BM_CLICK, 0, 0);
                }
                break;
            case WM_HOTKEY:
				if(messages.hwnd == edit) {
					switch (messages.wParam) {//Обработка горячих клавиш
						case HK_I:
							SendMessage(funcint, BM_CLICK, 0, 0);
							break;
						case HK_L:
							SendMessage(funclen, BM_CLICK, 0, 0);
							break;
						case HK_S:
							SendMessage(funcsave, BM_CLICK, 0, 0);
							break;
						case HK_R:
							SendMessage(funcroot, BM_CLICK, 0, 0);
							break;
						case HK_BS:
							SendMessage(funcclr, BM_CLICK, 0, 0);
							break;
						case HK_1:
							SendMessage(snmb0, BM_CLICK, 0, 0);
							break;
						case HK_2:
							SendMessage(snmb1, BM_CLICK, 0, 0);
							break;
						case HK_3:
							SendMessage(snmb2, BM_CLICK, 0, 0);
							break;
					}
                }
        }
        if(fGetMessage) {
            /* Translate virtual-key messages into character messages */
            TranslateMessage(&messages);
            /* Send message to WindowProcedure */
            DispatchMessage(&messages);
        }
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

BOOL CALLBACK GetBoundsDlgProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
        case WM_COMMAND:    /* сообщение от управляющих элементов */
            char text[50];
            if (LOWORD(wp) == boundsEnter) {
                EndDialog(hw, 0);
            } else if (LOWORD(wp) == hBoundEdit) {
                GetDlgItemText(hw, 2, LPSTR(text), 50);
                hcnt = atof(text);
            } else if (LOWORD(wp) == lBoundEdit) {
                GetDlgItemText(hw, 4, LPSTR(text), 50);
                lcnt = atof(text);
            }
    }
    return FALSE;
}

BOOL CALLBACK GetInitValueDlgProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
        case WM_COMMAND:    /* сообщение от управляющих элементов */
            char text[50];
            if (LOWORD(wp) == initValueEnter) {
                EndDialog(hw, 0);
            } else if (LOWORD(wp) == initValueEdit) {
                GetDlgItemText(hw, 2, LPSTR(text), 50);
                lcnt = atof(text);
            }
    }
    return FALSE;
}

/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {                /* handle the messages */
        case WM_CREATE:
            /* Set default count */
            hdc = GetDC(hwnd);
            brush0 = CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
            brush1 = CreateSolidBrush(RGB(0x99, 0xD9, 0xEA));
            font1 = CreateFont(-40, 12, 0, 0, 500, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Courier New");
            /* Draw widgets */
            edit = CreateWindow("EDIT", "Введите выражение", WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL, 10, 9, 375, 35, hwnd, NULL, GetModuleHandle(0), NULL);
            snmb0 = CreateWindow("BUTTON", "1-ое сохранение", WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_CENTER, 0, 60, 133, 55, hwnd, (HMENU)SNMB0, GetModuleHandle(0), NULL);
            snmb1 = CreateWindow("BUTTON", "2-ое сохранение", WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_CENTER, 133, 60, 133, 55, hwnd, (HMENU)SNMB1, GetModuleHandle(0), NULL);
            snmb2 = CreateWindow("BUTTON", "3-ое сохранение", WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_CENTER, 266, 60, 133, 55, hwnd, (HMENU)SNMB2, GetModuleHandle(0), NULL);
            save0 = CreateWindow("STATIC", "0", WS_VISIBLE | WS_CHILD | SS_LEFT, 0, 115, 133, 25, hwnd, NULL, GetModuleHandle(0), NULL);
            save1 = CreateWindow("STATIC", "0", WS_VISIBLE | WS_CHILD | SS_LEFT, 133, 115, 133, 25, hwnd, NULL, GetModuleHandle(0), NULL);
            save2 = CreateWindow("STATIC", "0", WS_VISIBLE | WS_CHILD | SS_LEFT, 266, 115, 133, 25, hwnd, NULL, GetModuleHandle(0), NULL);
            note = CreateWindow("EDIT", "Режим работы:", WS_VISIBLE | WS_CHILD | ES_CENTER, 0, 170, 117, 25, hwnd, NULL, GetModuleHandle(0), NULL);
            opermod = CreateWindow("BUTTON", "mod", WS_VISIBLE | WS_CHILD | BS_CENTER, 117, 140, 55, 55, hwnd, (HMENU)OPERMOD, GetModuleHandle(0), NULL);
            operctg = CreateWindow("BUTTON", "ctg", WS_VISIBLE | WS_CHILD | BS_CENTER, 172, 140, 55, 55, hwnd, (HMENU)OPERCTG, GetModuleHandle(0), NULL);
            opersign = CreateWindow("BUTTON", "sign", WS_VISIBLE | WS_CHILD | BS_CENTER, 227, 140, 55, 55, hwnd, (HMENU)OPERSIGN, GetModuleHandle(0), NULL);
            funccalc = CreateWindow("BUTTON", "Посчитать", WS_VISIBLE | WS_CHILD | BS_CENTER, 282, 140, 117, 55, hwnd, (HMENU)FUNCCALC, GetModuleHandle(0), NULL);
            rbtnfunc = CreateWindow("BUTTON", "с функциями", WS_VISIBLE | WS_CHILD | BS_CENTER | BS_AUTORADIOBUTTON | BS_BITMAP, 0, 195, 117, 55, hwnd, (HMENU)RBTNFUNC, GetModuleHandle(0), NULL);
            operdiv = CreateWindow("BUTTON", "div", WS_VISIBLE | WS_CHILD | BS_CENTER, 117, 195, 55, 55, hwnd, (HMENU)OPERDIV, GetModuleHandle(0), NULL);
            opersqrt = CreateWindow("BUTTON", "sqrt", WS_VISIBLE | WS_CHILD | BS_CENTER, 172, 195, 55, 55, hwnd, (HMENU)OPERSQRT, GetModuleHandle(0), NULL);
            operasin = CreateWindow("BUTTON", "arcsin", WS_VISIBLE | WS_CHILD | BS_CENTER, 227, 195, 55, 55, hwnd, (HMENU)OPERASIN, GetModuleHandle(0), NULL);
            funcint = CreateWindow("BUTTON", "Интегрировать", WS_VISIBLE | WS_CHILD | BS_CENTER, 282, 195, 117, 55, hwnd, (HMENU)FUNCINT, GetModuleHandle(0), NULL);
            rbtnexpr = CreateWindow("BUTTON", "с выражениями", WS_VISIBLE | WS_CHILD | BS_CENTER | BS_AUTORADIOBUTTON | BS_BITMAP, 0, 250, 117, 55, hwnd, (HMENU)RBTNEXPR, GetModuleHandle(0), NULL);
            opersin = CreateWindow("BUTTON", "sin", WS_VISIBLE | WS_CHILD | BS_CENTER, 117, 250, 55, 55, hwnd, (HMENU)OPERSIN, GetModuleHandle(0), NULL);
            operexp = CreateWindow("BUTTON", "exp", WS_VISIBLE | WS_CHILD | BS_CENTER, 172, 250, 55, 55, hwnd, (HMENU)OPEREXP, GetModuleHandle(0), NULL);
            operacos = CreateWindow("BUTTON", "arccos", WS_VISIBLE | WS_CHILD | BS_CENTER, 227, 250, 55, 55, hwnd, (HMENU)OPERACOS, GetModuleHandle(0), NULL);
            funclen = CreateWindow("BUTTON", "Найти длину", WS_VISIBLE | WS_CHILD | BS_CENTER, 282, 250, 117, 55, hwnd, (HMENU)FUNCLEN, GetModuleHandle(0), NULL);
            funcsave = CreateWindow("BUTTON", "Сохранить", WS_VISIBLE | WS_CHILD | BS_CENTER, 0, 305, 117, 55, hwnd, (HMENU)FUNCSAVE, GetModuleHandle(0), NULL);
            opercos = CreateWindow("BUTTON", "cos", WS_VISIBLE | WS_CHILD | BS_CENTER, 117, 305, 55, 55, hwnd, (HMENU)OPERCOS, GetModuleHandle(0), NULL);
            operln = CreateWindow("BUTTON", "ln", WS_VISIBLE | WS_CHILD | BS_CENTER, 172, 305, 55, 55, hwnd, (HMENU)OPERLN, GetModuleHandle(0), NULL);
            operatg = CreateWindow("BUTTON", "arctg", WS_VISIBLE | WS_CHILD | BS_CENTER, 227, 305, 55, 55, hwnd, (HMENU)OPERATG, GetModuleHandle(0), NULL);
            funcroot = CreateWindow("BUTTON", "Найти корень", WS_VISIBLE | WS_CHILD | BS_CENTER, 282, 305, 117, 55, hwnd, (HMENU)FUNCROOT, GetModuleHandle(0), NULL);
            opertg = CreateWindow("BUTTON", "tg", WS_VISIBLE | WS_CHILD | BS_CENTER, 117, 360, 55, 55, hwnd, (HMENU)OPERTG, GetModuleHandle(0), NULL);
            operlg = CreateWindow("BUTTON", "lg", WS_VISIBLE | WS_CHILD | BS_CENTER, 172, 360, 55, 55, hwnd, (HMENU)OPERLG, GetModuleHandle(0), NULL);
            operactg = CreateWindow("BUTTON", "arcctg", WS_VISIBLE | WS_CHILD | BS_CENTER, 227, 360, 55, 55, hwnd, (HMENU)OPERACTG, GetModuleHandle(0), NULL);
            funcclr = CreateWindow("BUTTON", "Очистить", WS_VISIBLE | WS_CHILD | BS_CENTER, 282, 360, 117, 55, hwnd, (HMENU)FUNCCLR, GetModuleHandle(0), NULL);
            log = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | ES_CENTER, 0, 415, 394, 35, hwnd, NULL, GetModuleHandle(0), NULL);
            /* Set style */
            SendMessage(edit, WM_SETFONT, (WPARAM)font1, 0);
            SendMessage(log, WM_SETFONT, (WPARAM)font1, 0);
            SendMessage(log, EM_SETREADONLY, 1, 0);
            SendMessage(rbtnfunc, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(rb0), IMAGE_BITMAP, 133, 0, 0));
            SendMessage(rbtnexpr, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(rb1), IMAGE_BITMAP, 133, 0, 0));
            SendMessage(rbtnexpr, BM_SETCHECK, BST_CHECKED, 0);
			
            EnableWindow(funcint, 0);
            EnableWindow(funclen, 0);
            EnableWindow(funcroot, 0);
			
			//Регистрация горячих клавиш
            RegisterHotKey(edit, HK_I, MOD_CONTROL, 0x49); //CTRL+I
            RegisterHotKey(edit, HK_L, MOD_CONTROL, 0x4C); //CTRL+L
            RegisterHotKey(edit, HK_R, MOD_CONTROL, 0x52); //CTRL+R
            RegisterHotKey(edit, HK_S, MOD_CONTROL, 0x53); //CTRL+S
            RegisterHotKey(edit, HK_BS, MOD_CONTROL, VK_BACK); //CTRL+BACKSPACE
            RegisterHotKey(edit, HK_1, MOD_CONTROL, 0x31); //CTRL+1
            RegisterHotKey(edit, HK_2, MOD_CONTROL, 0x32); //CTRL+2
            RegisterHotKey(edit, HK_3, MOD_CONTROL, 0x33); //CTRL+3
            break;
        case WM_PAINT:
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            UpdateWindow(hwnd);
            break;
        case WM_CTLCOLOREDIT:
            if (lParam == (LPARAM) edit) {
                if(fenter) {
                    clr = RGB(0x00, 0x00, 0x00);
                } else {
                    clr = RGB(0xAA, 0xAA, 0xAA);
                }
                SetTextColor((HDC)wParam, clr);
                return (INT_PTR) brush0;
            }
            if (lParam == (LPARAM) note) {
                clr = RGB(0x99, 0xD9, 0xEA);
                SetBkColor((HDC)wParam, clr);
                return (INT_PTR) brush1;
            }
            break;
        case WM_MOUSEMOVE: //Добавление убирание местозаменителя при наведении или отведении мыши
            cur_y = GET_Y_LPARAM(lParam);
            GetWindowText(edit, deft, 5);
            if(cur_y < 55 && !strcmp(deft, "Введ")) {//Убираем если есть
                SetWindowText(edit, "");
                fenter = 1;
            }
            if(cur_y > 55 && !strcmp(deft, "")) {//Добавляем
                if(fwork) { //Если работаем с выражениями
                    SetWindowText(edit, "Введите выражение");
                } else { //Если работаем с функциями
                    SetWindowText(edit, "Введите функцию");
                }
                fenter = 0;
            }
            break;
        case WM_COMMAND:
            char text[50], expr[256];
            switch(HIWORD(wParam)) {
                case EN_SETFOCUS:
                    if(lParam == (LPARAM) note) {
                        SendMessage(note, WM_KILLFOCUS, 0, 0);
                        SetWindowText(note, "Режим работы:");
                    }
                    break;
            }
            switch(LOWORD(wParam)) {
                case SNMB0:
                    GetWindowText(save0, text, 50);
                    setText(text);
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case SNMB1:
                    GetWindowText(save1, text, 50);
                    setText(text);
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case SNMB2:
                    GetWindowText(save2, text, 50);
                    setText(text);
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case OPERMOD:
                    setText(" mod ");
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case OPERCTG:
                    setText("ctg(");
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case OPERSIGN:
                    setText("sign(");
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case OPERDIV:
                    setText(" div ");
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case OPERSQRT:
                    setText("sqrt(");
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case OPERASIN:
                    setText("arcsin(");
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case OPERSIN:
                    setText("sin(");
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case OPEREXP:
                    setText("exp(");
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case OPERACOS:
                    setText("arccos(");
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case OPERCOS:
                    setText("cos(");
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case OPERLN:
                    setText("ln(");
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case OPERATG:
                    setText("arctg(");
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case OPERTG:
                    setText("tg(");
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case OPERLG:
                    setText("lg(");
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case OPERACTG:
                    setText("arcctg(");
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case FUNCCALC:
                    in = fopen("lex.txt", "w");
                    fprintf(in, "");
                    fclose(in);
                    GetWindowText(edit, expr, 256);
                    LexAnalyzer(&expr[0], 0);
                    if(SyntaxAnalyzer(log, 0)) {
                        SetWindowText(log, "");
                        rezult = Calc();
                        sprintf(expr, "%lf", rezult);
                        SetWindowText(edit, expr);
                    }
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case FUNCINT:
                    DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(5), hwnd, GetBoundsDlgProc);
                    in = fopen("lex.txt", "w");
                    fprintf(in, "");
                    fclose(in);
                    GetWindowText(edit, expr, 256);
                    LexAnalyzer(&expr[0], 1);
                    if(SyntaxAnalyzer(log, 1)) {
                        SetWindowText(log, "");
                        rezult = Integrate(lcnt, hcnt);
                        sprintf(expr, "%lf", rezult);
                        SetWindowText(edit, expr);
                    }
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case FUNCLEN:
                    DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(5), hwnd, GetBoundsDlgProc);
                    in = fopen("lex.txt", "w");
                    fprintf(in, "");
                    fclose(in);
                    GetWindowText(edit, expr, 256);
                    LexAnalyzer(&expr[0], 1);
                    if(SyntaxAnalyzer(log, 1)) {
                        SetWindowText(log, "");
                        rezult = GetLength(lcnt, hcnt);
                        sprintf(expr, "%lf", rezult);
                        SetWindowText(edit, expr);
                    }
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case FUNCSAVE:
                    saves[2] = saves[1];
                    saves[1] = saves[0];
                    saves[0] = rezult;
                    sprintf(text, "%lf", saves[0]);
                    SetWindowText(save0, text);
                    sprintf(text, "%lf", saves[1]);
                    SetWindowText(save1, text);
                    sprintf(text, "%lf", saves[2]);
                    SetWindowText(save2, text);
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case FUNCROOT:
                    DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(6), hwnd, GetInitValueDlgProc);
                    in = fopen("lex.txt", "w");
                    fprintf(in, "");
                    fclose(in);
                    GetWindowText(edit, expr, 256);
                    LexAnalyzer(&expr[0], 1);
                    if(SyntaxAnalyzer(log, 1)) {
                        SetWindowText(log, "");
                        rezult = GetRoot(lcnt);
                        sprintf(expr, "%lf", rezult);
                        SetWindowText(edit, expr);
                    }
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case FUNCCLR:
                    SetWindowText(edit, "");
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case RBTNFUNC:
                    EnableWindow(funccalc, 0);
                    EnableWindow(funcint, 1);
                    EnableWindow(funclen, 1);
                    EnableWindow(funcroot, 1);
                    GetWindowText(edit, deft, 5);
                    if(!strcmp(deft, "Введ")) {
                        SetWindowText(edit, "Введите функцию");
                    }
                    fwork = 0;
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
                case RBTNEXPR:
                    EnableWindow(funccalc, 1);
                    EnableWindow(funcint, 0);
                    EnableWindow(funclen, 0);
                    EnableWindow(funcroot, 0);
                    GetWindowText(edit, deft, 5);
                    if(!strcmp(deft, "Введ")) {
                        SetWindowText(edit, "Введите выражение");
                    }
                    fwork = 1;
                    SetFocus(edit);
                    SendMessage(edit, WM_KEYDOWN, VK_END, 1);
                    break;
            }
            break;
        case WM_DESTROY:
            DeleteObject(brush0);
            DeleteObject(brush1);
            DeleteObject(hdc);
            in = fopen("lex.txt", "w");
            fprintf(in, "");
            fclose(in);
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

