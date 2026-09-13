package OpenSSL::safe::installdata;

use strict;
use warnings;
use Exporter;
our @ISA = qw(Exporter);
our @EXPORT = qw(
    @PREFIX
    @libdir
    @BINDIR @BINDIR_REL_PREFIX
    @LIBDIR @LIBDIR_REL_PREFIX
    @INCLUDEDIR @INCLUDEDIR_REL_PREFIX
    @APPLINKDIR @APPLINKDIR_REL_PREFIX
    @ENGINESDIR @ENGINESDIR_REL_LIBDIR
    @MODULESDIR @MODULESDIR_REL_LIBDIR
    @PKGCONFIGDIR @PKGCONFIGDIR_REL_LIBDIR
    @CMAKECONFIGDIR @CMAKECONFIGDIR_REL_LIBDIR
    $VERSION @LDLIBS
);

our @PREFIX                     = ( 'D:\SandBox\TestOpenSSL\SecondLayer\ThirdLayer\Debug.x64\OpenSSL' );
our @libdir                     = ( 'D:\SandBox\TestOpenSSL\SecondLayer\ThirdLayer\Debug.x64\SSLlibs' );
our @BINDIR                     = ( 'D:\SandBox\TestOpenSSL\SecondLayer\ThirdLayer\Debug.x64\OpenSSL\bin' );
our @BINDIR_REL_PREFIX          = ( 'bin' );
our @LIBDIR                     = ( 'D:\SandBox\TestOpenSSL\SecondLayer\ThirdLayer\Debug.x64\SSLlibs' );
our @LIBDIR_REL_PREFIX          = ( '..\SSLlibs' );
our @INCLUDEDIR                 = ( 'D:\SandBox\TestOpenSSL\SecondLayer\ThirdLayer\Debug.x64\OpenSSL\include' );
our @INCLUDEDIR_REL_PREFIX      = ( 'include' );
our @APPLINKDIR                 = ( 'D:\SandBox\TestOpenSSL\SecondLayer\ThirdLayer\Debug.x64\OpenSSL\include\openssl' );
our @APPLINKDIR_REL_PREFIX      = ( 'include/openssl' );
our @ENGINESDIR                 = ( 'D:\SandBox\TestOpenSSL\SecondLayer\ThirdLayer\Debug.x64\SSLlibs\engines-3' );
our @ENGINESDIR_REL_LIBDIR      = ( 'engines-3' );
our @MODULESDIR                 = ( 'D:\SandBox\TestOpenSSL\SecondLayer\ThirdLayer\Debug.x64\SSLlibs\ossl-modules' );
our @MODULESDIR_REL_LIBDIR      = ( 'ossl-modules' );
our @PKGCONFIGDIR               = ( 'D:\SandBox\TestOpenSSL\SecondLayer\ThirdLayer\Debug.x64\SSLlibs' );
our @PKGCONFIGDIR_REL_LIBDIR    = ( '' );
our @CMAKECONFIGDIR             = ( 'D:\SandBox\TestOpenSSL\SecondLayer\ThirdLayer\Debug.x64\OpenSSL\..\SSLlibs\cmake\OpenSSL' );
our @CMAKECONFIGDIR_REL_LIBDIR  = ( 'cmake\OpenSSL' );
our $VERSION                    = '3.6.1';
our @LDLIBS                     =
    # Unix and Windows use space separation, VMS uses comma separation
    $^O eq 'VMS'
    ? split(/ *, */, 'ws2_32.lib gdi32.lib advapi32.lib crypt32.lib user32.lib ')
    : split(/ +/, 'ws2_32.lib gdi32.lib advapi32.lib crypt32.lib user32.lib ');

1;
