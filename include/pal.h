/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Black 0
#define Blue 1
#define Green 2
#define Cyan 3
#define Red 4
#define Magenta 5
#define Brown 6
#define Lightgray 7
#define Darkgray 8
#define Lightblue 9
#define Lightgreen A
#define Lightcyan B
#define Lightred C
#define Lightmagenta D
#define Yellow E
#define White F

#define __RH___S(color) #color
#define __RH__S(bg,fg) __RH___S(\x##bg##fg)
#define S(index,foreground,background,name,comment...) __RH__S(background,foreground)
#define S_(index,foreground,background,name,group,comment...) __RH__S(background,foreground)

/* This is the palette for the system when in color mode */
#define cpIDEColor\
        S_( 1,Blue,Lightgray,Color,Desktop) \
        \
        S_( 2,Black,Lightgray,Normal,Menus) \
        S( 3,Darkgray,Lightgray,Disabled) \
        S( 4,Red,Lightgray,Shortcut) \
        S( 5,Black,Green,Selected) \
        S( 6,Darkgray,Green,Selected disabled) \
        S( 7,Red,Green,Shortcut selected) \
        \
        S_( 8,Lightgray,Blue,Frame disabled,Blue Windows) \
        S( 9,White,Blue,Frame) \
        S( A,Lightgreen,Blue,Frame icons) \
        S( B,Blue,Cyan,Scroll bar page) \
        S( C,Blue,Cyan,Scroll bar icons) \
        S( D,Yellow,Blue,Static text) \
        S( E,Blue,Lightgray,Selected text) \
        \
        S( F,Black,Black,reserved) \
        \
        S_(10,Lightgray,Cyan,Frame disabled,Cyan Windows) \
        S(11,White,Cyan,Frame) \
        S(12,Lightgreen,Cyan,Frame icons) \
        S(13,Cyan,Blue,Scroll bar page) \
        S(14,Cyan,Blue,Scroll bar icons) \
        S(15,Yellow,Cyan,Static text) \
        S(16,Blue,Green,Selected text) \
        \
        S(17,Black,Black,reserved) \
        \
        S_(18,Lightgray,Black,Frame disabled,Gray Windows ) \
        S(19,White,Black,Frame) \
        S(1A,White,Black,Frame icons) \
        S(1B,Lightgray,Black,Scroll bar page) \
        S(1C,White,Black,Scroll bar icons) \
        S(1D,Lightgray,Black,Static text) \
        S(1E,Black,Lightgray,Selected text) \
        \
        S(1F,Black,Black,reserved) \
        \
        S_(20,Black,Lightgray,Frame disabled,Dialogs) \
        S(21,White,Lightgray,Frame) \
        S(22,Lightgreen,Lightgray,Frame icons) \
        S(23,Cyan,Blue,Scroll bar page) \
        S(24,Cyan,Blue,Scroll bar icons) \
        S(25,Black,Lightgray,Static text) \
        S(26,Black,Lightgray,Label normal) \
        S(27,White,Lightgray,Label selected) \
        S(28,Yellow,Lightgray,Label shortcut) \
        S(29,Black,Green,Button normal) \
        S(2A,Lightcyan,Green,Button default) \
        S(2B,White,Green,Button selected) \
        S(2C,Darkgray,Lightgray,Button disabled) \
        S(2D,Yellow,Green,Button shortcut) \
        S(2E,Black,Lightgray,Button shadow) \
        S(2F,Black,Cyan,Cluster normal) \
        S(30,White,Cyan,Cluster selected) \
        S(31,Yellow,Cyan,Cluster shortcut) \
        S(32,White,Blue,Input normal) \
        S(33,White,Green,Input selected) \
        S(34,Lightgreen,Blue,Input arrow) \
        S(35,Black,Green,History button) \
        S(36,Green,Lightgray,History sides) \
        S(37,Blue,Cyan,History bar page) \
        S(38,Blue,Cyan,History bar icons) \
        S(39,Black,Cyan,List normal) \
        S(3A,White,Green,List focused) \
        S(3B,Yellow,Cyan,List selected) \
        S(3C,Blue,Cyan,List divider) \
        S(3D,Cyan,Blue,Information pane) \
        \
        S(3E,Black,Black,reserved) \
        S(3F,Black,Black,reserved) \
        \
        S_(40,Lightgray,Blue,Frame disabled,Blue Editor) \
        S(41,White,Blue,Frame) \
        S(42,Lightgreen,Blue,Frame icons) \
        S(43,Blue,Cyan,Scroll bar page) \
        S(44,Blue,Cyan,Scroll bar icons) \
        S(45,Yellow,Blue,normal text) \
        S(46,Blue,Lightgray,marked text) \
        S(47,Lightgray,Blue,comment) \
        S(48,White,Blue,reserved word) \
        S(49,Yellow,Blue,identifier) \
        S(4A,White,Blue,symbol) \
        S(4B,Lightcyan,Blue,string) \
        S(4C,Lightmagenta,Blue,integer) \
        S(4D,Lightmagenta,Blue,float) \
        S(4E,Lightmagenta,Blue,octal) \
        S(4F,Lightmagenta,Blue,hex) \
        S(50,Lightcyan,Blue,character) \
        S(51,Lightgreen,Blue,preprocessor) \
        S(52,Black,Red,illegal char) \
        S(53,White,Blue,user defined words) \
        S(54,Black,Cyan,CPU line) \
        S(55,White,Red,Breakpoint) \
        S(56,White,Blue,symbol2) \
        S(57,Lightgray,Black,Cross cursor) \
        S(58,Black,Lightgray,editor statusline) \
        S(59,Lightred,Lightgray,parens matching) \
        S(5A,White,Magenta,rectangle block) \
        S(5B,Blue,Black,Odd tab) \
        S(5C,Lightblue,Black,Even tab) \
        \
        S(5D,Black,Black,reserved) \
        S(5E,Black,Black,reserved) \
        S(5F,Black,Black,reserved) \
        \
        S_(60,Lightgray,Blue,Frame disabled,Cyan Editor) \
        S(61,White,Blue,Frame) \
        S(62,Lightgreen,Blue,Frame icons) \
        S(63,Blue,Cyan,Scroll bar page) \
        S(64,Blue,Cyan,Scroll bar icons) \
        S(65,Yellow,Blue,normal text) \
        S(66,Blue,Lightgray,marked text) \
        S(67,Lightgray,Blue,comment) \
        S(68,White,Blue,reserved word) \
        S(69,Yellow,Blue,identifier) \
        S(6A,White,Blue,symbol) \
        S(6B,Lightcyan,Blue,string) \
        S(6C,Lightmagenta,Blue,integer) \
        S(6D,Lightmagenta,Blue,float) \
        S(6E,Lightmagenta,Blue,octal) \
        S(6F,Lightmagenta,Blue,hex) \
        S(70,Lightcyan,Blue,character) \
        S(71,Lightgreen,Blue,preprocessor) \
        S(72,Black,Red,illegal char) \
        S(73,White,Blue,user defined words) \
        S(74,Black,Cyan,CPU line) \
        S(75,White,Red,Breakpoint) \
        S(76,White,Blue,symbol2) \
        S(77,Lightgray,Black,Cross cursor) \
        S(78,Black,Lightgray,editor statusline) \
        S(79,Lightred,Lightgray,parens matching) \
        S(7A,White,Magenta,rectangle block) \
        S(7B,Blue,Black,Odd tab) \
        S(7C,Lightblue,Black,Even tab) \
        \
        S(7D,Black,Black,reserved) \
        S(7E,Black,Black,reserved) \
        S(7F,Black,Black,reserved) \
        \
        S_(80,Lightgray,Blue,Frame disabled,Gray Editor) \
        S(81,White,Blue,Frame) \
        S(82,Lightgreen,Blue,Frame icons) \
        S(83,Blue,Cyan,Scroll bar page) \
        S(84,Blue,Cyan,Scroll bar icons) \
        S(85,Yellow,Blue,normal text) \
        S(86,Blue,Lightgray,marked text) \
        S(87,Lightgray,Blue,comment) \
        S(88,White,Blue,reserved word) \
        S(89,Yellow,Blue,identifier) \
        S(8A,White,Blue,symbol) \
        S(8B,Lightcyan,Blue,string) \
        S(8C,Lightmagenta,Blue,integer) \
        S(8D,Lightmagenta,Blue,float) \
        S(8E,Lightmagenta,Blue,octal) \
        S(8F,Lightmagenta,Blue,hex) \
        S(90,Lightcyan,Blue,character) \
        S(91,Lightgreen,Blue,preprocessor) \
        S(92,Black,Red,illegal char) \
        S(93,White,Blue,user defined words) \
        S(94,Black,Cyan,CPU line) \
        S(95,White,Red,Breakpoint) \
        S(96,White,Blue,symbol2) \
        S(97,Lightgray,Black,Cross cursor) \
        S(98,Black,Lightgray,editor statusline) \
        S(99,Lightred,Lightgray,parens matching) \
        S(9A,White,Magenta,rectangle block) \
        S(9B,White,Black,Odd tab) \
        S(9C,Lightgray,Black,Even tab) \
        \
        S(9D,Black,Black,reserved) \
        S(9E,Black,Black,reserved) \
        S(9F,Black,Black,reserved) \
        \
        S_(A0,Lightgray,Cyan,Frame disabled,Info viewer) \
        S(A1,White,Cyan,Frame) \
        S(A2,Lightgreen,Cyan,Frame icons) \
        S(A3,Cyan,Blue,Scroll bar page) \
        S(A4,Cyan,Blue,Scroll bar icons) \
        S(A5,Black,Cyan,normal text) \
        S(A6,Yellow,Cyan,keyword) \
        S(A7,Yellow,Blue,selected keyword) \
        S(A8,Yellow,Red,marked keyword) \
        S(A9,Yellow,Green,marked text, currently fixed to "\0x70") \
        \
        S(AA,Black,Black,reserved) \
        S(AB,Black,Black,reserved) \
        S(AC,Black,Black,reserved) \
        S(AD,Black,Black,reserved) \
        S(AE,Black,Black,reserved) \
        S(AF,Black,Black,reserved) \
        \
        S_(B0,Black,Lightgray,Frame disabled,Disassembler) \
        S(B1,White,Lightgray,Frame) \
        S(B2,Lightgreen,Lightgray,Frame icons) \
        S(B3,Blue,Cyan,Scroll bar page) \
        S(B4,Blue,Cyan,Scroll bar icons) \
        S(B5,Black,Cyan,List normal) \
        S(B6,White,Green,List focused) \
        S(B7,Yellow,Cyan,List selected) \
        S(B8,Black,Magenta,CPU line) \
        S(B9,Black,Red,Breakpoint) \
        S(BA,Black,Cyan,Unchanged register) \
        S(BB,White,Cyan,Changed register) \
        S(BC,Yellow,Green,Focused register) \
        \
        S(BD,Black,Black,reserved) \
        S(BE,Black,Black,reserved) \
        S(BF,Black,Black,reserved) \
        \
        S_(C0,Black,Lightgray,Frame disabled,DataWindow) \
        S(C1,White,Lightgray,Frame) \
        S(C2,Lightgreen,Lightgray,Frame icons) \
        S(C3,Blue,Cyan,Scroll bar page) \
        S(C4,Blue,Cyan,Scroll bar icons) \
        S(C5,Black,Cyan,Normal Text(active)) \
        S(C6,White,Cyan,Normal Text(inactive)) \
        S(C7,White,Blue,Focused Text) \
        S(C8,Black,Magenta,Selected Text) \
        S(C9,Black,Red,reserved) \
        S(CA,Black,Cyan,reserved)

#define Normal 07
#define Light 0F
#define Inverse 70
#define Underline 01
#define Empty 00

#define __M(color) #color
#define _M(col) __M(\x##col)
#define M(index,color,name,comment...) _M(color)

/* This is the palette for the system when in monochrome mode */
#define cpIDEMono\
        M( 1,Normal,Color,Desktop) \
        \
        M( 2,Normal,normal text, Used by menus and statusline) \
        M( 3,Normal,disabled text) \
        M( 4,Underline,shortcut) \
        M( 5,Inverse,normal selected) \
        M( 6,Underline,disabled selected) \
        M( 7,Inverse,shortcut selected) \
        \
        M( 8,Normal,passive frame, Used by blue windows ) \
        M( 9,Light,active frame) \
        M( A,Normal,frame icons) \
        M( B,Normal,scrollbar) \
        M( C,Normal,scrollbar icons) \
        M( D,Normal,normal text) \
        M( E,Inverse,selected text) \
        \
        M( F,Empty,reserved) \
        \
        M(10,Normal,passive frame, Used by blue windows ) \
        M(11,Light,active frame) \
        M(12,Normal,frame icons) \
        M(13,Normal,scrollbar) \
        M(14,Normal,scrollbar icons) \
        M(15,Normal,normal text) \
        M(16,Inverse,selected text) \
        \
        M(17,Empty,reserved) \
        \
        M(18,Normal,passive frame, Used by blue windows ) \
        M(19,Light,active frame) \
        M(1A,Normal,frame icons) \
        M(1B,Normal,scrollbar) \
        M(1C,Normal,scrollbar icons) \
        M(1D,Normal,normal text) \
        M(1E,Inverse,selected text) \
        \
        M(1F,Empty,reserved) \
        \
        M(20,Normal,passive frame, Used by dialogs) \
        M(21,Light,active frame) \
        M(22,Normal,frame icons) \
        M(23,Normal,scrollbar) \
        M(24,Normal,scrollbar icons) \
        M(25,Normal,static text) \
        M(26,Normal,label normal) \
        M(27,Inverse,label selected) \
        M(28,Underline,label shortcut) \
        M(29,Inverse,button normal) \
        M(2A,Underline,button default) \
        M(2B,Inverse,button focused) \
        M(2C,Normal,button disabled) \
        M(2D,Normal,button shortcut) \
        M(2E,Normal,button shadow) \
        M(2F,Inverse,cluster normal) \
        M(30,Normal,cluster selected) \
        M(31,Light,cluster shortcut) \
        M(32,Underline,inputline normal) \
        M(33,Inverse,inputline selected) \
        M(34,Normal,inputline arrows) \
        M(35,Normal,history arrow) \
        M(36,Normal,history side) \
        M(37,Normal,history window scrollbar) \
        M(38,Normal,history window scrollbar icons) \
        M(39,Inverse,list normal) \
        M(3A,Normal,list focused) \
        M(3B,Underline,list selected) \
        M(3C,Normal,list divider) \
        M(3D,Normal,infopane) \
        \
        M(3E,Empty,reserved) \
        M(3F,Empty,reserved) \
        \
        M(40,Normal,passive frame, Used by blue editors) \
        M(41,Light,active frame) \
        M(42,Normal,frame icons) \
        M(43,Normal,scrollbar) \
        M(44,Normal,scrollbar icons) \
        M(45,Normal,normal text) \
        M(46,Inverse,marked text) \
        M(47,Light,comment) \
        M(48,Light,reserved word) \
        M(49,Normal,identifier) \
        M(4A,Light,symbol) \
        M(4B,Light,string) \
        M(4C,Light,integer decimal) \
        M(4D,Light,float) \
        M(4E,Light,octal) \
        M(4F,Light,hex) \
        M(50,Light,character) \
        M(51,Light,preprocessor) \
        M(52,Inverse,illegal character) \
        M(53,Light,user reserved word) \
        M(54,Underline,CPU line) \
        M(55,Inverse,breakpoint) \
        M(56,Light,symbol2) \
        M(57,Inverse,cross cursor) \
        M(58,Inverse,editor statusline) \
        M(59,Inverse,matching paranthesis) \
        M(5A,Inverse,rectangular blocks) \
        \
        M(5B,Empty,reserved) \
        M(5C,Empty,reserved) \
        M(5D,Empty,reserved) \
        M(5E,Empty,reserved) \
        M(5F,Empty,reserved) \
        \
        M(60,Normal,passive frame, Used by blue editors) \
        M(61,Light,active frame) \
        M(62,Normal,frame icons) \
        M(63,Normal,scrollbar) \
        M(64,Normal,scrollbar icons) \
        M(65,Normal,normal text) \
        M(66,Inverse,marked text) \
        M(67,Light,comment) \
        M(68,Light,reserved word) \
        M(69,Normal,identifier) \
        M(6A,Light,symbol) \
        M(6B,Light,string) \
        M(6C,Light,integer decimal) \
        M(6D,Light,float) \
        M(6E,Light,octal) \
        M(6F,Light,hex) \
        M(70,Light,character) \
        M(71,Light,preprocessor) \
        M(72,Inverse,illegal character) \
        M(73,Light,user reserved word) \
        M(74,Underline,CPU line) \
        M(75,Inverse,breakpoint) \
        M(76,Light,symbol2) \
        M(77,Inverse,cross cursor) \
        M(78,Inverse,editor statusline) \
        M(79,Inverse,matching paranthesis) \
        M(7A,Inverse,rectangular blocks) \
        \
        M(7B,Empty,reserved) \
        M(7C,Empty,reserved) \
        M(7D,Empty,reserved) \
        M(7E,Empty,reserved) \
        M(7F,Empty,reserved) \
        \
        M(80,Normal,passive frame, Used by blue editors) \
        M(81,Light,active frame) \
        M(82,Normal,frame icons) \
        M(83,Normal,scrollbar) \
        M(84,Normal,scrollbar icons) \
        M(85,Normal,normal text) \
        M(86,Inverse,marked text) \
        M(87,Light,comment) \
        M(88,Light,reserved word) \
        M(89,Normal,identifier) \
        M(8A,Light,symbol) \
        M(8B,Light,string) \
        M(8C,Light,integer decimal) \
        M(8D,Light,float) \
        M(8E,Light,octal) \
        M(8F,Light,hex) \
        M(90,Light,character) \
        M(91,Light,preprocessor) \
        M(92,Inverse,illegal character) \
        M(93,Light,user reserved word) \
        M(94,Underline,CPU line) \
        M(95,Inverse,breakpoint) \
        M(96,Light,symbol2) \
        M(97,Inverse,cross cursor) \
        M(98,Inverse,editor statusline) \
        M(99,Inverse,matching paranthesis) \
        M(9A,Inverse,rectangular blocks) \
        \
        M(9B,Empty,reserved) \
        M(9C,Empty,reserved) \
        M(9D,Empty,reserved) \
        M(9E,Empty,reserved) \
        M(9F,Empty,reserved) \
        \
        M(A0,Normal,passive frame, used by the help viewer) \
        M(A1,Light,active frame) \
        M(A2,Normal,Cyan,frame icons) \
        M(A3,Normal,scollbar) \
        M(A4,Normal,scrollbar icons) \
        M(A5,Normal,normal text) \
        M(A6,Underline,keyword) \
        M(A7,Inverse,selected keyword) \
        M(A8,Inverse,keyword in marked area) \
        M(A9,Inverse,marked text, currently fixed to "\0x70") \
        \
        M(AA,Empty,reserved) \
        M(AB,Empty,reserved) \
        M(AC,Empty,reserved) \
        M(AD,Empty,reserved) \
        M(AE,Empty,reserved) \
        M(AF,Empty,reserved) \
        \
        M(B0,Normal,Frame disabled, Disassembler) \
        M(B1,Light,Frame) \
        M(B2,Normal,Frame icons) \
        M(B3,Normal,Scroll bar page) \
        M(B4,Normal,Scroll bar icons) \
        M(B5,Inverse,List normal) \
        M(B6,Normal,List focused) \
        M(B7,Underline,List selected) \
        M(B8,Light,CPU line) \
        M(B9,Light,Breakpoint) \
        M(BA,Inverse,Unchanged register) \
        M(BB,Underline,Changed register) \
        M(BC,Normal,Focused register) \
        \
        M(BD,Empty,reserved) \
        M(BE,Empty,reserved) \
        M(BF,Empty,reserved)


#define cpIDEBlackWhite cpIDEColor
