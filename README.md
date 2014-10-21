Finite Size Lyapunov Exponent (FSLE) is a local lagrangian diagnostics that is widely used for the study of transport and mixing processes of oceanographic tracers (Sea surface temperature, Ocean color ...).
Its computation is derived from the definition of Finite-Time Lyapunov Exponent that allows the identification of  Lagrangian Coherent Structures.

Given a time series of meso-scale velocity field, this code computes corresponding sub-mesoscale maps of backward FLSE or FTLE. It also compute other diagnostics such as orientation of Finite-Time Lyapunov Vectors.

Install
=====

https://bitbucket.org/fbriol/lagrangian/wiki/INSTALL

Lagrangian Coherent Structures and Finite-Time Lyapunov Exponents
========================================================

The transport of a tracer in a fluid is closely related to emergent patterns that are commonly referred to as *coherent structures* or Lagrangian Coherent Structures (LCS) when they are defined using fluid trajectories. For time-independant dynamic systems, they correspond to  stable  and  unstable  manifolds  of  hyperbolic trajectories. Coherent structures delimit regions  of whirls,  stretching,  or contraction  of tracer: contraction is observed along stable manifolds whereas unstable manifolds correspond to divergent directions along which the tracer is stretched. Stable and unstable manifolds are material curves that act as transport barriers and that exhibits locally the strongest attraction, repulsion  or shearing in the  flow over  a finite-time  interval.
LCSs are usually indentified in a practicall manner as maximizing ridges of Finite-Time Lyapunov Exponents (FTLE) field.

FTLE is defined  as the largest eigenvalue of  the Cauchy-Green strain tensor  of   the  flow  map   (see  below  for  more   details).   The corresponding eigenvector is called Finite-Time Lyapunov Vector. FTLE and FSLE are  widely  used  to characterise  transport  and mixing processes in  oceanographic  flows where  the velocity field is only known as a finite data set. Recent work also demonstrate the potential use of FTLE in tracer image assimilation in geophysical models. [See also](http://core.kmi.open.ac.uk/download/pdf/4884607.pdf) which reviews the use  of  FTLE for detecting LCSs in other contexts.

FTLE and FSLE: definition and properties
=================================

FTLE is a  scalar local  notion that represents  the rate of  separation of initially   neighboring   particules   over   a   finite-time   window *[t,t+T]*. FTLE at a point x is defined as the growth factor of the norm of the perturbation delta started  around x at time t and  advected by the flow after an advection time T. Maximal stretching occurs  when delta is aligned  with the eigenvector associated to the maximum eigenvalue lambda_max of the Cauchy-Green strain tensor: D = M^*M where M : x(t) -> x(t+T)  is the flow map of the advection system.
This eigenvector  is referred  to as the *forward* Finite-Time Lyapunov Vector. The \textit{forward} FTLE at the point x is then defined as: FTLE(x) = (1/2T) log(lambda_max).

FSLE are FTLE where T is not fixed a-priori but determined so that the particule separate from a given distance. FSLE at a point x and for a final separation distance d is defined as: FSLE(x,d) = (1/2T(d)) log(lambda_max(d)). Seeding a  domain with particules initially located  on a grid leads to  the computation of a discretized  FSLE and FSLV fields.

*Backward* FSLE-Vs are  similarly defined, with the time direction  being  inverted  in the advection equation resolution. Ridges of BFTLE fields approximate attracting  LCSs.

Backward FTLE fields  show contours  that  correspond reasonably  well  to the  main structures  such  as filaments,  fronts  and  spirals  that appear  in geophysical and bio-geochemical tracer fields.
Also, orientation  of the gradient of a *passive* tracer   converges   to   that of backward FTLVs freely decaying  2D turbulence flow but this behaviour has been observed for  realistic oceanic  flows and  tracers. 

Furthermore,
\cite{2010-BERON-VERA-JPO,2010-BERON-VERA-JGR}
showed, using real data,
that  properties of FSLE and FTLE remain  valid  with  a mesoscale  advection,
i.e.  when the  resolution  of  the velocity  field  --- from  which
\mbox{FTLE-V} are computed --- is  much lower than the resolution of
the  observed  tracer  field \cite{2007-LEHAHN,2009b-DOVIDIO} .

References
=============================================================================


{2012-GAULTIER}
L. Gaultier, J. Verron, J.-M. Brankart, O. Titaud, and P. Brasseur. 
On the inversion of submesoscale tracer fields to estimate the surface ocean circulation. 
Journal of Marine Systems, 2012

{2011-TITAUD}
O. Titaud, J.-M. Brankart, and J. Verron. 
On the use of finite-time Lyapunov exponents and vectors for direct assimilation of tracer images into ocean models. 
Tellus A, 63(5):1038-1051, 2011. 


{1997-ARTALE}
Artale, V., Boffetta, G., Celani, A., Cencini, M. and Vulpiani, A.,
Dispersion of passive tracers in closed basins: Beyond the diffusion coefficient. 
Physics of Fluids 9(11):3162--3171, 1997.

1997-AURELL
Aurell, E., Boffetta, G., Crisanti, A., Paladin, G. and Vulpiani, A.,
Predictability in the large: an extension of the concept of lyapunov
exponent. Journal of Physics A: Mathematical and General, 30(1):1--26, 1997

{2010-BERON-VERA-JGR}
Beron-Vera, F.~J. Mixing by low- and high-resolution surface geostrophic
currents. J. Geophys. Res. 115(C10027), 2010.

{2008-BERON-VERA-ATMO}
Beron-Vera, F.~J., Brown, M.~G., Olascoaga, M.~J., Rypina, I.~I., Ko\c{c}ak, H.
and Udovydchenkov, I.~A. 
Zonal jets as transport barriers in planetary atmospheres. 
Journal of the Atmospheric Sciences 65(10),3316--3326, 2008.

{2009-BERON-VERA-JPO}
Beron-Vera, F.~J. and Olascoaga, M.~J. 
An assessment of the importance of
  chaotic stirring and turbulent mixing on the west florida shelf. {\em Journal
  of Physical Oceanography} {\bf 39}(7),~1743--1755.2009


\harvarditem[Beron-Vera et~al.]{Beron-Vera, Olascoaga and
  Goni}{2010}{2010-BERON-VERA-JPO}
Beron-Vera, F.~J., Olascoaga, M.~J. and Goni, G.~J. 2010. Surface ocean mixing
  inferred from different multisatellite altimetry measurements. {\em Journal
  of Physical Oceanography} {\bf 40}(11),~2466--2480.
\newblock \doi{10.1175/2010JPO4458.1}.



\harvarditem[{d'Ovidio} et~al.]{{d'Ovidio}, Fern{\'a}ndez,
  Hern{\'a}ndez-Garc{\'i}a and L{\'o}pez}{2004}{2004-DOVIDIO}
{d'Ovidio}, F., Fern{\'a}ndez, V., Hern{\'a}ndez-Garc{\'i}a, E. and L{\'o}pez,
  C. 2004. Mixing structures in the mediterranean sea from finite-size lyapunov
  exponents. {\em Geophysical Research Letters} {\bf 31}(L17203).
\newblock \doi{10.1029/2004GL020328}.

\harvarditem[{d'Ovidio} et~al.]{{d'Ovidio}, Isern-Fontanet, L{\'o}pez, Hern{\'a}ndez-Garc{\'i}a
  and Garc{\'i}a-Ladona}{2009a}{2009-DOVIDIO}
{d'Ovidio}, F., Isern-Fontanet, J., L{\'o}pez, C., Hern{\'a}ndez-Garc{\'i}a, E.
  and Garc{\'i}a-Ladona, E. 2009. Comparison between eulerian diagnostics and
  finite-size lyapunov exponents computed from altimetry in the algerian basin.
  {\em Deep Sea Research Part I: Oceanographic Research Papers} {\bf
  51}(1),~15--31.
\newblock \doi{10.1016/j.dsr.2008.07.014}.

\harvarditem[{d'Ovidio} et~al.]{{d'Ovidio}, Taillandier, Taupier-Letage and
  Mortier}{2009b}{2009b-DOVIDIO}
{d'Ovidio}, F., Taillandier, V., Taupier-Letage, I. and Mortier, L. 2009.
  Lagrangian validation of the mediterranean mean dynamic topography by
  extraction of tracer frontal structures. {\em Mercator Ocean Quarterly
  Newsletter} {\bf 32},~24--32.

\harvarditem{Haller}{2001a}{2001a-HALLER}
Haller, G. 2001a. Distinguished material surfaces and coherent structures in
  three-dimensional fluid flows. {\em Physica D} {\bf 149}(4),~248 -- 277.
\newblock \doi{10.1016/S0167-2789(00)00199-8}.

\harvarditem{Haller}{2001b}{2001-HALLER}
Haller, G. 2001b. Lagrangian structures and the rate of strain in a partition
  of two-dimensional turbulence. {\em Physics of Fluids} {\bf
  13}(11),~3365--3385.
\newblock \doi{10.1063/1.1403336}.

\harvarditem{Haller}{2002}{2002-HALLER}
Haller, G. 2002. Lagrangian coherent structures from approximate velocity data.
  {\em Physics of Fluids} {\bf 14}(6),~1851--1861.
\newblock \doi{10.1063/1.1477449}.

\harvarditem{Haller}{2011}{2011-HALLER-PhysicaD}
Haller, G. 2011. A variational theory of hyperbolic lagrangian coherent
  structures. {\em Physica D: Nonlinear Phenomena} {\bf 240}(7),~574 -- 598.
\newblock \doi{10.1016/j.physd.2010.11.010}.

\harvarditem{Haller and Yuan}{2000}{2000b_HALLER}
Haller, G. and Yuan, G. 2000. Lagrangian coherent structures and mixing in
  two-dimensional turbulence. {\em Physica D: Nonlinear Phenomena} {\bf
  147}(3-4),~352--370.
\newblock \doi{10.1016/S0167-2789(00)00142-1}.


\harvarditem{Lapeyre}{2002}{2002-LAPEYRE}
Lapeyre, G. 2002. Characterization of finite-time lyapunov exponents and
  vectors in two-dimensional turbulence. {\em Chaos} {\bf 12}(3),~688--698.
\newblock \doi{10.1063/1.1499395}.


\harvarditem[Lehahn et~al.]{Lehahn, {d'Ovidio}, L{\'e}vy and
  Heifetz}{2007}{2007-LEHAHN}
Lehahn, Y., {d'Ovidio}, F., L{\'e}vy, M. and Heifetz, E. 2007. Stirring of the
  northeast atlantic spring bloom: A lagrangian analysis based on
  multisatellite data. {\em J. Geophys. Res.} {\bf 112}(C08005).
\newblock \doi{10.1029/2006JC003927}.

\harvarditem[Lekien et~al.]{Lekien, Coulliette, Mariano, Ryan, Shay, Haller and
  Marsden}{2005}{2005a-LEKIEN-PhysicaD}
Lekien, F., Coulliette, C., Mariano, A.~J., Ryan, E.~H., Shay, L.~K., Haller,
  G. and Marsden, J. 2005. Pollution release tied to invariant manifolds: A
  case study for the coast of florida. {\em Physica D: Nonlinear Phenomena}
  {\bf 210}(1-2),~1 -- 20.
\newblock \doi{10.1016/j.physd.2005.06.023}.

\harvarditem[Mathur et~al.]{Mathur, Haller, Peacock, Ruppert-Felsot and
  Swinney}{2007}{2007-MATHUR-PhysRevLett}
Mathur, M., Haller, G., Peacock, T., Ruppert-Felsot, J.~E. and Swinney, H.~L.
  2007. Uncovering the lagrangian skeleton of turbulence. {\em Phys. Rev.
  Lett.} {\bf 98}(14),~144502.
\newblock \doi{10.1103/PhysRevLett.98.144502}.

\harvarditem{Olascoaga}{2010}{2010-OLASCOAGA-NPG}
Olascoaga, M.~J. 2010. Isolation on the west florida shelf with implications
  for red tides and pollutant dispersal in the gulf of mexico. {\em Nonlinear
  Processes in Geophysics} {\bf 17}(6),~685--696.
\newblock \doi{10.5194/npg-17-685-2010}.

\harvarditem[Olascoaga et~al.]{Olascoaga, Beron-Vera, Brand and
  Ko\c{c}ak}{2008}{2008-OLASCOAGA-JGR}
Olascoaga, M.~J., Beron-Vera, F.~J., Brand, L.~E. and Ko\c{c}ak, H. 2008.
  Tracing the early development of harmful algal blooms on the west florida
  shelf with the aid of lagrangian coherent structures. {\em J. Geophys. Res.}
  {\bf 113}(C12014).
\newblock \doi{10.1029/2007JC004533}.

\harvarditem[Olascoaga et~al.]{Olascoaga, Rypina, Brown, Beron-Vera, Ko\c{c}ak,
  Brand, Halliwell and Shay}{2006}{2006-OLASCOAGA-GRL}
Olascoaga, M.~J., Rypina, I.~I., Brown, M.~G., Beron-Vera, F.~J., Ko\c{c}ak,
  H., Brand, L.~E., Halliwell, G.~R. and Shay, L.~K. 2006. Persistent transport
  barrier on the west florida shelf. {\em Geophys. Res. Lett.} {\bf
  33}(L22603).
\newblock \doi{10.1029/2006GL027800}.

\harvarditem{Ott}{1993}{1994-OTT}
Ott, E. 1993. Chaos in dynamical systems. {\em Chaos in Dynamical Systems} Ott,
  E. Cambridge University Press, New York, USA.

\harvarditem{Ottino}{1989}{1989-OTTINO}
Ottino, J. 1989. The kinematics of mixing: Stretching, chaos, and transport.
  {\em The Kinematics of Mixing: Stretching, Chaos, and Transport} Ottino, J.
  Cambridge University Press, Cambridge.

\harvarditem{Peacock and Dabiri}{2010}{2010-PEACOCK-Chaos}
Peacock, T. and Dabiri, J. 2010. Introduction to focus issue: Lagrangian
  coherent structures. {\em Chaos} {\bf 20}(1),~017501.
\newblock \doi{10.1063/1.3278173}.

\harvarditem[Shadden et~al.]{Shadden, Lekien and Marsden}{2005}{2005-SHADDEN}
Shadden, S.~C., Lekien, F. and Marsden, J.~E. 2005. Definition and properties
  of lagrangian coherent structures from finite-time lyapunov exponents in
  two-dimensional aperiodic flows. {\em Physica D: Nonlinear Phenomena} {\bf
  212}(3-4),~271--304.
\newblock \doi{10.1016/j.physd.2005.10.007}.

\harvarditem[Shadden et~al.]{Shadden, Lekien, Paduan, Chavez and
  Marsden}{2009}{2009-SHADDEN-DeepSeaRes}
Shadden, S.~C., Lekien, F., Paduan, J.~D., Chavez, F.~P. and Marsden, J.~E.
  2009. The correlation between surface drifters and coherent structures based
  on high-frequency radar data in monterey bay. {\em Deep Sea Research Part II:
  Topical Studies in Oceanography} {\bf 56}(3-5),~161 -- 172.
\newblock \doi{10.1016/j.dsr2.2008.08.008}.

\harvarditem{Wiggins}{1992}{1992-WIGGINS}
Wiggins, S. 1992. Chaotic transport in dynamical systems. {\em Chaotic
  Transport in Dynamical Systems} Wiggins, S. Vol.~2 of {\em Interdisciplinary
  Applied Mathematics} Springer-Verlag, Berlin.