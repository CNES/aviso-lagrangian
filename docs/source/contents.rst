LCS and FTLE
============

This section discusses *Lagrangian Coherent Structures* (LCS) and
*Finite-Time Lyapunov Exponents* (FTLE)—formally, *Lagrangian Coherent
Structures and Finite-Time Lyapunov Exponents*—and their role in describing
tracer transport in fluid flows.

The transport of a tracer in a fluid is closely linked to the emergence of
patterns commonly referred to as *coherent structures*, or *Lagrangian Coherent
Structures* (LCS) when defined using fluid trajectories. In time-independent
dynamical systems, these structures correspond to the stable and unstable
manifolds of hyperbolic trajectories [1]_. Coherent structures delineate
regions of swirling, stretching, or contracting tracer distributions [2]_:
contraction occurs along stable manifolds, whereas unstable manifolds mark
divergent directions where tracers are stretched. Stable and unstable manifolds
are material curves that act as transport barriers, exhibiting the strongest
local attraction, repulsion, or shear in the flow over a finite time interval
[3]_. In practice, LCSs are often identified as ridges in the field of
Finite-Time Lyapunov Exponents (FTLE) [4]_, [5]_, [6]_, [7]_, [8]_.

FTLE is defined as the largest eigenvalue of the Cauchy–Green strain tensor of
the flow map (see below for more details). The corresponding eigenvector is
called the Finite-Time Lyapunov Vector. FTLE and FSLE are widely used to
characterize transport and mixing processes in oceanographic flows, especially
where the velocity field is only available as a finite dataset [9]_, [10]_,
[11]_, [12]_, [13]_, [14]_, [15]_, [16]_, [17]_, [18]_, [19]_, [20]_, [21]_.
Recent studies have also demonstrated the potential of FTLE for tracer image
assimilation in geophysical models [22]_, [23]_.

.. seealso::

	[24]_ for a review of FTLE applications in detecting LCSs in other contexts.

FTLE and FSLE: Definitions and Properties
##########################################

FTLE is a local scalar measure of the rate at which initially neighbouring
particles separate over a finite-time window :math:`[t, t+T]`. The FTLE at a
point :math:`x` is defined as the growth rate of the norm of a small
perturbation :math:`\delta` initiated around :math:`x` at time :math:`t` and
advected by the flow for a duration :math:`T`. Maximal stretching occurs when
:math:`\delta` aligns with the eigenvector associated with the largest
eigenvalue :math:`\lambda_{\max}` of the Cauchy–Green strain tensor
:math:`\Delta = M^T M`, where :math:`M : x(t) \mapsto x(t+T)` is the flow map of
the advection system. This eigenvector is referred to as the *forward*
Finite-Time Lyapunov Vector. The *forward* FTLE at :math:`x` is then defined as:

:math:`FTLE(x) = \frac{\log(\lambda_{\max})}{2T}`.

FSLE is similar to FTLE, but the integration time :math:`T` is not fixed
a priori; instead, it is determined so that the particle separation reaches a
prescribed final distance. The FSLE at point :math:`x` for a separation
distance :math:`d` is defined as:

:math:`FSLE(x, d) = \frac{\log(\lambda_{\max}(d))}{2T(d)}`.

Seeding a domain with particles initially located on a grid enables the
computation of discretized FSLE and FSLV fields.

*Backward* FSLE-Vs are defined in the same way, but with the time direction
reversed in the advection equation. Ridges in backward FTLE (BFTLE) fields
approximate attracting LCSs [25]_.

Backward FTLE fields reveal contours that correspond closely to major
structures—such as filaments, fronts, and spirals—found in geophysical and
biogeochemical tracer fields [10]_, [14]_, [15]_, [26]_. The orientation of the
gradient of a *passive* tracer has been shown to converge towards that of
backward FTLVs in freely decaying two-dimensional turbulence [27]_. This
behaviour has also been observed in realistic oceanic flows and tracers [20]_.

Furthermore, [11]_ and [28]_ demonstrated, using real data, that the properties
of FSLE and FTLE remain valid at the mesoscale, i.e., when the resolution of the
velocity field—used to compute FTLE-V—is much lower than the resolution of the
observed tracer field [20]_, [21]_.

References
##########

.. [1] Wiggins, S. 1992. Chaotic transport in dynamical systems. Vol. 2 of
	*Interdisciplinary Applied Mathematics*. Springer-Verlag, Berlin.

.. [2] Ottino, J. 1989. The kinematics of mixing: Stretching, chaos, and
	transport. Cambridge University Press, Cambridge.

.. [3] Haller, G. and Yuan, G. 2000. Lagrangian coherent structures and mixing
	in two-dimensional turbulence. *Physica D: Nonlinear Phenomena* **147**
	(3-4): 352--370.

.. [4] Haller, G. 2001. Distinguished material surfaces and coherent structures
	in three-dimensional fluid flows. *Physica D* **149** (4): 248 -- 277.

.. [5] Haller, G. 2001. Lagrangian structures and the rate of strain in a
	partition of two-dimensional turbulence. *Physics of Fluids* **13**
	(11): 3365 --3385.

.. [6] Haller, G. 2002. Lagrangian coherent structures from approximate
	velocity data. *Physics of Fluids* **14** (6): 1851--1861.

.. [7] Shadden, S.~C., Lekien, F. and Marsden, J. E. 2005. Definition and
	properties of lagrangian coherent structures from finite-time lyapunov
	exponents in two-dimensional aperiodic flows. *Physica D: Nonlinear Phenomena*
	**212** (3-4): 271--304.

.. [8] Mathur, M., Haller, G., Peacock, T., Ruppert-Felsot, J. E. and Swinney, H.
	L. 2007. Uncovering the lagrangian skeleton of turbulence. *Phys. Rev. Lett.*
	**98** (14): 144502

.. [9] Beron-Vera, F. J., Brown, M.~G., Olascoaga, M. J., Rypina, I. I., KoÃ§ak,
	H. and Udovydchenkov, I. A. 2008. Zonal jets as transport barriers in
	planetary atmospheres. *Journal of the Atmospheric Sciences* **65** (10): 3316
	--3326

.. [10] Beron-Vera, F. J. and Olascoaga, M. J. 2009. An assessment of the
	importance of chaotic stirring and turbulent mixing on the west florida shelf.
	*Journal of Physical Oceanography* **39** (7): 1743--1755.

.. [11] Beron-Vera, F. J., Olascoaga, M. J. and Goni, G. J. 2010. Surface ocean
	mixing inferred from different multisatellite altimetry measurements. *Journal
	of Physical Oceanography* **40** (11): 2466--2480.

.. [12] Lekien, F., Coulliette, C., Mariano, A. J., Ryan, E. H., Shay, L. K.,
	Haller, G. and Marsden, J. 2005. Pollution release tied to invariant
	manifolds: A case study for the coast of florida. *Physica D: Nonlinear
	Phenomena* **210** (1-2): 1--20.

.. [13] Olascoaga, M. J. 2010. Isolation on the west florida shelf with
	implications for red tides and pollutant dispersal in the gulf of mexico.
	*Nonlinear Processes in Geophysics* **17** (6): 685--696.

.. [14] Olascoaga, M. J., Beron-Vera, F. J., Brand, L. E. and Kocak, H. 2008.
	Tracing the early development of harmful algal blooms on the west florida
	shelf with the aid of lagrangian coherent structures. *J. Geophys. Res.*
	**113** (C12014).

.. [15] Shadden, S.~C., Lekien, F., Paduan, J. D., Chavez, F. P. and Marsden, J.
	E. 2009. The correlation between surface drifters and coherent structures
	based on high-frequency radar data in monterey bay. *Deep Sea Research Part
	II: Topical Studies in Oceanography* **56** (3-5): 161--172.

.. [16] Aurell, E., Boffetta, G., Crisanti, A., Paladin, G. and Vulpiani, A.,
	1997. Predictability in the large: an extension of the concept of lyapunov
	exponent. *Journal of Physics A: Mathematical and General* **30** (1):1--26

.. [17] Artale, V., Boffetta, G., Celani, A., Cencini, M. and Vulpiani, A., 1997.
	Dispersion of passive tracers in closed basins: Beyond the diffusion
	coefficient. *Physics of Fluids* **9** (11):3162--3171.

.. [18] d'Ovidio, F., Fernandez, V., Hernandez-Garcia, E. and Lopez, C. 2004.
	Mixing structures in the mediterranean sea from finite-size lyapunov
	exponents. *Geophysical Research Letters* **31** (L17203).

.. [19] d'Ovidio, F., Isern-Fontanet, J., Lopez, C., Hernandez-Garcia, E. and
	Garcia-Ladona, E. 2009. Comparison between eulerian diagnostics and finite-
	size lyapunov exponents computed from altimetry in the algerian basin. *Deep
	Sea Research Part I: Oceanographic Research Papers* **51** (1): 15--31.

.. [20] d'Ovidio, F., Taillandier, V., Taupier-Letage, I. and Mortier, L. 2009.
	Lagrangian validation of the mediterranean mean dynamic topography by
	extraction of tracer frontal structures. *Mercator Ocean Quarterly Newsletter*
	**32**: 24--32.

.. [21] Lehahn, Y., d'Ovidio, F., LÃ©vy, M. and Heifetz, E. 2007. Stirring of the
	northeast atlantic spring bloom: A lagrangian analysis based on multisatellite
	data. *J. Geophys. Res.* **112** (C08005).

.. [22] O. Titaud, J.-M. Brankart, and J. Verron. 2011. On the use of finite-time
	Lyapunov exponents and vectors for direct assimilation of tracer images into
	ocean models. *Tellus A*, **63** (5):1038-1051.

.. [23] L. Gaultier, J. Verron, J.-M. Brankart, O. Titaud, and P. Brasseur. 2012.
	On the inversion of submesoscale tracer fields to estimate the surface ocean
	circulation. *Journal of Marine Systems* **126**: 33--42.

.. [24] Peacock, T. and Dabiri, J. 2010. Introduction to focus issue: Lagrangian
	coherent structures. * Chaos* **20** (1): 017501.

.. [25] Haller, G. 2011. A variational theory of hyperbolic lagrangian coherent
	structures. *Physica D: Nonlinear Phenomena* **240** (7): 574 -- 598.

.. [26] Olascoaga, M. J., Rypina, I. I., Brown, M. G., Beron-Vera, F. J., Kocak,
	H., Brand, L.~E., Halliwell, G.~R. and Shay, L.~K. 2006. Persistent transport
	barrier on the west florida shelf. *Geophys. Res. Lett.* **33** (L22603).

.. [27] Lapeyre, G. 2002. Characterization of finite-time lyapunov exponents and
	vectors in two-dimensional turbulence. *Chaos* **12** (3): 688--698.

.. [28] Beron-Vera, F. J. Mixing by low- and high-resolution surface geostrophic
	currents. 2010. *J. Geophys. Res*. **115** (C10027)
