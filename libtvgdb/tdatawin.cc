/* DataWindow v0.02 */
/* Copyright (C) 1998 Laszlo Molnar */
/* This program is free software, see COPYING for details */

/* this stuff was inspired by data dump window in turbo debugger */

#define Uses_TListViewer
#define Uses_TScrollBar
#define Uses_TEvent
#define Uses_TPalette
#define Uses_TKeys
#define Uses_MsgBox
#define Uses_TIndicator

#define Uses_TDataWindow
#define Uses_tvgdbCommands
#include <libtvgdb.h>
#include <tvgdbhis.h>

#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <librhgdb.h>

#include <rhutils.h>
#include <rhgdbint.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*
 01 Normal text (active)
 02 Normal text (not active)
 03 Focused text
 04 Selected text
 05 Execution line
 06 Breakpoint
*/
#define cpDataViewer "\x06\x04\x00"

#define NONACCESS  0x1
#define EDITED     0x2

#define ADDRCHARS  10                   // "12345678: "

// a little indicator: show endian mode, radix etc.
class TDIndicator : public TIndicator
{
    char thestate[10];
public:
    enum IndiType {iChanged=1, iEndian, iRadix};
    TDIndicator(const TRect& bounds);
    virtual void draw();
    virtual void changeState(IndiType,int);
};

class TDataViewer : public TView
{
public:
    TDataViewer(const TRect& bounds, TScrollBar *aVScrollBar, const char *addr_txt);
    ~TDataViewer();
    virtual TPalette& getPalette() const;
    virtual void changeBounds(const TRect &bounds);
    virtual void setState(ushort aState, Boolean enable);
    virtual void draw();
    virtual void handleEvent(TEvent& event);
    void update(unsigned long address);
    void getLine(char *buf,char *cols,int row);
    void cursorHoriz(int);
    void adjustWindow();
    unsigned char *curs2memo();
        
    TDIndicator *indi;
    unsigned long mem_start;
    unsigned mem_len;
    unsigned char *memo;

    unsigned long orig_addr;
    char *orig_addr_txt;
    unsigned bytesperline;

    enum {dmBytes, dm2Bytes, dm4Bytes, dmChars, dmMAX};
    unsigned char dispmode;
    
    enum {rxHex, rxDec, rxMAX};
    unsigned char radix;
    unsigned char endian;

    static char targetEndian;
};

char TDataViewer::targetEndian = -1;

static int isvalid_address (char *taddr,unsigned long *addr)
{
    char buf[300], *p1;
    sprintf(buf,"x/1xb %s",taddr);
    reset_command++;
    Command(buf,0);
    reset_command--;

    if (gdb_output_buffer[0]=='0' && (p1=strchr(gdb_output_buffer,':'))!=NULL
        && strlen(p1)<ADDRCHARS)
    {
        sscanf(gdb_output_buffer,"%lx",addr);
        return 1;
    }
    return 0;
}

extern "C" int target_read_memory (unsigned long,char *,int);
extern "C" int target_write_memory (unsigned long,char *,int);
// the following 2 functions should not be in this file
static void readbytes(unsigned long addr,char *memo,unsigned len,unsigned clen)
{
    if (target_read_memory(addr,memo,len)==0)
        memset(memo+clen,0,len);
    else if (len > 1)
    {
        readbytes(addr,memo,len/2,clen);     // recursion rulez :-)
        readbytes(addr+len/2,memo+len/2,len-len/2,clen);
        // well, this sucks when there are too many unaccessible bytes
    }
    else
        memo[clen] = NONACCESS;
}

static void writebytes(unsigned long addr,char *memo, unsigned len, unsigned clen)
{
    unsigned ic,jc;
    
    for (ic = 0; ic < len && (memo[ic+clen] & NONACCESS); ic++)
        ;
    for (jc = ic; ic < len && !(memo[ic+clen] & NONACCESS); ic++)
        ;
    target_write_memory(addr+jc,memo+jc,ic-jc);
}

void TDataViewer::update(unsigned long addr)
{
    if (!memo)
    {
        mem_len = bytesperline*size.y;
        memo = (unsigned char*) malloc (mem_len*2);
    }
    
    mem_start = addr;
    readbytes(addr,(char*)memo,size.y*bytesperline,mem_len);
    drawView();
}

static const char * const notaccess = "----------- ";
static const unsigned char fieldlen[4][3] =
{
    {2,3,4}, {4,5,6}, {8,10,11}, {0,0,0},
};

static const char *fieldstr[4][2] =
{
    {"%02X ","%3u "}, {"%04X ","%5u "}, {"%08X ","%10u "}, {NULL,NULL},
};

static const unsigned char fieldbytes[4] = {1,2,4,1};

#ifdef __DJGPP__
#define toPrintable(uc) (uc)
#else
#define toPrintable(uc) ((uc)<0x7e && (uc)>=32 ? (uc) : '.')
#endif

void TDataViewer::getLine(char *buf,char *cols,int row)
{
    if (!memo)
    {
        *buf = 0;
        return;
    }
    
    unsigned ic;
    const unsigned bpl = bytesperline;
    unsigned char *mem = memo+row*bpl, *cmem=mem+mem_len, uc;
    unsigned short us;
    const unsigned fl = fieldlen[dispmode][radix]+1;
    const char *fs = fieldstr[dispmode][radix];
    const char *notacc = notaccess+12-fl;
    
    switch (dispmode)
    {
    case dmBytes:                       // 1-byte-length unsigned integers
        for (ic=0; ic<bpl; ic++, buf += fl, cols += fl)
        {
            if (cmem[ic] & EDITED)
                memset(cols, 1, fl-1);
            sprintf(buf,cmem[ic] & NONACCESS ? notacc : fs,mem[ic]);
        }
        if (radix==rxHex)
        {
            *buf++ = ' ';
            for (ic=0; ic<bpl; ic++)
            {
                cols++;
                if (cmem[ic] & EDITED)
                    *cols = 1;
                *buf++ = cmem[ic] & NONACCESS ? ' ' : toPrintable(mem[ic]);
            }
        }
        break;
    case dm2Bytes:                      // 2-byte-length unsigned integers
        for (ic=0; ic<bpl; ic+=2, buf += fl, cols += fl)
        {
            uc = cmem[ic]|cmem[ic+1];
            if (uc & EDITED)
                memset(cols, 1, fl-1);
            if (uc & NONACCESS)
            {
                memcpy(buf, notacc, fl);
                continue;
            }
            if (!endian)
                us = mem[ic] + mem[ic+1]*0x100;
            else
                us = mem[ic+1] + mem[ic]*0x100;
            sprintf(buf, fs, us);
        }
        break;
    case dm4Bytes:                      // 4-byte-length unsigned integers
        unsigned uw;
        for (ic=0; ic<bpl; ic+=4, buf += fl, cols += fl)
        {
            uc = cmem[ic]|cmem[ic+1]|cmem[ic+2]|cmem[ic+3];
            if (uc & EDITED)
                memset(cols, 1, fl-1);
            if (uc & NONACCESS)
            {
                memcpy(buf, notacc, fl);
                continue;
            }
            if (!endian)
                uw = mem[ic] + mem[ic+1]*0x100 + mem[ic+2]*0x10000
                    + mem[ic+3]*0x1000000;
            else
                uw = mem[ic+3] + mem[ic+2]*0x100 + mem[ic+1]*0x10000
                    + mem[ic]*0x1000000;
            sprintf(buf, fs, uw);
        }
        break;
    case dmChars:                       // characters only
        for (ic=0; ic<bpl; ic++, cols++)
        {
            if (cmem[ic] & EDITED)
                *cols = 1;
            *buf++ = (cmem[ic] & NONACCESS) ? ' ' : toPrintable(mem[ic]);
        }
        break;
    default:
        break;
    }
    *buf = 0;
}

void TDataViewer::draw()
{
    TDrawBuffer b;
    uchar normal_color = getColor(1);
    uchar changed_color = getColor(2);
    uchar focused_color = getColor(3);
    unsigned bpl = bytesperline;
    
    int ic,jc;
    char buf[/*size.x+1*/ 256],cols[256];
    for (ic=0; ic<size.y; ic++)
    {
        uchar color;
        memset(cols,0,size.x);
        sprintf(buf,"%08lX: ",ic*bpl+mem_start);
        getLine(buf+ADDRCHARS,cols,ic);

        color = normal_color;
        b.moveChar(0, ' ', color, size.x);
        b.moveCStr(0, buf, color);
        
        for (jc=ADDRCHARS; jc<size.x; jc++)
            if (cols[jc-ADDRCHARS])
                b.putAttribute(jc,changed_color);

        if (mem_start+ic*bpl<=orig_addr && mem_start+ic*bpl+bpl>orig_addr)
        {
            int fl,pos;
            fl = fieldlen[dispmode][radix];
            pos = ADDRCHARS + (orig_addr-mem_start-ic*bpl) / fieldbytes[dispmode] * (fl+1);
            if (dispmode==dmChars)
                fl=1;
            for (jc=0; jc<fl; jc++)
                b.putAttribute(pos+jc,focused_color);
        }

        writeLine(0, ic, size.x, 1, b);
    }
}

void TDataViewer::setState(ushort aState, Boolean enable)
{
    TView::setState(aState, enable);
    if (aState & sfFocused)
        drawView();
}

TPalette &TDataViewer::getPalette() const
{
    static TPalette pal( cpDataViewer, sizeof( cpDataViewer )-1 );
    return pal;
}

TDataViewer::TDataViewer(const TRect& bounds, TScrollBar *aVScrollBar, const char *taddr) :
    TView (bounds), mem_start(0), memo(0), bytesperline(16), dispmode(dmBytes), radix(rxHex)
{
    //helpCtx = hcDataViewer;
    if (targetEndian<0)
    {
        reset_command++;
        Command("show endian",0);
        reset_command--;
        if (strstr(gdb_output_buffer,"little"))
            targetEndian=0;
        else if (strstr(gdb_output_buffer,"big"))
            targetEndian=1;
        else
            targetEndian=0;
    }
    endian = targetEndian;
    orig_addr_txt=string_dup(taddr);
    isvalid_address(orig_addr_txt,&orig_addr);
    setCursor(ADDRCHARS,0);
    showCursor();
}

TDataViewer::~TDataViewer()
{
    string_free(orig_addr_txt);
    free(memo);
}

void TDataViewer::cursorHoriz(int delta)
{
    int cx = cursor.x - ADDRCHARS;
    unsigned fl = fieldlen[dispmode][radix];
    if (fl)
        cx = cx / (fl+1) * fl + (cx % (fl+1));
    if (delta > 0)
        cx = min(cx+delta, fl ? fl*bytesperline/fieldbytes[dispmode]-1 : bytesperline-1);
    else
        cx = max(cx+delta, 0);
    if (fl)
        cx = cx / fl * (fl+1) + (cx % fl);
    setCursor(ADDRCHARS + cx,cursor.y);
}

void TDataViewer::adjustWindow()
{
    unsigned xnew;
    xnew = bytesperline/fieldbytes[dispmode]*(1+fieldlen[dispmode][radix]);
    if (dispmode == dmBytes && radix == rxHex)
        xnew += bytesperline+2;
    if (dispmode == dmChars)
        xnew = bytesperline+1;
    owner->growTo(xnew+3+ADDRCHARS,owner->size.y);
}

unsigned char * TDataViewer::curs2memo()
{
    return memo + bytesperline * cursor.y
        + (cursor.x-ADDRCHARS) / (fieldlen[dispmode][radix]+1)
        * fieldbytes[dispmode];
}

static void target_fill_mem (unsigned long from, unsigned long len, unsigned value)
{
    // FIXME: handle smaller chunks instead
    char *mem = (char*)malloc(len);
    memset(mem,value,len);
    target_write_memory(from,mem,len);
    free(mem);
}

static void target_move_mem(unsigned long from,unsigned long to,unsigned len)
{
    // FIXME: handle smaller chunks instead
    char *mem = (char*)malloc(len);
    target_read_memory(from,mem,len);
    target_write_memory(to,mem,len);
    free(mem);
}

static int getParams (char *buf, unsigned long *l1, unsigned long *l2, unsigned long *l3)
{
    char *tp,*p1=buf;
    *l1 = strtoul(buf,&tp,0);
    if (tp == buf)
        return 1;
    while (*tp && *tp != ',')
        tp++;
    if (*tp)
        p1 = ++tp;
    *l2 = strtoul(p1,&tp,0);
    if (tp == p1)
        return 1;
    if (l3)
    {
        while (*tp && *tp != ',')
            tp++;
        if (*tp)
            p1 = ++tp;
        *l3 = strtoul(p1 = tp,&tp,0);
        if (tp == p1)
            return 1;
    }
    return 0;
}

void TDataViewer::handleEvent (TEvent& event)
{
    char buf[256];
    TView::handleEvent(event);

    unsigned long newaddr = mem_start;
    
    if (event.what == evMouseDown)
    {
        clearEvent( event );
    }
    else if (event.what == evKeyDown)
    {
        switch (event.keyDown.keyCode)
        {
        case kbUp:
            if (cursor.y == 0)
                newaddr = mem_start - bytesperline;
            else
                setCursor(cursor.x,cursor.y-1);
            break;
        case kbDown:
            if (cursor.y == size.y - 1)
                newaddr = mem_start + bytesperline;
            else
                setCursor(cursor.x,cursor.y+1);
            break;
        case kbRight:
            cursorHoriz(1);
            break;
        case kbCtrlRight:
            newaddr = mem_start + 1;
            break;
        case kbLeft:
            cursorHoriz(-1);
            break;
        case kbCtrlLeft:
            newaddr = mem_start - 1;
            break;
        case kbPgDn:
            newaddr = mem_start + size.y * bytesperline;
            break;
        case kbPgUp:
            newaddr = mem_start - size.y * bytesperline;
            break;
        case kbHome:
            setCursor(ADDRCHARS,cursor.y);
            break;
        case kbCtrlHome:
            setCursor(cursor.x,0);
            break;
#if 0
        case kbxxxx: // go to the current "orig_addr" address
            newaddr = orig_addr;
            setCursor(ADDRCHARS,0);
            break;
#endif
        case kbCtrlH: // reevalute the original address then go to there
            if (isvalid_address(orig_addr_txt,&newaddr))
            {
                orig_addr = newaddr;
                setCursor(ADDRCHARS,0);
            }
            else
            {
                clearEvent(event);
                messageBox(gdb_output_buffer,mfError|mfOKButton);
                return;
            }
            break;
        case kbCtrlEnd:
            setCursor(cursor.x,size.y-1);
            break;
        case kbEnd:
            cursorHoriz(size.x);
            break;
#ifdef __i386__
        case kbCtrlT:
            strcpy(buf,"$esp");
            goto checkaddress;
#endif
        case kbCtrlF: // follow pointer
            sprintf(buf,"%#lx", curs2memo() - memo + mem_start);
            goto checkaddress;
        case kbCtrlG: // goto to a new address
            strcpy(buf, orig_addr_txt);
            if (HistinputBox(_("Data window"),_("~E~xpression"),buf,255,tvgdb_History_Data_Entry) == cmOK)
            {
checkaddress:
                if (isvalid_address(buf,&newaddr))
                {
                    orig_addr = newaddr;
                    setCursor(ADDRCHARS,0);
                }
                else
                {
                    clearEvent(event);
                    messageBox(gdb_output_buffer,mfError|mfOKButton);
                    return;
                }
            }
            break;
        case kbCtrlD: // change display mode
            dispmode = (dispmode + 1) % dmMAX;
            bytesperline &= ~(fieldbytes[dispmode] - 1);
            update(mem_start);
            adjustWindow();
            setCursor(ADDRCHARS, cursor.y);
            break;
        case kbCtrlE: // change endianness
            endian ^= 1;
            indi->changeState(TDIndicator::iEndian, "eE"[endian]);
            if (fieldbytes[dispmode] > 1)
                update(mem_start);
            break;
        case kbCtrlX: // change radix
            radix = (radix + 1) % rxMAX;
            indi->changeState(TDIndicator::iRadix, "XD"[radix]);
            update(mem_start);
            adjustWindow();
            setCursor(ADDRCHARS, cursor.y);
            break;
        case kbGrayMinus: // decrease bytes/line
            if (bytesperline > fieldbytes[dispmode])
            {
                bytesperline -= fieldbytes[dispmode];
                update(mem_start);
                adjustWindow();
                setCursor(ADDRCHARS, cursor.y);
            }
            break;
        case kbGrayPlus: // increase bytes/line
            bytesperline += fieldbytes[dispmode];
            free(memo);
            memo = NULL;
            update(mem_start);
            adjustWindow();
            break;
        case kbEnter: // update changes
            indi->changeState(TDIndicator::iChanged, ' ');
            writebytes(mem_start, (char*)memo, bytesperline * size.y, mem_len);
            post_command_hook();
            break;
        case kbCtrlL: // clear
            sprintf(buf,"%#lx,", curs2memo() - memo + mem_start);
            if (HistinputBox(_("Clear Block"),_("~F~rom, length"),buf,255,tvgdb_History_Data_Entry) == cmOK)
            {
                unsigned long from,len;
                if (getParams(buf,&from,&len,NULL) || len > 0x100000)
                {
                    messageBox(_("Parameter Error!"),mfError|mfOKButton);
                    break;
                }
                target_fill_mem(from,len,0);
                post_command_hook();
            }
            break;
        case kbCtrlI: // fill
            sprintf(buf,"%#lx,,", curs2memo() - memo + mem_start);
            if (HistinputBox(_("Fill Block"),_("~F~rom, length, value"),buf,255,tvgdb_History_Data_Entry) == cmOK)
            {
                unsigned long from,len,value;
                if (getParams(buf,&from,&len,&value) || len > 0x100000)
                {
                    messageBox(_("Parameter Error!"),mfError|mfOKButton);
                    break;
                }
                target_fill_mem(from,len,value);
                post_command_hook();
            }
            break;
        case kbCtrlR: // read
            break;
        case kbCtrlW: // write
            break;
        case kbCtrlM: // move
            sprintf(buf,"%#lx,,", curs2memo() - memo + mem_start);
            if (HistinputBox(_("Move Block"),_("~F~rom, to, length"),buf,255,tvgdb_History_Data_Entry) == cmOK)
            {
                unsigned long from,to,len;
                if (getParams(buf,&from,&to,&len) || len > 0x100000)
                {
                    messageBox(_("Parameter Error!"),mfError|mfOKButton);
                    break;
                }
                target_move_mem(from,to,len);
                post_command_hook();
            }
            break;
        default:
            unsigned kc = event.keyDown.charScan.charCode;
            //fprintf(stderr,"%c",kc);
            if (dispmode == dmChars) // characters only
            {
                memo[cursor.y*bytesperline+cursor.x-ADDRCHARS] = kc;
                memo[cursor.y*bytesperline+cursor.x-ADDRCHARS+mem_len] |= EDITED; // mark changed
                indi->changeState(TDIndicator::iChanged, '*');
                cursorHoriz(1);
                drawView();
            }
            else if ((kc >= '0' && kc <= '9')
                     || (radix == rxHex && (kc|0x20) >= 'a' && (kc|0x20) <= 'f'))
            {
                getLine(buf, buf, cursor.y);
                buf[cursor.x-ADDRCHARS] = kc;
                for (kc = cursor.x-ADDRCHARS; kc && buf[kc] != ' '; kc--)
                    ;
                if (!sscanf(buf+kc, fieldstr[dispmode][radix], &kc))
                    break;
                unsigned char *mem = curs2memo();
                switch (dispmode)
                {
                case dmBytes:
                    *mem = kc;
                    break;
                case dm2Bytes:
                    mem[endian] = kc;
                    mem[1-endian] = kc >> 8;
                    break;
                case dm4Bytes:
                    mem[3*endian] = kc;
                    mem[1+endian] = kc>>8;
                    mem[2-endian] = kc>>16;
                    mem[3-3*endian] = kc>>24;
                    break;
                default:
                    break;
                }
                for (kc = 0; kc < fieldbytes[dispmode]; kc++)
                    mem[mem_len + kc] |= EDITED;
                indi->changeState(TDIndicator::iChanged, '*');
                cursorHoriz(1);
                drawView();
            }
            break;
        }
        if (newaddr != mem_start)
            update(newaddr);
        clearEvent(event);
    }
    else if( event.what == evBroadcast )
    {/*
        if( (options & ofSelectable) != 0 )
        {
            if( event.message.command == cmScrollBarClicked &&
                ( event.message.infoPtr == hScrollBar || 
                  event.message.infoPtr == vScrollBar ) )
                select();
            else if( event.message.command == cmScrollBarChanged )
            {
                if( vScrollBar == event.message.infoPtr )
                {
                    focusItemNum( vScrollBar->value );
                    drawView();
                }
                else if( hScrollBar == event.message.infoPtr )
                    drawView();
            }
        }*/
    }
}

void TDataViewer::changeBounds(const TRect &bounds)
{
    if (size.y < bounds.b.y-bounds.a.y)
    {
        free(memo);
        memo = NULL;
    }
    setBounds(bounds);
    update(mem_start);
    drawView();
}

/*
 01 Frame disabled
 02 Frame
 03 Frame icons
 04 Scroll bar page
 05 Scroll bar icons
 06-0A TDataViewer
 0B-0D TRegisters
*/

#define cpDataWindow "\xB0\xB1\xB2\xB3\xB4\xB5\xB6\xB7"\
                     "\xB8\xB9\xBA\xBB\xBC\xBD\xBE\xBF"

TPalette &TDataWindow::getPalette() const
{
    static TPalette pal (cpDataWindow,sizeof (cpDataWindow)-1);
    return pal;
}

TDataWindow *TDataWindow::windowlist = NULL;

TDataWindow::TDataWindow(const TRect &bounds, const char *aTitle) :
    TDialog(bounds, aTitle),
    TWindowInit(TDataWindow::initFrame)
{
    TScrollBar *vs;
    TRect r = getExtent();
    r.grow(-1,-1);
    
    vs = new TScrollBar(TRect(r.b.x-1,r.a.y,r.b.x,r.b.y));
    insert(vs);

    r.b.x--;
    viewer = new TDataViewer(r, vs, aTitle);
    insert(viewer);
    viewer->growMode = gfGrowHiX | gfGrowHiY;
    growMode = gfGrowLoY | gfGrowHiX | gfGrowHiY;
    flags |= wfGrow|wfZoom;

    viewer->indi = new TDIndicator(TRect(2,size.y-1,8,size.y));
    insert(viewer->indi);
    
    viewer->select();

    // add window to the linked list
    next = windowlist;
    windowlist = this;
}

TDataWindow::~TDataWindow()
{
    // remove window from the linked list
    TDataWindow **wp;
    for (wp = &windowlist; *wp != this; wp = &((*wp)->next))
        ;
    *wp = (*wp)->next;
}

void TDataWindow::updateAll()
{
    for (TDataWindow *wp = windowlist; wp; wp = wp->next)
        wp->viewer->update(wp->viewer->mem_start);
}

void TDataWindow::sizeLimits(TPoint& min, TPoint& max)
{
    TWindow::sizeLimits(min, max);
}

TDataWindow *TDataWindow::createNew()
{
    char buf[256];
    buf[0] = 0;
    if (HistinputBox(_("Data window"),_("~E~xpression"),buf,255,tvgdb_History_Data_Entry) == cmOK)
    {
        unsigned long addr;
        if (isvalid_address(buf,&addr))
        {
            TRect r(0,5,79,20);
            TDataWindow *dw=new TDataWindow(r,buf);
            dw->viewer->update(addr);
            return dw;
        }
        messageBox(gdb_output_buffer,mfError|mfOKButton);
    }
    return NULL;
}

TDIndicator::TDIndicator (const TRect& bounds) :
    TIndicator (bounds)
{
    strcpy(thestate,"  eX ");
}

void TDIndicator::draw()
{
    uchar color, frame;
    TDrawBuffer b;
    
    if ((state & sfDragging) == 0)
    {
        color = getColor(1);
        frame = dragFrame;
    }
    else
    {
        color = getColor(2);
        frame = normalFrame;
    }
    
    b.moveChar(0,frame,color,size.x);
    b.moveCStr(0,thestate,color);
    writeBuf(0,0,size.x,1,b);
}

void TDIndicator::changeState (IndiType snum,int value)
{
    thestate[snum] = value;
    drawView();
}


/*
 todo:
 -----
 - block(+clear ^L,+fill ^I,-read ^R,-write ^W,-move ^M)
 + goto   ^G
 + follow ^F
 + display(byte->half word->word->chars->float?->double?) ^D
 + radix 16-10?  ^X
 + endian? ^E
 + edit
 - search(-next,-prev) ^S ^N ^P
 + reevaluate & goto  ^H
 + bytesperline increase Gray+, decrease Gray-
 + auto adjust window size
 - bookmarks?
 - data breakpoints
 + goto stack  ^T

 - add mouse & scrollbar handling
 + remove magic constants
 / cleanup the code
 - comments
 - try to fix 32-bit dependencies
 
 - disassembler window: support intel syntax too
 */

