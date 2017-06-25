/** \file SpecialPoly.hh
\brief Header file for special polynomials in dmtpc::math
\author Jeremy Lopez
*/
#ifndef DMTPC_SPECIALPOLY_HH
#define DMTPC_SPECIALPOLY_HH
namespace dmtpc{
  namespace math{

    /** Calculates Legendre polynomials using the recursion formula
        \f[ (n+1)P_{n+1}(x) = (2n+1)xP_n(x)-nP_{n-1}(x) \f]
        where \f$P_0(x) = 1\f$ and \f$P_1(x) = x\f$.
        \param x the argument, -1 to 1
        \param n the nth polynomial
    */
    double legendrePoly(double x, int n);
    /** Calculate Legendre polynomial values up to a max index.
        \param x the argument, -1 to 1
        \param nmax the max index
        \param out the output array. Size must be nmax+1.
    */
    void legendrePoly(double x,int nmax,double* out);
    /** The L<sup>2</sup> inner product for Legendre polynomials,
        \f[ \int\limits_{-1}^{1}P_m(x)P_n(x)dx = \frac{2}{2n+1}\delta_{mn} \f]
        \param n1 the index of one polynomial
        \param n2 the index of the other polynomial
    */
    double legendreNorm(int n1, int n2);
    /** Associated Legendre Polynomial. Calculates using the recursion formula
        \f[ -(l+m+1)(l+m)P_{l}^{m}(x) = 
             P_l^{m+2} + 2(m+1)\frac{x}{\sqrt{1-x^2}}P_l^{m+1}.\f]
        If \f$m=0\f$, the Legendre polynomial is used. Otherwise
        if \f$x=0\f$ returns 0. This makes use of the formulas
        \f{eqnarray*}{
          P_l^l(x) &=& (-1)^l(2l-1)!!(1-x^2)^{l/2}\\
          P_{l+1}^{l+1}(x) &=& -(2l+1)\sqrt{1-x^2}P_l^l(x)\\
          P_{l+1}^l(x) &=& x(2l+1)P_l^l(x)\\
          P_l^{-m} &=& (-1)^m \frac{(l-m)!}{(l+m)!}P_l^m
        \f}
        \param x the argument, -1 to 1
        \param l the angular momentum
        \param m the z-component of the angular momentum
    */
    double assocLegendrePoly(double x, int l, int m);
    /** Calculates the normalized associated Legendre polynomial
        \f[ Y_l^m(\theta) = \sqrt{\frac{(2l+1)}{4\pi}\frac{(l-m)!}{(l+m)!}}P_l^m(\cos\theta)\f]
        \param theta the polar angle
        \param l the angular momentum
        \param m the z-component of the angular momentum
    */
    double sphericalHarmonicLegendre(double theta, int l, int m);
    /** Calculates the odd-symmetry real spherical harmonic
        \f[ Y_l^m(\theta,\phi) = \frac{1}{\pi}\sqrt{\frac{(2l+1)}{4\pi}\frac{(l-m)!}{(l+m)!}}P_l^m(\cos\theta)\sin m\phi\f]
        Note that the use of sine and cosine removes the sign of \f$m\f$, 
        so it should be positive. These definitions differ from the traditional complex spherical harmonics by a factor of \f$\pi^{-1}\f$.
        \param theta the polar angle
        \param phi the azimuthal angle
        \param l the angular momentum
        \param m the z-component of the angular momentum
    */
    double sphericalHarmonicSin(double theta,double phi,int l, int m);
    /** Calculates the even-symmetry real spherical harmonic. 
        \f[ Y_l^m(\theta,\phi) = \frac{1}{\pi}\sqrt{\frac{(2l+1)}{4\pi}\frac{(l-m)!}{(l+m)!}}P_l^m(\cos\theta)\cos m\phi\f]
        Note that the use of sine and cosine removes the sign of \f$m\f$, 
        so it should be positive.
        \param theta the polar angle
        \param phi the azimuthal angle
        \param l the angular momentum
        \param m the z-component of the angular momentum
    */
    double sphericalHarmonicCos(double theta,double phi,int l, int m);
    /** Calculates the L<sup>2</sup> inner product for the spherical harmonics given here,
        \f[ \int d\Omega Y^m_l(\theta,\phi)Y_{m'}^{l'}(\theta,\phi = \delta_{mm'}\delta_{ll'} \f]
        Note that this does not directly distinguish the even and odd harmonics. Use the signs of the
        z-components of the angular momenta to indicate same symmetry (same sign) or opposite symmetry (different signs).
        This works for the complex spherical harmonics as well.
        \param l1 the angular momentum of the first spherical harmonic
        \param l2 the angular momentum of the second
        \param m1 the z-component for the first spherical harmonic
        \param m2 the z-component for the second
    */
    double sphericalHarmonicNorm(int l1, int l2, int m1, int m2);
    /** Calculates the n<sup>th</sup> Hermite polynomial using the recursion formula 
        \f[H_{n+1}(x) = 2xH_n(x) - 2nH_{n-1}(x) \f]
        where \f$H_0(x) = 1\f$ and \f$H_1(x) = 2x\f$.
        \param x the argument, any real number
        \param n the index
    */
    double hermitePoly(double x, int n);
    /** Calculate Hermite polynomial values up to a max index.
        \param x the argument, a real number
        \param nmax the max index
        \param out the output array. Size must be nmax+1.
    */
    void hermitePoly(double x, int nmax, double* out);
    /** The L<sup>2</sup> inner product for Hermite polynomials,
        \f[ \int\limits_{-\infty}^{\infty}H_m(x)H_n(x)e^{-x^2}dx
            = \sqrt{\pi}2^n n!\delta_{nm}\f]
        \param n1 the index of one polynomial
        \param n2 the index of the other polynomial
    */
    double hermiteNorm(int n1, int n2);
    /** Calculates Laguerre polynomials using the recursion formula
        \f[ (n+1)L_{n+1}(x) = (2n+1-x)L_n(x)-nL_{n-1}(x) \f]
        where \f$L_0(x) = 1\f$ and \f$L_1(x) =1- x\f$.
        \param x the argument, -1 to 1
        \param n the nth polynomial
    */
    double laguerrePoly(double x, int n);
    /** Calculate Laguerre polynomial values up to a max index.
        \param x the argument, a nonnegative real numbr
        \param nmax the max index
        \param out the output array. Size must be nmax+1.
    */
    void laguerrePoly(double x, int nmax, double* out);
    /** The L<sup>2</sup> inner product for Laguerre polynomials,
        \f[ \int\limits_{0}^{\infty}L_m(x)L_n(x)e^{-x}dx
            = \delta_{nm}\f]
        \param n1 the index of one polynomial
        \param n2 the index of the other polynomial
    */
    double laguerreNorm(int n1, int n2);
    /** Calculates Chebyshev polynomials of the first kind using 
        the recursion formula
        \f[ T_{n+1}(x) = 2xT_n(x)-T_{n-1}(x) \f]
        where \f$T_0(x) = 1\f$ and \f$T_1(x) = x\f$.
        \param x the argument, -1 to 1
        \param n the nth polynomial
    */
    double chebyshevFirstPoly(double x, int n);
    /** Calculate the Chebyshev polynomials of the first kind 
        up to a max index.
        \param x the argument, -1 to 1
        \param nmax the max index
        \param out the output array. Size must be nmax+1.
    */
    void chebyshevFirstPoly(double x, int nmax, double* out);
    /** The L<sup>2</sup> inner product for Chebyshev polynomials 
        of the first kind,
        \f[ \int\limits_{-1}^{1}T_m(x)T_n(x)\frac{dx}{\sqrt{1-x^2}}
            = \delta_{nm}\left(1+\delta_{n0}\right)\frac{\pi}{2}\f]
        \param n1 the index of one polynomial
        \param n2 the index of the other polynomial
    */
    double chebyshevFirstNorm(int n1, int n2);
    /** Calculates Chebyshev polynomials of the second kind using 
        the recursion formula
        \f[ U_{n+1}(x) = 2xU_n(x)-U_{n-1}(x) \f]
        where \f$U_0(x) = 1\f$ and \f$U_1(x) = 2x\f$.
        \param x the argument, -1 to 1
        \param n the nth polynomial
    */
    double chebyshevSecondPoly(double x, int n);
    /** Calculate the Chebyshev polynomials of the second kind 
        up to a max index.
        \param x the argument, -1 to 1
        \param nmax the max index
        \param out the output array. Size must be nmax+1.
    */
    void chebyshevSecondPoly(double x, int nmax, double* out);
    /** The L<sup>2</sup> inner product for Chebyshev polynomials 
        of the second kind,
        \f[ \int\limits_{-1}^{1}U_m(x)U_n(x)\sqrt{1-x^2}dx
            = \delta_{nm}\frac{\pi}{2}\f]
        \param n1 the index of one polynomial
        \param n2 the index of the other polynomial
    */
    double chebyshevSecondNorm(int n1,int n2);
    

  }//math
}//dmtpc
#endif
