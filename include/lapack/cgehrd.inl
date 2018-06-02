/* ../../../dependencies/lapack/src/cgehrd.f -- translated by f2c (version 20061008).
   You must link the resulting object file with libf2c:
        on Microsoft Windows system, link with libf2c.lib;
        on Linux or Unix systems, link with .../path/to/libf2c.a -lm
        or, if you install libf2c.a in a standard place, with -lf2c -lm
        -- in that order, at the end of the command line, as in
                cc *.o -lf2c -lm
        Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

                http://www.netlib.org/f2c/libf2c.zip
*/




/* Subroutine */ static int cgehrd_(integer *n, integer *ilo, integer *ihi, scomplex *
        a, integer *lda, scomplex *tau, scomplex *work, integer *lwork, integer 
        *info)
{
/* Table of constant values */

static scomplex c_b2 = {1.f,0.f};
static integer c__1 = 1;
static integer c_n1 = -1;
static integer c__3 = 3;
static integer c__2 = 2;
static integer c__65 = 65;
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;
    scomplex q__1;

    /* Local variables */
    static integer i__;
    static scomplex t[4160]        /* was [65][64] */;
    static integer ib;
    static scomplex ei;
    static integer nb, nh, nx, iws;
     /* Subroutine */ int cgemm_(char *, char *, integer *, integer *, 
            integer *, scomplex *, scomplex *, integer *, scomplex *, integer *, 
            scomplex *, scomplex *, integer *, ftnlen, ftnlen);
    static integer nbmin, iinfo;
     /* Subroutine */ int cgehd2_(integer *, integer *, integer *, 
            scomplex *, integer *, scomplex *, scomplex *, integer *), clarfb_(
            char *, char *, char *, char *, integer *, integer *, integer *, 
            scomplex *, integer *, scomplex *, integer *, scomplex *, integer *, 
            scomplex *, integer *, ftnlen, ftnlen, ftnlen, ftnlen), clahrd_(
            integer *, integer *, integer *, scomplex *, integer *, scomplex *, 
            scomplex *, integer *, scomplex *, integer *), xerbla_(char *, 
            integer *, ftnlen);
     integer ilaenv_(integer *, char *, char *, integer *, integer *, 
            integer *, integer *, ftnlen, ftnlen);
    static integer ldwork, lwkopt;
    static logical lquery;


/*  -- LAPACK routine (version 3.0) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., */
/*     Courant Institute, Argonne National Lab, and Rice University */
/*     June 30, 1999 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CGEHRD reduces a scomplex general matrix A to upper Hessenberg form H */
/*  by a unitary similarity transformation:  Q' * A * Q = H . */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  ILO     (input) INTEGER */
/*  IHI     (input) INTEGER */
/*          It is assumed that A is already upper triangular in rows */
/*          and columns 1:ILO-1 and IHI+1:N. ILO and IHI are normally */
/*          set by a previous call to CGEBAL; otherwise they should be */
/*          set to 1 and N respectively. See Further Details. */
/*          1 <= ILO <= IHI <= N, if N > 0; ILO=1 and IHI=0, if N=0. */

/*  A       (input/output) COMPLEX array, dimension (LDA,N) */
/*          On entry, the N-by-N general matrix to be reduced. */
/*          On exit, the upper triangle and the first subdiagonal of A */
/*          are overwritten with the upper Hessenberg matrix H, and the */
/*          elements below the first subdiagonal, with the array TAU, */
/*          represent the unitary matrix Q as a product of elementary */
/*          reflectors. See Further Details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= MAX(1,N). */

/*  TAU     (output) COMPLEX array, dimension (N-1) */
/*          The scalar factors of the elementary reflectors (see Further */
/*          Details). Elements 1:ILO-1 and IHI:N-1 of TAU are set to */
/*          zero. */

/*  WORK    (workspace/output) COMPLEX array, dimension (LWORK) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  LWORK >= MAX(1,N). */
/*          For optimum performance LWORK >= N*NB, where NB is the */
/*          optimal blocksize. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  The matrix Q is represented as a product of (ihi-ilo) elementary */
/*  reflectors */

/*     Q = H(ilo) H(ilo+1) . . . H(ihi-1). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a scomplex scalar, and v is a scomplex vector with */
/*  v(1:i) = 0, v(i+1) = 1 and v(ihi+1:n) = 0; v(i+2:ihi) is stored on */
/*  exit in A(i+2:ihi,i), and tau in TAU(i). */

/*  The contents of A are illustrated by the following example, with */
/*  n = 7, ilo = 2 and ihi = 6: */

/*  on entry,                        on exit, */

/*  ( a   a   a   a   a   a   a )    (  a   a   h   h   h   h   a ) */
/*  (     a   a   a   a   a   a )    (      a   h   h   h   h   a ) */
/*  (     a   a   a   a   a   a )    (      h   h   h   h   h   h ) */
/*  (     a   a   a   a   a   a )    (      v2  h   h   h   h   h ) */
/*  (     a   a   a   a   a   a )    (      v2  v3  h   h   h   h ) */
/*  (     a   a   a   a   a   a )    (      v2  v3  v4  h   h   h ) */
/*  (                         a )    (                          a ) */

/*  where a denotes an element of the original matrix A, h denotes a */
/*  modified element of the upper Hessenberg matrix H, and vi denotes an */
/*  element of the vector defining H(i). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
/* Computing MIN */
    i__1 = 64, i__2 = ilaenv_(&c__1, "CGEHRD", " ", n, ilo, ihi, &c_n1, (
            ftnlen)6, (ftnlen)1);
    nb = MIN(i__1,i__2);
    lwkopt = *n * nb;
    work[1].r = (real) lwkopt, work[1].i = 0.f;
    lquery = *lwork == -1;
    if (*n < 0) {
        *info = -1;
    } else if (*ilo < 1 || *ilo > MAX(1,*n)) {
        *info = -2;
    } else if (*ihi < MIN(*ilo,*n) || *ihi > *n) {
        *info = -3;
    } else if (*lda < MAX(1,*n)) {
        *info = -5;
    } else if (*lwork < MAX(1,*n) && ! lquery) {
        *info = -8;
    }
    if (*info != 0) {
        i__1 = -(*info);
        xerbla_("CGEHRD", &i__1, (ftnlen)6);
        return 0;
    } else if (lquery) {
        return 0;
    }

/*     Set elements 1:ILO-1 and IHI:N-1 of TAU to zero */

    i__1 = *ilo - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
        i__2 = i__;
        tau[i__2].r = 0.f, tau[i__2].i = 0.f;
/* L10: */
    }
    i__1 = *n - 1;
    for (i__ = MAX(1,*ihi); i__ <= i__1; ++i__) {
        i__2 = i__;
        tau[i__2].r = 0.f, tau[i__2].i = 0.f;
/* L20: */
    }

/*     Quick return if possible */

    nh = *ihi - *ilo + 1;
    if (nh <= 1) {
        work[1].r = 1.f, work[1].i = 0.f;
        return 0;
    }

    nbmin = 2;
    iws = 1;
    if (nb > 1 && nb < nh) {

/*        Determine when to cross over from blocked to unblocked code */
/*        (last block is always handled by unblocked code). */

/* Computing MAX */
        i__1 = nb, i__2 = ilaenv_(&c__3, "CGEHRD", " ", n, ilo, ihi, &c_n1, (
                ftnlen)6, (ftnlen)1);
        nx = MAX(i__1,i__2);
        if (nx < nh) {

/*           Determine if workspace is large enough for blocked code. */

            iws = *n * nb;
            if (*lwork < iws) {

/*              Not enough workspace to use optimal NB:  determine the */
/*              minimum value of NB, and reduce NB or force use of */
/*              unblocked code. */

/* Computing MAX */
                i__1 = 2, i__2 = ilaenv_(&c__2, "CGEHRD", " ", n, ilo, ihi, &
                        c_n1, (ftnlen)6, (ftnlen)1);
                nbmin = MAX(i__1,i__2);
                if (*lwork >= *n * nbmin) {
                    nb = *lwork / *n;
                } else {
                    nb = 1;
                }
            }
        }
    }
    ldwork = *n;

    if (nb < nbmin || nb >= nh) {

/*        Use unblocked code below */

        i__ = *ilo;

    } else {

/*        Use blocked code */

        i__1 = *ihi - 1 - nx;
        i__2 = nb;
        for (i__ = *ilo; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
/* Computing MIN */
            i__3 = nb, i__4 = *ihi - i__;
            ib = MIN(i__3,i__4);

/*           Reduce columns i:i+ib-1 to Hessenberg form, returning the */
/*           matrices V and T of the block reflector H = I - V*T*V' */
/*           which performs the reduction, and also the matrix Y = A*V*T */

            clahrd_(ihi, &i__, &ib, &a[i__ * a_dim1 + 1], lda, &tau[i__], t, &
                    c__65, &work[1], &ldwork);

/*           Apply the block reflector H to A(1:ihi,i+ib:ihi) from the */
/*           right, computing  A := A - Y * V'. V(i+ib,ib-1) must be set */
/*           to 1. */

            i__3 = i__ + ib + (i__ + ib - 1) * a_dim1;
            ei.r = a[i__3].r, ei.i = a[i__3].i;
            i__3 = i__ + ib + (i__ + ib - 1) * a_dim1;
            a[i__3].r = 1.f, a[i__3].i = 0.f;
            i__3 = *ihi - i__ - ib + 1;
            q__1.r = -1.f, q__1.i = -0.f;
            cgemm_("No transpose", "Conjugate transpose", ihi, &i__3, &ib, &
                    q__1, &work[1], &ldwork, &a[i__ + ib + i__ * a_dim1], lda,
                     &c_b2, &a[(i__ + ib) * a_dim1 + 1], lda, (ftnlen)12, (
                    ftnlen)19);
            i__3 = i__ + ib + (i__ + ib - 1) * a_dim1;
            a[i__3].r = ei.r, a[i__3].i = ei.i;

/*           Apply the block reflector H to A(i+1:ihi,i+ib:n) from the */
/*           left */

            i__3 = *ihi - i__;
            i__4 = *n - i__ - ib + 1;
            clarfb_("Left", "Conjugate transpose", "Forward", "Columnwise", &
                    i__3, &i__4, &ib, &a[i__ + 1 + i__ * a_dim1], lda, t, &
                    c__65, &a[i__ + 1 + (i__ + ib) * a_dim1], lda, &work[1], &
                    ldwork, (ftnlen)4, (ftnlen)19, (ftnlen)7, (ftnlen)10);
/* L30: */
        }
    }

/*     Use unblocked code to reduce the rest of the matrix */

    cgehd2_(n, &i__, ihi, &a[a_offset], lda, &tau[1], &work[1], &iinfo);
    work[1].r = (real) iws, work[1].i = 0.f;

    return 0;

/*     End of CGEHRD */

} /* cgehrd_ */
