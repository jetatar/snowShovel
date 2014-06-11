#!/bin/sh
# $Id: getver.sh 4065 2007-06-05 13:23:11Z maartenb $

ret=Unknown

cd $ROOTSYS/tam

if [ -e version ]; then
    ret=`cat version`;
elif [ -d .svn ]; then
    if [ -e .svn/all-wcprops ]; then
        sta=`egrep -x '/svn/!svn/ver/.*/tam' .svn/all-wcprops`;
    elif [ -e .svn/dir-wcprops ]; then
        sta=`egrep -x '/svn/!svn/ver/.*/tam' .svn/dir-wcprops`;
    fi
    if [ -n "$sta" ]; then
        stb=`dirname $sta`;
        if [ -n "$stb" ]; then
            stc=`basename $stb`;
            if [ -n "$stc" ]; then
                ret=$stc;
            fi
        fi
    fi
fi

echo $ret
