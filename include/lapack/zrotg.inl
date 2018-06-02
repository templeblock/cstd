/* ../blas/src/zrotg.f -- translated by f2c (version 20061008).
   You must link the resulting object file with libf2c:
        on Microsoft Windows system, link with libf2c.lib;
        on Linux or Unix systems, link with .../path/to/libf2c.a -lm
        or, if you install libf2c.a in a standard place, with -lf2c -lm
        -- in that order, at the end of the command line, as in
                cc *.o -lf2c -lm
        Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

                http://www.netlib.org/f2c/libf2c.zip
*/



/* Subroutine */ static int zrotg_(dcomplex *ca, dcomplex *cb, doublereal *
        c__, dcomplex *s)
{
    /* System generated locals */
    doublereal d__1, d__2;
    dcomplex z__1, z__2, z__3, z__4;

    /* Builtin functions */
    double z_abs(dcomplex *);
    void z_div(dcomplex *, dcomplex *, dcomplex *);
    double sqrt(doublereal);
    void d_cnjg(dcomplex *, dcomplex *);

    /* Local variables */
    static doublereal norm;
    static dcomplex alpha;
    static doublereal scale;

    if (z_abs(ca) != 0.) {
        goto L10;
    }
    *c__ = 0.;
    s->r = 1., s->i = 0.;
    ca->r = cb->r, ca->i = cb->i;
    goto L20;
L10:
    scale = z_abs(ca) + z_abs(cb);
    z__2.r = scale, z__2.i = 0.;
    z_div(&z__1, ca, &z__2);
/* Computing 2nd power */
    d__1 = z_abs(&z__1);
    z__4.r = scale, z__4.i = 0.;
    z_div(&z__3, cb, &z__4);
/* Computing 2nd power */
    d__2 = z_abs(&z__3);
    norm = scale * sqrt(d__1 * d__1 + d__2 * d__2);
    d__1 = z_abs(ca);
    z__1.r = ca->r / d__1, z__1.i = ca->i / d__1;
    alpha.r = z__1.r, alpha.i = z__1.i;
    *c__ = z_abs(ca) / norm;
    d_cnjg(&z__3, cb);
    z__2.r = alpha.r * z__3.r - alpha.i * z__3.i, z__2.i = alpha.r * z__3.i + 
            alpha.i * z__3.r;
    z__1.r = z__2.r / norm, z__1.i = z__2.i / norm;
    s->r = z__1.r, s->i = z__1.i;
    z__1.r = norm * alpha.r, z__1.i = norm * alpha.i;
    ca->r = z__1.r, ca->i = z__1.i;
L20:
    return 0;
} /* zrotg_ */
