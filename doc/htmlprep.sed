# Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details 
# This file is part of RHIDE. 
s/comment --- Substitute @end xxx directives with @end_xxx macro calls ---/&/

s/@end table/@end_table/
s/@end itemize/@end_itemize/
s/@end enumerate/@end_enumerate/
s/@end example/@end_example/
s/@end smallexample/@end_smallexample/
s/@end display/@end_display/
s/@end quotation/@end_quotation/
s/@end menu/@end_menu/
s/@end format/@end_format/

s/comment --- Make @node lines call a macro ---/&/

s/@node\(.*\)/@node{\1}/

s/comment --- Convert menus to macro calls:            ---/&/
s/comment ---    @menu is left alone		       ---/&/
s/comment ---    menu items converted to }\n@mitem{... ---/&/
s/comment ---    @end menu converted to @end_menu      ---/&/

/^@menu/,/^@end_menu/ {
    s/comment --- The first menu item after @menu needs special    ---/&/
    s/comment --- treatment, because there is no @mitem{ before it ---/&/
    /^@menu/ {
        N
	s/\n/!!!!!/
    }
    s/^\* /@mitem{/
    s/,/\\,/g
    s/::/,/
    s/^[ 	]\+/&/
    s/^$//
    s/^\@\(mitem\|end_menu\)/}\
@\1/
    s/^\(@menu\)!!!!!\* /\1\
@mitem{/
}
