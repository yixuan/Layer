AC_INIT([Layer], [0.1], [yixuan.qiu@cos.name])

AC_CHECK_PROGS([PKG_CONFIG], [pkg-config])   
if test -n "${PKG_CONFIG}"; then
	if(${PKG_CONFIG} --exists gtk+-2.0); then
		GTK_CPPFLAGS=`${PKG_CONFIG} --cflags gtk+-2.0`
		GTK_LIBS=`${PKG_CONFIG} --libs gtk+-2.0`
    else
		echo "ERROR: gtk+-2.0 not found by pkg-config."
		exit 1
    fi
	if(${PKG_CONFIG} --exists freetype2); then
		FT_CPPFLAGS=`${PKG_CONFIG} --cflags freetype2`
		FT_LIBS=`${PKG_CONFIG} --libs freetype2`
    else
		echo "ERROR: freetype2 not found by pkg-config."
		exit 1
    fi
else
    echo "ERROR: Cannot find pkg-config."
    exit 1
fi

PKG_CPPFLAGS="${GTK_CPPFLAGS} ${FT_CPPFLAGS}"
PKG_LIBS="${GTK_LIBS} ${FT_LIBS}"

AC_SUBST(PKG_CPPFLAGS)
AC_SUBST(PKG_LIBS)
AC_OUTPUT(src/Makevars)

